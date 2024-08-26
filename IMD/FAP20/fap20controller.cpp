#include "fap20controller.h"
#include "libs/fpdevice.h"

#include <mutex>
#include <QLibrary>

Fap20Controller::Fap20Controller() {

}

int Fap20Controller::SafeOpenDevice()
{
    std::lock_guard<std::mutex> lock(_syncLock);
    return OpenDevice(0,0,0);
}

int Fap20Controller::SafeCloseDevice()
{
    std::lock_guard<std::mutex> lock(_syncLock);
    return CloseDevice();

}

int Fap20Controller::SafeResetDevice()
{
    std::lock_guard<std::mutex> lock(_syncLock);
    return 1;
}

int Fap20Controller::SafeLinkDevice(int password)
{
    std::lock_guard<std::mutex> lock(_syncLock);
    return LinkDevice(password);
}

void Fap20Controller::SafeCaptureImage()
{
    std::lock_guard<std::mutex> lock(_syncLock);
    return;
}

void Fap20Controller::SafeCaptureTemplate()
{
    std::lock_guard<std::mutex> lock(_syncLock);
    return;
}

void Fap20Controller::SafeEnrollTemplate()
{
    std::lock_guard<std::mutex> lock(_syncLock);
    return;
}

int Fap20Controller::SafeDeviceSoundBeep(int beep)
{
    std::lock_guard<std::mutex> lock(_syncLock);
    return beep;
}

int Fap20Controller::SafeGetImageSize(int *width, int *height){
    std::lock_guard<std::mutex> lock(_syncLock);
    return GetImageSize(width,height);
}


int Fap20Controller::SafeCreateTemplate(std::byte * pImage, int width, int height, std::byte * tp, int * templateSize)
{
    std::lock_guard<std::mutex> lock(_syncLock);
    return 1;
}

bool Fap20Controller::SafeGetTemplateByEnl(std::byte * fpbuf, int * fpsize){
    std::lock_guard<std::mutex> lock(_syncLock);
    return 1;
}

int Fap20Controller::SafeMatchTemplate(std::byte * pSrcData, std::byte * pDstData){
    std::lock_guard<std::mutex> lock(_syncLock);
    return 1;
}

int Fap20Controller::SafeMatchTemplates(std::byte * pSrcData, std::byte * pDstFullData,int nDstCount,int nDstSize,int nThreshold){
    std::lock_guard<std::mutex> lock(_syncLock);
    return 1;
}

bool Fap20Controller::SafeDeviceLedState(int type, int status)
{
    return 1;
}
