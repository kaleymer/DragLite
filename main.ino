#include <TinyGPS++.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17

TinyGPSPlus gps;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  // Display setup
  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (gps.location.isValid()) {
    display.print("Lat: "); display.println(gps.location.lat(), 6);
    display.print("Lng: "); display.println(gps.location.lng(), 6);
  }

  if (gps.speed.isValid()) {
    display.print("Speed: "); display.println(gps.speed.mph());
  }

  display.display();
  delay(100);  // Adjust delay based on the GPS update rate
}
