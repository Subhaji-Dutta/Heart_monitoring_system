#include <Wire.h>
#include <SPI.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>

#define REPORTING_PERIOD_MS     1000



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
PulseOximeter pox;
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define ssid "#VEER"
#define pass "12345678"
uint32_t tsLastReport = 0;
float BPM, SpO2,HR;

void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Connecting to ");
       Serial.println(ssid);

 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
    Serial.print("Initializing pulse oximeter..");
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
   pox.setOnBeatDetectedCallback(onBeatDetected);
   display.display();
}

void loop()
{
    // Make sure to call update as fast as possible
    pox.update();
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    BPM = pox.getHeartRate();
    HR=map(BPM,0,180,0,120);
    SpO2 = pox.getSpO2();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.print("Heart rate:");
        Serial.print(HR);
        Serial.print("SpO2:");
        Serial.print(SpO2);
        Serial.println("%");
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(0,0);
        display.print(F("BPM: "));
        display.print(HR);
        display.setCursor(0,20);
        display.print(F("SpO2: "));
        display.print(SpO2);
        display.display();
 tsLastReport = millis();
    }
  }
