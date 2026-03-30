#line 1 "/home/pmei/Projekt/RC/em33-2/em33-2-mesh.cpp"
/*
 * em33-2-mesh.cpp
 *
 * Startet den Mesh Network Task, der die Kommunikation im Mesh Netzwerk ermöglicht.
 * Alle Funktionen, die mit dem Mesh Netzwerk zu tun haben, sind hier definiert.
 *
 * Created By: Pius Meier
 * Created on: 20.02.2026
 *
 */

#include "em33-2.h"

// #define EM33_2_MESH_LOG true
#define EM33_2_MESH_LOG false

painlessMesh mesh;

void meshSetup()
{
    IPAddress myAPIP(0, 0, 0, 0);

    mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION); // set before init() so that you can see startup messages

    mesh.init(EM33_MESH_PREFIX, EM33_MESH_PASSWORD, EM33_MESH_PORT, WIFI_AP_STA, 6);

    mesh.stationManual(EM33_STATION_SSID, EM33_STATION_PASSWORD);

    mesh.onReceive(&receivedCallback);

    mesh.setHostname(EM33_2_HOSTNAME);

    if (!MDNS.begin(EM33_2_HOSTNAME))
    {
        LOG_E(EM33_2_MESH_LOG, "Error setting up MDNS responder!");
    }

    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    mesh.setRoot(true);
    // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
    mesh.setContainsRoot(true);
#define EM33_2_MESH_LOG true
    myAPIP = IPAddress(mesh.getAPIP());
    LOG_II(EM33_2_MESH_LOG, "My AP IP is ", myAPIP.toString());
#define EM33_2_MESH_LOG false
}

// callback function -> what does the ESP32 need to do when a message from the mesh network is received? -> run function "receivedCallback()"
void receivedCallback(const uint32_t &from, const String &msg)
{
    StaticJsonDocument<200> doc;

    LOG_IH(EM33_2_MESH_LOG, String(millis()).c_str());
    LOG_IF(EM33_2_MESH_LOG, " bridge: Received from %u msg=%s\n", from, msg.c_str());
    sendWebSocketData(msg);
    deserializeJson(doc, msg);

    if (doc["hostname"] == EM33_1_HOSTNAME)
    {
        em33MsgLok.newData = true;
        em33MsgLok.Spannung = doc["Spannung"] | 0.0;
        em33MsgLok.Strom = doc["Strom"] | 0.0;
        em33MsgLok.speed = doc["speed"] | 0;
        em33MsgLok.speedp = doc["speedp"] | 0;
        em33MsgLok.brakep = doc["brakep"] | 0;
        em33MsgLok.hostname = doc["hostname"] | "";
    }
    else if (doc["hostname"] == EM33_2_HOSTNAME)
    {
        em33MsgW1.newData = true;
        em33MsgW1.Spannung = doc["Spannung"] | 0.0;
        em33MsgW1.Strom = doc["Strom"] | 0.0;
        em33MsgW1.speed = doc["speed"] | 0;
        em33MsgW1.speedp = doc["speedp"] | 0;
        em33MsgW1.brakep = doc["brakep"] | 0;
        em33MsgW1.hostname = doc["hostname"] | "";
    }
    else if (doc["hostname"] == EM33_3_HOSTNAME)
    {
        em33MsgW2.newData = true;
        em33MsgW2.Spannung = doc["Spannung"] | 0.0;
        em33MsgW2.Strom = doc["Strom"] | 0.0;
        em33MsgW2.speed = doc["speed"] | 0;
        em33MsgW2.speedp = doc["speedp"] | 0;
        em33MsgW2.brakep = doc["brakep"] | 0;
        em33MsgW2.hostname = doc["hostname"] | "";
    }
}

// Funktion um die lokale IP Adresse des ESP32 zu bekommen
IPAddress getlocalIP()
{
    return IPAddress(mesh.getStationIP());
}

// logt die aktuellen IPs
unsigned long logMeshIP(int interval, unsigned long previousMillis)
{
    IPAddress myIP(0, 0, 0, 0);

    unsigned long now = millis(); // read out the current "time" ("millis()" gives the time in ms since the Arduino started)
    if ((unsigned long)(now - previousMillis) >= interval)
    {                         // check if "interval" ms has passed since last time the clients were updated
        previousMillis = now; // reset previousMillis
        LOG_I(EM33_2_MESH_LOG, String(previousMillis));
        myIP = IPAddress(mesh.getStationIP());
        LOG_II(EM33_2_MESH_LOG, "My IP is ", myIP.toString().c_str());
        myIP = IPAddress(mesh.getAPIP());
        LOG_II(EM33_2_MESH_LOG, "My AP IP is ", myIP.toString().c_str());
    }
    return previousMillis;
}

void meshSendBroadcast(String msg)
{
    mesh.sendBroadcast(msg);
}

void meshMain(void *parameter)
{
    int interval = 5000;              // send data to the client every 1000ms -> 1s
    unsigned long previousMillis = 0; // we use the "millis()" command for time reference and this will output an unsigned long

    meshSetup();
    for (;;)
    {
        mesh.update();
        previousMillis = logMeshIP(interval, previousMillis);
        tDelay(20);
    }
}