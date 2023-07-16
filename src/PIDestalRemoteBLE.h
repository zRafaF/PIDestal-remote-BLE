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

#define PASSWORD_ARRAY_SIZE 6

/*
Minimum received package size.

Takes password, pid, commas into account.

Does not care for the extra info
*/
#define MIN_RECEIVED_SIZE (PASSWORD_ARRAY_SIZE + 1 + 6 + 1 + 6 + 1 + 6 + 1)  // = 28

// You can disable the Bluetooth during the preprocesse by definining "DO_NOT_USE_BLUETOOTH"
// #define DO_NOT_USE_BLUETOOTH

namespace PID_BLE {

struct ResponseBLE {
    float kp;
    float ki;
    float kd;
    char extra[EXTRA_INFO_ARRAY_SIZE];
    bool valid;
};

extern BLEStringCharacteristic pidConstsCharacteristic;

}  // namespace PID_BLE

class PIDestalRemoteBLE {
   public:
    PIDestalRemoteBLE(
        PIDestal& _pidPtr,
        const char* deviceUUID);

    // Initialize should be called during setup()
    void initialize(const char* deviceName, char myPassword[PASSWORD_ARRAY_SIZE]);

    // Process should be called during loop()
    void process();

    // Sets the extra info value, it The array should be of size EXTRA_INFO_ARRAY_SIZE
    void setExtraInfo(char info[EXTRA_INFO_ARRAY_SIZE]) { strcpy(extraInfo, info); }
    char* getExtraInfo() { return extraInfo; }

   private:
    String getFormattedPackage();

    // Encodes the PID to a string
    String pidToString(PID pid);

    // Decodes a received package and returns a formatted response
    PID_BLE::ResponseBLE decodeReceived(String received);

    BLEService pidService;
    String myDeviceName;
    String lastReceivedValue;
    char extraInfo[EXTRA_INFO_ARRAY_SIZE] = "";
    char password[PASSWORD_ARRAY_SIZE];
    PIDestal* pidPtr;
};

#endif  // PIDESTAL_REMOTE_BLE_H