# 🌡️ ESP32 기반 코로나19 실시간 알리미 (IoT Project)

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![ESP32](https://img.shields.io/badge/ESP32-323330?style=for-the-badge&logo=espressif&logoColor=white)
![Communication](https://img.shields.io/badge/I2C-Communication-orange?style=for-the-badge)

> **"손을 대면 오늘의 코로나 현황을 알려줍니다."** > 공공데이터포털 API와 온도 센서를 활용한 실시간 정보 시각화 IoT 디바이스

<br/>

## 📸 프로젝트 시연
<img src="https://github.com/lee729882/covid19-arduino/assets/..." width="600">

<br/>

## 🛠 시스템 구조 (Architecture)

```mermaid
graph TD;
    A[User Touch] -->|NTC Sensor| B(ESP32 MCU);
    B -->|WiFi Request| C{Open API Server};
    C -->|XML Data| B;
    B -->|Parsing Logic| D[Calc: Today - Yesterday];
    D -->|I2C Protocol| E[LCD Display];

📌 주요 기능 (Key Features)인터랙티브 트리거: 상시 켜져 있는 것이 아니라, NTC 온도 센서에 체온이 감지될 때만 데이터를 요청하여 전력 효율 증대실시간 데이터 파싱: 임베디드 환경(C언어)에서 무거운 XML 라이브러리 없이 스트림 방식의 경량 파싱 구현자동 날짜 계산: NTP 서버 동기화를 통해 별도의 RTC 모듈 없이도 '오늘'과 '어제'의 날짜를 자동으로 계산신규 확진자 산출: API가 누적 데이터만 제공하는 점을 해결하기 위해 (금일 누적 - 전일 누적) 차분 알고리즘 적용⚡ 기술적 도전 및 해결 (Troubleshooting)1. 하드웨어 제약 극복 (Uno → ESP32)Issue: 초기 기획된 Arduino Uno는 네트워크 기능 부재로 실시간 API 통신 불가Solution: WiFi 및 Bluetooth가 내장된 ESP32로 마이그레이션 진행. 더 높은 메모리 용량을 활용하여 데이터 처리 버퍼 확보.2. XML 데이터 파싱 최적화Issue: API 응답(XML)이 길어 전체를 String에 저장할 경우 힙 메모리 부족(Heap Fragmentation) 발생 가능성Solution: 필요한 태그(<decideCnt>)의 위치만 인덱싱하여 값을 추출하는 함수 직접 구현C++// 실제 구현 코드 중 XML 파싱 부분
String parse_xml(String tag, String parse_str) {
  String st_tag = "<" + tag + ">";
  String ed_tag = "</" + tag + ">";
  
  int st_index = parse_str.indexOf(st_tag);
  int ed_index = parse_str.indexOf(ed_tag);
  
  // 태그 사이의 값만 안전하게 추출
  return parse_str.substring(st_index + st_tag.length(), ed_index);
}
⚙️ 하드웨어 구성 (Bill of Materials)ComponentDescriptionNoteESP32 Dev ModuleMain ControllerWiFi Built-inI2C LCD (16x2)DisplayAddress: 0x27 or 0x3FNTC ThermistorTemperature Sensor10k OhmResistor10k OhmFor Voltage Divider🚀 설치 및 실행 (How to Run)라이브러리 설치: Arduino IDE 라이브러리 매니저에서 LiquidCrystal_I2C 설치API 키 발급: 공공데이터포털에서 [보건복지부 코로나19 감염현황] 활용 신청설정 변경: covid19-arduino.ino 파일 내 WiFi 정보 및 API Key 수정C++const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
String ServiceKey = "YOUR_API_KEY";
