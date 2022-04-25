/*
  ESPWiFiWebUpdate.h

  ESP WiFi Web Update and auto DHCP.

  @author:    Henrique Vilela
  @version:   1.0
  @date:      2022-04-01

  @description:
    This code merges functionality from 'old' ESPWifiSetupNew.h and WebUpdate.h
    libraries to a single library, with two single functions.
 */

#ifndef __ESPWIFIWEBUPDATE__H__
#define __ESPWIFIWEBUPDATE__H__

#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#include "acesso.h"

#ifndef STASSID
#define STASSID MINHA_REDE
#define STAPSK SENHA_REDE
#endif

const char* host = "new-device";
const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

bool timeout = true;

/*
  ESPWiFiWebUpdateSetup()
  @usage:
    Put this function in setup() without parameters.
 */
String ESPWiFiWebUpdateSetup() {
  Serial.begin(115200);
  //////////////// CONEXÃO AUTOMÁTICA E IP VIA DHCP ////////////////
  WiFi.mode(WIFI_STA);
  WiFi.hostname("New_VL_Device");
  
  if (WiFi.SSID() != NULL) {
    WiFi.begin(WiFi.SSID().c_str(), WiFi.psk().c_str());
    long start = millis();
    while (millis() - start < 30000) {
      if (WiFi.status() == WL_CONNECTED) {
        timeout = false;
        break;
      }
      Serial.print(".");
      delay(500);
    }
  }
  if (timeout) {
    timeout = true;
    WiFi.beginSmartConfig();
    long start = millis();
    while (millis() - start < 30000) {
      if (WiFi.status() == WL_CONNECTED) {
        timeout = false;
        break;
      }
      Serial.print(">");
      delay(500);
    }
  }
  if (timeout) {
    Serial.println("Connection failed!");
    delay(5000);
    ESP.restart();
    delay(500);
  }

  //////////////// FIM CONEXÃO AUTOMÁTICA E IP VIA DHCP ///////////////

  MDNS.begin(host);
  httpUpdater.setup(&httpServer);

  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf(
      "\nHTTPUpdateServer ready! Open http://%s.local/update in your browser\n",
      host);

  Serial.println("IP: " + WiFi.localIP().toString());

  httpServer.on("/", []() {
    httpServer.send(
        200, "text/plain",
        "Conectado ao ESP8266 pelo IP: " + WiFi.localIP().toString());
  });
  httpServer.on("/hostname",
                []() { httpServer.send(200, "text/plain", WiFi.hostname()); });

  return (WiFi.localIP().toString());
}

/*
    ESPWiFiWebUpdateLoop()
    @usage:
        Put this function in loop() without parameters.
 */
void ESPWiFiWebUpdateLoop() {
  httpServer.handleClient();
  MDNS.update();
}

#endif  //!__ESPWIFIWEBUPDATE__H__