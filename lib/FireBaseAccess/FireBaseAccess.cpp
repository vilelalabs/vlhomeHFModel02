/*
  Implementation of the FireBaseAccess.h functions
*/

#include "FireBaseAccess.h"

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 250;

// device data
static int devicePosition = -1;
String selfIPAddress;

// databases
FirebaseData fbd;
FirebaseData fbd_ip;
FirebaseData fbd_new_device;
FirebaseData fbd_new_value;

// auxiliar variables
int deviceStatus = 0;
int lastDeviceStatus = 0;

/*
 * @description: Begin the Firebase connection and recognize the position
 *  of the device in the ambients on App.
 *
 * @param: selfIPAddress - IP address of the device
 *
 * @return:true if device found, false otherwise
 *
 * @usage: put this function in setup().
 */
bool FireBaseAccessSetup(String _selfIPAddress) {
  selfIPAddress = _selfIPAddress;
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // find the corresponding device by Ip Address
  for (int i = 0; i < getDeviceAmount() + 1; i++) {
    Firebase.getString(fbd_ip, "devices/" + (String)i + "/ipAddress");
    const String ipFromFirebase = fbd_ip.stringData();
    if (ipFromFirebase == selfIPAddress) {
      devicePosition = i;
      Serial.println("Device " + ipFromFirebase + " found at position: [" +
                     (String)devicePosition + "]");
      break;
    }
  }
  // if device not found, exit
  if (devicePosition == -1) {
    Serial.println("Device not found in Firebase Database.");
    return false;
  }
  // starts the stream of data from Firebase
  Firebase.beginStream(fbd, "devices/" + (String)devicePosition + "/value");

  startMillis = millis();

  return true;
}

/*
 * @description: updates the readValue function every period of
 *  time defined by period variable.
 *
 * @usage: put this function in loop().
 */
void FireBaseAccessLoop(int ctrl_pin) {
  currentMillis = millis();
  if (currentMillis - startMillis >= period) {
    readValue(ctrl_pin);
    startMillis = currentMillis;
  }
}

/*
 * @description: Read the value of the device from Firebase
 *
 * @usage: put this function in FireBaseAccessLoop().
 */
void readValue(int ctrl_pin) {
  if (Firebase.readStream(fbd)) {
    String data = fbd.stringData();
    if (data == "Acesa")
      deviceStatus = 1;
    else if (data == "Apagada")
      deviceStatus = 0;

    if (lastDeviceStatus != deviceStatus) {
      Serial.println(data);
      if (deviceStatus == 1) {
        digitalWrite(ctrl_pin, HIGH);
        lastDeviceStatus = 1;
      } else if (deviceStatus == 0) {
        digitalWrite(ctrl_pin, LOW);
        lastDeviceStatus = 0;
      }
    }
  }
}

/*
 * @description: Adds a new device to Firebase Database if it doesn't exists.
 *
 * @param: selfIPAddress - IP address of the device
 * @param: deviceType - type of device
 * @param: startValue - initial value of the device
 * 
 * @usage: put this function in FireBaseAccessSetup().
 */
bool newDeviceKey(String _selfIPAddress, String deviceType, String startValue) {
  int newDevicePosition = getDeviceAmount();
  const String newDeviceIPPath =
      "devices/" + (String)newDevicePosition + "/ipAddress";
  const String newDeviceTypePath =
      "devices/" + (String)newDevicePosition + "/type";
  const String newDeviceValuePath =
      "devices/" + (String)newDevicePosition + "/value";

  Firebase.setString(fbd_new_device, newDeviceIPPath, _selfIPAddress);
  Firebase.setString(fbd_new_device, newDeviceTypePath, deviceType);
  Firebase.setString(fbd_new_device, newDeviceValuePath, startValue);
  return true;
}


/*
 * @description: toggles Value for the device in Firebase Database
 *
 * @usage: use to toggle the value of the device.

*/
void toggleDeviceValue(int ctrl_pin){
  const String deviceValuePath =
      "devices/" + (String)devicePosition + "/value";

  Firebase.getString(fbd_new_value, deviceValuePath);
      String value = fbd_new_value.stringData();

  value=="Acesa"? value="Apagada": value="Acesa";

  if (value == "Acesa")
        deviceStatus = 1;
  else if (value == "Apagada")
        deviceStatus = 0;

  Firebase.setString(fbd_new_value, deviceValuePath, value);

  if (lastDeviceStatus != deviceStatus) {
    Serial.println(value);
    if (deviceStatus == 1) {
      digitalWrite(ctrl_pin, HIGH);
      lastDeviceStatus = 1;
    } else if (deviceStatus == 0) {
      digitalWrite(ctrl_pin, LOW);
      lastDeviceStatus = 0;
  }
  }
}
/*
 * @description: Get the size of the devices array on Firebase
 *
 * @usage: use every place you need to know how much devices are in the
 *  firebase database.
 */
int getDeviceAmount() {
  int deviceAmount = 0;
  int devicesCount = 0;
  while (Firebase.getString(fbd_ip,
                            "devices/" + (String)devicesCount + "/ipAddress")) {
    devicesCount++;
  }
  deviceAmount = devicesCount;
  return deviceAmount;
}
