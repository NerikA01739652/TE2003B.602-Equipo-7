#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// ==========================================================
//                  CONFIGURACIÓN WIFI Y MQTT
// ==========================================================

const char* ssid             = "qwerty";
const char* password         = "12345678";

const char* mqtt_server      = "192.168.137.73";//192.168.137.31
const char* mqtt_user        = "user";
const char* mqtt_password    = "12345678";

const char* motor_data_topic = "motor/data";

const char* motor_control_topic = "motor/control";

// ==========================================================
//                  UART STM32 -> ESP8266
// ==========================================================

// RX, TX
SoftwareSerial stmSerial(D5, D6);

// ==========================================================
//                    OBJETOS GLOBALES
// ==========================================================

WiFiClient espClient;
PubSubClient client(espClient);

// ==========================================================
//                  VARIABLES GLOBALES
// ==========================================================

float rpm = 0;
float vel = 0;
int marcha = 0;

// Últimos valores válidos
float rpm_last = 0;
float vel_last = 0;
int marcha_last = 0;

String serialData = "";

// ======================================
// VARIABLES DE MODO REMOTO
// ======================================

int ctrl_avanzar = 0;
int ctrl_frenar  = 0;
int ctrl_modo    = 0;

// ==========================================================
//                  CONEXIÓN WIFI
// ==========================================================

void setup_wifi() {

    delay(10);

    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {

        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi conectado");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

}

// ==========================================================
//                 RECONEXIÓN MQTT
// ==========================================================

void reconnect() {

    while (!client.connected()) {

        Serial.print("Conectando MQTT...");

        if (client.connect(
                "ESP8266Client",
                mqtt_user,
                mqtt_password
            )) {

            Serial.println(" conectado");
            client.subscribe(motor_control_topic);
            
        } else {

            Serial.print(" fallo rc=");
            Serial.print(client.state());

            Serial.println(" reintentando en 2 segundos");

            delay(2000);
        }
    }
}

// ==========================================================
//                 RECEPCIÓN MQTT y TRANSMISIÓN UART
// ==========================================================

void callback(char* topic, byte* payload, unsigned int length)
{
    Serial.print("Mensaje recibido en: ");
    Serial.println(topic);

    String content = "";

    for (unsigned int i = 0; i < length; i++)
    {
        content += (char)payload[i];
    }

    Serial.println(content);

    // ======================================
    // JSON
    // ======================================

    JsonDocument doc;

    DeserializationError error =
        deserializeJson(doc, content);

    if (error)
    {
        Serial.println("Error JSON");
        return;
    }

    int avanzar = doc["avanzar"];
    int frenar  = doc["frenar"];
    int modo    = doc["modo"];

    Serial.println("MQTT REMOTO RPI:");

    Serial.print(avanzar);
    Serial.print(",");
    Serial.print(frenar);
    Serial.print(",");
    Serial.println(modo);

    // ======================================
    // ENVIAR A STM32
    // ======================================

    stmSerial.print(avanzar);

    stmSerial.print(",");

    stmSerial.print(frenar);

    stmSerial.print(",");

    stmSerial.println(modo);
}

// ==========================================================
//                         SETUP
// ==========================================================

void setup() {

    // Monitor Serial PC
    Serial.begin(9600);

    // UART STM32
    stmSerial.begin(9600);

    Serial.println();
    Serial.println("=================================");
    Serial.println(" ESP8266 MQTT Bridge Iniciado ");
    Serial.println("=================================");

    setup_wifi();

    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

// ==========================================================
//                 LEER DATOS STM32
// ==========================================================

void recibirDatosSTM32() {

    while (stmSerial.available()) {

        char c = stmSerial.read();

        // Fin de línea
        if (c == '\n') {

            serialData.trim();

            // ------------------------------------------
            // Formato esperado:
            // rpm,vel,marcha
            // Ejemplo:
            // 3500,42,3
            // ------------------------------------------

            int firstComma  = serialData.indexOf(',');
            int secondComma = serialData.indexOf(',', firstComma + 1);

            if (firstComma > 0 && secondComma > 0) {

                String rpmStr =
                    serialData.substring(0, firstComma);

                String velStr =
                    serialData.substring(
                        firstComma + 1,
                        secondComma
                    );

                String marchaStr =
                    serialData.substring(secondComma + 1);

                // Convertir UNA sola vez
                float rpmTemp = rpmStr.toFloat();
                float velTemp = velStr.toFloat();
                int marchaTemp = marchaStr.toInt();

                // Validar rangos
                if ((rpmTemp >= 0 && rpmTemp <= 6000) &&
                    (velTemp >= 0 && velTemp <= 200) &&
                    (marchaTemp >= 0 && marchaTemp <= 4)) {

                    rpm = rpmTemp;
                    vel = velTemp;
                    marcha = marchaTemp;

                    // Guardar últimos válidos
                    rpm_last = rpm;
                    vel_last = vel;
                    marcha_last = marcha;

                } else {

                    Serial.println("Dato fuera de rango");

                    // Mantener últimos válidos
                    rpm = rpm_last;
                    vel = vel_last;
                    marcha = marcha_last;
                }

                Serial.println("--------------------------------");
                Serial.println("Datos recibidos STM32:");

                Serial.print("RPM: ");
                Serial.println(rpm);

                Serial.print("Velocidad: ");
                Serial.println(vel);

                Serial.print("Marcha: ");
                Serial.println(marcha);

                // ======================================
                // CREAR JSON
                // ======================================

                JsonDocument jsonDocument;

                jsonDocument["rpm"]    = rpm;
                jsonDocument["vel"]    = vel;
                jsonDocument["marcha"] = marcha;

                String jsonString;

                serializeJson(jsonDocument, jsonString);

                // ======================================
                // PUBLICAR MQTT
                // ======================================

                bool status = client.publish(
                    motor_data_topic,
                    jsonString.c_str()
                );

                if (status) {

                    Serial.println("MQTT enviado:");
                    Serial.println(jsonString);

                } else {

                    Serial.println("Error al publicar MQTT");
                }
            }

            serialData = "";

        } else {

            serialData += c;
        }
    }
}

// ==========================================================
//                          LOOP
// ==========================================================

void loop() {

    // ----------------------------------------------
    // Verificar WiFi
    // ----------------------------------------------

    if (WiFi.status() != WL_CONNECTED) {

        Serial.println("WiFi desconectado");
        setup_wifi();
    }

    // ----------------------------------------------
    // Verificar MQTT
    // ----------------------------------------------

    if (!client.connected()) {

        reconnect();
    }

    client.loop();

    // ----------------------------------------------
    // Leer STM32
    // ----------------------------------------------
    
    recibirDatosSTM32();
}