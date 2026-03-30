/*
 * em33-2-websockets.cpp
 *
 * Created By: Pius Meier
 * Created on: 20.02.2026
 *
 */

#include "em33-2.h"

#define EM33_2_SOCKET_LOG true

// Websocket server auf Port 81
WebSocketsServer webSocket(81);

// Funktion um eine random zahl an alle Clients zu senden, wenn ein bestimmtes Intervall vergangen ist
unsigned long sendWebSocketMessage(int interval, unsigned long previousMillis)
{

    unsigned long now = millis(); // read out the current "time" ("millis()" gives the time in ms since the Arduino started)

    if ((unsigned long)(now - previousMillis) >= interval)
    { // check if "interval" ms has passed since last time the clients were updated

        String str = String(random(100));    // create a random value
        webSocket.broadcastTXT(str.c_str()); // send char_array to clients
        previousMillis = now;                // reset previousMillis
    }
    return previousMillis;
}

void sendWebSocketData(String data)
{
    webSocket.broadcastTXT(data.c_str());
}

// callback function -> what does the ESP32 need to do when an event from the websocket is received? -> run function "webSocketEvent()"
void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length)
{ // num: id of the client who send the event, type: type of message, payload: actual data sent and length: length of payload
    switch (type)
    {                         // switch on the type of information sent
    case WStype_DISCONNECTED: // if a client is disconnected, then type == WStype_DISCONNECTED
        LOG_I(EM33_2_SOCKET_LOG, String("Client " + String(num) + " disconnected").c_str());
        break;
    case WStype_CONNECTED: // if a client is connected, then type == WStype_CONNECTED
        LOG_I(EM33_2_SOCKET_LOG, String("Client " + String(num) + " connected").c_str());
        // optionally you can add code here what to do when connected
        break;
    case WStype_TEXT: // if a client has sent data, then type == WStype_TEXT
        LOG_I(EM33_2_SOCKET_LOG, String("Client " + String(num) + " sent: " + (char *)payload).c_str());
        break;
    }
}

// Funktion um den Websocket Server zu starten
void webSocketSetup()
{
    webSocket.begin();
    webSocket.onEvent(webSocketEvent); // define a callback function -> what does the ESP32 need to do when an event from the websocket is received? -> run function "webSocketEvent()"
    LOG_I(EM33_2_SOCKET_LOG, "Websocket server started on port 81");
}

// Funktion um den Websocket Server in einem eigenen Task zu betreiben
void webSockethMain(void *parameter)
{
    int interval = 2000;              // send data to the client every 1000ms -> 1s
    unsigned long previousMillis = 0; // we use the "millis()" command for time reference and this will output an unsigned long

    webSocketSetup();
    for (;;)
    {
        webSocket.loop();
        // previousMillis = sendWebSocketMessage(interval, previousMillis);
    }
}