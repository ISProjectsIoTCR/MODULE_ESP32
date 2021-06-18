#include <Arduino.h>

//CONEXION CON IoTPROJECTS
String dId = " ";
String webhook_pass = " ";
String webhook_endpoint = "http://3.142.89.107:3001/api/getdevicecredentials";
const char *mqtt_server= "app.iotcostarica.ml";


//CONFIGURACION DE WiFi
const char *wifi_ssid = "IoTCR_Wifi";
const char *wifi_password = "- - - -";

const long sendDBInterval = 300000;
