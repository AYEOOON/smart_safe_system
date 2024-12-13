#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <wiringPi.h>
#include "adxl345.h"
#include "checkPW.h"

#define LED1 23 // LED1 GPIO 핀
#define LED2 16 // LED2 GPIO 핀
#define SWITCH 24 // 스위치 GPIO 핀

// // 공유 데이터 구조체
// SharedData sharedData = {
//     .x = 0,
//     .y = 0,
//     .z = 0,
//     .running = 1
// };

SharedData sharedData = {
    .x = 0,
    .y = 0,
    .y = 0,
    .running = 1,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .triggerBuzzer = 0
};
SharedData *data = &sharedData; // 포인터 설정


// LED 상태 제어 함수
void ledControl(int safeUnlocked) {
    if (safeUnlocked == 1) {
        digitalWrite(LED1, HIGH); // 열림 상태 표시
        digitalWrite(LED2, LOW);
    } else {
        digitalWrite(LED1, LOW); // 초기 잠금 상태
        digitalWrite(LED2, HIGH);
    }
}

// 스위치 입력 처리 스레드
void *switchThread(void *arg) {
    while (sharedData.running) {
        if (digitalRead(SWITCH) == LOW) { // 스위치 눌림 감지
            printf("Switch pressed. Starting Bluetooth input...\n");
            // ledControl(1); // 연결 상태 표시
            checkPW(); // 비밀번호 확인 루틴 실행
            ledControl(0); // 초기화 상태로 복구
        }
        delay(100); // 디바운스 처리
    }
    return NULL;
}

int main() {
    pthread_t sensorTid, buzzerTid, switchTid;

    // GPIO 초기화
    if (wiringPiSetupGpio() < 0) {
        fprintf(stderr, "Failed to setup GPIO.\n");
        return -1;
    }

    // 핀 설정
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(SWITCH, INPUT);
    pullUpDnControl(SWITCH, PUD_UP); // 풀업 저항 설정
    ledControl(0); // 초기화 상태로 복구


    // SPI 및 ADXL345 초기화
    if (initSPI() == -1) {
        return 1;
    }
    writeRegister_ADXL345(DATA_FORMAT, 0x01); // +- 4G 설정
    writeRegister_ADXL345(BW_RATE, 0x0C);    // 데이터 출력 속도 400 Hz
    writeRegister_ADXL345(POWER_CTL, 0x08);  // 활성화

    // 스레드 생성
    if (pthread_create(&sensorTid, NULL, sensorThread, &sharedData) != 0) {
        perror("Failed to create sensor thread");
        return 1;
    }

    if (pthread_create(&buzzerTid, NULL, buzzerThread, &sharedData) != 0) {
        perror("Failed to create buzzer thread");
        return 1;
    }

    if (pthread_create(&switchTid, NULL, switchThread, NULL) != 0) {
        perror("Failed to create switch thread");
        return 1;
    }

    // // 메인 루프: 상태 출력
    // while (sharedData.running) {
    //     pthread_mutex_lock(&sharedData.mutex);
    //     printf("X: %d, Y: %d, Z: %d\n", sharedData.x, sharedData.y, sharedData.z);
    //     pthread_mutex_unlock(&sharedData.mutex);
    //     delay(100); // 상태 출력 주기
    // }

    // 스레드 종료 대기
    pthread_join(sensorTid, NULL);
    pthread_join(buzzerTid, NULL);
    pthread_join(switchTid, NULL);

    return 0;
}
