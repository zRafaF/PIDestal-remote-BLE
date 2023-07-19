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

// How many pids can this class update
#define PIDS_ARRAY_SIZE 10

#define DEFAULT_SERVICE_UUID "3e60a07c-235e-11ee-be56-0242ac120002"

/*
Minimum received package size.

Takes password, pid, commas into account.

Does not care for the extra info
*/
#define MIN_RECEIVED_SIZE (PASSWORD_ARRAY_SIZE + 1 + 6 + 1 + 6 + 1 + 6 + 1)  // = 28

// You can disable the Bluetooth during the preprocesse by definining "DO_NOT_USE_BLUETOOTH"
// #define DO_NOT_USE_BLUETOOTH

class PIDestalRemoteBLE {
   public:
    PIDestalRemoteBLE(PIDestal* _pidAPtr);
    PIDestalRemoteBLE(PIDestal* _pidArrayPtr[], int arraySize);

    // Initialize should be called during setup()
    void initialize(const char* deviceName, const char* myPassword[PASSWORD_ARRAY_SIZE]);

    // Process should be called during loop()
    void process();

    // Sets the extra info value, it The array should be of size EXTRA_INFO_ARRAY_SIZE
    void setExtraInfo(const char* info[EXTRA_INFO_ARRAY_SIZE]) { strcpy(extraInfo, info); }
    void setExtraInfo(String info) { info.toCharArray(extraInfo, EXTRA_INFO_ARRAY_SIZE); }
    char* getExtraInfo() { return extraInfo; }

    PIDestal getFirstPidConsts() { return ptrArray ? ptrArray[0]->getPidConsts() : PIDestal(0, 0, 0); }
    void setPidArrayConsts(PIDestal newPID) {
        for (int i = 0; i < ptrArraySize; i++) {
            ptrArray[i]->setPidConsts(newPID);  // Accessing the x variable of each PIDestal object
        }
    }

   private:
    bool checkValidPassword(String buffer);

    String extractStringFromData(String buffer);

    void processReceivedData();
    void updateGetters();

    String lastExtra;
    PID lastPID;

    String lastReceivedP;
    String lastReceivedI;
    String lastReceivedD;
    String lastReceivedExtra;

    String myDeviceName;

    char extraInfo[EXTRA_INFO_ARRAY_SIZE] = "";
    char password[PASSWORD_ARRAY_SIZE];

    PIDestal** ptrArray;   // Initialize with nullptr
    int ptrArraySize = 0;  // Initialize with nullptr

    BLEService pidService;

    // characteristics
    BLEStringCharacteristic pGetCharacteristic;
    BLEStringCharacteristic iGetCharacteristic;
    BLEStringCharacteristic dGetCharacteristic;
    BLEStringCharacteristic extraGetCharacteristic;

    BLEStringCharacteristic pSetCharacteristic;
    BLEStringCharacteristic iSetCharacteristic;
    BLEStringCharacteristic dSetCharacteristic;
    BLEStringCharacteristic extraSetCharacteristic;
};

#endif  // PIDESTAL_REMOTE_BLE_H