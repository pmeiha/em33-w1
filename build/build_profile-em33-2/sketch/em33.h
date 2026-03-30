#line 1 "/home/pmei/Projekt/RC/em33-2/em33.h"
/*
 * em33.h globales haederfile für das Projekt
 */

#pragma once

#include "FS.h"
#include <LittleFS.h>

#include "log.h"
#include "em33-filehandler.h"

#define tDelay(t) vTaskDelay(t / portTICK_PERIOD_MS)

#define IPAddressStr(ip) ip[0], ip[1], ip[2], ip[3]

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED
#endif

#define EM33_MESH_PREFIX "em33-mesh"
#define EM33_MESH_PASSWORD "ganzGeheim"
#define EM33_MESH_PORT 5555

#define EM33_STATION_SSID "em33-ap"
#define EM33_STATION_PASSWORD "ganz1Geheim"

#define EM33_1_HOSTNAME "em33-lok"
#define EM33_2_HOSTNAME "em33-w1"
#define EM33_3_HOSTNAME "em33-w2"

#define EM33_PULSE_SPEED_PIN 2
#define EM33_PULSE_BRAKE_PIN 3

#define EM33_SERVO_BRAKE_PIN 20
#define EM33_SERVO_SPEED_PIN 10

#define EM33_SPEED_MIN 1000 // microseconds (us)
#define EM33_SPEED_MAX 2000 // microseconds (us)

#define EM33_BRAKE_MIN 1000 // microseconds (us)
#define EM33_BRAKE_MAX 2000 // microseconds (us)

typedef struct
{
    bool newData; // Flag to indicate if new data has been received
    float Spannung;
    float Strom;
    int speed;
    int speedp;
    int brakep;
    String hostname;
} em33Msg_t;