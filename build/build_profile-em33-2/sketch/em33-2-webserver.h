#line 1 "/home/pmei/Projekt/RC/em33-2/em33-2-webserver.h"
/*
 * em33-2-webserver.h haederfile
 */

#pragma once

// webserver funktions prototypen
void webServerSetup();
void checkHtmlFiles();
void sendOkResponse(AsyncWebServerRequest *request);
void FileDir(AsyncWebServerRequest *request);
void FileUpload(AsyncWebServerRequest *request);
void printDirectory(AsyncWebServerRequest *request, const char *dirname, uint8_t levels, String &webpage);
void FS_file_download(AsyncWebServerRequest *request, String filename);
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
void notFound(AsyncWebServerRequest *request);
void FS_file_delete(AsyncWebServerRequest *request, String filename);
void SendHTML_Header(AsyncWebServerRequest *request, String &webpage);
void SendHTML_Content(AsyncWebServerRequest *request, String webpage);
void SendHTML_Stop(AsyncWebServerRequest *request);
void ReportFSNotPresent(AsyncWebServerRequest *request);
void ReportFileNotPresent(AsyncWebServerRequest *request, String target);
void ReportCouldNotCreateFile(AsyncWebServerRequest *request, String target);
String fileSize(int bytes);
void appendPageHeader(String &webpage);
void appendPageFooter(String &webpage);
bool appendPageFile(String &webpage, String filename);
