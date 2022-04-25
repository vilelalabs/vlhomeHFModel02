#ifndef __FIREBASEACCESS__H__
#define __FIREBASEACCESS__H__

#include <Arduino.h>
#include <FirebaseESP8266.h>
#include "firebase_auth.h"

bool FireBaseAccessSetup(String _selfIPAddress);
void FireBaseAccessLoop(int ctrl_pin);

// auxiliar functions
void readValue(int ctrl_pin);
int getDeviceAmount();
bool newDeviceKey(String _selfIPAddress, String deviceType, String startValue);
void toggleDeviceValue(int ctrl_pin);
#endif  //!__FIREBASEACCESS__H__