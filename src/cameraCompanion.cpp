#include <Arduino.h>
#include "cameraCompanion.h"

struct frameData
{
    /* data */
    size_t len;                 /*!< Length of the buffer in bytes */
};
const size_t frameDataSize = sizeof(frameData);

union imgPacket{
    frameData Info;
    uint8_t byteArray[frameDataSize];
};



imgPacket frameinformation;

camCompanion::camCompanion(HardwareSerial &camSerial){
    _camSerial = &camSerial;
}

void camCompanion::receiveFrame(){
    Serial2.write(0x11);
    while(!Serial2.available()){
        
    }
    Serial2.read(frameinformation.byteArray, frameDataSize);
    Serial.print(frameinformation.Info.len);
    imgBufLen = frameinformation.Info.len;
    imgBuf = (uint8_t *)malloc(imgBufLen);
    //Serial2.read(); //used to remove first NULL byte
    Serial2.readBytes(imgBuf, imgBufLen);
}
void camCompanion::requestFrame(){
    Serial2.write(0x10);
}

void camCompanion::freeFrame(){
    free(imgBuf);
}