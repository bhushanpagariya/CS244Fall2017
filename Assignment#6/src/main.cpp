#include "Arduino.h"
#include <Wire.h>
#include "MAX30105.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "SparkFunLIS3DH.h"
#include "SPI.h"

// WiFi settings
const char *ssid = "UCInet Mobile Access";

const char* host = "ec2-35-167-126-243.us-west-2.compute.amazonaws.com";

MAX30105 particleSensor;
LIS3DH myIMU; //Default constructor is I2C, addr 0x19.

void initAccelerometer()
{
  myIMU.settings.accelSampleRate = 50;  //Hz.  Can be: 0,1,10,25,50,100,200,400,1600,5000 Hz
  myIMU.settings.accelRange = 2;      //Max G force readable.  Can be: 2, 4, 8, 16

  myIMU.settings.adcEnabled = 0;
  myIMU.settings.tempEnabled = 0;
  myIMU.settings.xAccelEnabled = 1;
  myIMU.settings.yAccelEnabled = 1;
  myIMU.settings.zAccelEnabled = 1;
  myIMU.begin();
}

void initParticleSensor()
{
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }

  //Setup to sense a nice looking saw tooth on the plotter
  byte ledBrightness = 0x1F; //Options: 0=Off to 255=50mA
  byte sampleAverage = 1; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 3; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 50; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
  particleSensor.setSampleRate(0x00);

    //Take an average of IR readings at power up
  const byte avgAmount = 64;
  long baseValue = 0;
  for (byte x = 0 ; x < avgAmount ; x++)
  {
    //Read the IR value
    baseValue += particleSensor.getIR(); 
  }
  baseValue /= avgAmount;

  //Pre-populate the plotter so that the Y scale is close to IR values
  for (int x = 0 ; x < 500 ; x++)
    Serial.println(baseValue);

}

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
    // WiFi.begin(ssid);
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
  Serial.println("Initializing...");

  connectToWiFi();
  
  // Init Particle Sensor
  initParticleSensor();
  // Init LIS3DH Sensor
  initAccelerometer();

  char * tempStr = (char *)malloc(15000);
  uint32_t ir_val;
  uint32_t red_val;
  int maxSample = 150;
  int index = 0;
  while(index<200){

    int currentSample = 0;
    int start = 0;
    
    while(currentSample < maxSample) {
      particleSensor.getIRRed(ir_val, red_val);
      sprintf(tempStr+start, "%d,%d,", ir_val, red_val);
      while(tempStr[start]!='\0')
        start++;

      String temp = (String(myIMU.readFloatAccelX())+","+myIMU.readFloatAccelY()+","+myIMU.readFloatAccelZ()+",");
      temp.toCharArray(tempStr+start, temp.length()+1);
      start+=temp.length();
      currentSample++;
    }
    index++;
    HTTPClient http;    
    http.begin("http://ec2-35-167-126-243.us-west-2.compute.amazonaws.com/nojson.php");
    http.addHeader("Content-Type", "text/plain");

    int httpCode = http.POST(tempStr);
  Serial.println(httpCode);
    http.end();
  }
  Serial.println("Done");
}

void loop()
{
}
