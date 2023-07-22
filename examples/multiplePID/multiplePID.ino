// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <PIDestalRemoteBLE.h>

PIDestal pid1(0.1, 0.2, 0.3);
PIDestal pid2(1.75, 24.2, 29.9);

PIDestal* pidArray[] = {&pid1, &pid2};

PIDestalRemoteBLE myRemote(pidArray, 2);


void setup() {
    Serial.begin(115200);
    myRemote.initialize("ESP_BOARD", "123123");
}

void loop() {
    myRemote.process();

    Serial.print("PID1 P: ");
    Serial.print(pid1.getPidConsts().p);
    Serial.print(" i: ");
    Serial.print(pid1.getPidConsts().i);
    Serial.print(" d: ");
    Serial.print(pid1.getPidConsts().d);
    Serial.print("\t");
    Serial.print("PID2 P: ");
    Serial.print(pid2.getPidConsts().p);
    Serial.print(" i: ");
    Serial.print(pid2.getPidConsts().i);
    Serial.print(" d: ");
    Serial.println(pid2.getPidConsts().d);
  
    delay(30);
}
