#include "Arduino.h"
#include <Wire.h>
#include "MAX30105.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "SparkFunLIS3DH.h"
#include "Wire.h"
#include "SPI.h"
#include <vector>
using namespace std;

String deviceName = "CS244";
LIS3DH myIMU; //Default constructor is I2C, addr 0x19.

// WiFi settings
const char *ssid = "UCInet Mobile Access";

String serverEndPoint = "http://ec2-35-166-1-233.us-west-2.compute.amazonaws.com/server_acc_code.php";
HTTPClient http;    //Declare object of class HTTPClient
int numReadingsPerBatch = 80;

int numSamples = 0;
vector<String> jsonReadings;

void connectToWiFi()
{
    byte mac[6];
    WiFi.macAddress(mac);
    char MAC_char[18]="";

   for (int i = 0; i < sizeof(mac); ++i)
    {
        sprintf(MAC_char, "%s%02x:", MAC_char, mac[i]);
    }

    Serial.print("Mac address : ");
    Serial.print(MAC_char);

    WiFi.begin(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    // Print the IP address
    Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Program started");
  connectToWiFi();
  //Call .begin() to configure the IMU
  myIMU.begin();

  while(1) {
    // Create JSON for X,Y and Z values
    String xval = "{\"x\":[";
    String yval = "\"y\":[";
    String zval = "\"z\":[";
        
    int currentSample = 0;
    int maxSample = 100;
          
    while(currentSample < maxSample-1) {   
      xval += myIMU.readFloatAccelX();
      xval += ",";
      yval += myIMU.readFloatAccelY();
      yval += ",";
      zval += myIMU.readFloatAccelZ();
      zval += ",";
      currentSample++;
      // Delay of 20msec - 50Hz sampling frequency
      delay(20);
    }
    xval += myIMU.readFloatAccelX();
    xval += "],";
    yval += myIMU.readFloatAccelY();
    yval += "],";
    zval += myIMU.readFloatAccelZ();
    zval += "]}";
    xval += yval;
    xval += zval;

    HTTPClient http;    //Declare object of class HTTPClient
       
    http.begin("http://ec2-35-167-126-243.us-west-2.compute.amazonaws.com/acc.php");      //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header
       
    int httpCode = http.POST(xval);   //Send the request
    String payload = http.getString();   //Get the response payload
      
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
       
    http.end();  //Close connection
  }
}

void loop()
{
  // Nothing to be callend in loop
}