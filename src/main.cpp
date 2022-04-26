/*
  Main.cpp
  VLHome Hardware Firmware - Model 02

  @author:    Henrique Vilela
  @version:   1.0
  @date:      2022-04-24

  @description:
    This code is exclusive for Model 02 VLHome Hardware.
    Controls:
      - 01 Relay on GPIO12 of ESP8266-12F board.
      - 01 Switch on GPIO05 of ESP8266-12F board.

 */

#include <Arduino.h>

#include "ESPWiFiWebUpdate.h"
#include "FireBaseAccess.h"

#define CTRL_PIN 12
#define SWITCH_PIN 5
#define TYPE "light"
#define START_VALUE "Apagada"

// global variables
bool switchStatus = false;

// functions prototypes
void initializePages();

void setup() {
  String ipAddress = ESPWiFiWebUpdateSetup();
  initializePages();

  pinMode(CTRL_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);

  if (!FireBaseAccessSetup(ipAddress)) {
    newDeviceKey(ipAddress, TYPE, START_VALUE);
    Serial.println("Device not found on Firebase, adding a new device!");
    Serial.println("Restarting...");
    delay(4000);
    ESP.restart();
    delay(1000);
  }
  // update the device value with the current status on Firebase
  // readValue(CTRL_PIN); startValue(CTRL_PIN)????
}
//------------------------------------------------------------------------------

void loop() {
  ESPWiFiWebUpdateLoop();
  FireBaseAccessLoop(CTRL_PIN);

  if(digitalRead(SWITCH_PIN) != switchStatus ){
        toggleDeviceValue(CTRL_PIN);
        switchStatus = !switchStatus;
  }
 
}

//------------------------------------------------------------------------------
void initializePages() {
  // páginas de acionamento do dispositivo via web
  httpServer.on("/toggle", []() {
    toggleDeviceValue(CTRL_PIN);
    httpServer.send(200, "text/plain",
      digitalRead(CTRL_PIN)?" Lampada Acesa":"Lampada Apagada");
  });
  // para dispositivos apenas ON/OFF --------------------------------
  httpServer.on("/statusCTRL_PIN", []() {
    String msg;
    msg += digitalRead(CTRL_PIN) ? "CTRL_PIN_ON" : "CTRL_PIN_OFF";
    httpServer.send(200, "text/plain", msg);
  });
}