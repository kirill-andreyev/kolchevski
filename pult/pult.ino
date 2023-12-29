#include <esp_now.h>
#include <WiFi.h>
#include "GyverButton.h"
 
// ЗАМЕНИТЕ МАС-АДРЕСОМ ПЛАТЫ-ПОЛУЧАТЕЛЯ
uint8_t broadcastAddress[] = {0x48, 0x3F, 0xDA, 0x49, 0x2B, 0x88};
unsigned long timing;
const int axisX = 1;
const int axisY = 2;
const int axisZ = 3;
int upperButton = 7;
int middleButton = 8;
int lowerButton = 9;
int lowerLeftButton = 10;
const int blueLed = 6;
const int greenLed = 5;
 
GButton button1(upperButton);
GButton button2(middleButton);
GButton button3(lowerButton);
GButton button4(lowerLeftButton);

// Структура в скетче платы-отправителя
// должна совпадать с оной для получателя
typedef struct struct_message {
  int axisX;
  int axisY;
  int axisZ;
  int ledStatus;
} struct_message;
 
// Создаем структуру сообщения myData
struct_message myData;
 
// Обратная функция отправки
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    if (status == 0) 
  {
    analogWrite(greenLed, 20);
  }
  else 
  {
    analogWrite(greenLed, 0);
    analogWrite(blueLed, 20);
    delay(500);
    analogWrite(blueLed, 0);
    delay(500);
  }
}
 
void setup() {
  // Запускаем монитор порта
  Serial.begin(115200);
 pinMode(blueLed, OUTPUT);
 pinMode(greenLed, OUTPUT);
 pinMode(axisZ, INPUT_PULLUP);

  // Выбираем режим WiFi
  WiFi.mode(WIFI_STA);
 
  // Запускаем протокол ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
 
  // Регистрируем отправку сообщения
  esp_now_register_send_cb(OnDataSent);
  
  // Указываем получателя
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  // Указываем данные, которые будем отправлять
 myData.axisX = analogRead(axisX) / 1000;
 myData.axisY = analogRead(axisY) / 1000;
 myData.axisZ = 1 - digitalRead(axisZ);

button1.tick();
button2.tick();
button3.tick();
button4.tick();

 if (button1.isClick())
 {
     if (myData.ledStatus == 1)
     {
         myData.ledStatus = 0;
     }
     else
     {
         myData.ledStatus = 1;
     }
 }
 if (button2.isClick())
 {
     if (myData.ledStatus == 2)
     {
         myData.ledStatus = 0;
     }
     else
     {
         myData.ledStatus = 2;
     }
 }
 if (button3.isClick())
 {
     if (myData.ledStatus == 3)
     {
         myData.ledStatus = 0;
     }
     else
     {
         myData.ledStatus = 3;
     }
 }
 if (button4.isClick())
 {
     if (myData.ledStatus == 4)
     {
         myData.ledStatus = 0;
     }
     else
     {
         myData.ledStatus = 4;
     }
 }
 
  // Отправляем сообщение
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
}