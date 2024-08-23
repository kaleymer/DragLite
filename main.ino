#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SFE_UBLOX_GNSS myGNSS;
unsigned long startTime = 0;
bool timing = false;
float lastZeroToSixtyTime = -1.0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Initializing...");
  display.display();

  Serial.println(F("Initializing GPS module..."));
  
  // Timeout mechanism
  unsigned long initStart = millis();
  const unsigned long initTimeout = 5000; // 5 seconds timeout
  
  while (!myGNSS.begin()) {
    if (millis() - initStart >= initTimeout) {
      Serial.println(F("GPS initialization failed!"));
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("GPS Init Failed");
      display.display();
      while (1);
    }
    Serial.println(F("Retrying GPS initialization..."));
    delay(500);
  }

  Serial.println(F("GPS module initialized successfully."));
  
  display.clearDisplay();
  display.display();
}

void loop() {
  myGNSS.checkUblox();

  if (myGNSS.getGnssFixOk()) {
    double speed = myGNSS.getGroundSpeed() / 1000.0 * 2.23694; // Convert m/s to mph
    if (speed < 1.5) speed = 0; // Eliminate 1mph glitch
    int roundedSpeed = round(speed); // Round speed to the nearest integer

    if (timing) {
      if (roundedSpeed >= 60) {
        unsigned long elapsedTime = millis() - startTime;
        lastZeroToSixtyTime = elapsedTime / 1000.0;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("0-60 Time: ");
        display.print(lastZeroToSixtyTime, 3);
        display.print(" sec");
        timing = false;
      }
    } else if (roundedSpeed > 0) {
      startTime = millis();
      timing = true;
    }

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Speed: ");
    display.print(roundedSpeed);
    display.println(" mph");

    if (roundedSpeed == 0) {
      display.setCursor(0, 20);
      display.print("Ready to launch...");
    }

    // Display last 0-60 time at the bottom
    display.setCursor(0, 56);
    display.print("Last 0-60: ");
    if (lastZeroToSixtyTime > 0) {
      display.print(lastZeroToSixtyTime, 3);
      display.println(" sec");
    } else {
      display.println("--");
    }
  } else {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Waiting for GPS ...");
  }

  display.display();
  delay(25);
}
