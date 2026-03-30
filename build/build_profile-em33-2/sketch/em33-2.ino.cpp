#include <Arduino.h>
#line 1 "/home/pmei/Projekt/RC/em33-2/em33-2.ino"
/*
 * em33-2.ino main file für den ersten Sitzwagen
 */

#include "em33-2.h"

// enable log messages for this file
#define EM33_2LOG true

TaskHandle_t meshTask = NULL;
TaskHandle_t webSocketTask = NULL;
TaskHandle_t pulsTask = NULL;
TaskHandle_t servoTask = NULL;

em33Msg_t em33MsgLok = {false, 0.0, 0.0, 0, 0, 0, ""};
em33Msg_t em33MsgW1 = {false, 0.0, 0.0, 0, 0, 0, ""};
em33Msg_t em33MsgW2 = {false, 0.0, 0.0, 0, 0, 0, ""};

int em33StatusCode = 0;

// initialisiere die tasks
#line 22 "/home/pmei/Projekt/RC/em33-2/em33-2.ino"
void setup();
#line 80 "/home/pmei/Projekt/RC/em33-2/em33-2.ino"
void loop();
#line 22 "/home/pmei/Projekt/RC/em33-2/em33-2.ino"
void setup()
{
    Serial.begin(115200);
    delay(5000);
    LOG_I(EM33_2LOG, "start setup");
    pinMode(LED_BUILTIN, OUTPUT);

    startFilehandler();

    // initialize and start mesh network
    xTaskCreatePinnedToCore(
        meshMain,   // funktion
        "meshMain", // task name
        4096,       // stacksize 9k
        NULL,       // parameter
        1,          // priorität
        &meshTask,  // taskhandel
        0);         // Core

    // initialize and start webserver (is allready a task)
    webServerSetup();

    // start web socket server
    xTaskCreatePinnedToCore(
        webSockethMain,   // funktion
        "webSockethMain", // task name
        4096,             // stacksize 9k
        NULL,             // parameter
        1,                // priorität
        &webSocketTask,   // taskhandel
        0);               // Core

    /* // start puls task
    xTaskCreatePinnedToCore(
        pulsMain,   // funktion
        "pulsMain", // task name
        4096,       // stacksize 9k
        NULL,       // parameter
        1,          // priorität
        &pulsTask,  // taskhandel
        0);         // Core
    */

    // start servo task
    xTaskCreatePinnedToCore(
        servoMain,   // funktion
        "servoMain", // task name
        4096,        // stacksize 9k
        NULL,        // parameter
        1,           // priorität
        &servoTask,  // taskhandel
        0);          // Core

    LOG_I(EM33_2LOG, "setup completed");
}

// loop wird nicht benutzt, da alle funktionen in tasks laufen
// blinke das onboard LED entsprechend des Statuscodes
void loop()
{
    unsigned int i;

    for (i = 0; i <= em33StatusCode; i++)
    {
        digitalWrite(LED_BUILTIN, LOW);
        tDelay(250);
        digitalWrite(LED_BUILTIN, HIGH);
        tDelay(250);
    }
    tDelay(1750);
}

