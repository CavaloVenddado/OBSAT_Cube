#ifndef Logs_H
#define Logs_H
#include "_types.h"
#include "Arduino.h"
#define MISSION_DIR_PEFIX "/FDR_"//name of mission root dir (one for every mission)
#define MISSION_EXTRAS_DIR_NAME "IMG" //extra files (mission generated files) dir name. (needs / at the end)
#define MISSION_DATA_FILE_NAME "dados.csv"//name of csv sensor data
#define MISSION_LOGS_FILE_NAME "logs.log"//name of system logs and events file

enum  logLevels{
    DATA, 
    INFO, 
    WARN, 
    ERROR
};

class Logs{
    public:
        static String missionDirPath;
        static String extrasDirPath;
        static void init();
        static void createFileStructure();
        static void log(String message, logLevels level);
        static void datalog();
    private:
        static String dataFilePath;
        static String logFilePath;
};
#endif