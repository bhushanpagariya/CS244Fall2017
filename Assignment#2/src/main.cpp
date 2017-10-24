#include "Arduino.h"
#include <Wire.h>
#include "MAX30105.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WiFi settings
const char *ssid = "UCInet Mobile Access";

const char* host = "ec2-35-167-126-243.us-west-2.compute.amazonaws.com";

MAX30105 particleSensor;

void printMacAddress()
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

  printMacAddress();
  

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }

  //Setup to sense a nice looking saw tooth on the plotter
  byte ledBrightness = 0x1F; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 3; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 50; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings

  //Arduino plotter auto-scales annoyingly. To get around this, pre-populate
  //the plotter with 500 of an average reading from the sensor

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


for(int k = 0; k<60; k++) {
      
  String ir = "{\"ir\":[";
  String red = "\"red\":[";
      
  int currentSample = 0;
  int maxSample = 100;
          
  Serial.println("Start");
  while(currentSample < maxSample-1) {
    if(currentSample%100 == 0)
      Serial.println(currentSample);
  
      ir += particleSensor.getIR();
      ir += ",";
      red += particleSensor.getRed();
      red += ",";
      currentSample++;
    }
    Serial.println("Done");
    ir += particleSensor.getIR();
    ir += "],";
    red += particleSensor.getRed();
    red += "]}";
    ir += red;
    
    //Declare object of class HTTPClient
    HTTPClient http;    
       
    //Specify request destination
    http.begin("http://ec2-35-167-126-243.us-west-2.compute.amazonaws.com/main.php");
    //Specify content-type header
    http.addHeader("Content-Type", "application/json");
       
    Serial.println("After begin");
    //Send the request
    int httpCode = http.POST(ir);
    //Get the response payload
    String payload = http.getString();
    //Print HTTP return code
    Serial.println(httpCode);
    //Print request response payload
    Serial.println(payload);
    //Close connection
    http.end();
  }
}

void loop()
{
}
