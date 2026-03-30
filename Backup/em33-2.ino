/*
 *  Programm für die EM 3/3
 *
 *  Das Motoren Signal, Bremssignal und der aktuell bezogene Strom wird via WiFi zu den Sitzwagen übertragen.
 *  Der erste Sitzwagen übernimmt die Hauptseuerung. Dieser Kontroller macht auch den WiFi Hotspot.
 *  Wird der Hotspot (der erste Sitzwagen) nicht gefunden wird dies durch das Blinken im ersten Digit der Spannung angezeigt.
 *  Die remote Aufgaben sind passiv, diese müssen von extern abgefragt werden.
 *
 *  em33-1 ist für die Lock
 *  em33-2 ist für den ersten Sitzwagen
 *  em33-3 ist für den zweiten Sitzwagen
 *
 *  Die weiteren Sitzwagen nehmen die Verbindung zum ersten Sitzwagen auf und erhalten die Signale von dort.
 *
 */

// #include <WiFi.h>
#include "em33.h"
#include "em33-2.h"

// #include <NetworkClient.h>
//  #include <WiFiAP.h>
//  #include "esp_log.h"
//   #include "mylog.h"
//  #include <esp32-hal-log.h>
//  #include <FreeRTOSConfig.h>
//  #include <Arduino_FreeRTOS.h>

#define EM33_2LOG true

TaskHandle_t wifiAPTask = NULL;
TaskHandle_t Task2Handle = NULL;

void setup()
{
  Serial.begin(115200);
  tDelay(5000);
  LOG_I(EM33_2LOG, "start setup");
  pinMode(LED_BUILTIN, OUTPUT);

  // start Wiffi AP as task
  xTaskCreatePinnedToCore(
      wifiAP,      // funktion
      "wifiAP",    // task name
      4096,        // stacksize 9k
      NULL,        // parameter
      1,           // priorität
      &wifiAPTask, // taskhandel
      0);          // Core
}

void loop()
{
  unsigned int i;
  for (i = 0; i <= WifiStatusCode; i++)
  {
    digitalWrite(LED_BUILTIN, LOW);
    tDelay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    tDelay(250);
  }
  tDelay(1750);
}
