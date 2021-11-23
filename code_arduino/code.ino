#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN 5 // what digital pin we're connected to

#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);
// Rewrite with home wifi
const char* ssid = "admin";
const char* password = "Nhom13";
int RELAYMOTOR = 14; //GPIO14 (D5)
int LED = 4; // GPIO4 (D4)
WiFiServer server(80);

IPAddress staticIP(192,168,1,22);
IPAddress gateway(192,168,1,9);
IPAddress subnet(255,255,255,0);

void setup(){
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYMOTOR, OUTPUT);
  pinMode(LED, OUTPUT);
  if (!WiFi.config(staticIP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  Serial.print("Connecting to the Newtork");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected"); 
  server.begin();
  Serial.println("Server started");
  Serial.print("IP Address of network: "); // will IP address on Serial Monitor
  Serial.println(WiFi.localIP());
  Serial.print("Copy and paste the following URL: https://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop(){
  WiFiClient client = server.available();
  if (!client){
    return;}
    
  Serial.println("Waiting for new client");
  while(!client.available())
  {
    delay(1);
  }
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  
  int value = HIGH;
  if(request.indexOf("/LED=ON") != -1){
    
    digitalWrite(LED, HIGH); // Turn LED ON
    value = HIGH;
  }

  if(request.indexOf("/LED=OFF") != -1){

    digitalWrite(LED, LOW); // Turn LED OFF
    value = LOW;
  } 
 delay(2000);
 float h = dht.readHumidity();
 // Read temperature as Celsius (the default)
 float t = dht.readTemperature();
 // Read temperature as Fahrenheit (isFahrenheit = true)
 float f = dht.readTemperature(true);

// Check if any reads failed and exit early (to try again).
 if (isnan(h) || isnan(t) || isnan(f)) {
 Serial.println("Failed to read from DHT sensor!");
 return;
 }

// Compute heat index in Fahrenheit (the default)
 float hif = dht.computeHeatIndex(f, h);
 // Compute heat index in Celsius (isFahreheit = false)
 float hic = dht.computeHeatIndex(t, h, false);
 
 Serial.print("Humidity: ");
 Serial.print(h);
 Serial.print(" %\t");
 Serial.print("Temperature: ");
 Serial.print(t);
 Serial.print(" *C ");
 Serial.print(f);
 Serial.print(" *F\t");
 Serial.print("Heat index: ");
 Serial.print(hic);
 Serial.print(" *C ");
 Serial.print(hif);
 Serial.println(" *F");
 // Set the h and t reasonable to turn on/off the relay
 if ( h > 50 && t > 34) {
  Serial.print("Turn on Motor\n");
  digitalWrite(RELAYMOTOR, HIGH);
  delay(5000);
  Serial.print("Turn off  Motor\n");
  digitalWrite(RELAYMOTOR, LOW);
  delay(500);
}
  
//*------------------HTML Page Code---------------------*//

  client.println("HTTP/1.1 200 OK"); //
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print(" CONTROL LED: ");
  if(value == HIGH){
    client.print("ON");
  }
  else
  {
    client.print("OFF");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>ON</button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>OFF</button></a><br />");
  client.println("<p>Humidity: ");
  client.println(dht.readHumidity());
  client.println("%</p>");
  client.println("<p>Temperature : ");
  client.println(dht.readTemperature());
  client.println("0C ~ ");
  client.println(dht.readTemperature(true));
  client.println("0F</p>");
  // Set the h and t reasonable
  if ( h > 50 && t > 34) client.println("<p>Too hot and dry so motor set to ON for 5 secs</p>");
  client.println("</html>");
  
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
