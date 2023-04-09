#include <heltec.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <bits/error_constants.h>

// Replace with your network credentials
const char* ssid = "Mohawk Private_IoT";
const char* password = "l@zyButton19";

// USGS Water Services API URL and station ID
const char* url = "https://waterservices.usgs.gov/nwis/iv/?sites=02169000&parameterCd=00060&siteStatus=all&format=json";

void WIFISetUp(void)
{
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.begin("Mohawk Guest_IoT","$picyDrum40");
  delay(100);

  byte count = 0;
  while(WiFi.status() != WL_CONNECTED && count < 10)
  {
    count ++;
    delay(500);
    Heltec.display -> drawString(0, 0, "Connecting...");
    Heltec.display -> display();
  }

  Heltec.display -> clear();
  if(WiFi.status() == WL_CONNECTED)
  {
    Heltec.display -> drawString(0, 0, "Connecting...OK.");
    Heltec.display -> display();
//    delay(500);
  }
  else
  {
    Heltec.display -> clear();
    Heltec.display -> drawString(0, 0, "Connecting...Failed");
    Heltec.display -> display();
//    while(1);
  }
  Heltec.display -> drawString(0, 10, "WIFI Setup done");
  Heltec.display -> display();
  delay(500);
}

void RiverData(void) 
{
  // Send HTTP GET request to USGS Water Services API
  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();

  // Parse JSON response and extract streamflow data
  if (httpResponseCode == 200) 
  {
    String response = http.getString();
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, response);

    String flow = doc["value"]["timeSeries"][0]["values"][0]["value"][0]["_value"];
    String unit = doc["value"]["timeSeries"][0]["variable"]["unit"]["unitCode"];

    // Clear the display
    Heltec.display->clear();

    // Draw the streamflow data on the display
    Heltec.display->setFont(ArialMT_Plain_16);
    Heltec.display->drawString(0, 0, "Streamflow:");
    Heltec.display->setFont(ArialMT_Plain_24);
    Heltec.display->drawString(0, 20, String(flow));
    Heltec.display->setFont(ArialMT_Plain_16);
    Heltec.display->drawString(85, 30, unit);

    // Update the display
    Heltec.display->display();
  }
  else
  {
    Serial.print("Error");
    Serial.println(httpResponseCode);
  }
}

void setup()
{
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);
  
  WIFISetUp();
}

void loop()
{
  RiverData();
  delay(2000);
}
