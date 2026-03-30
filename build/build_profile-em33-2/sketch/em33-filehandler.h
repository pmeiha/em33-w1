#line 1 "/home/pmei/Projekt/RC/em33-2/em33-filehandler.h"
/*
 * em33-2-websockets.h haederfile
 */

#pragma once

extern bool FS_present;

bool startFilehandler();
bool deleteFile2(fs::FS &fs, String path);