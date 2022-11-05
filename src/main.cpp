#include "PION_System.h"
#include <Arduino.h>
#include "FS.h"                // SD Card ESP32
#include "SD.h"            // SD Card ESP32
#include "cameraCompanion.h"
#include "logger.h"
#include "talker.h"

camCompanion myESPcamera(Serial2);

System cubesat;
Logs myLogger;
comms myTalker;

/**
 * @brief connects and sends json
 * 
 */
bool sendData(){
    bool success = false;
    myLogger.log("Inicio de transmissão", INFO);
    cubesat.setLed(L3, HIGH);
    if(myTalker.connect()){
        myLogger.log("Conectado com sucesso!", INFO);
        Serial.print("sucesso na conexão!");
    }else{
        myLogger.log("Erro na conexão!", ERROR);
        cubesat.setRGB(RED);
        cubesat.doBeeps(5,100,100);
        cubesat.setRGB(OFF);
        return false;
    }
    cubesat.setLed(L3, LOW);
    cubesat.setLed(L2, HIGH);
    myLogger.log("enviandoJSON...", INFO);
    if(myTalker.send()){
        myLogger.log("Enviado com sucesso! (HTTP 200)", INFO);
        Serial.print("sucesso no envio! response 200");
        success = true;
    }else{
        myLogger.log("Erro na transmissão!", ERROR);
        success = false;
    }
    myTalker.disconnect();
    cubesat.setLed(L2, LOW);
    return success;
}

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
    //write to sdcard
    Serial.print("writing Bytes: ");
    Serial.println(myESPcamera.imgBufLen);
    file.write(myESPcamera.imgBuf, myESPcamera.imgBufLen);
    file.close();
    //confirms file size by opening file and counting bytes
    File fileConfirm = fs.open(path.c_str(), FILE_READ);
    size_t fileSize = fileConfirm.size();
    fileConfirm.close();
    //if they are not the same, we have a problem.
    if(fileSize != myESPcamera.imgBufLen){
        myLogger.log("Frame ["+ path +"] was written partially to card", WARN);
        Serial.println("oh no! my data :(");
        Serial.print(fileSize);
        Serial.print(" out of ");
        Serial.print(myESPcamera.imgBufLen);
    }
    myESPcamera.freeFrame();
    Serial.println("done");
}

unsigned long lastSentMillis = 0;
bool success = false;
void tryData(){
    if(millis() - lastSentMillis >= 240000 or !success){
        lastSentMillis = millis();
        success = sendData();
    }
}

void loop()
{
    //digitalWrite(LAMP_PIN, LOW); 
    cubesat.setLed(L4, HIGH);
    String name = myLogger.extrasDirPath + String(imgnum) + ".jpg";
    picture(name, SD);
    myTalker.photos_num = imgnum;
    imgnum ++;
    cubesat.setLed(L4, LOW);
    cubesat.setLed(L1, HIGH);
    myLogger.log("imagem " + name + " capturada", DATA);
    myLogger.datalog();
    cubesat.setLed(L1, LOW);
    tryData();
}
