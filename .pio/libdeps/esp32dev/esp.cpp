/*
   Connect the SD card to the following pins:

   SD Card | ESP32
      D2       -
      D3       SS
      CMD      MOSI
      VSS      GND
      VDD      3.3V
      CLK      SCK
      VSS      GND
      D0       MISO
      D1       -
*/
#include "Wire.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
//#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#define OFFSET 0  // us (tient compte du temps d'exécution de updateTime)
#define sleepPeriod 10000000ul  // 10 seconds sleep
MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;
#define OUTPUT_READABLE_ACCELGYRO
esp_sleep_wakeup_cause_t wakeup_reason;
RTC_DATA_ATTR byte bootCount = 0;
RTC_DATA_ATTR time_t now;
RTC_DATA_ATTR uint64_t Mics = 0;
RTC_DATA_ATTR struct tm * timeinfo;
String dosya;
void printLocalTime() {
  time(&now);
  timeinfo = localtime (&now);
  Serial.println (asctime(timeinfo));
  //Serial.printf ("%s\n", asctime(timeinfo));
  delay(2); // 26 bytes @ 115200 baud is less than 2 ms
}
void updateTime (uint64_t elapsedTime) { // elapsedTime in us
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
}

uint32_t baslangic;
uint32_t gecen_zaman;

int ornek_say;
#define timeSeconds 10
#define BUTTON_PIN_BITMASK 0x1000 // 2^33 in hex
//RTC_DATA_ATTR int bootCount = 0;

// Set GPIOs for LED and PIR Motion Sensor
const int vt_pin = 25;
const int uyuma_pin = 26;
const int wake_pin = 12;

const int switch1 = 33;
const int switch2 = 32;
const int switch3 = 35;
const int switch4 = 34;

// Timer: Auxiliary variables
//unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char * path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char * path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  // Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    // Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char * path) {
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}
void uyu() {
  Serial.println("Uyuyor");
  startTimer = true;
  lastTrigger = millis();
}
void uyan() {
  digitalWrite(33, HIGH);
  startTimer = true;
  lastTrigger = millis();
}
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}



void setup() {


  Serial.begin(115200);
  Wire.begin(21, 22);
   Serial.println(dosya);
  while (!Serial); // wait for Leonardo enumeration, others continue immediately
  accelgyro.initialize();
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  // use the code below to change accel/gyro offset values
  /*
    Serial.println("Updating internal sensor offsets...");
    // -76  -2359 1688  0 0 0
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
    accelgyro.setXGyroOffset(220);
    accelgyro.setYGyroOffset(76);
    accelgyro.setZGyroOffset(-85);
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
  */


  pinMode(vt_pin, INPUT_PULLUP);
  pinMode(uyuma_pin, INPUT_PULLUP);
  pinMode(wake_pin, INPUT_PULLUP);

  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  pinMode(switch4, INPUT_PULLUP);



wakeup_reason = esp_sleep_get_wakeup_cause();

if(wakeup_reason==ESP_SLEEP_WAKEUP_EXT0)
{
  Serial.println("Uyandim");
 

}

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0); //1 = High, 0 = Low
  // attachInterrupt(digitalPinToInterrupt(uyuma_pin), uyu, ONLOW);
  // attachInterrupt(digitalPinToInterrupt(wake_pin), uyan, ONLOW);
#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif

  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");

  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  }
  else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void loop() {
   printLocalTime(); // This sets the internal clock
   Serial.println("************");
/*////////////////////////////////////
Sağ ayak:Switch1 ON :
Sol ayak:Switch1 OFF:
          TIRIS RAHVAN  KOŞU    SIĞIR
Switch2   OFF     OFF    ON     ON
Switch3   OFF     ON     OFF    ON
////////////////////////////////////////*/

  if (ornek_say == 0)
  {
    if (digitalRead(switch1) == HIGH)
    {
      Serial.println("SAG");
      appendFile(SD, "/dosya.txt", "\tSAG\t");
    }
    else
    {
      appendFile(SD, "/dosya.txt", "\tSOL\t");
      Serial.println("SOL");
    }
    if (digitalRead(switch2) == LOW && digitalRead(switch3) == LOW)
    {
      Serial.println("TIRIS");
      appendFile(SD, "/dosya.txt", "\t\tTIRIS\n");
    }
    if (digitalRead(switch2) == LOW && digitalRead(switch3) == HIGH)
    {
      Serial.println("RAHVAN");
      appendFile(SD, "/dosya.txt", "\t\tRAHVAN\n");
    }
    if (digitalRead(switch2) == HIGH && digitalRead(switch3) == LOW)
    {
      Serial.println("KOSU");
      appendFile(SD, "/dosya.txt", "\t\tKOSU\n");
    }

    String giris = String("ax") + "," + String("ay") + "," + String("az") + "," + String("gx") + "," + String("gy") + "," + String("gz") + "," + String("gecen_zaman") +
                   "," + String("ornek_say") + "\n";
    int str_len = giris.length() + 1;
    char b[str_len];
    giris.toCharArray(b, str_len);
    appendFile(SD, "/dosya.txt", b);
  }
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  if (digitalRead(25) == LOW)
  {
    Serial.println("pin12 LOW");
    //Go to sleep now
    Serial.println("Going to sleep now");
    esp_deep_sleep_start();
    Serial.println("This will never be printed");

  }
  gecen_zaman = millis() - baslangic;


  // Define
  String str = String(ax) + "," + String(ay) + "," + String(az) + "," + String(gx) + "," + String(gy) + "," + String(gz) + "," + String(gecen_zaman) +
               "," + String(ornek_say) + "\n";
  Serial.println(str);


  // Length (with one extra character for the null terminator)
  int str_len = str.length() + 1;

  // Prepare the character array (the buffer)
  char a[str_len];

  // Copy it over
  str.toCharArray(a, str_len);
  appendFile(SD, "/dosya.txt", a);

  baslangic = millis();
  ornek_say++;
  delay(10);
}