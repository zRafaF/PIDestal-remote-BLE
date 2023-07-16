// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PIDESTAL_REMOTE_BLE_H
#define PIDESTAL_REMOTE_BLE_H

#include <ArduinoBLE.h>
#include <PIDestal.h>
#include <arduino.h>

#define EXTRA_INFO_ARRAY_SIZE 64

// You can disable the Bluetooth during the preprocesse by definining "DO_NOT_USE_BLUETOOTH"
// #define DO_NOT_USE_BLUETOOTH

namespace PID_BLE {

extern BLEStringCharacteristic pidConstsCharacteristic;

String pidToString(PID pid);

}  // namespace PID_BLE

class PIDestalRemoteBLE {
   public:
    PIDestalRemoteBLE(
        PIDestal& _pidPtr,
        const char* deviceUUID);

    // Initialize should be called during setup()
    void initialize(const char* deviceName);

    // Process should be called during loop()
    void process();

    // Sets the extra info value, it The array should be of size EXTRA_INFO_ARRAY_SIZE
    void setExtraInfo(char info[EXTRA_INFO_ARRAY_SIZE]) { strcpy(extraInfo, info); }
    char* getExtraInfo() { return extraInfo; }

   private:
    String getFormattedPackage();

    BLEService pidService;

    String myDeviceName;
    char extraInfo[EXTRA_INFO_ARRAY_SIZE];
    PIDestal* pidPtr;
};

#endif  // PIDESTAL_REMOTE_BLE_H