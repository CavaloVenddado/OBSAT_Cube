#include "logger.h"
#include <Arduino.h>

#include "_config.h"
#include "_types.h"
#include "FS.h"
#include "SD.h"

#include "PION_System.h"
#include "PION_Sensors.h"
String Logs::missionDirPath = "";
String Logs::dataFilePath = "";
String Logs::logFilePath = "";
String Logs::extrasDirPath = "";

/**
 * @brief checks and mounts sd card
 */
void Logs::init(){
    pinMode(PIN_SD_SS, OUTPUT);

    if(!SD.begin(PIN_SD_SS)){
        LOG("SD Card Mount Falhou");ENDL;
    }

    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        LOG("Não ha cartão SD, reiniciando...");ENDL;
        System::doBeeps(10, 100, 500);
        ESP.restart();
    }
    Serial.print("sd card detected, with: ");
    Serial.print(SD.usedBytes());
    Serial.print(" out of ");
    Serial.println(SD.cardSize());
}

/**
 * @brief find an unused directory according to logger.h defines, if already exists, up the suffix.
 *  ex: /prefix<suffix number>/
 * @param fs system's filesystem
 * @param prefix prefix for the folder name
 * @return String path to directory (/prefix0/)
 */
String createFolder(fs::FS &fs, String prefix){
    uint id = 0;
    String path = prefix + String(id);
    while(fs.exists(path)){
        id++;
        path = prefix + String(id);
    }
    Serial.println("criando em: "+path);
    if(!fs.mkdir(path)){
        Serial.println("Falha ao criar diretorio");
    }
    if(!fs.mkdir(path+ "/" + MISSION_EXTRAS_DIR_NAME)){
        Serial.println("Falha ao criar diretorio");
    }
    return path + "/";
}

/**
 * @brief Create a dataloging file and its header
 * 
 * @param fs system's filesystem
 * @param dataFilePath String path to file
 */
void createDatalogFile(fs::FS &fs, String dataFilePath){
    File dataFile = fs.open(dataFilePath, FILE_WRITE);
    if(!dataFile){
        Serial.println("Falha ao abrir para escrita");
        return;
    }
    const String CSVDataStructure = "tempo(ms),temperatura(C),umidade(%),pressao(Pa),co2(ppm),luminosidade(%),acelX(m/s2),accelY,acelZ,giroX(graus/s),giroY,giroZ,magX(uT),magY,magZ,bateria(V)";
    if(!dataFile.println(CSVDataStructure)){
        Serial.println("Falha na escrita");
        return;
    }
    dataFile.close();
}

/**
 * @brief Create system's event log file and its header
 * 
 * @param fs system's filesystem
 * @param dataFilePath String path to file
 */
void createlogFile(fs::FS &fs, String logFilePath){
    File logFile = fs.open(logFilePath, FILE_WRITE);
    if(!logFile){
        Serial.println("Falha ao abrir para escrita");
        return;
    }
    if(!logFile.println(String(millis()) + " [BEGIN] Cavalo Vendado Satelite BEGIN LOG FILE")){
        Serial.println("Falha na escrita");
        return;
    }
}

/**
 * @brief creates filesystem's structures and file headers
 * @details
 * creates this file structure for every mission
 *  +missao_num - mission dir
 *      |dados.csv
 *      |logs.log
 *      +fotos - extras dir
 *          |foto_1.jpeg
 *          |...
 */
void Logs::createFileStructure(){
    Logs::missionDirPath = createFolder(SD, MISSION_DIR_PEFIX);
    Logs::dataFilePath = Logs::missionDirPath + MISSION_DATA_FILE_NAME;
    Logs::logFilePath = Logs::missionDirPath + MISSION_LOGS_FILE_NAME;
    Logs::extrasDirPath = Logs::missionDirPath + MISSION_EXTRAS_DIR_NAME + "/";
    createDatalogFile(SD, Logs::dataFilePath);
    createlogFile(SD, Logs::logFilePath);
}

/**
 * @brief appends message to log file on filesystem structure.
 * 
 * @param message logging message
 * @param level DATA, INFO, WARN or ERROR
 */
void Logs::log(String message, logLevels level){
    File logFile = SD.open(logFilePath, FILE_APPEND);
    if(!logFile){
        Serial.println("Falha ao abrir para escrita");
        return;
    }
    logFile.print(String(millis()));
    switch (level){
        case DATA:
            logFile.print(" [DATA] ");
            break;
        case INFO:
            logFile.print(" [INFO] ");
            break;
        case WARN:
            logFile.print(" [WARN] ");
            break;
        case ERROR:
            logFile.print(" [ERROR] ");
            break;
    }
    logFile.println(message);
    logFile.close();
}
/**
 * @brief appends datalog file with sensor values.
 * 
 */
void Logs::datalog(){
    File file = SD.open(Logs::dataFilePath, FILE_APPEND);
    if(!file){
        Serial.println("Falha ao abrir para gravacao");
        return;
    }
    file.print(millis());
    file.write(',');
    file.print(Sensors::temperature);
    file.write(',');
    file.print(Sensors::humidity);
    file.write(',');
    file.print(Sensors::pressure);
    file.write(',');
    file.print(Sensors::CO2Level);
    file.write(',');
    file.print(Sensors::luminosity);
    file.write(',');
    file.print(Sensors::accel[0]);
    file.write(',');
    file.print(Sensors::accel[1]);
    file.write(',');
    file.print(Sensors::accel[2]);
    file.write(',');
    file.print(Sensors::gyro[0]);
    file.write(',');
    file.print(Sensors::gyro[1]);
    file.write(',');
    file.print(Sensors::gyro[2]);
    file.write(',');
    file.print(Sensors::mag[0]);
    file.write(',');
    file.print(Sensors::mag[1]);
    file.write(',');
    file.print(Sensors::mag[2]);
    file.write(',');
    file.println(System::batteryVoltage);
    file.close();
}