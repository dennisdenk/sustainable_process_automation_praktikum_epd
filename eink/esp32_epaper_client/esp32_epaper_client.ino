// Based on:

/* Waveshare Library epd1in54_V2
 * changed by Andreas Wolter 29.09.2020
 *
 * Demo copyright Waveshare 2019
 *
 * changes:	- ESP32 / ESP8266 compatibility
 *			- epd Constructor changed (input own pin numbers)
 */ 

#include <SPI.h>
#include "epd1in54_V2.h"
#include "epdpaint.h"
#include <stdio.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR SSID";
const char* password = "YOUR PASSWORD";
// const char* serverName = "https://fc48-213-147-125-202.ngrok-free.app/convert";
const char* serverName = "http://173.212.218.68:5000/convert2";

const char* imageUrl = "/image";

/* Info: For connecting DIN / CLK pins to MOSI / SCK pins check datasheed and pinout
 * For example:
 * ESP32 MOSI = 23, SCK = 18
 * ESP8266 MOSI = D7 (GPIO 13), SCK = D5 (GPIO 14)
 * Arduino Uno MOSI = 11, SCK = 13
 */

// Uncommend the Line with board configuration of your choice
// (On D1 Mini ESP8266 Pin D4 (GPIO2) prevents upload to the board)
// EPD epd(RESET PIN, DC PIN, CS PIN, BUSY PIN);
//
//Epd epd(5, 0, SS, 4); 		// my Pins ESP8266 (D1, D3, D8, D2)
Epd epd(33, 25, 26, 27); 		// my Pins ESP32 (Reset, DC, CS, Busy)
//Epd epd; 						// default Pins: Reset = 8, DC = 9, CS = 10, Busy 7

unsigned char image[1024];
Paint paint(image, 0, 0);

unsigned long time_start_ms;
unsigned long time_now_s;
#define COLORED     0
#define UNCOLORED   1

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("e-Paper init and clear");
  epd.LDirInit();
  epd.Clear();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  paint.SetWidth(200);
  paint.SetHeight(24);

  Serial.println("e-Paper paint");
  paint.Clear(COLORED);
  paint.DrawStringAt(30, 4, "Welcome", &Font16, UNCOLORED);
  epd.SetFrameMemory(paint.GetImage(), 0, 10, paint.GetWidth(), paint.GetHeight());

  Serial.println("END OF SETUP");
}

String httpGETRequest(const char* serverName) {
  HTTPClient http;

  http.begin(serverName);
  int httpResponseCode = http.GET();

  String payload = "{}"; 

  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED) {

    String imageData = httpGETRequest(serverName);

    // Parse JSON data
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, imageData);

    // Access the matrix array
    JsonArray matrix = doc["matrix"];

    // Iterate over the matrix
    paint.SetWidth(200);
    paint.SetHeight(200);
    paint.Clear(UNCOLORED);
    int rowNr = 0;
    for (JsonArray row : matrix) {
      int valNr = 0;
      for (int val : row) {
        valNr = valNr +1;
        Serial.print(val);
        paint.DrawAbsolutePixel(rowNr, valNr, val);
        Serial.print(" ");
      }
        rowNr = rowNr + 1;
        Serial.println();
      }

      epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
      epd.DisplayFrame();
    } else {
      Serial.println("Error on HTTP request");
    }
  
  // delay(300000);
  delay(60000);
}
