#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Servo.h>
#include <FastLED.h>

#define LED_PIN     14

// Information about the LED strip itself
#define NUM_LEDS    264
#define CHIPSET     WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define BRIGHTNESS  50
#define TEMPERATURE_1 Tungsten100W
#define TEMPERATURE_2 OvercastSky

// How many seconds to show each temperature before switching
#define DISPLAYTIME 20
// How many seconds to show black between switches
#define BLACKTIME   3

Servo s1;
Servo s2;
unsigned long timing;
int servoXposition = 90;
int servoYposition = 90;

// Structure to hold the received message
typedef struct struct_message {
  int axisX;
  int axisY;
  int axisZ;
  int ledStatus;
} struct_message;

// Create an instance of the struct_message
struct_message myData;

// Callback function to handle received data
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  // Copy the incoming data to the myData structure
  memcpy(&myData, incomingData, sizeof(myData));
  // Print the received message
}

void setup() {
  delay( 3000 ); // power-up safety delay


  // It's important to set the color correction for your LED strip here,
  // so that colors can be more accurately rendered through the 'temperature' profiles
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness( BRIGHTNESS );

  //Initialize the serial monitor
  //s1.attach(4);
  //s2.attach(5);
  myData.axisX = 5;
  myData.axisY = 5;

  // Set WiFi mode to Station mode
  WiFi.mode(WIFI_STA);
 
  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set the role of this device as a slave
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
 
  // Register callback for received data
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() 
{
  
  if(millis() - timing <=20) {
    return;
  }
  timing = millis();

  if(myData.axisX < 5 && servoXposition > 45) {
      servoXposition -= 1;
  }
  else if(myData.axisX > 5 && servoXposition < 135)
  {
      servoXposition += 1;
  }
    
    
  if(myData.axisY < 5 && servoYposition > 45)
  {
    servoYposition -= 1;
  }
  else if(myData.axisY > 5 && servoYposition < 135)
  {
    servoYposition += 1;
  }
    
  if(myData.axisZ == 1)
  {
    servoXposition = 90;
    servoYposition = 90;
  }

  //s1.write(servoXposition);
  //s2.write(servoYposition);

  if (myData.ledStatus == 1)
  {
      for (int i = 0; i < NUM_LEDS; i++)
      {
          leds[i].setRGB(0, 0, 255);
      }
      FastLED.show();
  }
  else if (myData.ledStatus == 2)
  {
      for (int i = 0; i < NUM_LEDS; i++)
      {
          leds[i].setRGB(255, 0, 0);
      }
      FastLED.show();
  }
  else if (myData.ledStatus == 3)
  {
      for (int i = 0; i < NUM_LEDS; i++)
      {
          leds[i].setRGB(0, 255, 0);
      }
      FastLED.show();
  }
  else if (myData.ledStatus == 4)
  {
      for (int i = 0; i < NUM_LEDS; i++)
      {
          for (int b = 0; b < 255; b++)
          {
              leds[i].setRGB(b, 0, 0);
              FastLED.show();
          }
      }
  }
  else if (myData.ledStatus == 0)
  {
      for (int i = 0; i < NUM_LEDS; i++)
      {
          FastLED.clear();
      }
  }
 
  FastLED.delay(8);
}
