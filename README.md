# 🔒 스마트 금고 시스템 (Smart Safe System)

## 📅 2024-2 임베디드 시스템 기말 프로젝트

---

## 📌 프로젝트 개요

스마트 금고 시스템은 **보안을 강화**하기 위해 설계된 임베디드 시스템 프로젝트입니다.

사용자는 **스위치를 눌러 블루투스 통신을 활성화**한 뒤, 스마트폰으로 금고를 제어할 수 있습니다.  
금고는 **올바른 비밀번호**가 입력되었을 때에만 잠금이 해제되며, 잘못된 비밀번호 입력 시 열리지 않습니다.

---

## 🌟 주요 기능

1. **스위치를 이용한 블루투스 연결**
   - 사용자가 **스위치를 누르면 블루투스 통신**이 활성화됩니다.

2. **블루투스 기반 비밀번호 입력**
   - 블루투스 모듈을 사용하여 사용자 스마트폰과 통신.
   - 스마트폰 앱으로 **비밀번호 입력**.

3. **비밀번호 인증**
   - 입력된 비밀번호를 **사전에 설정된 값**과 비교.
   - **비밀번호 일치** 시 금고 잠금 해제, 불일치 시 잠금 유지.

4. **잠금 장치 제어**
   - 서보 모터를 이용하여 금고의 잠금 상태를 **열거나 닫음**.

5. **LED 상태 표시**
   - 인증 성공 시 초록색 LED 점등.  
   - 인증 실패 시 빨간색 LED 점등.

---

## 🛠️ 설계 개요

### 🎛️ 시스템 구성
- **스위치**: 블루투스 통신 활성화
- **블루투스 모듈**: 사용자 스마트폰과의 데이터 통신
- **서보 모터**: 금고 잠금 장치 제어
- **LED**: 인증 상태 표시
- **가속도 센서**: 금고 이동 감지 (추가 보안)

---

## ⚙️ 하드웨어 구성

1. **라즈베리 파이 및 GPIO 핀 연결**
   - **스위치**  
     - GPIO 핀: **GPIO 24**
   - **블루투스 모듈**
     - RX: **GPIO 1**  
     - TX: **GPIO 0**
   - **서보 모터**
     - PWM 제어: **GPIO 12**
   - **LED 상태 표시**
     - 초록색 LED: **GPIO 19** (성공 시 점등)  
     - 빨간색 LED: **GPIO 12** (실패 시 점등)
   - **가속도 센서**
     - SDA: **GPIO 10**
     - SDO: **GPIO 9**
     - SDL: **GPIO 11**
     - CS: **GPIO 8**
    - **부저**
      - GPIO 핀: **GPIO 18**

---

## 💻 소프트웨어 구성

1. **스위치 입력 확인**
   - 사용자가 스위치를 누르면 블루투스 모듈이 활성화됩니다.

2. **블루투스 통신**
   - 스마트폰 앱을 통해 비밀번호를 입력받아 처리.

3. **비밀번호 인증**
   - 비밀번호 일치 여부를 판단:
     - **일치**: 서보 모터가 작동하여 금고 열림.  
     - **불일치**: 금고가 잠긴 상태 유지, 빨간색 LED 점등.

4. **LED 상태 표시**
   - 초록색 LED: 인증 성공 표시  
   - 빨간색 LED: 인증 실패 표시

5. **가속도 센서 활용**
   - 금고의 이동이 감지될 경우 경고음 발생.

---

### 로직 전체 설명:
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

### 시스템 흐름도:
![image](https://github.com/user-attachments/assets/2eec8462-f6b4-4956-a414-a8285fe98ec8)


---

## 🎥 프로젝트 동영상 및 설계 사진

- **설계 사진**  
  아래는 하드웨어 설계 및 시스템 구성 사진입니다:  
  

- **프로젝트 시연 동영상**  
  시연 동영상을 보려면 아래 링크를 클릭하세요:  
  
---


> 💡 _스마트 금고 시스템은 안전성과 편리함을 모두 제공하기 위해 설계되었습니다.  
> 프로젝트에 대한 의견 및 피드백은 언제든지 환영합니다!_
