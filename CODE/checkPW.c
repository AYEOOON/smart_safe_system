#include "checkPW.h"

// 비밀번호와 복구 질문 정답
const char password[PASSWORD_LENGTH] = {'1', '2', '3', '4'};
const char recoveryAnswer[RECOVERY_ANSWER_LENGTH] = {'k', 'i', 't'};

// 서보모터 제어 함수
void rotate_Servo(int angle) {
    pinMode(SERVO, PWM_OUTPUT);  // PWM 출력 모드 설정
    pwmSetMode(PWM_MODE_MS);     // 마이크로초 모드 설정
    pwmSetRange(2000);           // PWM 범위 설정
    pwmSetClock(192);            // PWM 클럭 설정
    pwmWrite(SERVO, angle);      // 서보모터 각도 설정
}

// 다중 바이트 송신 함수
void serialWriteBytes(const int fd, const char* s) {
    write(fd, s, strlen(s));
}

// 1바이트 수신 함수
unsigned char serialRead(const int fd) {
    unsigned char x;
    if (read(fd, &x, 1) != 1) {
        return -1;
    }
    return x;
}

// 비밀번호 확인 함수
int checkPassword(const char* input) {
    for (int i = 0; i < PASSWORD_LENGTH; i++) {
        if (input[i] != password[i]) {
            return 0;  // 비밀번호가 다르면 0 반환
        }
    }
    return 1;  // 비밀번호가 일치하면 1 반환
}

// 복구 질문 확인 함수
int checkRecoveryAnswer(const char* input) {
    for (int i = 0; i < RECOVERY_ANSWER_LENGTH; i++) {
        if (input[i] != recoveryAnswer[i]) {
            return 0;
        }
    }
    return 1;  // 정답이 맞으면 1 반환
}

// 비밀번호 관리 함수
void checkPW() {
    int fd_serial;
    unsigned char dat;
    char input[10] = {0};  // 입력 버퍼
    int inputIndex = 0;
    int safeUnlocked = 0;  // 금고 잠금 상태 (0: 잠금, 1: 해제)
    int attempts = 0;      // 비밀번호 입력 시도 횟수
    int recoveryMode = 0;  // 복구 질문 상태 (0: 비활성, 1: 활성)

    // GPIO 초기화
    if (wiringPiSetupGpio() < 0) {
        printf("Failed to setup GPIO.\n");
        return;
    }

    // UART 초기화
    if ((fd_serial = serialOpen(UART2_DEV, BAUD_RATE)) < 0) {
        printf("Unable to open serial device.\n");
        return;
    }

    // 블루투스 연결 성공 메시지
    printf("블루투스 연결이 완료 되었습니다.\n");
    serialWriteBytes(fd_serial, "비밀번호를 입력하세요\n");

    while (1) {
        // 데이터 수신 여부 확인
        if (serialDataAvail(fd_serial)) {
            dat = serialRead(fd_serial);  // 1바이트 읽기

            if (recoveryMode == 0) {  // 복구 질문 상태가 아닐 때
                if (safeUnlocked == 0) {  // 금고가 잠긴 상태
                    input[inputIndex++] = dat;   // 입력 버퍼에 저장

                    if (inputIndex == PASSWORD_LENGTH) {  // 비밀번호 4자리 입력 완료
                        input[inputIndex] = '\0';  // 문자열 종료
                        
                        // 비밀번호 검증
                        if (checkPassword(input)) {
                            printf("비밀번호 일치! 금고 해제\n");
                            serialWriteBytes(fd_serial, "잠금이 해제되었습니다.\n");

                            // 서보모터를 250도로 회전하여 잠금 해제
                            rotate_Servo(250);
                            safeUnlocked = 1;  // 금고 상태를 '해제'로 설정
                            ledControl(safeUnlocked);

                            // 사용자에게 잠금 안내 메시지
                            serialWriteBytes(fd_serial, "금고를 잠그려면 '@'를 입력하세요\n");
                            attempts = 0;  // 시도 횟수 초기화
                        } else {
                            attempts++;  // 실패 시도 횟수 증가
                            printf("비밀번호 틀림: %d/%d.\n", attempts, MAX_ATTEMPTS);

                            // 실패 시도 횟수를 휴대폰에 출력
                                char attemptMessage[50];
                                snprintf(attemptMessage, sizeof(attemptMessage), "비밀번호 틀림: %d/%d\n", attempts, MAX_ATTEMPTS);
                                serialWriteBytes(fd_serial, attemptMessage);

                            if (attempts >= MAX_ATTEMPTS) {
                                serialWriteBytes(fd_serial, "비밀번호 입력 시도 초과! 출신 학교가 어디입니까?\n");
                                recoveryMode = 1;  // 복구 질문 활성화
                            } else {
                                serialWriteBytes(fd_serial, "비밀번호가 틀렸습니다. 다시 입력하세요.\n");
                            }
                        }

                        inputIndex = 0;  // 다음 입력을 위해 초기화
                        memset(input, 0, sizeof(input));  // 입력 버퍼 초기화
                    }
                } else {  // 금고가 해제된 상태
                    if (dat == '@') {  // '@' 입력 시 금고 잠금
                        printf("Locking safe.\n");
                        serialWriteBytes(fd_serial, "금고가 잠겼습니다.\n");

                        // 서보모터를 50도로 회전하여 잠금
                        rotate_Servo(50);
                        safeUnlocked = 0;  // 금고 상태를 '잠금'으로 설정
                        ledControl(safeUnlocked);
                        // 사용자에게 비밀번호 입력 안내 메시지 전송
                        serialWriteBytes(fd_serial, "비밀번호를 입력하세요\n");
                    }
                }
            } else {  // 복구 질문 상태
                input[inputIndex++] = dat;  // 입력 버퍼에 저장

                if (dat == '\n' || inputIndex >= sizeof(input) - 1) {  // 엔터키 입력 또는 버퍼 초과
                    input[inputIndex] = '\0';  // 문자열 종료

                    if (checkRecoveryAnswer(input)) {
                        printf("Recovery answer correct. Resetting attempts.\n");
                        serialWriteBytes(fd_serial, "정답입니다. 다시 비밀번호를 입력하세요.\n");
                        attempts = 0;  // 시도 횟수 초기화
                        recoveryMode = 0;  // 복구 질문 비활성
                    } else {
                        printf("Recovery answer incorrect.\n");
                        serialWriteBytes(fd_serial, "정답이 틀렸습니다. 프로그램을 종료합니다.\n");
                        exit(1);  // 프로그램 종료
                    }

                    inputIndex = 0;  // 입력 버퍼 초기화
                    memset(input, 0, sizeof(input));  // 버퍼 초기화
                }
            }

            // 복구 질문 이후 시도 초과 처리
            if (recoveryMode == 1 && attempts >= MAX_ATTEMPTS) {
                printf("Maximum attempts exceeded after recovery. Exiting program.\n");
                serialWriteBytes(fd_serial, "비밀번호 입력 시도 초과. 프로그램을 종료합니다.\n");
                exit(1);  // 프로그램 종료
            }
        }

        delay(10);  // CPU 사용량 줄이기
    }
}