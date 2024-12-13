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

void triggerBuzzerPWM(int durationMs, int frequency) {
    softToneCreate(BUZZER_GPIO);  // 소프트웨어 PWM 초기화
    softToneWrite(BUZZER_GPIO, frequency); // 주파수 설정
    delay(durationMs); // 소리 지속 시간
    softToneWrite(BUZZER_GPIO, 0); // PWM 끄기
}

// 센서 데이터를 읽는 스레드
void *sensorThread(void *arg) {
    SharedData *data = (SharedData *)arg;
    short prevX = 0, prevY = 0, prevZ = 0;

    while (data->running) {
        short x, y, z;
        readAccelerometerData(&x, &y, &z);

        pthread_mutex_lock(&data->mutex);
        data->x = x;
        data->y = y;
        data->z = z;

        // 이전값과의 차이를 계산
        if (abs(x - prevX) > 50 || abs(y - prevY) > 50 || abs(z - prevZ) > 50) {
            data->triggerBuzzer = 1; // 부저 활성화 플래그 설정
        }
        pthread_mutex_unlock(&data->mutex);

        // 현재값을 이전값으로 저장
        prevX = x;
        prevY = y;
        prevZ = z;

        usleep(50000); // 50ms 대기
    }
    return NULL;
}

// 부저 스레드
void *buzzerThread(void *arg) {
    SharedData *data = (SharedData *)arg;
    const int duration = 300; // 부저 울림 시간 (300ms)
    const int frequency = 3000; // 부저 주파수 (3kHz)

    while (data->running) {
        pthread_mutex_lock(&data->mutex);
        if (data->triggerBuzzer) {
            data->triggerBuzzer = 0; // 플래그 초기화
            pthread_mutex_unlock(&data->mutex);

            triggerBuzzerPWM(duration, frequency); // 부저 작동
        } else {
            pthread_mutex_unlock(&data->mutex);
        }

        usleep(100000); // 100ms 대기
    }
    return NULL;
}
