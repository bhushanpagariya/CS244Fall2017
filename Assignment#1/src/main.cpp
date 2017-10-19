#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "string"
 using namespace std;

String deviceName = "CS244";

int curOffset = 0;

// WiFi settings
const char *ssid = "UCInet Mobile Access";
const char* host = "ec2-35-167-126-243.us-west-2.compute.amazonaws.com";

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

    WiFi.begin(ssid); //, password);
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
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
    
  Serial.begin(115200);
  Serial.println("Program started");
  printMacAddress();
  
}

void loop()
{

  // You can write your logic here
  // Below are the LED samples from the hands-on

  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);
  // wait for a second
  delay(1000);
  // turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);
   // wait for a second
  delay(1000);

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  // Sending Name as UCI and age as a counter
  String url = "/main.php?name=UCI&age=";
  url += curOffset;
  curOffset++;
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" + 
  "Connection: close\r\n\r\n");

}