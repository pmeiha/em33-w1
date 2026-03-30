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

em33Config_t em33Config;

void saveConfig()
{
    if (FS_present)
    {
        DynamicJsonDocument doc(512);
        doc["version"] = em33Config.version;
        doc["ledIntensity"] = em33Config.ledIntensity;
        doc["hostname"] = em33Config.hostname;
        doc["stationSSID"] = em33Config.stationSSID;
        doc["stationPassword"] = em33Config.stationPassword;
        doc["blinkStatus"] = em33Config.blinkStatus;

        File configFile = LittleFS.open("/config.json", "w");
        if (configFile)
        {
            serializeJson(doc, configFile);
            configFile.close();
            LOG_I(EM33_2LOG, "Config saved successfully");
        }
        else
        {
            LOG_I(EM33_2LOG, "Failed to open config file for writing");
        }
    }
    else
    {
        LOG_I(EM33_2LOG, "FS not present, cannot save config");
    }
}

void saveIntensity(int8_t inten)
{

    em33Config.ledIntensity = inten;
    // has to be fixed  setIntensity(inten);
    saveConfig();
}

void saveBlinkStatus(bool status)
{

    em33Config.blinkStatus = status;
    saveConfig();
}

void saveSSID(String ssid)
{

    em33Config.stationSSID = ssid;
    saveConfig();
}

void savePWD(String pwd)
{

    em33Config.stationPassword = pwd;
    saveConfig();
}

void createConfig()
{

    em33Config.version = 1;
    em33Config.ledIntensity = 6;
    em33Config.hostname = EM33_1_HOSTNAME;
    em33Config.stationSSID = EM33_STATION_SSID;
    em33Config.stationPassword = EM33_STATION_PASSWORD;
    em33Config.blinkStatus = true;

    saveConfig();
}

void loadConfig()
{
    if (FS_present)
    {
        File configFile = LittleFS.open("/config.json", "r");
        if (configFile)
        {
            size_t size = configFile.size();
            std::unique_ptr<char[]> buf(new char[size]);
            configFile.readBytes(buf.get(), size);
            DynamicJsonDocument doc(512);
            DeserializationError error = deserializeJson(doc, buf.get());
            if (!error)
            {
                em33Config.version = doc["version"].as<int>();
                em33Config.ledIntensity = doc["ledIntensity"].as<int>();
                em33Config.hostname = doc["hostname"].as<String>();
                em33Config.stationSSID = doc["stationSSID"].as<String>();
                em33Config.stationPassword = doc["stationPassword"].as<String>();
                em33Config.blinkStatus = doc["blinkStatus"].as<bool>();
                LOG_I(EM33_2LOG, "Config loaded successfully");
            }
            else
            {
                LOG_I(EM33_2LOG, "Failed to parse config file");
                createConfig();
            }
        }
        else
        {
            LOG_I(EM33_2LOG, "No config file found, using default values");
            createConfig();
        }
    }
    else
    {
        LOG_I(EM33_2LOG, "FS not present, cannot load config");
        createConfig();
    }
}

// initialisiere die tasks
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
