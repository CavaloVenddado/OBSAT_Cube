#include "talker.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "PION_Sensors.h"
#include "PION_System.h"
#include "webpages.h"
#include "SD.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");


/**
 * @brief connect to wifi and server
 *
 * @return true = connected with sucess
 * @return false = failed to connect
 */
bool comms::connect()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD); // can configure to
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        // Send File
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", indexHTML);
        request->send(response); 
    });
    server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
        // Send File
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", indexHTML);
        request->send(response); 
    });
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        // Send File
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", indexCSS);
        request->send(response); 
    });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
        // Send File
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", indexJS);
        request->send(response); 
    });
    server.on("/frame.jpg", HTTP_GET, [this](AsyncWebServerRequest *request){
        const File SDfile = SD.open(framePath, FILE_READ);
        if (SDfile){
            Serial.printf("[HTTP]\tSD File exists [%d]\r\n", SDfile);
        } else {
            Serial.printf("[HTTP]\tSD File DOESN'T exists [%d] <<< ERROR !!!\r\n", SDfile);
        }
        AsyncWebServerResponse *response = request->beginChunkedResponse("text/css", [SDfile](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
            File SDLambdaFile = SDfile;                                   // Local copy of file pointer
            Serial.printf("[HTTP]\t[%d]\tINDEX [%d]\tBUFFER_MAX_LENGHT [%d]\r\n", index, SDLambdaFile.size(), maxLen);
            return SDLambdaFile.read(buffer, maxLen);
        }
        );
        request->send(response);
    }
    );
    server.addHandler(&ws);
    server.begin();
    return true;
}
/**
 * @brief rounds values to 2 decimal points.
 *
 * @param value
 * @return double
 */
double round2(double value)
{
    return round(value * 100) / 100.0;
}
/**
 * @brief constructs and serializes json, then sends via HTTP
 *
 * @return true
 * @return false
 */
bool comms::send()
{
    // generate json object
    ws.cleanupClients();
    DynamicJsonDocument jsonBuffer(512);
    jsonBuffer["equipe"] = TEAM_NUM;
    jsonBuffer["bateria"] = System::batteryVoltage;
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
    comms::payload = jsonBuffer.createNestedObject("payload"); // max 90 bytes
    JsonArray mag = comms::payload.createNestedArray("mag");
    mag.add(round2(Sensors::mag[0]));
    mag.add(round2(Sensors::mag[1]));
    mag.add(round2(Sensors::mag[2]));
    comms::payload["CO2"] = Sensors::CO2Level;
    comms::payload["hum"] = round2(Sensors::humidity);
    comms::payload["fotos"] = photos_num;

    // Mede o tamanho do buffer do JSON
    size_t len = measureJson(jsonBuffer);
    // Cria um espaço na RAM de (len + 1)
    AsyncWebSocketMessageBuffer *buffer = ws.makeBuffer(len);
    if (buffer)
    {
        // Transforma o JSON em um grande texto e o coloca no espaço criado anteriormente
        serializeJson(jsonBuffer, (char *)buffer->get(), len + 1);
        // Envia pelo WebSocket para todos os usuários
        ws.textAll(buffer);
        return true;
    }
    return false;
}
void comms::disconnect()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect();
    }
}