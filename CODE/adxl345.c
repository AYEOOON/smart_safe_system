#include "adxl345.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

// SPI 초기화
int initSPI() {
    if (wiringPiSetupGpio() == -1) {
        fprintf(stderr, "Failed to setup GPIO.\n");
        return -1;
    }

    if (wiringPiSPISetupMode(SPI_CH, SPI_SPEED, SPI_MODE) == -1) {
        fprintf(stderr, "Failed to setup SPI.\n");
        return -1;
    }

    pinMode(CS_GPIO, OUTPUT);
    pinMode(BUZZER_GPIO, OUTPUT);
    digitalWrite(CS_GPIO, HIGH); // CS 초기화
    digitalWrite(BUZZER_GPIO, LOW); // 부저 초기화
    return 0;
}

// ADXL345 레지스터에 쓰기
void writeRegister_ADXL345(char address, char value) {
    unsigned char buff[2] = { address, value };
    digitalWrite(CS_GPIO, LOW);
    wiringPiSPIDataRW(SPI_CH, buff, 2);
    digitalWrite(CS_GPIO, HIGH);
}

// ADXL345 레지스터 읽기
void readRegister_ADXL345(char registerAddress, int numBytes, char *values) {
    values[0] = 0x80 | registerAddress;
    if (numBytes > 1) values[0] |= 0x40;

    digitalWrite(CS_GPIO, LOW);
    wiringPiSPIDataRW(SPI_CH, values, numBytes + 1);
    digitalWrite(CS_GPIO, HIGH);
}

// 가속도 데이터 읽기
void readAccelerometerData(short *x, short *y, short *z) {
    unsigned char buffer[7] = {0};
    readRegister_ADXL345(DATAX0, 6, buffer);

    *x = ((short)buffer[2] << 8) | (short)buffer[1];
    *y = ((short)buffer[4] << 8) | (short)buffer[3];
    *z = ((short)buffer[6] << 8) | (short)buffer[5];
}

// 부저 제어 함수
void triggerBuzzer(int durationMs) {
    digitalWrite(BUZZER_GPIO, HIGH);
    delay(durationMs);
    digitalWrite(BUZZER_GPIO, LOW);
}

// 센서 데이터를 읽는 스레드
void *sensorThread(void *arg) {
    SharedData *data = (SharedData *)arg;

    while (data->running) {
        short x, y, z;
        readAccelerometerData(&x, &y, &z);

        pthread_mutex_lock(&data->mutex);
        data->x = x;
        data->y = y;
        data->z = z;
        pthread_mutex_unlock(&data->mutex);

        usleep(50000); // 50ms 대기
    }
    return NULL;
}

// 부저를 제어하는 스레드
void *buzzerThread(void *arg) {
    SharedData *data = (SharedData *)arg;
    const short threshold = 50;

    while (data->running) {
        pthread_mutex_lock(&data->mutex);
        short x = data->x, y = data->y, z = data->z;
        pthread_mutex_unlock(&data->mutex);

        short deltaX = abs(x), deltaY = abs(y), deltaZ = abs(z);

        if (deltaX > threshold || deltaY > threshold || deltaZ > threshold) {
            triggerBuzzer(50000); // 5000ms 부저 울림
        }

        usleep(10); // 100ms 대기
    }
    return NULL;
}
