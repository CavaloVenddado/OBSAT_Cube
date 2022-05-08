#ifndef talker_H
#define talker_H
#include <Arduino.h>
#include <ArduinoJson.h>
//define constants
#define WIFI_SSID "roboticawifi2.4g"
#define WIFI_PASSWORD "roboticamedia123"
#define NETWORK_SERVER "http://ptsv2.com/t/k5w4v-1650841701/post"
#define CONNECTION_RETRY_COUNT 3
#define CONNECTION_TIMEOUT 10000
#define TEAM_NUM 0 //this will be given on launch day

class comms{
    public:
        bool connect();
        bool send();
        void disconnect();
        JsonObject payload;
    private:

};
#endif