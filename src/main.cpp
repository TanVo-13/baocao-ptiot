#include <WiFi.h>
#include "PubSubClient.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <ArduinoJson.h>

const char * MQTTServer = "broker.emqx.io";
const char * MQTT_Topic = "HeThongNhaThongMinh";

// Tạo ID ngẫu nhiên tại: https://www.guidgen.com/
const char * MQTT_ID = "c868b704-fd6b-44b0-8e69-b6da2c63496b";
int Port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

//Khai báo lcd
LiquidCrystal_I2C lcd(0x27, 20, 4); //Địa chỉ I2C phổ biến là 0x27 hoặc 0x3F

//Khai báo chân
//Trang
#define DHTPIN 23//Cảm biến dht22
#define DHTTYPE DHT22 // Loại cảm biến
DHT dht(DHTPIN, DHTTYPE);
#define khigas 35 //Cảm biến mq-2
//Tân
#define echoPin 25   //Cảm biến hcsr-04
#define trigPin 26
#define anhsang 32 //Cảm biến ldr
//Việt
#define doamdat 34 //Cảm biến độ ẩm đất (potentiometer)
#define mua 17 //Cảm biến mưa (button)

//Khai bán chân thiết bị
//Trang
#define dieuhoa 18 //Máy điều hòa (relay module)
#define coi 19 // Buzzer
//Việt
#define dirPin 2  // Motor tưới nước(Stepper)
#define stepPin 0
#define sleepPin 4
#define resetPin 16
#define maiche 15 // Servo
//Tân
#define ledPin 33 // Đèn
#define dirPin 27 // Motor bơm nước hồ bơi(Stepper)
#define stepPin 14
#define sleepPin 12
#define resetPin 13

void WIFIConnect() {
  Serial.println("Connecting to SSID: Wokwi-GUEST");
  WiFi.begin("Wokwi-GUEST", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected");
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

void MQTT_Reconnect() {
  while (!client.connected()) {
    if (client.connect(MQTT_ID)) {
      Serial.print("MQTT Topic: ");
      Serial.print(MQTT_Topic);
      Serial.print(" connected");
      client.subscribe(MQTT_Topic);
      Serial.println("");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  String stMessage;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    stMessage += (char)message[i];
  }
  Serial.println();
  if (stMessage == "on") {
    digitalWrite(ledPin, HIGH);
  }
  else if (stMessage == "off") {
    digitalWrite(ledPin, LOW);
  }
}

void setup() {
  Serial.begin(115200);

  // Khởi tạo LCD
  lcd.init(); 
  lcd.backlight(); 
  lcd.setCursor(0, 0);
  lcd.print("Dang ket noi WiFi");

  // Kết nối WiFi và MQTT
  WIFIConnect();
  client.setServer(MQTTServer, Port);
  client.setCallback(callback);

  // Khởi tạo các chân thiết bị và cảm biến

  // Trang
  dht.begin();
  pinMode(dieuhoa, OUTPUT);
  pinMode(coi, OUTPUT);

  // Tân
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(anhsang, INPUT);
  pinMode(ledPin, OUTPUT);

  pinMode(27, OUTPUT);  // dirPin - motor hồ bơi
  pinMode(14, OUTPUT);  // stepPin
  pinMode(12, OUTPUT);  // sleepPin
  pinMode(13, OUTPUT);  // resetPin

  // Việt
  pinMode(doamdat, INPUT);
  pinMode(mua, INPUT);
  pinMode(dirPin, OUTPUT);    // motor tưới cây
  pinMode(stepPin, OUTPUT);
  pinMode(sleepPin, OUTPUT);
  pinMode(resetPin, OUTPUT);
  pinMode(maiche, OUTPUT); // servo

  // Thông báo lên LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi da ket noi");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
}

void loop() {
  delay(10);
  if (!client.connected()) {
    MQTT_Reconnect();
  }
  client.loop();

  static unsigned long lastSend = 0;
  if (millis() - lastSend > 1000) {
    lastSend = millis();

    float nhietdo = dht.readTemperature();
    float doam = dht.readHumidity();
    int mq2_value = analogRead(khigas);
    int ldr_value = analogRead(anhsang);
    int doamdat_value = analogRead(doamdat);
    int trangthai_mua = digitalRead(mua); // 0: có mưa, 1: không mưa (nút nhấn)

    // Đo khoảng cách từ HC-SR04
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);
    float khoangcach = duration * 0.034 / 2;

    if (!isnan(nhietdo) && !isnan(doam)) {
      Serial.print("Nhiet do: "); Serial.print(nhietdo); Serial.print(" *C, ");
      Serial.print("Do am: "); Serial.print(doam); Serial.print(" %, ");
      Serial.print("Khi gas: "); Serial.print(mq2_value); Serial.print(", ");
      Serial.print("Cuong do anh sang: "); Serial.print(ldr_value); Serial.print(", ");
      Serial.print("Do am dat: "); Serial.print(doamdat_value); Serial.print(", ");
      Serial.print("Khoang cach: "); Serial.print(khoangcach); Serial.print(" cm, ");
      Serial.print("Mua: "); Serial.println(trangthai_mua);

      // Tạo JSON để gửi đi
      StaticJsonDocument<512> doc;
      doc["nhietdo"] = nhietdo;
      doc["doam"] = doam;
      doc["khigas"] = mq2_value;
      doc["anhsang"] = ldr_value;
      doc["doamdat"] = doamdat_value;
      doc["khoangcach"] = khoangcach;
      doc["mua"] = trangthai_mua;

      char buffer[256];
      serializeJson(doc, buffer);
      client.publish(MQTT_Topic, buffer);
    } else {
      Serial.println("Doc du lieu that bai!");
    }
  }
}
