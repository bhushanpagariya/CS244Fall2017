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
MAX30105 particleSensor;

// WiFi settings
const char *ssid = "AndroidAP";

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

    WiFi.begin(ssid, "bhushan123");
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

  // Initializing Accelerometer
  //Call .begin() to configure the IMU
  //Accel sample rate and range effect interrupt time and threshold values!!!
  myIMU.settings.accelSampleRate = 50;  //Hz.  Can be: 0,1,10,25,50,100,200,400,1600,5000 Hz
  myIMU.settings.accelRange = 2;      //Max G force readable.  Can be: 2, 4, 8, 16
  myIMU.settings.adcEnabled = 0;
  myIMU.settings.tempEnabled = 0;
  myIMU.settings.xAccelEnabled = 1;
  myIMU.settings.yAccelEnabled = 1;
  myIMU.settings.zAccelEnabled = 1;
  myIMU.begin();

  // Initializing MAX30105
  byte ledBrightness = 0x1F; //Options: 0=Off to 255=50mA
  byte sampleAverage = 1; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 3; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 50; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

  while(1) {
    // Create JSON for X,Y and Z values
    String xval = "{\"x\":[";
    String yval = "\"y\":[";
    String zval = "\"z\":[";
    String ir = "{\"ir\":[";
    String red = "\"red\":[";
    
    int currentSample = 0;
    int maxSample = 100;
          
    while(currentSample < maxSample-1) {   
      // Fetch accelerometer values
      xval += myIMU.readFloatAccelX();
      xval += ",";
      yval += myIMU.readFloatAccelY();
      yval += ",";
      zval += myIMU.readFloatAccelZ();
      zval += ",";
      // Fetch IR and RED values from MAX30105 sensor
      ir += particleSensor.getIR();
      ir += ",";
      red += particleSensor.getRed();
      red += ",";    
      currentSample++;
    }
    xval += myIMU.readFloatAccelX();
    xval += "],";
    yval += myIMU.readFloatAccelY();
    yval += "],";
    zval += myIMU.readFloatAccelZ();
    zval += "],";
    ir += particleSensor.getIR();
    ir += "],";
    red += particleSensor.getRed();
    red += "]}";
    xval += yval;
    xval += zval;
    xval += ir;
    xval += red;

    HTTPClient http;    //Declare object of class HTTPClient
       
    http.begin("http://ec2-35-167-126-243.us-west-2.compute.amazonaws.com/main.php");      //Specify request destination
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