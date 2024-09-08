DragLite

DragLite is a custom GPS telemetry device, using an ESP32 and NEO-M9N GPS module to track acceleration times, displayed on an OLED screen. Built for car enthusiasts, it actively measures performance metrics like 0-60 times.

Project Timeline

Phase 1: Hardware selection and initial GPS/ESP32 connection testing.
Phase 2: Configured GPS for 10Hz output, extracted speed data, and displayed metrics.
Phase 3: Upgraded to a larger ST7735 display, moved components to a custom PCB, and optimized code.
Components

ESP32
NEO-M9N GPS
ST7735 OLED Screen
Soldered PCB (50mm x 70mm)
Implementation

Integrated hardware using SPI and I2C communication.
Optimized display updates to reduce flicker.
Ensured accurate 0-60 measurements using precise timing.
Results

Device operates with ~97% accuracy for 0-60 times, with room for future improvements.
Future Work

Development of a 3D-printed enclosure with mounting capabilities and added functionality.
Feel free to check out my LinkedIn or email me at kaleweimer@gmail.com for more details!
