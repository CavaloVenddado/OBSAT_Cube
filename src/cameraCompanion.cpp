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
/**
 * @brief Construct a new cam Companion::cam Companion object
 * 
 * @param camSerial serial port used by camera
 */
camCompanion::camCompanion(HardwareSerial &camSerial){
    camCompanion::_camSerial = &camSerial;
}

/**
 * @brief receive a frame from the camera, allocating buffer memory
 * 
 * @return true frame was received sucessfuly
 * @return false frame was received partially
 */
bool camCompanion::receiveFrame(){
    _camSerial->write(0x11);
    while(!_camSerial->available()){
        
    }
    _camSerial->read(frameinformation.byteArray, frameDataSize);
    imgBufLen = frameinformation.Info.len;
    imgBuf = (uint8_t *)malloc(imgBufLen);
    //Serial2.read(); //used to remove first NULL byte
    size_t recvlen = Serial2.readBytes(imgBuf, imgBufLen);
    return imgBufLen <= recvlen;
}
/**
 * @brief send command to camera asking to take picture
 * 
 */
void camCompanion::requestFrame(){
    _camSerial->write(0x10);
}
/**
 * @brief release allocated buffer memory
 * 
 */
void camCompanion::freeFrame(){
    free(imgBuf);
}