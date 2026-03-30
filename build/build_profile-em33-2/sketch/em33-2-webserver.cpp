#line 1 "/home/pmei/Projekt/RC/em33-2/em33-2-webserver.cpp"
/*
 * em33-2-websserver.cpp
 *
 * Created By: Pius Meier
 * Created on: 20.02.2026
 *
 */

#include "em33-2.h"

#define EM33_2_WEB_LOG true

AsyncWebServer webServer(80);

void webServerSetup()
{

  // check / create minimal files für den webserver
  checkHtmlFiles();

  // Async webserver - websocket page
  webServer.on("/ws", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/index.html", "text/html"); });

  // Async webserver - file directory page also download and delete functions
  webServer.on("/", FileDir);

  // Async webserver - form to file upload page
  webServer.on("/upload", FileUpload);

  // Async webserver - file upload handler, also send ok response to client
  webServer.on("/fupload", HTTP_POST, sendOkResponse, handleFileUpload);

  // Async webserver - handler for not found pages
  webServer.onNotFound(notFound);

  // start web server
  webServer.begin();

  LOG_I(EM33_2_WEB_LOG, "Webserver started");
}

void sendOkResponse(AsyncWebServerRequest *request)
{
  request->send(200);
}

/*********  FUNCTIONS  **********/

// prüfe ob die minimalen Dateien für den Webserver vorhanden sind, wenn nicht, erstelle sie mit Standardinhalt
void checkHtmlFiles()
{

  // ist das Filesystem vorhanden?
  if (!FS_present)
  {
    LOG_I(EM33_2_WEB_LOG, "FS nicht vorhanden, Webserver wird nicht funktionieren");
    return;
  }

  if (!LittleFS.exists("/header.html"))
  {
    LOG_I(EM33_2_WEB_LOG, "file /header.html not found, creating it");
    File file = LittleFS.open("/header.html", "w");
    file.print(R"(<!DOCTYPE html><html> 
      <head><title>ESP32 File System</title></head>
      <body><h1>header.html not Found upload a valid version</h1>
      <ul><li><a href='/'>List</a></li><li><a href='/upload'>Upload</a></li></ul>)");
    file.close();
  }

  if (!LittleFS.exists("/upload.html"))
  {
    LOG_I(EM33_2_WEB_LOG, "file /upload.html not found, creating it");
    File file = LittleFS.open("/upload.html", "w");
    file.print(R"(<!DOCTYPE html><html>
      <head><title>ESP32 File System</title></head>
      <body><h3>file /upload.html not found : Select File to Upload</h3>
      <FORM action='/fupload' method='post' enctype='multipart/form-data'>
      <input class='buttons' style='width:25%' type='file' name='fupload' id='fupload' value=''>
      <button class='buttons' style='width:10%' type='submit'>Upload File</button>
      <br></FORM><a href='/'>[Back]</a><br><br></body></html>)");
    file.close();
  }
}

// start seite des Webservers, listet die Dateien auf und gibt die Möglichkeit zum Löschen und Hochladen von Dateien
void FileDir(AsyncWebServerRequest *request)
{
  if (FS_present)
  {
    // Aktionen entsprechend der Post Anfrage, Download oder Delete
    if (request->args() > 0) // prüfe die mitgegebenen Parameter der Post Anfrage, ignoriere unbekante Parameter
    {
      LOG_I(EM33_2_WEB_LOG, String(request->arg((size_t)0)).c_str());

      String Order = request->arg((size_t)0);

      if (Order.indexOf("download_") >= 0) // bearbeite die download Anfragen, lösche den Präfix "download_" und rufe die Download Funktion auf
      {
        Order.remove(0, 9);
        FS_file_download(request, Order);
        LOG_I(EM33_2_WEB_LOG, Order.c_str());
      }

      if ((request->arg((size_t)0)).indexOf("delete_") >= 0) // bearbeite die delete Anfragen, lösche den Präfix "delete_" und rufe die Delete Funktion auf
      {
        Order.remove(0, 7);
        FS_file_delete(request, Order);
        LOG_I(EM33_2_WEB_LOG, Order.c_str());
      }
    }

    File root = LittleFS.open("/"); // öffne das Root Verzeichnis um die Dateien aufzulisten
    String webpage = "";

    if (root)
    {
      // Webseite mit Tabelle der Dateien erstellen
      appendPageHeader(webpage);
      webpage += F("<table align='center'>");
      webpage += F("<tr><th>Filename</th><th style='width:20%'>Type (File/Dir)</th><th>File Size</th></tr>");
      LOG_D(EM33_2_WEB_LOG, webpage.c_str());

      root.rewindDirectory();
      printDirectory(request, "/", 0, webpage);
      webpage += F("</table>");
      LOG_I(EM33_2_WEB_LOG, webpage.c_str());

      root.close();
    }
    else
    {
      webpage += F("<h3>No Files Found</h3>");
    }

    // Webseite abschliessen und an den Client senden
    appendPageFooter(webpage);
    SendHTML_Content(request, webpage);
  }
  else
    ReportFSNotPresent(request);
}

// Upload a file to the FS
void FileUpload(AsyncWebServerRequest *request)
{
  String webpage = "";

  LOG_I(EM33_2_WEB_LOG, "start uploading file");

  appendPageHeader(webpage);

  if (!appendPageFile(webpage, "/upload.html"))
  {
    LOG_I(EM33_2_WEB_LOG, "file /upload.html not found");
  }

  LOG_I(EM33_2_WEB_LOG, webpage.c_str());

  request->send(200, "text/html", webpage);
}

// Prints the directory rekursiv
void printDirectory(AsyncWebServerRequest *request, const char *dirname, uint8_t levels, String &webpage)
{

  LOG_D(EM33_2_WEB_LOG, String("Listing directory: " + String(dirname)).c_str());

  File root = LittleFS.open(dirname);

  if (!root)
  {
    LOG_I(EM33_2_WEB_LOG, "Failed to open directory");
    root.close();
    return;
  }

  if (!root.isDirectory())
  {
    LOG_I(EM33_2_WEB_LOG, "Not a directory");
    root.close();
    return;
  }

  File file = root.openNextFile();

  int i = 0;
  while (file)
  {
    if (file.isDirectory())
    {
      webpage += "<tr><td>" + String(dirname) + String(file.name()) + "/</td><td>Dir</td><td></td></tr>";
      printDirectory(request, String(dirname + String(file.name()) + "/").c_str(), levels - 1, webpage);
    }
    else
    {

      webpage += F("<tr><td>");
      webpage += String(dirname) + String(file.name());
      webpage += F("</td><td>File</td><td>");
      webpage += fileSize(file.size());
      webpage += F("</td><td><FORM action='/' method='post'>");
      webpage += F("<button type='submit' name='download' value='download_");
      webpage += String(dirname) + String(file.name());
      webpage += F("'>Download</button></FORM></td><td>");
      webpage += F("<FORM action='/' method='post'>");
      webpage += F("<button type='submit' name='delete' value='delete_");
      webpage += String(dirname) + String(file.name());
      webpage += F("'>Delete</button></FORM></td></tr>");
    }
    file = root.openNextFile();
    i++;
  }
  file.close();
  root.close();
}

// Download a file from the FS, it is called in void FS_dir()
void FS_file_download(AsyncWebServerRequest *request, String filename)
{
  if (FS_present)
  {
    File download = LittleFS.open("/" + filename);

    if (download)
    {
      LOG_I(EM33_2_WEB_LOG, String("Downloading file: " + filename + " size: " + String(download.size())).c_str());
      download.close();

      request->send(LittleFS, filename, String(), true);
    }
    else
      ReportFileNotPresent(request, "download");
  }
  else
    ReportFSNotPresent(request);
}

// eine unbekante webseite wurde angefordert, sende eine 404 Seite zurück
void notFound(AsyncWebServerRequest *request)
{
  LOG_I(EM33_2_WEB_LOG, "NOT_FOUND: ");
  if (request->method() == HTTP_GET)
    LOG_I(EM33_2_WEB_LOG, "GET");
  else if (request->method() == HTTP_POST)
    LOG_I(EM33_2_WEB_LOG, "POST");
  else if (request->method() == HTTP_DELETE)
    LOG_I(EM33_2_WEB_LOG, "DELETE");
  else if (request->method() == HTTP_PUT)
    LOG_I(EM33_2_WEB_LOG, "PUT");
  else if (request->method() == HTTP_PATCH)
    LOG_I(EM33_2_WEB_LOG, "PATCH");
  else if (request->method() == HTTP_HEAD)
    LOG_I(EM33_2_WEB_LOG, "HEAD");
  else if (request->method() == HTTP_OPTIONS)
    LOG_I(EM33_2_WEB_LOG, "OPTIONS");
  else
    LOG_I(EM33_2_WEB_LOG, "UNKNOWN");
  LOG_I(EM33_2_WEB_LOG, String(" http://" + request->host() + request->url()).c_str());
  request->send(404, "text/plain", "Not found.");
}

// File upload handler
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{

  LOG_I(EM33_2_WEB_LOG, String("Client:" + request->client()->remoteIP().toString() + " " + request->url()).c_str());

  if (!index)
  {
    // open the file on first call and store the file handle in the request object
    request->_tempFile = LittleFS.open("/" + filename, "w");
    LOG_I(EM33_2_WEB_LOG, String("Upload Start: " + filename).c_str());
  }

  if (len)
  {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data, len);
    LOG_I(EM33_2_WEB_LOG, String("Writing file: " + filename + " index=" + String(index) + " len=" + String(len)).c_str());
  }

  if (final)
  {
    // close the file handle as the upload is now done
    request->_tempFile.close();
    LOG_I(EM33_2_WEB_LOG, String("Upload Complete: " + filename + ",size: " + String(index + len)).c_str());
    request->redirect("/");
  }
}

// Delete a file from the FS, it is called in void FS_dir()
void FS_file_delete(AsyncWebServerRequest *request, String filename)
{
  String webpage = "";

  if (FS_present)
  {
    File dataFile = LittleFS.open(filename, FILE_READ); // Now read data from FS
    if (dataFile)
    {
      dataFile.close();

      if (deleteFile2(LittleFS, filename))
      {
        LOG_II(EM33_2_WEB_LOG, "File deleted successfully: ", filename.c_str());
        webpage += "<h3>File '" + filename + "' has been erased</h3>";
        webpage += F("<a href='/'>[Back]</a><br><br>");
      }
      else
      {
        webpage += F("<h3>File was not deleted - error</h3>");
        webpage += F("<a href='/'>[Back]</a><br><br>");
      }
    }
    else
    {
      ReportFileNotPresent(request, "delete");
    }

    appendPageFooter(webpage);
    SendHTML_Header(request, webpage);
  }
  else
    ReportFSNotPresent(request);
}

// SendHTML_Header  LOG_I(EM33_2_WEB_LOG, webpage.c_str());
void SendHTML_Header(AsyncWebServerRequest *request, String &webpage)
{

  AsyncWebServerResponse *response = request->beginResponse(200, "text/html", webpage);
  response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  response->addHeader("Pragma", "no-cache");
  response->addHeader("Expires", "-1");
  request->send(response);
}

// SendHTML_Content
void SendHTML_Content(AsyncWebServerRequest *request, String webpage)
{
  AsyncWebServerResponse *response = request->beginResponse(200, "text/html", webpage);
  request->send(response);
}

// ReportFSNotPresent
void ReportFSNotPresent(AsyncWebServerRequest *request)
{
  String webpage = "";

  SendHTML_Header(request, webpage);
  webpage += F("<h3>No FS present</h3>");
  webpage += F("<a href='/'>[Back]</a><br><br>");
  appendPageFooter(webpage);
  SendHTML_Content(request, webpage);
}

// ReportFileNotPresent
void ReportFileNotPresent(AsyncWebServerRequest *request, String target)
{
  String webpage = "";
  webpage += F("<h3>File does not exist</h3>");
  webpage += F("<a href='/'>");
  webpage += target + "'>[Back]</a><br><br>";
  appendPageFooter(webpage);
  SendHTML_Content(request, webpage);
}

// ReportCouldNotCreateFile
void ReportCouldNotCreateFile(AsyncWebServerRequest *request, String target)
{
  String webpage = "";
  SendHTML_Header(request, webpage);
  webpage += F("<h3>Could Not Create Uploaded File (write-protected?)</h3>");
  webpage += F("<a href='/'>");
  webpage += target + "'>[Back]</a><br><br>";
  appendPageFooter(webpage);
  SendHTML_Content(request, webpage);
}

// File size conversion
String fileSize(int bytes)
{
  String fsize = "";
  if (bytes < 1024)
    fsize = String(bytes) + " B";
  else if (bytes < (1024 * 1024))
    fsize = String(bytes / 1024.0, 3) + " KB";
  else if (bytes < (1024 * 1024 * 1024))
    fsize = String(bytes / 1024.0 / 1024.0, 3) + " MB";
  else
    fsize = String(bytes / 1024.0 / 1024.0 / 1024.0, 3) + " GB";
  return fsize;
}

void appendPageHeader(String &webpage)
{
  File file = LittleFS.open("/header.html", "r");
  if (file)
  {
    webpage += file.readStringUntil('\0'); // read until end of file
    file.close();
    LOG_I(EM33_2_WEB_LOG, webpage.c_str());
  }
  else
  {
    LOG_I(EM33_2_WEB_LOG, "file /header.html not found");
  }
}

bool appendPageFile(String &webpage, String filename)
{
  File file = LittleFS.open(filename, "r");
  if (file)
  {
    webpage += file.readStringUntil('\0'); // read until end of file
    file.close();
    return true;
  }
  else
  {
    return false;
  }
}

// Saves repeating many lines of code for HTML page footers
void appendPageFooter(String &webpage)
{
  webpage += F("</body></html>");
}