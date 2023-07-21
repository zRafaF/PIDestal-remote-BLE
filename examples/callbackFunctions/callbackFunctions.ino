// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <PIDestalRemoteBLE.h>

void print25(){
  Serial.println(25);
}

void printHEEEERE(){
  Serial.println("HEEEERE");
}


PIDestal pid1(1,2,3);

PIDestalRemoteBLE myRemote(&pid1);

/*
  Creating an array of FunctionPointer s =
  Observe that we do not CALL the function, but PASS it.

*/
PIDestalRemoteBLE::FunctionPointer functions[] = {print25, printHEEEERE};


void setup() {
    Serial.begin(115200);
    myRemote.initialize("ESP_BOARD", "123123");
    
    // Functions passed to this function can be called thought Bluetooth
    myRemote.setCallbackFunctions(functions, 2);

    // Calls the function "print25()"
    myRemote.runFunctionByIndex(0);

}



void loop() {
    myRemote.process();

    delay(30);
}
