#ifndef FAP20THREAD_H
#define FAP20THREAD_H
#include <QThread>
#include "IMD/FAP20/fap20reader.h"

#define WORK_CAPTUREIMAGE   1
#define WORK_ENROLTP        2
#define WORK_CAPTURETP      3

#define FPM_PLACEFINGER     0x01
#define FPM_LIFTFINGER      0x02
#define FPM_DRAWIMAGE       0x03
#define FPM_CAPTUREIMAGE    0x04
#define FPM_ENROLTP         0x05
#define FPM_CAPTURETP       0x06

class Fap20Thread : public QThread
{
    Q_OBJECT
public:
    Fap20Thread(Fap20Reader *gui,int worktype);
    ~Fap20Thread();

    void run() override;

    unsigned char* GetImageData();
    unsigned char* GetTemplateData();
    int GetImageSize();
    int GetTemplateSize();


private:
    void CaptureImage();
    void CaptureTemplate();
    void EnrolTemplate();

private:
    Fap20Reader *pFap20;
    int m_WorkType;

    int m_TemplateSize;
    int m_ImageSize;

    unsigned char m_TemplateData[1024];
    unsigned char m_ImageData[300000];
};


#endif // FAP20THREAD_H
