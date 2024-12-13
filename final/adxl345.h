#ifndef ADXL345_H
#define ADXL345_H

#include <pthread.h>
#include <softTone.h>

// GPIO 및 SPI 설정
#define CS_GPIO 8       // CS 핀
#define BUZZER_GPIO 18  // 부저 핀
#define SPI_CH 0
#define SPI_SPEED 1000000 // 1MHz
#define SPI_MODE 3

// ADXL345 레지스터
#define BW_RATE 0x2C
#define POWER_CTL 0x2D
#define DATA_FORMAT 0x31
#define DATAX0 0x32
#define DATAX1 0x33
#define DATAY0 0x34
#define DATAY1 0x35
#define DATAZ0 0x36
#define DATAZ1 0x37

// 공유 데이터 및 동기화 객체
typedef struct {
    short x, y, z;             // 가속도 데이터
    pthread_mutex_t mutex;     // 데이터 보호용 뮤텍스
    int running;               // 스레드 종료 플래그
    int triggerBuzzer;
} SharedData;
// extern SharedData sharedData;

// 함수 선언
int initSPI();
void writeRegister_ADXL345(char address, char value);
void readRegister_ADXL345(char registerAddress, int numBytes, char *values);
void readAccelerometerData(short *x, short *y, short *z);
void *sensorThread(void *arg);
void *buzzerThread(void *arg);
void triggerBuzzer(int durationMs); // 공용 부저 제어 함수
void triggerBuzzerPWM(int durationMs, int frequency);


#endif
