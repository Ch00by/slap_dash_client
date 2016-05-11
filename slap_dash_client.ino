#include <Wire.h>
#include <SoftwareSerial.h>
#include <ESP8266.h>
#include "rgb_led.h"
#include "accel_sensor.h"

#define ARD_RX_ESP_TX   (8)
#define ARD_TX_ESP_RX   (10)
#define ESP_RST         (4)
#define TAP_INT         (2)

/* Globals */
SoftwareSerial softser(ARD_RX_ESP_TX, ARD_TX_ESP_RX);
//ESP8266 wifi(&softser, &Serial, ESP_RST);
ESP8266 wifi(&softser, NULL, ESP_RST);
char WiFiBuffer[256];

#define ESP_SSID "tech guest" // Your network name here
#define ESP_PASS "bigred38" // Your network password here

#define HOST     "www.neil-lakin.com"     // Host to contact
#define GET_PAGE "/test_api"             // Web page to request
//#define POST_PAGE "/test_post"          // for testing post requests
#define POST_RUN  "/runner/Neil"
#define PORT     80                     // 80 = HTTP default port

void setup() {
  // put your setup code here, to run once:
  initRGB();
  pinMode(TAP_INT, INPUT);
  initAccelSensor();
  toggleClick(true);
  wifi.setBootMarker(F("ready"));
  softser.begin(9600); // Soft serial connection to ESP8266
  Serial.begin(57600); while(!Serial); // UART serial debug
  
  // Test if module is ready
  Serial.print(F("Hard reset..."));
  if(!wifi.hardReset()) {
    Serial.println(F("no response from module."));
    for(;;);
  }
  Serial.println(F("OK."));
  
  Serial.print(F("Soft reset..."));
  if(!wifi.softReset()) {
    Serial.println(F("no response from module."));
    for(;;);
  }
  
  Serial.print(F("Connecting to WiFi..."));
  if(wifi.connectToAP(F(ESP_SSID), F(ESP_PASS))) {

    // IP addr check isn't part of library yet, but
    // we can manually request and place in a string.
    Serial.print(F("OK\nChecking IP addr..."));
    wifi.println(F("AT+CIFSR"));
    if(wifi.readLine(WiFiBuffer, sizeof(WiFiBuffer))) {
      Serial.println(WiFiBuffer);
      wifi.find(); // Discard the 'OK' that follows

      Serial.print(F("Connecting to host..."));
      if(wifi.connectTCP(F(HOST), PORT)) {
        Serial.println(F("Connected to server."));
      } else {
        Serial.println(F("Failed to connect to server."));
      }
    } else {
      Serial.println(F("Failed to get IP..."));
    }
  } else {
    Serial.println(F("Failed to connect to AP"));
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(TAP_INT)) {
    Serial.print("tapped");
    wifi.postURL(F(POST_RUN));
    wifi.find(F("}"));
    clearInterrupt();
    delay(2);
  }
}
