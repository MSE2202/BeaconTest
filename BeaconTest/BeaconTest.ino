// 
// MSEduino Beacon Test
// 
//  Language: Arduino (C++)
//  Target:   ESP32-S3
//  Author:   Michael Naish
//  Date:     2023 12 19 
//
//  To program and use ESP32-S3
//   
//  File->Preferences:
//    Additional Boards Manager URLs: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
//
//  Tools->:
//    Board: "Adafruit Feather ESP32-S3 No PSRAM"
//    Upload Speed: "921600"
//    USB CDC On Boot: "Enabled"
//    USB Firmware MSC on Boot: "Disabled"
//    USB DFU On Bot: "Disabled"
//    Upload Mode:"UART0/Hardware CDC"
//    SPU Frequency: "240MHz (WiFi)"
//    Flash Mode: "QIO 80MHz"
//    Flash SIze: "4MB (32Mb)"
//    Partition Scheme: "Default 4MB with spiffs (1.2MB app/1.5MB SPIFFS)"
//    Core Debug Level: "Verbose"
//    PSRAM: 'Disabled"
//    Arduino Runs On: "Core 1"
//    Events Run On: "Core 1"
//
//  To program, press and hold the reset button then press and hold program button, release the reset 
//  button then release the program button 
//

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Function declarations
void doHeartbeat();

// Constants
const int cHeartbeatInterval = 75;                 // heartbeat update interval, in milliseconds
const int cSmartLED          = 21;                 // When DIP switch S1-4 is on, SMART LED is connected to GPIO21
const int cSmartLEDCount     = 1;                  // Number of Smart LEDs in use
const int cIRDetector        = 9;                  // IR detector input on GPIO9

// Variables
boolean heartbeatState       = true;               // state of heartbeat LED
unsigned long lastHeartbeat  = 0;                  // time of last heartbeat state change
unsigned long curMillis      = 0;                  // current time, in milliseconds
unsigned long prevMillis     = 0;                  // start time for delay cycle, in milliseconds

// Declare SK6812 SMART LED object
//   Argument 1 = Number of LEDs (pixels) in use
//   Argument 2 = ESP32 pin number 
//   Argument 3 = Pixel type flags, add together as needed:
//     NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//     NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//     NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//     NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//     NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel SmartLEDs(cSmartLEDCount, cSmartLED, NEO_RGB + NEO_KHZ800);

// Smart LED brightness for heartbeat
unsigned char LEDBrightnessIndex = 0; 
unsigned char LEDBrightnessLevels[] = {0, 0, 0, 5, 15, 30, 45, 60, 75, 90, 105, 120, 135, 
                                       150, 135, 120, 105, 90, 75, 60, 45, 30, 15, 5, 0};

void setup() {
  Serial.begin(9600);                              // initialize serial output

  // Set up SmartLED
  SmartLEDs.begin();                               // initialize smart LEDs object
  SmartLEDs.clear();                               // clear pixel
  SmartLEDs.setPixelColor(0, SmartLEDs.Color(0,0,0)); // set pixel colours to black (off)
  SmartLEDs.setBrightness(0);                      // set brightness [0-255]
  SmartLEDs.show();                                // update LED

  // initialize serial input for IR detector: 1200 baud, 8 data bits, no parity bit, 1 stop bit
  Serial2.begin(1200, SERIAL_8N1, cIRDetector);
}

void loop() {
  char IRReceivedData = 0;                         // data from IR detector

	if (Serial2.available() > 0) {                   // if data is received
	  IRReceivedData = Serial2.read();               // read character
    Serial.printf("%c\n", IRReceivedData);         // print character to serial output
  }

  doHeartbeat();                                   // blink heartbeat LED
}

// update heartbeat LED
void doHeartbeat() {
  curMillis = millis();                            // get the current time in milliseconds
  // check to see if elapsed time matches the heartbeat interval
  if ((curMillis - lastHeartbeat) > cHeartbeatInterval) {
    lastHeartbeat = curMillis;                     // update the heartbeat time for the next update
    LEDBrightnessIndex++;                          // shift to the next brightness level
    if (LEDBrightnessIndex > sizeof(LEDBrightnessLevels)) { // if all defined levels have been used
      LEDBrightnessIndex = 0;                      // reset to starting brightness
    }
    SmartLEDs.setBrightness(LEDBrightnessLevels[LEDBrightnessIndex]); // set brightness of heartbeat LED
    SmartLEDs.setPixelColor(0, SmartLEDs.Color(0, 250, 0)); // set pixel colours to green
    SmartLEDs.show();                              // update LED
  }
}