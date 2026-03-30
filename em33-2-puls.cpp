/*
 * em33-2-puls.cpp
 *
 * liest die daten des rc-empfängers aus und speichert sie in der globalen variable "em33Msg"
 *
 * Created By: Pius Meier
 * Created on: 20.02.2026
 *
 */

#include "em33-2.h"

#define EM33_2_PULS_LOG true

const int speedPin = 2; // GPIO pin connected to the speed signal from the RC receiver
const int brakePin = 4; // GPIO pin connected to the brake signal from the RC receiver

em33Msg_t em33Msg; // global variable to store the values read from the RC receiver

void pulsSetup()
{
    pinMode(speedPin, INPUT);
    pinMode(brakePin, INPUT);

    LOG_I(EM33_2_PULS_LOG, "Puls task setup completed");
}

void pulsMain(void *parameter)
{
    pulsSetup();

    while (true)
    {
        unsigned long pulsSpeedDuration;
        unsigned long pulsBrakeDuration;

        // read the duration of the speed signal pulse
        pulsSpeedDuration = pulseIn(speedPin, HIGH);
        // read the duration of the brake signal pulse
        pulsBrakeDuration = pulseIn(brakePin, HIGH);

        // store the values in the global variable "em33Msg"
        em33Msg.speedp = pulsSpeedDuration;
        em33Msg.brakep = pulsBrakeDuration;

        LOG_I(EM33_2_PULS_LOG, String("Speed pulse duration: " + String(pulsSpeedDuration) + " ms").c_str());
        LOG_I(EM33_2_PULS_LOG, String("Brake pulse duration: " + String(pulsBrakeDuration) + " ms").c_str());

        tDelay(2000); // delay to avoid flooding the log and to give some time for other tasks
    }
}