// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PIDESTAL_REMOTE_BLE_H
#define PIDESTAL_REMOTE_BLE_H

#include <ArduinoBLE.h>
#include <PIDestal.h>
#include <arduino.h>

// You can rename the device by defining "BLE_DEVICE_NAME"
// WARNING: You need to define BEFORE including this library
#ifndef BLE_DEVICE_NAME
#define BLE_DEVICE_NAME "PIDestal Remote BLE"
#endif

// You can disable the Bluetooth during the preprocesse by definining "DO_NOT_USE_BLUETOOTH"
// #define DO_NOT_USE_BLUETOOTH

namespace PID_BLE {

extern BLEService pidService;
extern BLEStringCharacteristic pidConstsCharacteristic;

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