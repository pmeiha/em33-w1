#line 1 "/home/pmei/Projekt/RC/em33-2/Backup/em33-2.h"
/*
 * em33-2.h haederfile für den ersten Sitzwagen
 */

#pragma once

int WifiStatusCode = 0; // wifi status code

void wifiAP(void *parameter);

IPAddress ap_ip(10, 10, 10, 1);
IPAddress ap_mask(255, 255, 255, 0);
IPAddress ap_leaseStart(10, 10, 10, 2);
IPAddress ap_dns(10, 10, 10, 1);
