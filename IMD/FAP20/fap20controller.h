#ifndef FAP20CONTROLLER_H
#define FAP20CONTROLLER_H

#include <mutex>
#include <QString>

class Fap20Controller
{
public:

    Fap20Controller();

    std::mutex _syncLock;

    static const int FPM_DEVICE = 0x01;  //Device
    static const int FPM_PLACE = 0x02;   //Place Finger
    static const int FPM_LIFT = 0x03;    //Lift Finger
    static const int FPM_CAPTURE = 0x04; //Capute Image
    static const int FPM_GENCHAR = 0x05; //Capture Feature
    static const int FPM_ENRFPT = 0x06;  //Enrol Feature
    static const int FPM_NEWIMAGE = 0x07;//New Image
    static const int FPM_TIMEOUT = 0x08; //Time Out
    static const int FPM_BUSY = 0xFF;    //Device Busy

    static const int SOUND_BEEP = 0;
    static const int SOUND_OK = 1;
    static const int SOUND_FAIL = 2;

public:
    int SafeOpenDevice();
    int SafeCloseDevice();
    int SafeResetDevice();

    int SafeLinkDevice(int password);

    void SafeCaptureImage();
    void SafeCaptureTemplate();
    void SafeEnrollTemplate();

    int SafeDeviceSoundBeep(int beep);

    int SafeGetImageSize(int *width, int *height);

    int SafeGetRawImage(std::byte * imagedata, int * size);
    int SafeGetBmpImage(std::byte * imagedata, int * size);

    int SafeGetWorkMsg();
    int SafeGetRetMsg();

    int SafeCreateTemplate(std::byte * pImage, int width, int height, std::byte * tp, int * templateSize);
    bool SafeGetTemplateByEnl(std::byte * fpbuf, int * fpsize);
    int SafeMatchTemplate(std::byte * pSrcData, std::byte * pDstData);
    int SafeMatchTemplates(std::byte * pSrcData, std::byte * pDstFullData,int nDstCount,int nDstSize,int nThreshold);
    bool SafeDeviceLedState(int type, int status);
};



#endif // FAP20CONTROLLER_H
