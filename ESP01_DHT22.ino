

/* Domotics JSON URLs: https://www.domoticz.com/wiki/Domoticz_API/JSON_URL's

DTH demo with ESP8266-01 connected to Domoticz

You need to connect ESP01 pin 8 (GPIO16) to reset!

Modify domoticz IP+IDX and flash ESP; Power it on and connect to "SetupSensor" Wifi @ 192.168.4.1 ; Enter Wifi SSID+Key; Have fun

P.Jeschofnik 11/2021

some code snippets are from the Inet :)
https://www.domoticz.com/wiki/ESP8266_WiFi_module
*/

#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>



///////////////////////////////////////////////////////////////////////////


const char * domoticz_server = "192.168.0.34"; //Domoticz ip
int port = 80; //Domoticz port

int idx = 61; //IDX for temp+hum  --- virtual Temp + Humidity

#define sleeptime 300e6 //deepsleep time between updates (here 300 seconds)

#define tempcorr -0.8 //temperaturecorrection for DHT22 if needed (set this to 0 if not needed)


////////////////////////////////////////////////////////////////////////////


DHT dht(2, DHT22); //init DHT
WiFiClient client;
WiFiManager wifiManager;

float humidity = 0;
float temp = 0;

unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

void setup()
{
  
  wifiManager.autoConnect("SetupSensor"); //hotspotname for firststart
  
  dht.begin();
  Serial.begin(9600);
   
}

void loop()
{
unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= 1000)
  {
    previousMillis = currentMillis;
    
    getSensor(); //get Sensordata
  printInfo(); //print sensordata to domoticz
  }
 
  unsigned long currentMillis2 = millis();
  
  if (currentMillis2 - previousMillis2 >= 3000)
  {
    previousMillis2 = currentMillis2;
    ESP.deepSleep(sleeptime); 
  }
  
  //}
  
}

void getSensor()
{
  
    humidity = dht.readHumidity();
    temp = dht.readTemperature() + tempcorr;
    Serial.println(humidity);
    Serial.println(temp);
  
    if(temp>1000){ 
      temp=(temp-6553.6);
    }
}

void printInfo()
{
  // Domoticz format /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TEMP;HUM;HUM_STAT

  if (client.connect(domoticz_server, port)) {

    client.print("GET /json.htm?type=command&param=udevice&idx=");
    client.print(idx);
    client.print("&nvalue=0&svalue=");
    client.print(temp);
    client.print(";");
    client.print(humidity);
    client.print(";0"); //Value for HUM_STAT. Can be one of: 0=Normal, 1=Comfortable, 2=Dry, 3=Wet
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.print(domoticz_server);
    client.print(":");
    client.println(port);
    client.println("User-Agent: Arduino-ethernet");
    client.println("Connection: close");
    client.println();

  }


}
