#include <Arduino.h>
#include "FS.h"                // SD Card ESP32
#include "SD.h"            // SD Card ESP32
#define Buzzer 25
#define I2C_DEV_ADDR 0x30
#define PIN_SD_SS 15
File file;
uint8_t temp[100000];

struct frameData
{
    /* data */
    size_t len;                 /*!< Length of the buffer in bytes */
};
const int frameDataSize = sizeof(frameData);

union imgPacket{
    frameData Info;
    uint8_t byteArray[frameDataSize];
};



imgPacket frameinformation;
void receiveFrame(){
    Serial2.write(0x11);
    while(!Serial2.available()){
        
    }
    Serial2.read(frameinformation.byteArray, frameDataSize);
    Serial.println(frameinformation.Info.len);
    if(frameinformation.Info.len > sizeof(temp)){
        esp_restart();
    }
    //Serial2.read(); //used to remove first NULL byte
    Serial2.readBytes(temp, frameinformation.Info.len);
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200);
    Serial2.flush();
    Serial.setDebugOutput(true);
    pinMode(PIN_SD_SS, OUTPUT);
    if(!SD.begin(PIN_SD_SS)){
        Serial.println("SD Card Mount Failed");
    }
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD Card attached");
        return;
    }
    //digitalWrite(LAMP_PIN, LOW); 
    String path = "/picture.jpg";

    fs::FS &fs = SD; 
    Serial.printf("Picture file name: %s\n", path.c_str());
    
    file = fs.open(path.c_str(), FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file in writing mode");
    }
    delay(2000);
    Serial2.write(0x10);
    delay(200);
    Serial.println("recv");
    delay(200);
    receiveFrame();
    Serial.println("writing");
    file.write(temp, frameinformation.Info.len);
    
    file.close();
    Serial.println("done");
}
void loop()
{
    delay(1000);
}
