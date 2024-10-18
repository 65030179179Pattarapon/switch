#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "vvenom01";
const char* password = "pomnew01";
const String apiKey = "htM5g1RLt0iY4hIPjj2vbZd2PYPonEsovsrScZZOZBepvv66gIlGamiVEqAupRS9zd1TjaxejxBTgFTLMbZBJWdYq2NzncfGk2B0hpyl5e9kDZvjGkq2ZkWiB6UoJ3px";
const String serverURL = "http://192.168.137.247:3305/sensor-data";

const int buttonPin = 17;        // กำหนดขา GPIO ที่เชื่อมต่อกับปุ่มกด
int buttonState = 0;             // ตัวแปรเก็บสถานะของปุ่ม
int lastButtonState = HIGH;      // ตัวแปรเก็บสถานะล่าสุด
int count = 0;                   // ตัวแปรนับจำนวนการกดปุ่ม
unsigned long buttonPressTime = 0; // ตัวแปรเก็บเวลาเมื่อปุ่มถูกกดค้าง
bool buttonPressed = false;      // เช็คสถานะการกดปุ่ม

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);

  // เชื่อมต่อ WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  buttonState = digitalRead(buttonPin);
  
  // ตรวจสอบว่าปุ่มถูกกด (การเปลี่ยนสถานะจาก HIGH ไป LOW)
  if (buttonState == LOW && lastButtonState == HIGH) {
    count++; // เพิ่มจำนวนการกดปุ่ม
    Serial.print("Button pressed: ");
    Serial.println(count);
    sendData(count);  // ส่งข้อมูลเมื่อมีการกดปุ่ม
    delay(200);  // ป้องกันการอ่านซ้ำเร็วเกินไป
    buttonPressed = true;  // ตั้งค่าการกดปุ่ม
    buttonPressTime = millis();  // บันทึกเวลาเมื่อปุ่มถูกกด
  }

  // ตรวจสอบว่าปุ่มถูกกดค้างไว้
  if (buttonPressed && buttonState == LOW) {
    // ถ้ากดปุ่มค้างไว้นานกว่า 5 วินาที (5000 ms)
    if (millis() - buttonPressTime >= 5000) {
      count = 0; // รีเซ็ตค่าการนับ
      Serial.println("Count reset to 0");
      sendData(count); // ส่งข้อมูลการรีเซ็ตไปยังเซิร์ฟเวอร์
      delay(500);  // รอให้สัญญาณแสดงผล
    }
  }

  // รีเซ็ตสถานะเมื่อปล่อยปุ่ม
  if (buttonState == HIGH) {
    buttonPressed = false;  // รีเซ็ตสถานะการกดปุ่ม
  }

  lastButtonState = buttonState;  // เก็บสถานะล่าสุด
}

void sendData(int count) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL + "?api_key=" + apiKey + "&count=" + String(count));
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.println("Error on sending data");
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
