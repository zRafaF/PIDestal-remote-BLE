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
    delay(30);
}
