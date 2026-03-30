/*
 * em33-2-mesh.h haederfile
 */

#pragma once

// mesh funktions prototypen
// void meshSetup();
// void mesh_loop();
void receivedCallback(const uint32_t &from, const String &msg);
// IPAddress getlocalIP();
void meshSendBroadcast(String msg);
void meshMain(void *parameter);