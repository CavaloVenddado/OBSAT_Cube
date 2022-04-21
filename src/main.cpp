#include "PION_System.h"
#include <Arduino.h>
#include "FS.h"                // SD Card ESP32
#include "SD.h"            // SD Card ESP32
#include "cameraCompanion.h"
#include "logger.h"

camCompanion myESPcamera(Serial2);
System cubesat;
Logs myLogger;
void setup() {
    cubesat.initNoNetwork();
    //Serial.begin(115200);
    Serial2.begin(115200);
    myLogger.init();
    myLogger.createFileStructure();
    while(Serial2.available()){
        Serial2.read();
    }
    Serial2.flush();
    Serial.setDebugOutput(true);
    //cubesat.doBeeps(3, 100, 500);
    //cubesat.activateSDLog();
    delay(10000);
    myLogger.log("Sistema iniciado", INFO);
}
int imgnum = 0;

/**
 * @brief function for taking pictures with espcamera
 * 
 * @param path path where image will be stored (has to include filename and extension)
 * @param fs system's filesystem
 */
void picture(String path, fs::FS &fs){
    Serial.printf("Picture file name: %s\n", path.c_str());
    //open the file
    File file = fs.open(path.c_str(), FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file in writing mode");
        return;
    }
    myESPcamera.requestFrame();//take picture
    Serial.println("recv");
    //log if the frame was not fully received
    if(!myESPcamera.receiveFrame()){
        myLogger.log("Frame ["+ path +"] was received partially", WARN);
    }
    Serial.println("writing");
    file.write(myESPcamera.imgBuf, myESPcamera.imgBufLen);
    file.close();
    myESPcamera.freeFrame();
    Serial.println("done");
}

void loop()
{
    //digitalWrite(LAMP_PIN, LOW); 
    String name = myLogger.extrasDirPath + String(imgnum) + ".jpg";
    cubesat.setRGB(PURPLE);
    picture(name, SD);
    cubesat.setRGB(OFF);
    imgnum ++;
    cubesat.setLed(L1, HIGH);
    myLogger.log("imagem " + name + " capturada", DATA);
    myLogger.datalog();
    cubesat.setLed(L1, LOW);
}
