#include "checkPW.h"
#include "adxl345.h"

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

// 비밀번호 검증 함수
int validateInput(const char* input, const char* correctAnswer, int length) {
    for (int i = 0; i < length; i++) {
        if (input[i] != correctAnswer[i]) {
            return 0;
        }
    }
    return 1;
}

// 금고 잠금 해제 상태 처리
void handleUnlock(int fd_serial, SharedData* data, int* attempts, int* safeUnlocked) {
    printf("Password correct! Unlocking safe.\n");
    serialWriteBytes(fd_serial, "잠금이 해제되었습니다.\n");

    rotate_Servo(250);
    *safeUnlocked = 1;
    ledControl(*safeUnlocked);

    serialWriteBytes(fd_serial, "금고를 잠그려면 '@'를 입력하세요\n");

    pthread_mutex_lock(&data->mutex);
    *attempts = 0;
    pthread_mutex_unlock(&data->mutex);
}

// 금고 잠금 상태 처리
void handleLock(int fd_serial, int* safeUnlocked) {
    printf("Locking safe.\n");
    serialWriteBytes(fd_serial, "금고가 잠겼습니다.\n");

    rotate_Servo(50);
    *safeUnlocked = 0;
    ledControl(*safeUnlocked);

    serialWriteBytes(fd_serial, "비밀번호를 입력하세요\n");
}

// 비밀번호 실패 처리
void handlePasswordFailure(int fd_serial, SharedData* data, int* attempts, int* recoveryMode) {
    pthread_mutex_lock(&data->mutex);
    (*attempts)++;
    int currentAttempts = *attempts;
    pthread_mutex_unlock(&data->mutex);

    printf("Password incorrect. Attempt %d/%d.\n", currentAttempts, MAX_ATTEMPTS);

    if (currentAttempts >= MAX_ATTEMPTS) {
        serialWriteBytes(fd_serial, "비밀번호 입력 시도 초과! 출신 학교가 어디입니까?\n");

        pthread_mutex_lock(&data->mutex);
        data->triggerBuzzer = 1;
        *recoveryMode = 1;
        pthread_mutex_unlock(&data->mutex);
    } else {
        serialWriteBytes(fd_serial, "비밀번호가 틀렸습니다. 다시 입력하세요.\n");
    }
}

// 복구 질문 처리
void handleRecovery(int fd_serial, SharedData* data, char* input, int* attempts, int* recoveryMode) {
    if (validateInput(input, recoveryAnswer, RECOVERY_ANSWER_LENGTH)) {
        printf("Recovery answer correct. Resetting attempts.\n");
        serialWriteBytes(fd_serial, "정답입니다. 다시 비밀번호를 입력하세요.\n");

        pthread_mutex_lock(&data->mutex);
        *attempts = 0;
        *recoveryMode = 0;
        data->triggerBuzzer = 0;
        pthread_mutex_unlock(&data->mutex);
    } else {
        printf("Recovery answer incorrect.\n");
        serialWriteBytes(fd_serial, "정답이 틀렸습니다. 프로그램을 종료합니다.\n");
        exit(1);
    }
}

// 비밀번호 관리 함수
void checkPW() {
    extern SharedData* data;  // SharedData 구조체 포인터를 외부에서 가져옴
    int fd_serial;            // 시리얼 장치 파일 디스크립터
    unsigned char dat;        // 수신된 데이터 저장 변수
    char input[10] = {0};     // 사용자 입력을 저장하는 버퍼
    int inputIndex = 0;       // 현재 입력 인덱스
    int safeUnlocked = 0;     // 금고 상태 플래그 (0: 잠금, 1: 해제)
    int attempts = 0;         // 비밀번호 입력 실패 횟수
    int recoveryMode = 0;     // 복구 모드 플래그 (0: 비활성, 1: 활성)

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

    // 초기화 메시지 출력
    printf("Bluetooth connection established.\n");
    serialWriteBytes(fd_serial, "비밀번호를 입력하세요\n");

    // 메인 루프
    while (1) {
        // 수신 데이터가 있을 경우 처리
        if (serialDataAvail(fd_serial)) {
            dat = serialRead(fd_serial);  // 1바이트 데이터 읽기

            if (recoveryMode == 0) {  // 복구 모드가 비활성화된 경우
                if (safeUnlocked == 0) {  // 금고가 잠긴 상태
                    input[inputIndex++] = dat;  // 입력 데이터를 버퍼에 저장

                    if (inputIndex == PASSWORD_LENGTH) {  // 비밀번호 길이만큼 입력받았을 때
                        input[inputIndex] = '\0';  // 문자열 종료

                        if (validateInput(input, password, PASSWORD_LENGTH)) {  // 비밀번호 검증
                            handleUnlock(fd_serial, data, &attempts, &safeUnlocked);
                        } else {
                            handlePasswordFailure(fd_serial, data, &attempts, &recoveryMode);
                        }

                        inputIndex = 0;  // 입력 버퍼 초기화
                        memset(input, 0, sizeof(input));
                    }
                } else if (dat == '@') {  // 금고가 해제된 상태에서 '@' 입력 시 잠금
                    handleLock(fd_serial, &safeUnlocked);
                }
            } else {  // 복구 모드가 활성화된 경우
                input[inputIndex++] = dat;  // 입력 데이터를 버퍼에 저장

                if (dat == '\n' || inputIndex >= (int)sizeof(input) - 1) {  // 엔터 입력 또는 버퍼 초과
                    input[inputIndex] = '\0';  // 문자열 종료

                    if (!validateInput(input, recoveryAnswer, RECOVERY_ANSWER_LENGTH)) {
                        pthread_mutex_lock(&data->mutex);
                        attempts++;
                        pthread_mutex_unlock(&data->mutex);
                        if (attempts >= MAX_ATTEMPTS) {
                            printf("Recovery attempts exceeded. Exiting program.\n");
                            serialWriteBytes(fd_serial, "비밀번호 입력 시도 초과. 프로그램을 종료합니다.\n");
                            exit(1);
                        }
                    } else {
                        attempts = 0;  // 복구 질문 성공 시 시도 횟수 초기화
                        recoveryMode = 0;
                    }

                    inputIndex = 0;  // 입력 버퍼 초기화
                    memset(input, 0, sizeof(input));
                }
            }
        }

        delay(10);  // CPU 사용량 줄이기 위해 대기
    }
}
