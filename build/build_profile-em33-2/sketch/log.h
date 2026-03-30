#line 1 "/home/pmei/Projekt/RC/em33-2/log.h"
/*
log.h definiert das debuglogging für das Projekt

example:
    LOG_I(SETUP, "meldung1");
    LOG_I(true, "meldung2");
#define SETUP1 true
    LOG_I(SETUP1, "meldung3");
    int i = 421;
    LOG_I(SETUP1, "meldung3 " + String(i, HEX));
#define Setup1 false

*/
#pragma once

// debug einschalten
#define DEBUG_EM33
// debug level einschalten
#define DEBUG_D // Debug
#define DEBUG_I // Info
#define DEBUG_W // Warning
#define DEBUG_E // Error

// define tags used in programm code
#define SETUP true

// define log macros
#ifdef DEBUG_EM33

/*
void Serialprint(const String filename, const int line, const Printable &message)
{
    Serial.printf("%s %i ", filename, line);
    Serial.println(message);
}
*/

#ifdef DEBUG_D
#define LOG_D(TAG, message) TAG ? Serial.printf("%s %i %s\n", __FILE_NAME__, __LINE__, message) : 0
#else
#define LOG_D(TAG, message)
#endif
#ifdef DEBUG_I
#define LOG_I(TAG, message) TAG ? Serial.printf("%s %i %s\n", __FILE_NAME__, __LINE__, message) : 0
#define LOG_IH(TAG, message) TAG ? Serial.printf("%s %i %s", __FILE_NAME__, __LINE__, message) : 0
#define LOG_IF(TAG, format, _VA_LIST...) TAG ? Serial.printf(format, ##_VA_LIST) : 0
#define LOG_IP(TAG, message) TAG ? Serial.println(message) : 0
#define LOG_II(TAG, message1, message2) TAG ? Serial.printf("%s %i %s%s\n", __FILE_NAME__, __LINE__, message1, message2) : 0
#else
#define LOG_I(TAG, message)
#define LOG_II(TAG, message1, message2)
#endif
#ifdef DEBUG_W
#define LOG_W(TAG, message) TAG ? Serial.printf("%s %i %s\n", __FILE_NAME__, __LINE__, message) : 0
#else
#define LOG_W(TAG, message)
#endif
#ifdef DEBUG_E
#define LOG_E(TAG, message) TAG ? Serial.printf("%s %i %s\n", __FILE_NAME__, __LINE__, message) : 0
#else
#define LOG_E(TAG, message)
#endif
#else
#define LOG_D(TAG, message)
#define LOG_I(TAG, message)
#define LOG_II(TAG, message1, message2)
#define LOG_W(TAG, message)
#define LOG_E(TAG, message)
#endif
