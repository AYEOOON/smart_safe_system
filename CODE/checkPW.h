#ifndef CHECKPW_H
#define CHECKPW_H

#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// UART 및 서보모터 설정
#define BAUD_RATE 115200
#define UART2_DEV "/dev/ttyAMA1"  // UART 장치 파일 경로
#define SERVO 12                  // 서보모터 핀 번호

// 비밀번호 및 복구 질문 설정
#define MAX_ATTEMPTS 3
#define PASSWORD_LENGTH 4
#define RECOVERY_ANSWER_LENGTH 3

// 비밀번호 및 복구 정답
extern const char password[PASSWORD_LENGTH];
extern const char recoveryAnswer[RECOVERY_ANSWER_LENGTH];

// 함수 선언
void rotate_Servo(int angle);                     // 서보모터 제어 함수
void serialWriteBytes(const int fd, const char* s); // 다중 바이트 송신
unsigned char serialRead(const int fd);           // 1바이트 수신
int checkPassword(const char* input);             // 비밀번호 확인
int checkRecoveryAnswer(const char* input);       // 복구 질문 확인
void checkPW();                                   // 전체 비밀번호 관리 함수

#endif  // CHECKPW_H
