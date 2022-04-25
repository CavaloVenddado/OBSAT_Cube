#include "talker.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "PION_Sensors.h"
#include "PION_System.h"
int tries = 0;
void onWifiConnected(WiFiEvent_t event)
{
    Serial.printf("Wifi event: %i\n", event);
    if(event == SYSTEM_EVENT_STA_DISCONNECTED){
        tries++;
    }
    if (event != SYSTEM_EVENT_STA_GOT_IP)
    return;
    Serial.println("Connected!");
}

/**
 * @brief connect to wifi and server
 * 
 * @return true = connected with sucess
 * @return false = failed to connect
 */
bool comms::connect(){
    WiFi.onEvent(onWifiConnected);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting");
    long startTime = millis();
    while(WiFi.status() != WL_CONNECTED) {
        delay(50);
        Serial.print(".");
        if(tries >= CONNECTION_RETRY_COUNT or millis() > (startTime + CONNECTION_TIMEOUT)){//if maxed out retry or maxtime
            WiFi.disconnect();
            return 0;
        }
    }
    Serial.println();
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    return 1;
}
/**
 * @brief rounds values to 2 decimal points.
 * 
 * @param value 
 * @return double 
 */
double round2(double value) {
   return round(value * 100) / 100.0;
}
/**
 * @brief constructs and serializes json, then sends via HTTP
 * 
 * @return true 
 * @return false 
 */
bool comms::send(){
    //generate json object
    DynamicJsonDocument jsonBuffer(512);
    jsonBuffer["equipe"] = TEAM_NUM;
    jsonBuffer["bateria"] = System::battery;
    jsonBuffer["temperatura"] = Sensors::temperature;
    jsonBuffer["pressao"] = Sensors::pressure;
    JsonArray accel = jsonBuffer.createNestedArray("acelerometro");
    accel.add(Sensors::accel[0]);
    accel.add(Sensors::accel[1]);
    accel.add(Sensors::accel[2]);
    JsonArray gyro = jsonBuffer.createNestedArray("giroscopio");
    gyro.add(Sensors::gyro[0]);
    gyro.add(Sensors::gyro[1]);
    gyro.add(Sensors::gyro[2]);
    comms::payload = jsonBuffer.createNestedObject("payload");//max 90 bytes
    JsonArray mag = comms::payload.createNestedArray("mag");
    mag.add(round2(Sensors::mag[0]));
    mag.add(round2(Sensors::mag[1]));
    mag.add(round2(Sensors::mag[2]));
    comms::payload["CO2"] = Sensors::CO2Level;
    comms::payload["Hum"] = round2(Sensors::humidity);
    comms::payload["fotos"] = 3;
    JsonArray aval = comms::payload.createNestedArray("aval");
    aval.add(0);
    aval.add(1);
    aval.add(2);
    //prepare (serialize) data
    uint8_t * serializedBuf;
    size_t len = measureJson(jsonBuffer);
    serializedBuf = (uint8_t *)malloc(len+1);
    serializeJson(jsonBuffer, serializedBuf, len+1);
    //print serialized buffer for debug
    Serial.write(serializedBuf, len);
    Serial.println();
    //start sending data
    HTTPClient http;
    if(!http.begin(NETWORK_SERVER)){
        free(serializedBuf);
        return 0;
    }
    Serial.println("connected");
    uint responseCode = http.POST(serializedBuf, len);
    Serial.print("response: ");
    Serial.println(responseCode);
    if(responseCode == 200){
        return 1;
    }else{
        return 0;
    }
    free(serializedBuf);
    return 1;
}
void comms::disconnect(){
    if(WiFi.status() != WL_CONNECTED){
        WiFi.disconnect();
    }
}