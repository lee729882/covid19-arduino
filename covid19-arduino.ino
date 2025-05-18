#include <HTTPClient.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include "time.h"
#include <Wire.h>
#include "LiquidCrystal_I2C.h"
#define H_SECOND 3600

const char* ntpServer = "pool.ntp.org"; //사용할 ntpserver 주소
const char* ssid= "wifi_ssid"; //와이파이 아이디
const char* passwd= "wifi_password"; // 와이파이 비번


String Covid19_url="";

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup(){
  Serial.begin(115200);
  lcd.begin();
  lcd.backlight();

  //api 인증키
  String key="api_key";
  
// 코로나 19 감염현황 정보를 가져오는 서비스 요청 메세지
  Covid19_url="http://openapi.data.go.kr/openapi/service/rest/Covid19/getCovid19InfStateJson?pageNo=1&numOfRows=1&serviceKey=";
  Covid19_url+=key;

//와이파이 접속하기 
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
}
void loop(){
  
  // 코로나19 감염자 현황 가져오기
  struct tm timeinfo, temp,*yesterday,*before_yes; //시간 정보를 저장할 구조체
  String OnedayAgo_deside, TwodayAgo_deside;
  
  //configtime 함수를 통해 local 시간 정보를 가져온다.
  configTime(H_SECOND*9, 0, ntpServer);
  getLocalTime(&timeinfo);
  temp=timeinfo;
  time_t convert=mktime(&temp);
  yesterday=localtime(&convert);
  
  //하루전 누적 확진자 수를 가져오기
  String OnedayAgo=String(yesterday->tm_year+1900,DEC)+String(yesterday->tm_mon+1,DEC)+String(yesterday->tm_mday-1,DEC);
  String make_date="";
  Serial.println(OnedayAgo);
  make_date+="&starCreateDt=";
  make_date+=OnedayAgo;
  make_date+="&endCreateDt=";
  make_date+=OnedayAgo;
  HTTPClient http;
  http.begin(Covid19_url+make_date);
  int httpCode = http.GET();
  
  if(httpCode >0){ 
     if(httpCode == HTTP_CODE_OK){
      String payload = http.getString();
      OnedayAgo_deside=parse_xml("decideCnt",payload);
      Serial.println(OnedayAgo_deside);
  }
    else{
    Serial.println(httpCode);
  }
  }
      else{
       Serial.println(httpCode);
  }
  make_date="";
  temp=timeinfo;
  temp.tm_mday-=1;
  convert=mktime(&temp);
  before_yes=localtime(&convert);
  
  //이틀전 누적 확진자 수를 가져옴
  
  String TwodayAgo=String(before_yes->tm_year+1900,DEC)+String(before_yes->tm_mon+1,DEC)+String(before_yes->tm_mday-1,DEC);
  Serial.println(TwodayAgo);
  make_date+="&startCreateDt=";
  make_date+=TwodayAgo;
  make_date+="&endCreateDt=";
  make_date+=TwodayAgo;
  http.begin(Covid19_url+make_date);
  int httpCode = http.GET();
  
  if(httpCode>0){
    if(httpCode == HTTP_CODE_OK){
      String payload = http.getString();
      TwodayAgo_deside=parse_xml("decideCnt",payload);
      Serial.println(parse_xml("decideCnt",payload));
      lcd.setCursor(0,0);
      String covid = "Covid-19:"+String(OnedayAgo_deside.toInt()-TwodayAgo_deside.toInt(),DEC);
      lcd.print(covid.c_str());
  }
  else{
    Serial.println(httpCode);
  }
  }
  else{
    Serial.println(httpCode);
  }
  http.end();
  delay(3600000);
}

//xml을 string으로 변환
String parse_xml(String tag, String parse_str){
  String st_tag="<"+tag+">";
  String ed_tag="</"+tag+">";
  int st_index=parse_str.indexOf(st_tag);
  int ed_index=parse_str.indexOf(ed_tag);
  return parse_str.substring(st_index+st_tag.length(),ed_index);
  }
