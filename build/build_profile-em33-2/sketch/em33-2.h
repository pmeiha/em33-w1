#line 1 "/home/pmei/Projekt/RC/em33-2/em33-2.h"
/*
 * em33-2.h haederfile für den ersten Sitzwagen
 */

// /home/pmei/.arduino15/internal/Servo_1.3.0_497671ba0520c8c4/Servo/src/esp32/ServoTimers.h
// wert ersetzen SOC_LEDC_TIMER_BIT_WIDE_NUM -> SOC_LEDC_TIMER_BIT_WIDTH
// #define SOC_LEDC_TIMER_BIT_WIDE_NUM SOC_LEDC_TIMER_BIT_WIDTH

#include <WiFi.h>
#include <AsyncTCP.h>
#include <painlessMesh.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <WebSocketsServer.h>
#include <ESP32Servo.h>

#include "em33.h"
#include "em33-2-webserver.h"
#include "em33-2-websockets.h"
#include "em33-2-mesh.h"
#include "em33-2-puls.h"
#include "em33-2-servo.h"
#include "em33-filehandler.h"

#pragma once

extern int em33StatusCode;

extern em33Msg_t em33MsgLok;
extern em33Msg_t em33MsgW1;
extern em33Msg_t em33MsgW2;