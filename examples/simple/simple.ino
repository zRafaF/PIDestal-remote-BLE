// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <PIDestalRemoteBLE.h>


// Criando um PID chamado "pid1"
// Voce pode alterar os valores para ver como eles afetam
PID meuPID = {
    0.5,
    0.001,
    0.1,
};

PIDestal pid1(meuPID);

PIDestalRemoteBLE myRemote(pid1, "3e60a07c-235e-11ee-be56-0242ac120002");

void setup() {
    Serial.begin(115200);
    myRemote.initialize("My Simple ESP32 Connection Name");
    // pid1.useDeltaTime = false; // Disabling the use of deltatime
    myRemote.setExtraInfo("MINHA INFO EXTRA");
}

int t = 2000;
float result1 = 0;
float objetivo = 0;

void loop() {
    myRemote.process();
    if (t > 80) {
        // O objetivo fica mudando de tempo em tempo
        objetivo = random(-100, 100);
        t = 0;
    }

    // Calcula o PID e adiciona ao resultado
    result1 += pid1.calculate(objetivo - result1);

    // Cria um teto maximo e min para o grafico nao ficar mudando toda hora
    Serial.print(100);
    Serial.print("\t");
    Serial.print(-100);
    Serial.print("\t");

    // Plota o grafico
    Serial.print(objetivo);
    Serial.print("\t");
    Serial.println(result1);

    t += 1;
    delay(30);
}