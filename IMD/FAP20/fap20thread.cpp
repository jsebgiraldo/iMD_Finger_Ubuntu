#include "./libs/fpdevice.h"

#include "fap20thread.h"
#include "fap20reader.h"
Fap20Thread::Fap20Thread(Fap20Reader *fap20,int worktype)
{
    pFap20 = fap20;
    m_WorkType=worktype;
}

Fap20Thread::~Fap20Thread()
{
}


void Fap20Thread::run()
{
    qDebug() << "Run Fap20 Thread";

    switch (m_WorkType) {
    case WORK_CAPTUREIMAGE:
        CaptureImage();
        break;
    case WORK_ENROLTP:
        EnrolTemplate();
        break;
    case WORK_CAPTURETP:
        CaptureTemplate();
        break;
    default:
        break;
    }
}

void Fap20Thread::CaptureImage()
{
    emit pFap20->FPMessage(FPM_PLACEFINGER,0,0,0);
    while(true)
    {
        if(GetDeviceImage())
            break;
    }
    emit pFap20->FPMessage(FPM_LIFTFINGER,0,0,0);
    if(UpDeviceImage(m_ImageData,&m_ImageSize))
    {
        emit pFap20->FPMessage(FPM_DRAWIMAGE,0,m_ImageData,m_ImageSize);
        emit pFap20->FPMessage(FPM_CAPTUREIMAGE,1,m_ImageData,m_ImageSize);
        return;
    }
    emit pFap20->FPMessage(FPM_CAPTUREIMAGE,0,0,0);

    qDebug() << "Capture Image Done";
}

void Fap20Thread::CaptureTemplate()
{
    emit pFap20->FPMessage(FPM_PLACEFINGER,0,0,0);
    while(true)
    {
        if(GetDeviceImage())
            break;
    }
    emit pFap20->FPMessage(FPM_LIFTFINGER,0,0,0);
    if(UpDeviceImage(m_ImageData,&m_ImageSize))
    {
        emit pFap20->FPMessage(FPM_DRAWIMAGE,0,m_ImageData,m_ImageSize);
    }
    if(GenerateDeviceTemplate(1))
    {
        if(UpDeviceTemplate(1,m_TemplateData,&m_TemplateSize))
        {
            m_TemplateSize=m_TemplateSize/2;
            emit pFap20->FPMessage(FPM_CAPTURETP,1,m_TemplateData,m_TemplateSize);
            return;
        }
    }
    emit pFap20->FPMessage(FPM_CAPTURETP,0,0,0);
}

void Fap20Thread::EnrolTemplate()
{
    emit pFap20->FPMessage(FPM_PLACEFINGER,0,0,0);
    while(true)
    {
        if(GetDeviceImage())
            break;
    }
    if(UpDeviceImage(m_ImageData,&m_ImageSize))
    {
        emit pFap20->FPMessage(FPM_DRAWIMAGE,0,m_ImageData,m_ImageSize);
    }
    if(GenerateDeviceTemplate(1)==0)
    {
        emit pFap20->FPMessage(FPM_ENROLTP,0,0,0);
        return;
    }

    if(UpDeviceTemplate(1,m_TemplateData,&m_TemplateSize))
    {
        emit pFap20->FPMessage(FPM_ENROLTP,1,m_TemplateData,m_TemplateSize);
        return;
    }

    emit pFap20->FPMessage(FPM_ENROLTP,0,0,0);
}

unsigned char* Fap20Thread::GetImageData()
{
    return m_ImageData;
}

unsigned char* Fap20Thread::GetTemplateData()
{
    return m_TemplateData;
}

int Fap20Thread::GetImageSize()
{
    return m_ImageSize;
}

int Fap20Thread::GetTemplateSize()
{
    return m_TemplateSize;
}
