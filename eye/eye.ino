#include <pwmWrite.h>
#include <WiFi.h>
#include <esp_now.h>
#include <FastLED.h>

#define LED_PIN     40

// Information about the LED strip itself
#define NUM_LEDS    61
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

#define turnServo 37
#define liftServo 35
#define takeoutServo 39
#define gripServo 33
#define controlServo 18
#define speedControl 16

Pwm pwm = Pwm();

unsigned long timing;
int turnPosition = 90;
int liftPosition = 55;
int takeoutPosition = 40;
int gripPosition = 145;
int controlPosition = 90;
int speed = 0;

// Structure to hold the received message
typedef struct struct_message {
  int axisX;
  int axisY;
  int axisZ;
  int ledStatus;
  int controlMode;
} struct_message;

// Create an instance of the struct_message
struct_message myData;

// Callback function to handle received data
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  // Copy the incoming data to the myData structure
  memcpy(&myData, incomingData, sizeof(myData));
  // Print the received message
}

void setup() {
  delay( 3000 ); // power-up safety delay
  pinMode(speedControl, OUTPUT);

  // It's important to set the color correction for your LED strip here,
  // so that colors can be more accurately rendered through the 'temperature' profiles
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness( BRIGHTNESS );

  //Initialize the serial monitor
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
 
  // Register callback for received data
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() 
{
  
  if(millis() - timing <=20) 
  {
    return;
  }
  timing = millis();

if(myData.controlMode == 1)
{
    if(myData.axisX < 4 && turnPosition > 0) 
    {
      turnPosition -= 1;
    }
    else if(myData.axisX > 7 && turnPosition < 180)
    {
      turnPosition += 1;
    }
    
    if(myData.axisY < 4 && liftPosition > 0)
    {
      liftPosition -= 1;
    }
    else if(myData.axisY > 7 && liftPosition < 110)
    {
      liftPosition += 1;
    }
}

if(myData.controlMode == 2)
{
    if(myData.axisX < 4 && gripPosition > 110)
    {
      gripPosition -= 1;
    }
    else if(myData.axisX > 7 && gripPosition < 180)
    {
      gripPosition += 1;
    }

    if(myData.axisY < 4 && takeoutPosition > 0)
    {
      takeoutPosition -= 1;
    }
    else if(myData.axisY > 7 && takeoutPosition < 80)
    {
      takeoutPosition += 1;
    }
}

if(myData.controlMode == 3)
{
    if(myData.axisX < 4 && controlPosition > 80)
    {
      controlPosition -= 1;
    }
    else if(myData.axisX > 7 && controlPosition < 115)
    {
      controlPosition += 1;
    }

    if(myData.axisY > 7 && speed > 0)
    {
      speed -= 1;
    }
    else if(myData.axisY < 4 && speed < 230)
    {
      speed += 1;
    }
}

  if(myData.axisZ == 1)
  {
    turnPosition = 90;
    liftPosition = 55;
    takeoutPosition = 40;
    gripPosition = 145;
    controlPosition = 90;
    speed = 0;
  }

  if(myData.ledStatus == 1)
  {
    speed = 230;
  }

  pwm.writeServo(turnServo, turnPosition);
  pwm.writeServo(liftServo, liftPosition);
  pwm.writeServo(takeoutServo, takeoutPosition);
  pwm.writeServo(gripServo, gripPosition);
  pwm.writeServo(controlServo, controlPosition);
  analogWrite(speedControl, speed);

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
      static uint8_t starthue = 0;
  fill_rainbow( leds + 5, NUM_LEDS - 5, --starthue, 20);

  // Choose which 'color temperature' profile to enable.
  uint8_t secs = (millis() / 1000) % (DISPLAYTIME * 2);
  if( secs < DISPLAYTIME) {
    FastLED.setTemperature( TEMPERATURE_1 ); // first temperature
    leds[0] = TEMPERATURE_1; // show indicator pixel
  } else {
    FastLED.setTemperature( TEMPERATURE_2 ); // second temperature
    leds[0] = TEMPERATURE_2; // show indicator pixel
  }

  // Black out the LEDs for a few secnds between color changes
  // to let the eyes and brains adjust
  if( (secs % DISPLAYTIME) < BLACKTIME) {
    memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
  }
  
  FastLED.show();
  FastLED.delay(8);
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
