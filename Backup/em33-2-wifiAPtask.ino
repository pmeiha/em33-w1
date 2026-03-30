
#include <NetworkEvents.h>

// #include <ETH.h>
#include <WiFi.h>

// #include <Network.h>
//  #include <NetworkClient.h> // ????
//  #include <WiFiAP.h>        // ????
#include "em33.h"
#include "em33-2.h"
#include "em33-2-wifiAPtask.h"

#define APtaskLog true

NetworkServer server(80);

void APsetup()
{

    char strBuf[] = "                                                      ";

    LOG_I(APtaskLog, "Configuring access point...");

    WiFi.onEvent(
        [](WiFiEvent_t event, WiFiEventInfo_t info)
        {
            char strBuf[] = "00:00:00:00:00:00          ";

            switch (event)
            {
            case ARDUINO_EVENT_WIFI_STA_START:
                LOG_I(APtaskLog, "STA Started");
                break;

                // case ARDUINO_EVENT_WIFI_STA_CONNECTED:
                //    LOG_I(APtaskLog, "STA Connected");
                //    break;

                // case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                //    LOG_I(APtaskLog, "STA Got IP");
                //    Serial.println(WiFi.STA);
                //    break;

                // case ARDUINO_EVENT_WIFI_STA_LOST_IP:
                //    LOG_I(APtaskLog, "STA Lost IP");
                //    break;

                // case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                //     LOG_I(APtaskLog, "STA Disconnected");
                //     WiFi.AP.enableNAPT(false);
                //     break;

                // case ARDUINO_EVENT_WIFI_STA_STOP:
                //     LOG_I(APtaskLog, "STA Stopped");
                //     break;

            case ARDUINO_EVENT_WIFI_AP_START:
                LOG_IH(APtaskLog, "AP Started ");
                LOG_IP(APtaskLog, WiFi.AP);
                break;

            case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
                // Serial.println();
                sprintf(strBuf, MACSTR, MAC2STR(info.wifi_ap_staconnected.mac));
                LOG_II(APtaskLog, "AP STA Connected ", strBuf);
                break;

            case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
                sprintf(strBuf, MACSTR, MAC2STR(info.wifi_ap_stadisconnected.mac));
                LOG_II(APtaskLog, "AP STA Disconnected ", strBuf);
                sprintf(strBuf, "%d", info.wifi_ap_stadisconnected.reason);
                LOG_II(APtaskLog, "reason ", strBuf);
                break;

            case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
                LOG_IH(APtaskLog, "AP STA IP Assigned: ");
                LOG_IP(APtaskLog, IPAddress(info.wifi_ap_staipassigned.ip.addr));
                break;

            case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
                LOG_I(APtaskLog, "AP Probe Request Received");
                break;
            case ARDUINO_EVENT_WIFI_AP_STOP:
                LOG_I(APtaskLog, "AP Stopped");
                break;

            default:
                LOG_I(APtaskLog, NetworkEvents::eventName(event));
                break;
            }
        });

    if (!WiFi.softAPConfig(ap_ip, ap_ip, ap_mask, ap_leaseStart, ap_dns))
    {
        LOG_E(APtaskLog, "Soft AP config failed.");
        while (1)
            ;
    }

    WiFi.softAPsetHostname(em33_2_hostname);

    if (!WiFi.softAP(ssid, Wifipass))
    {
        LOG_E(APtaskLog, "Failed to start AP!");
        ;
        return;
    }

    IPAddress myIP = WiFi.softAPIP();
    sprintf(strBuf, IPSTR, IPAddressStr(myIP));
    LOG_II(APtaskLog, "AP IP address: ", strBuf);

    server.begin();
    LOG_I(APtaskLog, "Server started");
}

void APloop()
{

    NetworkClient client = server.accept(); // listen for incoming clients
    char strBuf[] = "                                                      ";

    if (client)
    {                                    // if you get a client,
        LOG_I(APtaskLog, "New Client."); // print a message out the serial port
        sprintf(strBuf, "remote IP Addr: %d.%d.%d.%d remote Port: %d", IPAddressStr(client.remoteIP()), client.remotePort());
        LOG_I(APtaskLog, strBuf);
        // Serial.println(client.remoteIP());
        // Serial.println(client.remotePort());
        //  mac ? Serial.println(client.);
        String currentLine = ""; // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                // Serial.write(c);        // print it out the serial monitor
                if (c == '\n')
                { // if the byte is a newline character
                    LOG_I(APtaskLog, currentLine.c_str());

                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        // the content of the HTTP response follows the header:
                        client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
                        client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }

                // Check to see if the client request was "GET /H" or "GET /L":
                if (currentLine.endsWith("GET /H"))
                {
                    WifiStatusCode = 1; // GET /H turns the LED on
                }
                if (currentLine.endsWith("GET /L"))
                {
                    WifiStatusCode = 2; // GET /L turns the LED off
                }
            }
        }
        // close the connection:
        client.stop();
        LOG_I(APtaskLog, "Client Disconnected.");
    }
}

void wifiAP(void *parameter)
{
    APsetup();
    for (;;)
    {
        APloop();
    }
}
