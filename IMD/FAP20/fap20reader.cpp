#include "IMD/FAP20/fap20reader.h"
#include "./libs/fpdevice.h"

Fap20Reader::Fap20Reader()
{
}

Fap20Reader::~Fap20Reader()
{
}

Fap20Reader::Fap20Reader(MainWindow *gui,int worktype)
{
    pMain=gui;
    m_WorkType=worktype;
    connect(this,SIGNAL(FPMessage(int,int,unsigned char *,int)),this,SLOT(slotFPMessage(int,int,unsigned char *,int)));
}

void Fap20Reader::run()
{
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

void Fap20Reader::slotFPMessage(int worktype,int retval,unsigned char* data,int size)
{
    if(pMain!=NULL)
    {
        pMain->FPMessage(worktype,retval,data,size);
    }
}

void Fap20Reader::CaptureImage()
{
    emit FPMessage(FPM_PLACEFINGER,0,0,0);
    while(true)
    {
        if(GetDeviceImage())
            break;
    }
    emit FPMessage(FPM_LIFTFINGER,0,0,0);
    if(UpDeviceImage(m_ImageData,&m_ImageSize))
    {
        emit FPMessage(FPM_DRAWIMAGE,0,m_ImageData,m_ImageSize);
        emit FPMessage(FPM_CAPTUREIMAGE,1,m_ImageData,m_ImageSize);
        return;
    }
    emit FPMessage(FPM_CAPTUREIMAGE,0,0,0);
}

void Fap20Reader::CaptureTemplate()
{
    emit FPMessage(FPM_PLACEFINGER,0,0,0);
    while(true)
    {
        if(GetDeviceImage())
            break;
    }
    emit FPMessage(FPM_LIFTFINGER,0,0,0);
    if(UpDeviceImage(m_ImageData,&m_ImageSize))
    {
        emit FPMessage(FPM_DRAWIMAGE,0,m_ImageData,m_ImageSize);
    }
    if(GenerateDeviceTemplate(1))
    {
        if(UpDeviceTemplate(1,m_TemplateData,&m_TemplateSize))
        {
            m_TemplateSize=m_TemplateSize/2;
            emit FPMessage(FPM_CAPTURETP,1,m_TemplateData,m_TemplateSize);
            return;
        }
    }
    emit FPMessage(FPM_CAPTURETP,0,0,0);
}

void Fap20Reader::EnrolTemplate()
{
    emit FPMessage(FPM_PLACEFINGER,0,0,0);
    while(true)
    {
        if(GetDeviceImage())
            break;
    }
    if(UpDeviceImage(m_ImageData,&m_ImageSize))
    {
        emit FPMessage(FPM_DRAWIMAGE,0,m_ImageData,m_ImageSize);
    }
    if(GenerateDeviceTemplate(1)==0)
    {
        emit FPMessage(FPM_ENROLTP,0,0,0);
        return;
    }
    emit FPMessage(FPM_LIFTFINGER,0,0,0);
    while(true)
    {
        if(GetDeviceImage()==0)
            break;
    }
    emit FPMessage(FPM_PLACEFINGER,0,0,0);
    while(true)
    {
        if(GetDeviceImage())
            break;
    }
    if(UpDeviceImage(m_ImageData,&m_ImageSize))
    {
        emit FPMessage(FPM_DRAWIMAGE,0,m_ImageData,m_ImageSize);
    }
    if(GenerateDeviceTemplate(2)==0)
    {
        emit FPMessage(FPM_ENROLTP,0,0,0);
        return;
    }
    if(CombineDeviceTemplate())
    {
        if(UpDeviceTemplate(1,m_TemplateData,&m_TemplateSize))
        {
            emit FPMessage(FPM_ENROLTP,1,m_TemplateData,m_TemplateSize);
            return;
        }
    }
    emit FPMessage(FPM_ENROLTP,0,0,0);
}

unsigned char* Fap20Reader::GetImageData()
{
    return m_ImageData;
}

unsigned char* Fap20Reader::GetTemplateData()
{
    return m_TemplateData;
}

int Fap20Reader::GetImageSize()
{
    return m_ImageSize;
}

int Fap20Reader::GetTemplateSize()
{
    return m_TemplateSize;
}
