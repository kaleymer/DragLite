#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Wire.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

// Define pins for ST7735 display
#define TFT_CS     5    // Chip select pin
#define TFT_RST    17   // Reset pin (or set to -1 and connect to ESP32's RESET pin)
#define TFT_DC     16   // Data/Command pin

// Initialize the ST7735 display
Adafruit_ST7735 display = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// GPS module
SFE_UBLOX_GNSS myGNSS;

unsigned long startTime = 0;
bool timing = false; // timing = true means signals that 0-60 measurement has begun
float lastZeroToSixtyTime = -1.0; // 0-60 time will display -- until first time recorded  
int lastRoundedSpeed = -1; // To track the last displayed speed
int state = 1; // 0 = running, 1 = waiting for gps

void setup() {
  Serial.begin(115200);

  // Initialize the ST7735 display
  display.initR(INITR_BLACKTAB);
  display.fillScreen(ST7735_BLACK);
  display.setRotation(3); // Adjust as needed

  display.setTextSize(2);
  display.setTextColor(ST7735_WHITE);
  display.setCursor(0, 0);
  display.print("Initializing...");

  Wire.begin();  // Initialize I2C for GPS

  // Timeout mechanism for GPS initialization
  unsigned long initStart = millis();
  const unsigned long initTimeout = 5000; // 5 seconds timeout

  while (!myGNSS.begin()) {
    if (millis() - initStart >= initTimeout) {
      Serial.println(F("GPS initialization failed!"));
      display.fillScreen(ST7735_BLACK);
      display.setCursor(0, 0);
      display.setTextSize(2);
      display.print("GPS Init Failed");
      while (1);
    }
    Serial.println(F("Retrying GPS initialization..."));
    delay(500);
  }

  Serial.println(F("GPS module initialized successfully."));

  display.fillScreen(ST7735_BLACK);

}

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastUpdate >= 200) { // Update no more than every 200ms (5 times per second)
    myGNSS.checkUblox();

    if (myGNSS.getGnssFixOk()) {
      double speed = myGNSS.getGroundSpeed() / 1000.0 * 2.23694; // Convert m/s to mph
      if (speed < 1.5) {
        speed = 0; // Eliminate 1mph glitch
      }
      int roundedSpeed = round(speed); // Round speed to the nearest integer
      if (roundedSpeed == 0 && lastRoundedSpeed != 0) {
        timing = false;
        display.fillRect(0, 40, 160, 16, ST7735_BLACK);
        display.setCursor(0, 40);
        display.print("Ready to launch...");
      }

      if (timing) {
        if (roundedSpeed >= 60) {
          unsigned long elapsedTime = millis() - startTime;
          lastZeroToSixtyTime = elapsedTime / 1000.0;
          timing = false;
        }
      } else if (roundedSpeed > 0 && lastRoundedSpeed == 0) {
        startTime = millis();
        timing = true;
      }
      // re write the static text after gps signal lost
      if (state == 1){ 
        display.fillScreen(ST7735_BLACK);
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.print("Speed: ");
        display.setTextSize(1);
        display.setCursor(0, 40);
        if (!timing) display.print("Ready to launch...");
        display.setTextSize(1.7);
        display.setCursor(0, 110);
        display.print("Last 0-60: ");
        state = 0;
      }
      // Update speed display
      if (lastRoundedSpeed != roundedSpeed){
        Serial.println(roundedSpeed);
        Serial.println(lastRoundedSpeed);
        Serial.println("changing speeds");
        display.setCursor(15 * 5, 0); // Move cursor to where the speed number starts
        display.fillRect(15 * 5, 0, 85, 16, ST7735_BLACK); // Clear previous speed display
        display.setTextSize(2);
        display.print(roundedSpeed);
        display.print(" mph");
      }
      if (timing){
          display.setTextSize(1);
          display.fillRect(0, 40, 160, 16, ST7735_BLACK); // Clear "Ready to launch..."
          display.setCursor(0, 40);
          display.print("Time running... ");
          display.print((millis() - startTime)/1000.0, 2);
          display.setTextSize(1.7);
          display.setCursor(0, 110);
          display.print("Last 0-60: ");
      }

      // Update last 0-60 time
      display.setCursor(10 * 6, 110); // Move cursor to where the last 0-60 time starts
      display.fillRect(10 * 6, 110, 60, 16, ST7735_BLACK); // Clear previous last 0-60 time
      display.setTextSize(1.7);
      if (lastZeroToSixtyTime > 0) {
        display.print(lastZeroToSixtyTime, 3);
        display.print("s");
      } else {
        display.print("--");
      }
      lastRoundedSpeed = roundedSpeed;
    } else {
      // Display waiting message
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.fillRect(0, 0, 160, 16, ST7735_BLACK); // Clear previous speed text 
      display.print("Waiting for GPS...");
      state = 1; 
    }

    lastUpdate = currentTime;
  }
}
