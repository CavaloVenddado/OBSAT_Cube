#include "PION_System.h"
#include <Arduino.h>
#include "FS.h"                // SD Card ESP32
#include "SD.h"            // SD Card ESP32
#include "cameraCompanion.h"

#define I2C_DEV_ADDR 0x30
#define PIN_SD_SS 15

camCompanion myESPcamera(Serial2);
System cubesat;

void setup() {
    cubesat.init();
    //Serial.begin(115200);
    Serial2.begin(115200);
    while(Serial2.available()){
        Serial2.read();
    }
    Serial2.flush();
    Serial.setDebugOutput(true);
    if (cubesat.getSDStatus() == NO_SD){
        cubesat.toggleSD();
    }
    while(cubesat.getSDStatus() == NO_SD){
        Serial.println("Impossível continuar, nenhum cartão sd!");
        delay(5000);
    }
    #ifndef PION_SYSTEM_H
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
    #endif
    //cubesat.doBeeps(3, 100, 500);
    //cubesat.activateSDLog();
    delay(10000);
    
}
int imgnum = 0;
void picture(String path, fs::FS &fs){
    
    Serial.printf("Picture file name: %s\n", path.c_str());
    
    File file = fs.open(path.c_str(), FILE_WRITE);
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
    cubesat.setRGB(PURPLE);
    picture(name, SD);
    cubesat.setRGB(OFF);
    imgnum ++;
    cubesat.setLed(1, HIGH);
    delay(10000);
    cubesat.setLed(1, LOW);
}
