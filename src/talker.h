#ifndef talker_H
#define talker_H
#include <Arduino.h>
#include <ArduinoJson.h>
//define constants
#define TEAM_NUM 0 //this will be given on launch day
#define WIFI_SSID "roboticawifi2.4g"
#define WIFI_PASSWORD "roboticamedia123"
#define NETWORK_SERVER "https://obsat.org.br/teste_post/envio.php"
#define CONNECTION_RETRY_COUNT 3

#define CONNECTION_TIMEOUT 10000


class comms{
    public:
        bool connect();
        bool send();
        void disconnect();
        JsonObject payload;
        uint16_t photos_num;
    private:

};
#endif