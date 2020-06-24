#include <FirebaseArduino.h>
#include "WiFiManager.h"
#include <MQTTClient.h>
#include "DHT.h"
#include "NTPClient.h"
#include "WiFiUdp.h"

#define FIREBASE_HOST "takwamaryam.firebaseio.com"               // the project name address from firebase id
#define FIREBASE_AUTH "x2R3RhahnLh8ofmWzRw9wihETE44pzJLGd2zeqrh" // the secret key generated from firebase


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

WiFiManager wifiManager;
WiFiClient net;
MQTTClient client;


DHT dht13(13, DHT11);

String fireStatus;
String string_MQTT1;
String string_MQTT2;
unsigned long lastMillis;
char ssid[20] = "";
char pass[20] = "";



void setup() {
  Serial.println("Connected");
  Serial.println("Your IP is");
  Serial.println((WiFi.localIP().toString()));

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  wifiManager.autoConnect("WIFI_takwa");
  Serial.println("Sucessefully connected to WiFi :");
  client.begin("broker.hivemq.com", net);  // client MQTT
  client.onMessage(messageReceived);
  Serial.print("\nconnecting...");
  Serial.println( WiFi.SSID());
  WiFi.SSID().toCharArray(ssid, 20);
  Serial.print("ssid read: ");
  Serial.println(ssid);
  WiFi.psk().toCharArray(pass, 20);
  Serial.print("pass read: ");
  Serial.println(pass);
  Serial.println("connected to wifi");
  pinMode(2, OUTPUT);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Connected to wifi");
  printWifiStatus();
  Serial.println("\n connected!");
  client.subscribe("/Relay1");

  timeClient.begin();
  timeClient.setTimeOffset(3600);
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }

}

// the loop function runs over and over again forever
void loop() {


  client.loop();
  if (!client.connected())
    {
    client.disconnect();
    client.unsubscribe("/Test_LED");
    client.begin("broker.hivemq.com", net);
    while (!client.connect("PFA2")) {
      Serial.print("connected to cloud");
      delay(1000);                                        // wait 10 seconds for connection:
    }

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);         // connect to firebase
    Firebase.setString("led1", "100");                    //send initial string of led
    fireStatus = Firebase.getString("led1");              // get ld status input from firebase
    Serial.println(fireStatus);

    client.subscribe("/Test_LED");

    delay(1000);
  }

  if (millis() - lastMillis > 2000) {

    lastMillis = millis();


    StaticJsonBuffer<300> JSONbuffer;
    JsonObject& JSONencoder = JSONbuffer.createObject();


    JsonArray& temps = JSONencoder.createNestedArray("temps");
    JsonArray& temperature = JSONencoder.createNestedArray("Température");
    JsonArray& humidity = JSONencoder.createNestedArray("Humidity");
    
    temps.add(timeClient.getFormattedDate() );
    temperature.add((dht13.readTemperature( )));
    humidity.add((dht13.readHumidity( )));

    //values.add(23);
    //***********************
    Serial.println("Température");
    temperature.printTo(Serial);
    delay(10000);
    Serial.println("Humidity");
    humidity.printTo(Serial);
    //***********************
    delay(4000);

 char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Firebase.pushString("Base Des Mesures", JSONmessageBuffer);
  }
}

void messageReceived(String &topic, String &payload)
{
  string_MQTT1 = payload;
}

void printWifiStatus() 
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  WiFi.SSID().toCharArray(ssid, 20);
  Serial.print("ssid read: ");
  Serial.println(ssid);
  WiFi.psk().toCharArray(pass, 20);
  Serial.print("pass read: ");
  Serial.println(pass);

  // print your WiFi device's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

}
