# MODULE_ESP32
Base de código para ESP32 compatible con la plataforma IoTProjects. 

¿Para que es esto?

IoTProjects es una plataforma en la Nube para microcontroladores Arduino, ESP y cualquier dispositivo que soporte el protocolo MQTT.
Su principal cualidad es que no depende de ningún servicio de terceros como Adafruit, Ubidots, Cayenne u otro.

Aquí un DEMO: https://app.iotcostarica.ml/demologin


¿Como funciona?

PASO_1:  Ingrese a su cuenta en IoTProjects y comience a crear el TEMPLATE de su proyecto

PASO_2: Agrege un dispositivo y seleccione el template creado. Automaticamente la plataforma le generará un PASSWORD de conexión.

PASO_3: Descargue este proyecto y en "USER-VARIABLES" complete la información correspondiente a WIFI, IoTPROJECTS

PASO_4: Gestione las lecturas de sus sensores con un constructor Json y envíelo como parametro a la función sendToDashboard(config);

NOTA: ESTE PROYECTO YA INCLUYE DOS WIDGETS INPUT Y WIDGETS OUTPUTS COMO BASE DE APRENDIZAJE

