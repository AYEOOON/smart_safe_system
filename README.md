# 🔒 스마트 금고 시스템 (Smart Safe System)

## 2024-2 임베디드 시스템 02분반 6조

#### 팀원
- **20190362 김희겸** 
- **20200888 이재우**   
- **20220242 김아연** 
- **20220625 신채연**

### 🧾 목차
1. [프로젝트 개요](#-1-프로젝트-개요)  
2. [주요 기능](#-2-주요-기능)  
3. [설계 개요](#%EF%B8%8F-3-설계-개요)  
4. [하드웨어 구성](#%EF%B8%8F-4-하드웨어-구성)  
5. [소프트웨어 구성](#-5-소프트웨어-구성)  
6. [로직 전체 설명](#-6-로직-전체-설명)  
7. [코드 구조 및 설명](#-7-코드-구조-및-설명)  
8. [프로젝트 시연 동영상](#-8-프로젝트-시연-동영상)   


### 📌 1. 프로젝트 개요

**스마트 금고 시스템**은 보안을 강화하고 사용자 편의성을 높이기 위해 설계된 임베디드 시스템 프로젝트입니다.  

#### **목표**  
- 사용자가 **스마트폰과 블루투스 통신을 통해 금고를 제어**할 수 있도록 하여, 간단하고 효율적인 보안 경험을 제공  
- 비밀번호 인증, 복구 질문, 움직임 감지 등의 기능을 통해 **보안성을 극대화**

#### **금고 전체 사진**
<img src="https://github.com/user-attachments/assets/11a4bfbf-36cb-4f68-bb08-fe2825013a4e" width="400" />


### 🌟 2. 주요 기능

- **스위치를 이용한 블루투스 연결**  
  - 사용자가 **스위치를 누르면 블루투스 통신**이 활성화.

- **블루투스 기반 비밀번호 입력**  
  - 블루투스 모듈을 사용하여 사용자 스마트폰과 통신.  
  - 스마트폰 앱으로 **비밀번호 입력**.

- **비밀번호 인증**  
  - 입력된 비밀번호를 **사전에 설정된 값**과 비교.  
  - **비밀번호 일치** 시 금고 잠금 해제, 불일치 시 잠금 유지.

- **잠금 장치 제어**  
  - 서보 모터를 이용하여 금고의 잠금 상태를 **열거나 닫음**.

- **LED 상태 표시**  
  - 해제 시 초록색 LED 점등.  
  - 잠금 시 빨간색 LED 점등.

- **가속도 센서 활용**  
  - 금고의 움직임 또는 충격 감지 시 경고음 발생.


### 🛠️ 3. 설계 개요
#### 설계 사진
![image](https://github.com/user-attachments/assets/0f0c3a2d-b5da-4e7c-aabe-8555c8d7e592)


#### 시스템 구성
- **라즈베리 파이**: 시스템의 중앙 제어 역할 수행.  
- **스위치**: 블루투스 연결 활성화.  
- **블루투스 모듈 (HC-05)**: 스마트폰과의 데이터 통신.  
- **서보 모터**: 금고 잠금 장치 제어.  
- **LED**: 인증 상태 및 시스템 상태 표시.  
- **가속도 센서 (ADXL345)**: 금고 이동 및 충격 감지.  
- **부저**: 경고 알림 출력.  


### ⚙️ 4. 하드웨어 구성
### 전체 회로도
<img src="https://github.com/user-attachments/assets/ce25c9fe-ccd8-4521-aaa5-56a0608f9697" width="700" />


**라즈베리 파이 및 GPIO 핀 연결**
| 구성 요소           | 역할 및 상태                 | GPIO 핀  |
|--------------------|-------------------------|----------|
| **스위치**         | 블루투스 통신 활성화           | GPIO 24  |
| **블루투스 모듈**    | RX                      | GPIO 1   |
|                    | TX                      | GPIO 0   |
| **서보 모터**       | PWM 제어                 | GPIO 12  |
| **LED 상태 표시**    | 초록색 LED (해제 시 점등)     | GPIO 23  |
|                    | 빨간색 LED (잠금 시 점등)     | GPIO 16  |
| **가속도 센서**     | SDA                     | GPIO 10  |
|                    | SDO                     | GPIO 9   |
|                    | SDL                     | GPIO 11  |
|                    | CS                      | GPIO 8   |
| **부저**           | 경고음 출력                 | GPIO 18  |



### 💻 5. 소프트웨어 구성

#### (1) SPI 기반 센서 제어
- **WiringPi 라이브러리 사용**  
  - SPI 통신 초기화 및 데이터를 주고받습니다.  
  - ADXL345 센서와 SPI 프로토콜로 연결되어 센서 데이터를 읽고 해석.  

- **주요 함수**  
  - **레지스터 조작**  
    - `writeRegister_ADXL345()`: 특정 레지스터에 데이터 기록.  
    - `readRegister_ADXL345()`: 특정 레지스터에서 데이터 읽기.  
  - **데이터 읽기**  
    - `readAccelerometerData()`: 가속도 데이터를 읽어 반환.


#### (2) 스레드 구조
- **센서 스레드**: 가속도 데이터를 지속적으로 모니터링.  
  - 특정 조건 이상 움직임 발생 시 **부저 플래그**를 활성화.  
- **부저 스레드**: 부저 플래그 확인 후 경고음을 발생.  
- **비밀번호 인증 스레드**: 스마트폰 앱으로부터 입력받은 비밀번호를 검증.  


#### (3) 금고 잠금/해제
- **비밀번호 입력 및 검증**  
  - 사용자가 **UART**를 통해 입력한 비밀번호를 검증.  
  - 비밀번호 검증 함수: `checkPassword()`  
    - 일치 시: 서보 모터를 제어하여 금고 잠금 해제.  
    - 불일치 시: 잠금 유지 및 실패 횟수 증가.

- **서보 모터 제어**  
  - 서보 모터 작동 함수: `rotate_Servo()`  
  - 비밀번호 인증 성공 시 금고를 엽니다.

- **비밀번호 실패 관리**  
  - 잘못된 비밀번호 입력 횟수가 초과되면 **복구 질문**으로 전환.


#### (4) 비밀번호 복구
- **복구 질문 처리**  
  - 사용자에게 복구 질문을 제시하며, 정답 입력 시 시도 횟수가 초기화.  
  - 복구 질문의 정답 입력 실패 시 시스템을 종료.


#### (5) LED 상태 및 경고음
- **LED 상태 표시**  
  - 초록색 LED: 인증 성공 및 금고 잠금 해제 표시.  
  - 빨간색 LED: 인증 실패 및 잠금 유지 표시.


- **경고음 발생**  
  - 금고가 움직이거나 충격이 발생하면 부저를 울려 경고음을 출력.  
  - 소프트웨어 PWM 방식으로 구현되어 유연한 제어 가능.


### 🧩 6. 로직 전체 설명:
![image](https://github.com/user-attachments/assets/36a39a92-2c1c-4ad0-b660-2f633faeff19)

1. **사용자 접근 감지 및 블루투스 연결**:
   - 스위치를 눌러 블루투스 연결을 시도하고, 성공 시 비밀번호 입력 요청.

2. **비밀번호 인증**:
   - 비밀번호가 일치하면 금고 잠금 해제 및 성공 LED 점등.
   - 불일치 시 실패 횟수를 기록하며, 3회 초과 시 검증 질문 요청.

3. **검증 질문**:
   - 검증 질문에 응답이 정확하면 비밀번호 재입력 요청.
   - 응답이 틀리면 시스템 잠금 및 부저 울림.

4. **금고 움직임 감지**:
   - 금고 이동 또는 충격 시 부저 울림 및 경고 LED 점등.
   - 이상이 없으면 정상 대기 상태 유지.

### ⚓ 7. 코드 구조 및 설명
스마트 금고 시스템의 코드는 기능별로 나뉘어 있으며, 주요 구성 요소는 다음과 같습니다:


#### 1. SPI 초기화 및 GPIO 설정

**설명**  
라즈베리 파이에서 SPI 통신 및 GPIO 핀을 초기화하여 하드웨어와 통신 가능하게 설정합니다.

**주요 기능**
- SPI 초기화
- GPIO 핀 모드 설정
- 초기 상태 설정

```c
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
```



#### 2. ADXL345 센서 제어

**설명**  
가속도 센서(ADXL345)와 SPI를 통해 통신하여 데이터를 읽고, 금고 움직임을 감지합니다.

**주요 기능**  
센서 레지스터 쓰기/읽기
가속도 데이터 읽기
움직임 값 확인

**(1) 레지스터 쓰기 및 읽기***
```c
void writeRegister_ADXL345(char address, char value) {
    unsigned char buff[2] = { address, value };
    digitalWrite(CS_GPIO, LOW);
    wiringPiSPIDataRW(SPI_CH, buff, 2);
    digitalWrite(CS_GPIO, HIGH);
}

void readRegister_ADXL345(char registerAddress, int numBytes, char *values) {
    values[0] = 0x80 | registerAddress;
    if (numBytes > 1) values[0] |= 0x40;

    digitalWrite(CS_GPIO, LOW);
    wiringPiSPIDataRW(SPI_CH, values, numBytes + 1);
    digitalWrite(CS_GPIO, HIGH);
}
```

**(2) 가속도 데이터 읽기***
```c
void readAccelerometerData(short *x, short *y, short *z) {
    unsigned char buffer[7] = {0};
    readRegister_ADXL345(DATAX0, 6, buffer);

    *x = ((short)buffer[2] << 8) | (short)buffer[1];
    *y = ((short)buffer[4] << 8) | (short)buffer[3];
    *z = ((short)buffer[6] << 8) | (short)buffer[5];
}
```



#### 3. ADXL345 센서 제어

**설명**  
가속도 센서와 SPI 통신을 통해 데이터를 읽고 금고의 움직임을 감지합니다.

**주요 코드**  

**(1) 레지스터 쓰기 및 읽기***
```c
void writeRegister_ADXL345(char address, char value) {
    unsigned char buff[2] = { address, value };
    digitalWrite(CS_GPIO, LOW);
    wiringPiSPIDataRW(SPI_CH, buff, 2);
    digitalWrite(CS_GPIO, HIGH);
}

void readRegister_ADXL345(char registerAddress, int numBytes, char *values) {
    values[0] = 0x80 | registerAddress;
    if (numBytes > 1) values[0] |= 0x40;

    digitalWrite(CS_GPIO, LOW);
    wiringPiSPIDataRW(SPI_CH, values, numBytes + 1);
    digitalWrite(CS_GPIO, HIGH);
}
```

#### (2) 가속도 데이터 읽기
```c
void readAccelerometerData(short *x, short *y, short *z) {
    unsigned char buffer[7] = {0};
    readRegister_ADXL345(DATAX0, 6, buffer);

    *x = ((short)buffer[2] << 8) | (short)buffer[1];
    *y = ((short)buffer[4] << 8) | (short)buffer[3];
    *z = ((short)buffer[6] << 8) | (short)buffer[5];
}
```



#### 4. 부저 제어  
**설명**
부저를 통해 경고음을 발생시키며, 소프트웨어 PWM을 사용하여 주파수를 제어합니다.

**주요 코드**
```c
void triggerBuzzer(int durationMs) {
    digitalWrite(BUZZER_GPIO, HIGH);
    delay(durationMs);
    digitalWrite(BUZZER_GPIO, LOW);
}

void triggerBuzzerPWM(int durationMs, int frequency) {
    softToneCreate(BUZZER_GPIO);
    softToneWrite(BUZZER_GPIO, frequency);
    delay(durationMs);
    softToneWrite(BUZZER_GPIO, 0);
}
```



#### 5. 멀티스레드 구성
**설명**  
스레드를 사용하여 센서 데이터를 읽고, 부저를 제어하는 작업을 병렬로 처리합니다.

**주요 코드**  
**(1) 센서 스레드**
```c
void *sensorThread(void *arg) {
    SharedData *data = (SharedData *)arg;
    short prevX = 1, prevY = 50, prevZ = 118;
    while (data->running) {
        short x, y, z;
        readAccelerometerData(&x, &y, &z);

        if (abs(x - prevX) > 100 || abs(y - prevY) > 100 || abs(z - prevZ) > 100) {
            data->triggerBuzzer = 1;
        }

        pthread_mutex_lock(&data->mutex);
        data->x = x;
        data->y = y;
        data->z = z;
        pthread_mutex_unlock(&data->mutex);

        prevX = x;
        prevY = y;
        prevZ = z;

        usleep(50000);
    }
    return NULL;
}
```


**(2) 부저 스레드**
```c
void *buzzerThread(void *arg) {
    SharedData *data = (SharedData *)arg;
    const int duration = 800;
    const int frequency = 3000;

    while (data->running) {
        pthread_mutex_lock(&data->mutex);
        if (data->triggerBuzzer) {
            data->triggerBuzzer = 0;
            pthread_mutex_unlock(&data->mutex);
            triggerBuzzerPWM(duration, frequency);
        } else {
            pthread_mutex_unlock(&data->mutex);
        }
        usleep(100000);
    }
    return NULL;
}
```



#### 6. 공유 데이터 구조  
**설명**  
스레드 간 데이터 공유 및 동기화를 위한 구조체를 정의합니다.

**주요 코드**  
```c
typedef struct {
    short x, y, z;
    int triggerBuzzer;
    int running;
    pthread_mutex_t mutex;
} SharedData;
```



#### 7. 메인 함수  
**설명**  
시스템 초기화 및 스레드 실행을 통해 전체 금고 동작을 관리합니다.

**주요 코드**  
```c
int main() {
    SharedData data = { .x = 0, .y = 0, .z = 0, .triggerBuzzer = 0, .running = 1 };
    pthread_mutex_init(&data.mutex, NULL);

    if (initSPI() == -1) return -1;

    pthread_t sensorThreadId, buzzerThreadId;
    pthread_create(&sensorThreadId, NULL, sensorThread, &data);
    pthread_create(&buzzerThreadId, NULL, buzzerThread, &data);

    printf("Press ENTER to exit.\n");
    getchar();

    data.running = 0;
    pthread_join(sensorThreadId, NULL);
    pthread_join(buzzerThreadId, NULL);
    pthread_mutex_destroy(&data.mutex);

    return 0;
}
```



### 🎥 8. 프로젝트 시연 동영상
- **프로젝트 시연 동영상**  
  - 잠금 해제 1
    - 첫 시도에 잠금 해제하는 경우

https://github.com/user-attachments/assets/4c12469d-2c60-4fe5-bf89-9a35286dfe9b

<img src="https://github.com/user-attachments/assets/0559d63c-82b9-4512-b933-910b839538fe" width="300" />

---

  - 잠금 해제 2
    - 복구 질문 인증 후 잠금 해제하는 경우

https://github.com/user-attachments/assets/4e36a09b-10d6-4e73-bd57-1084b098775e

<img src="https://github.com/user-attachments/assets/48a2db38-89c6-4133-862c-e30efb08b104" width="300" />

---

  - 복구 질문에 대한 답이 틀린 경우
    
https://github.com/user-attachments/assets/6d1f785f-994d-4a8c-b0b2-6ee003f67b2a

<img src="https://github.com/user-attachments/assets/b7b8bf47-e589-4b8c-998d-a52317af92e3" width="300" />

---

  - 복구 질문을 통과한 후 3회 틀릴 경우

https://github.com/user-attachments/assets/9ac86663-bbeb-430d-9497-d364cd326788

<img src="https://github.com/user-attachments/assets/effe9537-e587-473a-8608-593d7095ec35" width="300" />

---

  - 금고를 도둑질 하려는 경우

https://github.com/user-attachments/assets/ce64c342-890e-40ff-9b80-c0cc89e04f33



---

> 💡 프로젝트에 대한 의견 및 피드백은 언제든지 환영합니다!.._
