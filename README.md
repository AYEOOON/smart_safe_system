# 🔒 스마트 금고 시스템 (Smart Safe System)

## 2024-2 임베디드 시스템 02분반 6조

#### 팀원
- **20190362 김희겸** 👴  
- **20200888 이재우** 🏋️‍♀️  
- **20220242 김아연** 🧙‍♂️  
- **20220625 신채연** 🕵️‍♂️  

## 🧾 목차
1. [📌 1. 프로젝트 개요](#1.-프로젝트-개요)  
   - [목표](#목표)  
   - [금고 전체 사진](#금고-전체-사진)  
2. [ 🌟 2. 주요 기능](#2.-주요-기능)  
3. [🛠️ 3. 설계 개요](#3.-설계-개요)
   - [설계 사진](#설계-사진)
   - [시스템 구성](#시스템-구성)   
4. [⚙️ 4. 하드웨어 구성](#4.-하드웨어-구성)  
   - [GPIO 핀 매핑](#gpio-핀-매핑)  
   - [구성도](#구성도)  
6. [소프트웨어 구성](#소프트웨어-구성)  
   - [SPI 기반 센서 제어](#spi-기반-센서-제어)  
   - [멀티스레드 구조](#멀티스레드-구조)  
   - [금고 잠금/해제](#금고-잠금해제)  
   - [비밀번호 복구](#비밀번호-복구)  
   - [LED 상태 및 경고음](#led-상태-및-경고음)  
   - [전체 로직 설명](#전체-로직-설명)  
7. [프로젝트 시연 동영상](#프로젝트-시연-동영상)  
   - [잠금 해제 시나리오 1](#잠금-해제-시나리오-1)  
   - [잠금 해제 시나리오 2](#잠금-해제-시나리오-2)  
   - [복구 질문 실패 시나리오](#복구-질문-실패-시나리오)  
   - [복구 실패 후 추가 잠금](#복구-실패-후-추가-잠금)  
   - [금고 도난 시나리오](#금고-도난-시나리오)  


## 📌 1. 프로젝트 개요

**스마트 금고 시스템**은 보안을 강화하고 사용자 편의성을 높이기 위해 설계된 임베디드 시스템 프로젝트입니다.  

### 🎯 **목표**  
- 사용자가 **스마트폰과 블루투스 통신을 통해 금고를 제어**할 수 있도록 하여, 간단하고 효율적인 보안 경험을 제공  
- 비밀번호 인증, 복구 질문, 움직임 감지 등의 기능을 통해 **보안성을 극대화**

### 📸 **금고 전체 사진**
<여기 금고 전체 사진 넣기>


## 🌟 2. 주요 기능

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


## 🛠️ 3. 설계 개요
### 🔍 설계 사진
![image](https://github.com/user-attachments/assets/0f0c3a2d-b5da-4e7c-aabe-8555c8d7e592)


### 🎛️ 시스템 구성
- **라즈베리 파이**: 시스템의 중앙 제어 역할 수행.  
- **스위치**: 블루투스 연결 활성화.  
- **블루투스 모듈 (HC-05)**: 스마트폰과의 데이터 통신.  
- **서보 모터**: 금고 잠금 장치 제어.  
- **LED**: 인증 상태 및 시스템 상태 표시.  
- **가속도 센서 (ADXL345)**: 금고 이동 및 충격 감지.  
- **부저**: 경고 알림 출력.  


## ⚙️ 4. 하드웨어 구성

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



## 💻 5. 소프트웨어 구성

### (1) SPI 기반 센서 제어
- **WiringPi 라이브러리 사용**  
  - SPI 통신 초기화 및 데이터를 주고받습니다.  
  - ADXL345 센서와 SPI 프로토콜로 연결되어 센서 데이터를 읽고 해석.  

- **주요 함수**  
  - **레지스터 조작**  
    - `writeRegister_ADXL345()`: 특정 레지스터에 데이터 기록.  
    - `readRegister_ADXL345()`: 특정 레지스터에서 데이터 읽기.  
  - **데이터 읽기**  
    - `readAccelerometerData()`: 가속도 데이터를 읽어 반환.


### (2) 스레드 구조
- **센서 스레드**: 가속도 데이터를 지속적으로 모니터링.  
  - 특정 조건 이상 움직임 발생 시 **부저 플래그**를 활성화.  
- **부저 스레드**: 부저 플래그 확인 후 경고음을 발생.  
- **비밀번호 인증 스레드**: 스마트폰 앱으로부터 입력받은 비밀번호를 검증.  


### (3) 금고 잠금/해제
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


### (4) 비밀번호 복구
- **복구 질문 처리**  
  - 사용자에게 복구 질문을 제시하며, 정답 입력 시 시도 횟수가 초기화.  
  - 복구 질문의 정답 입력 실패 시 시스템을 종료.


### (5) LED 상태 및 경고음
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


## 🎥 7. 프로젝트 시연 동영상
- **프로젝트 시연 동영상**  
  - 잠금 해제 1
    - 첫 시도에 잠금 해제하는 경우
      
https://github.com/user-attachments/assets/a1bf8d51-70a8-48e2-bc75-a86c381aa04c

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
