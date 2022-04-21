#ifndef camCompanion_h
#define camCompanion_h
#endif
#include <Arduino.h>
class camCompanion{
    public:
        camCompanion(HardwareSerial &camSerial);
        void requestFrame();
        bool receiveFrame();
        void freeFrame();
        size_t imgBufLen;
        uint8_t * imgBuf;
    private:
        HardwareSerial *_camSerial;
};
