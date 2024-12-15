# 🔒 스마트 금고 시스템 (Smart Safe System)

## 2024-2 임베디드 시스템 02분반 6조

#### 20190362 김희겸
#### 20200888 이재우
#### 20220242 김아연
#### 20220625 신채연


## 📌 1. 프로젝트 개요

스마트 금고 시스템은 **보안을 강화**하기 위해 설계된 임베디드 시스템 프로젝트입니다.

사용자는 **스위치를 눌러 블루투스 통신을 활성화**한 뒤, 스마트폰으로 금고를 제어할 수 있습니다.  
금고는 **올바른 비밀번호**가 입력되었을 때에만 잠금이 해제되며, 잘못된 비밀번호 입력 시 열리지 않습니다.

<여기 금고 전체 사진 넣기>


## 🌟 2. 주요 기능

- **스위치를 이용한 블루투스 연결**  
  - 사용자가 **스위치를 누르면 블루투스 통신**이 활성화됩니다.

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

### 🎛️ 시스템 구성
- **스위치**: 블루투스 통신 활성화
- **블루투스 모듈**: 사용자 스마트폰과의 데이터 통신
- **서보 모터**: 금고 잠금 장치 제어
- **LED**: 인증 상태 표시
- **가속도 센서**: 금고 이동 감지 (추가 보안)


## ⚙️ 4. 하드웨어 구성

**라즈베리 파이 및 GPIO 핀 연결**
   - **스위치**  
     - GPIO 핀: **GPIO 24**
   - **블루투스 모듈**
     - RX: **GPIO 1**  
     - TX: **GPIO 0**
   - **서보 모터**
     - PWM 제어: **GPIO 12**
   - **LED 상태 표시**
     - 초록색 LED: **GPIO 23** (해제 시 점등)  
     - 빨간색 LED: **GPIO 16** (잠금 시 점등)
   - **가속도 센서**
     - SDA: **GPIO 10**
     - SDO: **GPIO 9**
     - SDL: **GPIO 11**
     - CS: **GPIO 8**
    - **부저**
      - GPIO 핀: **GPIO 18**


## 💻 5. 소프트웨어 구성

### (1) SPI 기반 센서 제어
- **WiringPi 라이브러리 사용**  
  - SPI 통신 초기화 및 데이터를 주고받습니다.  
  - ADXL345 센서와 SPI 프로토콜로 연결되어 센서 데이터를 읽고 해석합니다.  

- **주요 함수**  
  - **레지스터 조작**  
    - `writeRegister_ADXL345()`: 특정 레지스터에 데이터 기록.  
    - `readRegister_ADXL345()`: 특정 레지스터에서 데이터 읽기.  
  - **데이터 읽기**  
    - `readAccelerometerData()`: 가속도 데이터를 읽어 반환.


### (2) 스레드 구조
- **센서 스레드(sensorThread)**  
  - ADXL345 센서에서 데이터를 지속적으로 읽어 특정 값 이상의 움직임이 발생하면 **부저 활성 플래그**를 설정합니다.  

- **부저 스레드(buzzerThread)**  
  - 부저 활성 플래그 상태를 확인하여 **부저를 소프트웨어 PWM 방식**으로 울립니다.  
  - 경고음 발생 시 LED 상태도 갱신됩니다.


### (3) 금고 잠금/해제
- **비밀번호 입력 및 검증**  
  - 사용자가 **UART**를 통해 입력한 비밀번호를 검증합니다.  
  - 비밀번호 검증 함수: `checkPassword()`  
    - 일치 시: 서보 모터를 제어하여 금고 잠금 해제.  
    - 불일치 시: 잠금 유지 및 실패 횟수 증가.

- **서보 모터 제어**  
  - 서보 모터 작동 함수: `rotate_Servo()`  
  - 비밀번호 인증 성공 시 금고를 엽니다.

- **비밀번호 실패 관리**  
  - 잘못된 비밀번호 입력 횟수가 초과되면 **복구 질문**으로 전환합니다.


### (4) 비밀번호 복구
- **복구 질문 처리**  
  - 사용자에게 복구 질문을 제시하며, 정답 입력 시 시도 횟수가 초기화됩니다.  
  - 복구 질문의 정답 입력 실패 시 시스템을 종료합니다.


### (5) LED 상태 및 경고음
- **LED 상태 표시**  
  - 초록색 LED: 인증 성공 및 금고 잠금 해제 표시.  
  - 빨간색 LED: 인증 실패 및 잠금 유지 표시.  
  - 움직임 감지 시 경고 LED 점등.

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


## 🎥 7. 프로젝트 동영상 및 설계 사진

- **설계 사진**  
![image](https://github.com/user-attachments/assets/0f0c3a2d-b5da-4e7c-aabe-8555c8d7e592)


- **프로젝트 시연 동영상**  
  시연 동영상을 보려면 아래 링크를 클릭하세요:  
  📺 링크 삽입



> 💡 프로젝트에 대한 의견 및 피드백은 언제든지 환영합니다!.._
