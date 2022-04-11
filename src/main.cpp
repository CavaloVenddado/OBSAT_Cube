#include <Arduino.h>
#include "FS.h"                // SD Card ESP32
#include "SD.h"            // SD Card ESP32
#include "cameraCompanion.h"
#define I2C_DEV_ADDR 0x30
#define PIN_SD_SS 15

File file;
camCompanion myESPcamera(Serial2);

void setup() {
    //cubesat.init();
    Serial.begin(115200);
    Serial2.begin(115200);
    while(Serial2.available()){
        Serial2.read();
    }
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
    /*
    interface.doBeeps(3, 20, 100);
    interface.toggleSiren();
    interface.setLed(0, false);
    */
   delay(10000);
}
int imgnum = 0;
void picture(String path){
    fs::FS &fs = SD; 
    Serial.printf("Picture file name: %s\n", path.c_str());
    
    file = fs.open(path.c_str(), FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file in writing mode");
    }
    myESPcamera.requestFrame();
    Serial.println("recv");
    myESPcamera.receiveFrame();
    Serial.println("writing");
    file.write(myESPcamera.imgBuf, myESPcamera.imgBufLen);
    file.close();
    myESPcamera.freeFrame();
    Serial.println("done");
}
void loop()
{
    
    //digitalWrite(LAMP_PIN, LOW); 
    String name = "/picture" + String(imgnum) + ".jpg";
    picture(name);
    imgnum ++;
}
