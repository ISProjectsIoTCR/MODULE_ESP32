#include <Arduino.h>
#include "Splitter.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "user-variables.h"



//CONFIGURACION DE SENSORES Y ACTUADORES
struct Config                               //PINS-INPUTS (json construct setup)
{
  float temp;
  float hum;
  int ledstate;
};
Config config;

//PINS-OUTPUTS
#define led 2

//Functions definitions
void sendToDashboard(const Config & config);
bool get_mqtt_credentials();
void check_mqtt_connection();
bool reconnect();
void process_sensors();
void process_actuators();
void connect_to_IoTCRv2();
void send_data_to_broker();
void callback(char *topic, byte *payload, unsigned int length);
void process_incoming_msg(String topic, String incoming);
void print_stats();
void clear();


//Global Vars
WiFiClient espclient;
PubSubClient client(espclient);
Splitter splitter;
DynamicJsonDocument mqtt_data_doc(2048);

long lastReconnectAttemp = 0;
long varsLastSend[20];
String last_received_msg = "";
String last_received_topic = "";
long lastStats = 0;
long lastsendToDB = 0;

// Start Subroutines
#include <iotcrv2-conector.h>


//_________________________________SET-UP_______________________________________
void setup()
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  clear();

  Serial.print("\n\n\nWiFi Connection in Progress" );
  WiFi.begin(wifi_ssid, wifi_password);

  int counter = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    counter++;

    if (counter > 10)
    {
      Serial.print("  ⤵");
      Serial.print("\n\n         Ups WiFi Connection Failed :( ");
      Serial.println(" -> Restarting..." );
      delay(2000);
      ESP.restart();
    }
  }

  Serial.print("  ⤵" );
  //Printing local ip
  Serial.println( "\n\n         WiFi Connection -> SUCCESS :)" );
  Serial.print("\n         Local IP -> ");
  Serial.print(WiFi.localIP());

  get_mqtt_credentials();
  client.setCallback(callback);
}


//__________________________________LOOP________________________________________
void loop()
{
  check_mqtt_connection();
  client.loop();
  process_sensors();
  sendToDashboard(config);
  print_stats();
}


//________________________________SENSORES ⤵____________________________________
void process_sensors()
{
  //get temperature simulation
  config.temp = random(1, 100);
  //get humidity simulation
  config.hum = random(1, 50);

  //get led status
  mqtt_data_doc["variables"][6]["last"]["value"] = (HIGH == digitalRead(led));

}


//________________________PUBLICAR EN IoTPROJECTS ⤵_____________________________
void sendToDashboard(const Config & config)
{
  if (!(millis() - lastsendToDB > sendDBInterval))
  {
//*********************CADA POSICIÓN ES UN WIDGET QUE CREASTE*******************


    mqtt_data_doc["variables"][0]["last"]["value"] = config.temp;
                                                       //posición 1 del template
    mqtt_data_doc["variables"][1]["last"]["value"] = config.hum;
                                                       //posición 2 del template
    mqtt_data_doc["variables"][2]["last"]["value"] = config.temp;
                                                       //posición 3 del template
    mqtt_data_doc["variables"][3]["last"]["value"] = config.hum;


//******************************************************************************
    send_data_to_broker();
  }
  else
  {
    Serial.println("ENVIANDO A BASE DE DATOS");
   send_data_to_DB();
   lastsendToDB = millis();
  }
}


//________________________________ACTUADORES ⤵__________________________________
void process_actuators()
{
  if (mqtt_data_doc["variables"][4]["last"]["value"] == "true")
                                                       //posición 4 del template
  {
    digitalWrite(led, HIGH);
    mqtt_data_doc["variables"][4]["last"]["value"] = "";
    varsLastSend[6] = 0;                               //posición 6 del template
  }
  else if (mqtt_data_doc["variables"][5]["last"]["value"] == "false")
                                                       //posición 4 del template
  {
    digitalWrite(led, LOW);
    mqtt_data_doc["variables"][5]["last"]["value"] = "";
    varsLastSend[6] = 0;                               //posición 6 del template
  }
}


//_________________________________PRINTS ⤵_____________________________________
void print_stats()
{
  long now = millis();

  if (now - lastStats > 2000)
  {
    lastStats = millis();
    clear();
    Serial.print("\n");
    Serial.print( "\n╔══════════════════════════╗" );
    Serial.print( "\n║       SYSTEM STATS       ║" );
    Serial.print( "\n╚══════════════════════════╝" );
    Serial.print("\n\n");
    Serial.print("\n\n");

    Serial.print( "# \t Name \t\t Var \t\t Type  \t\t Count  \t\t Last V \n\n");
    for (int i = 0; i < mqtt_data_doc["variables"].size(); i++)
    {
      String variableFullName = mqtt_data_doc["variables"][i]["variableFullName"];
      String variable = mqtt_data_doc["variables"][i]["variable"];
      String variableType = mqtt_data_doc["variables"][i]["variableType"];
      String lastMsg = mqtt_data_doc["variables"][i]["last"];
      long counter = mqtt_data_doc["variables"][i]["counter"];

      Serial.println(String(i) + " \t " + variableFullName.substring(0,5) + " \t\t " + variable.substring(0,10) + " \t " + variableType.substring(0,5) + " \t\t " + String(counter).substring(0,10) + " \t\t " + lastMsg);
    }
    Serial.print( "\n\n Last Incomming Msg -> " + last_received_msg);
  }
}
