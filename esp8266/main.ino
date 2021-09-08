#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Servo.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <MFRC522.h>
#include <cxg_JSTime.h>

Servo servo;

ESP8266WebServer web(80);
JSTime jsTime;
int servoStatus = 0;
const int RST_PIN = 5;     // Configurable, see typical pin layout above
const int SS_PIN = 4;     // Configurable, see typical pin layout above
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte keynuid[4] = {0, 0, 0, 0};
byte key2nuid[4] = {0, 0, 0, 0};

void opendoor() {4
  servoStatus = 1;
  servo.write(90);//舵机旋转180度，拉起门栓
  buzzersuccess();
  int timeId = jsTime.setTimeout([]() {
    servo.write(1);
    servoStatus = 0;
  }, 10000); //延迟10秒,舵机复位，放下门栓
  led_off();
}

void handleOpenDoor() {
  if(servoStatus == 0){
  web.send(200, "text/plain", "1");
  opendoor();
  }else{
  web.send(200, "text/plain", "2");
  }
}

void handleHeartbeat() {
  web.send(200, "text/plain", "1");
}

void handleindex() {
  web.send(200, "text/plain", "This is a unsmart door locker by Mion!");
}

void handleNotFound() {
  web.send(404, "text/plain", "Oops!Page is missing!");
}

void handleRestart() {
  web.send(200, "text/plain", "1");
  ESP.restart();
}

void handleLedoff() {
  web.send(200, "text/plain", "1");
  led_off();
  }

void buzzersuccess() {
  tone(2, 523,200);
  delay(200);
  noTone(2);
  tone(2, 587);
  delay(200);
  noTone(2);
  tone(2, 659);
  delay(200);
  noTone(2);
}

void buzzerfailed() {
  tone(2, 261);
  delay(200);
  noTone(2);
  delay(100);
  tone(2, 261);
  delay(200);
  noTone(2);
}

void printnuid(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], OCT);
  }
}

void led_off(){
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);

  //初始化舵机
  servo.attach(0, 500, 2500);
  servo.write(1);
  Serial.println("Servo initialized.");

  //连接网络

  WiFi.begin("SSID", "PASSWORD");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Scan wifi...");
    delay(1000);
  }
  Serial.println('\n');                     // WiFi连接成功后
  Serial.print("Connected to ");            // NodeMCU将通过串口监视器输出。
  Serial.println(WiFi.SSID());              // 连接的WiFI名称
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  //初始化WEB服务
  web.on("/", handleindex);
  web.on("/check", handleHeartbeat);
  web.on("/open", handleOpenDoor);
  web.on("/restart", handleRestart);
  web.on("/ledoff", handleLedoff);
  web.onNotFound(handleNotFound);
  web.begin();
  Serial.println("Webserver begain.");

  //初始化NFC
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  Serial.println("NFC module initialized.");
}

void loop() {
  jsTime.refresh();
  web.handleClient();

  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  if ( ! rfid.PICC_ReadCardSerial())
    return;

  if (rfid.uid.uidByte[0] == keynuid[0] ||
      rfid.uid.uidByte[1] == keynuid[1] ||
      rfid.uid.uidByte[2] == keynuid[2] ||
      rfid.uid.uidByte[3] == keynuid[3] ) {
    Serial.println("NFC card right.");
    opendoor();
  }else if (rfid.uid.uidByte[0] == key2nuid[0] ||
      rfid.uid.uidByte[1] == key2nuid[1] ||
      rfid.uid.uidByte[2] == key2nuid[2] ||
      rfid.uid.uidByte[3] == key2nuid[3] ) {
    Serial.println("NFC card right.");
    opendoor();
  }else {
    buzzerfailed();
    Serial.println("NFC card wrong.Card OTC NID:");
    printnuid(rfid.uid.uidByte, rfid.uid.size);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  led_off();
}
