#line 1 "/home/pmei/Projekt/RC/em33-2/em33-filehandler.cpp"

#define EM33_2_FS true
#define FORMAT_LITTLEFS_IF_NEEDED true

#include "em33.h"
#include "em33-2.h"

bool FS_present = false;

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.path(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char *path)
{
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path))
  {
    Serial.println("Dir created");
  }
  else
  {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char *path)
{
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path))
  {
    Serial.println("Dir removed");
  }
  else
  {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("- file written");
  }
  else
  {
    Serial.println("- write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
    Serial.println("- message appended");
  }
  else
  {
    Serial.println("- append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2)
{
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (fs.rename(path1, path2))
  {
    Serial.println("- file renamed");
  }
  else
  {
    Serial.println("- rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path))
  {
    Serial.println("- file deleted");
  }
  else
  {
    Serial.println("- delete failed");
  }
}

// SPIFFS-like write and delete file, better use #define CONFIG_LITTLEFS_SPIFFS_COMPAT 1

void writeFile2(fs::FS &fs, const char *path, const char *message)
{
  if (!fs.exists(path))
  {
    if (strchr(path, '/'))
    {
      Serial.printf("Create missing folders of: %s\r\n", path);
      char *pathStr = strdup(path);
      if (pathStr)
      {
        char *ptr = strchr(pathStr, '/');
        while (ptr)
        {
          *ptr = 0;
          fs.mkdir(pathStr);
          *ptr = '/';
          ptr = strchr(ptr + 1, '/');
        }
      }
      free(pathStr);
    }
  }

  Serial.printf("Writing file to: %s\r\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("- file written");
  }
  else
  {
    Serial.println("- write failed");
  }
  file.close();
}

bool deleteFile2(fs::FS &fs, String path)
{
  bool result = false;

  LOG_I(EM33_2_FS, String("Deleting file and empty folders on path: " + path).c_str());

  if (fs.remove(path))
  {
    LOG_I(EM33_2_FS, "- file deleted");
    result = true;
  }
  else
  {
    LOG_E(EM33_2_FS, "- delete failed");
    result = false;
  }

  char *pathStr = strdup(path.c_str());
  if (pathStr)
  {
    char *ptr = strrchr(pathStr, '/');
    if (ptr)
    {
      LOG_I(EM33_2_FS, String(String("Removing all empty folders on path: ") + path).c_str());
    }
    while (ptr)
    {
      *ptr = 0;
      fs.rmdir(pathStr);
      ptr = strrchr(pathStr, '/');
    }
    free(pathStr);
  }
  return result;
}

void testFileIO(fs::FS &fs, const char *path)
{
  Serial.printf("Testing file I/O with %s\r\n", path);

  static uint8_t buf[512];
  size_t len = 0;
  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }

  size_t i;
  Serial.print("- writing");
  uint32_t start = millis();
  for (i = 0; i < 20; i++)
  {
    if ((i & 0x001F) == 0x001F)
    {
      Serial.print(".");
    }
    file.write(buf, 512);
  }
  Serial.println("");
  uint32_t end = millis() - start;
  Serial.printf(" - %u bytes written in %lu ms\r\n", 2048 * 512, end);
  file.close();

  file = fs.open(path);
  start = millis();
  end = start;
  i = 0;
  if (file && !file.isDirectory())
  {
    len = file.size();
    size_t flen = len;
    start = millis();
    Serial.print("- reading");
    while (len)
    {
      size_t toRead = len;
      if (toRead > 512)
      {
        toRead = 512;
      }
      file.read(buf, toRead);
      if ((i++ & 0x001F) == 0x001F)
      {
        Serial.print(".");
      }
      len -= toRead;
    }
    Serial.println("");
    end = millis() - start;
    Serial.printf("- %zu bytes read in %lu ms\r\n", flen, end);
    file.close();
  }
  else
  {
    Serial.println("- failed to open file for reading");
  }
}

bool startFilehandler()
{
  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_NEEDED))
  {
    FS_present = false;
    LOG_E(EM33_2_FS, "LITTLEFS Mount Failed");
    return false;
  }

  FS_present = true;
  LOG_I(EM33_2_FS, "LITTLEFS present");

  /*
  Serial.println("SPIFFS-like write file to new path and delete it w/folders");
  writeFile2(LittleFS, "/new1/new2/new3/hello3.txt", "Hello3");
  listDir(LittleFS, "/", 3);
  //  deleteFile2(LittleFS, "/new1/new2/new3/hello3.txt");

  listDir(LittleFS, "/", 3);
  createDir(LittleFS, "/mydir");
  writeFile(LittleFS, "/mydir/hello2.txt", "Hello2");
  listDir(LittleFS, "/", 1);
  //  deleteFile(LittleFS, "/mydir/hello2.txt");
  //  removeDir(LittleFS, "/mydir");
  listDir(LittleFS, "/", 1);
  writeFile(LittleFS, "/hello.txt", "Hello ");
  appendFile(LittleFS, "/hello.txt", "World!\r\n");
  readFile(LittleFS, "/hello.txt");
  renameFile(LittleFS, "/hello.txt", "/foo.txt");
  readFile(LittleFS, "/foo.txt");
  //  deleteFile(LittleFS, "/foo.txt");
  testFileIO(LittleFS, "/test.txt");
  //  deleteFile(LittleFS, "/test.txt");

  Serial.println("Test complete");
  /**/

  return true;
}
