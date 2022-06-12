#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
RTC_DATA_ATTR byte bootCount = 0;
RTC_DATA_ATTR time_t now;
RTC_DATA_ATTR uint64_t Mics = 0;
RTC_DATA_ATTR struct tm *timeinfo;
#define OFFSET 0               // us (tient compte du temps d'exécution de updateTime)
#define sleepPeriod 10000000ul // 10 seconds sleep
String dosya;

void printLocalTime()
{
  time(&now);
  timeinfo = localtime(&now);
  Serial.println(asctime(timeinfo));
  // Serial.printf("%s\n",asctime(timeinfo));
  delay(2); // 26 bytes@115200 baud is less than 2 ms
}
/*void updateTime (uint64_t elapsedTime) { // elapsedTime in us
  if (elapsedTime == 0) Mics += micros();
  else Mics += elapsedTime;
  if (Mics > 1000000) {
    Mics = Mics % 1000000;
    now += Mics / 1000000;
  }
}
void sommeil (unsigned long chrono) {
  updateTime (sleepPeriod - (micros() - chrono) + OFFSET);
  esp_sleep_enable_timer_wakeup(sleepPeriod - (micros() - chrono));
  esp_deep_sleep_start();
}*/
void updateTime(uint64_t elapsedTime)
{ // elapsedTime in us
  if (elapsedTime == 0)
    Mics += micros();
  else
    Mics += elapsedTime;
  if (Mics > 10000000)
  {
    Mics = Mics % 1000000;
    now += Mics / 10000000;
  }
}
void sommeil(unsigned long chrono)
{
  updateTime(sleepPeriod - (micros() - chrono) + OFFSET);
  esp_sleep_enable_timer_wakeup(sleepPeriod - (micros() - chrono));
  esp_deep_sleep_start();
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory:%s\n", dirname);
  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
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
        listDir(fs, file.name(), levels - 1);
      }
    }

    else
    {
      Serial.println("FILE:");
      Serial.println(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}
void createDir(fs::FS &fs,const char* path){

Serial.printf("Creating Dir:%s\n",path);
if (fs.mkdir(path))
{
  Serial.println("Dir created");
}
else{
  Serial.println("Mkdir failed");
}
}
/*void removeDir(fs::FS &fs, const char * path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}*/
void removeDir(fs::FS &fs,const char* path){
  Serial.printf("Removing Dir:%s\n",path);
  if (fs.rmdir(path))
  {
    Serial.printf("Dir removed");
  }
  else{
    Serial.println("rmdir failed");
  }
  }

void setup()
{
  // put your setup code here, to run once:
}

void loop()
{
  // put your main code here, to run repeatedly:
}