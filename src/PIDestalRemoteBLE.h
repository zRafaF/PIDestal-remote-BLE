// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PIDESTAL_REMOTE_BLE_H
#define PIDESTAL_REMOTE_BLE_H

#include <ArduinoBLE.h>
#include <Pidestal.h>
#include <arduino.h>

// You can disable the Bluetooth during the preprocesse by definining "DO_NOT_USE_BLUETOOTH"
// #define DO_NOT_USE_BLUETOOTH

namespace PID_BLE {

BLEService pidService("3e60a07c-235e-11ee-be56-0242ac120002");
BLEStringCharacteristic pidConstsCharacteristic("13078cd8-235e-11ee-be56-0242ac120002", BLERead | BLEWrite, 512);

String pidToString(PID pid);

}  // namespace PID_BLE

class PIDestalRemoteBLE {
   public:
    PIDestalRemoteBLE(PIDestal& _pidPtr, String deviceName);

    // Initialize should be called during setup()
    void initialize();

    // Process should be called during loop()
    void process();

   private:
    String myDeviceName;
    PIDestal* pidPtr;
};

#endif  // PIDESTAL_REMOTE_BLE_H