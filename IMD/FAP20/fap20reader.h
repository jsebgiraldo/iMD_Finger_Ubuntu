#ifndef FAP20READER_H
#define FAP20READER_H


#include <QThread>
#include "mainwindow.h"

#define WORK_CAPTUREIMAGE   1
#define WORK_ENROLTP        2
#define WORK_CAPTURETP      3

#define FPM_PLACEFINGER     0x01
#define FPM_LIFTFINGER      0x02
#define FPM_DRAWIMAGE       0x03
#define FPM_CAPTUREIMAGE    0x04
#define FPM_ENROLTP         0x05
#define FPM_CAPTURETP       0x06



class Fap20Reader : public QThread
{
    Q_OBJECT
public:
    Fap20Reader();
    Fap20Reader(MainWindow *gui,int worktype);
    ~Fap20Reader();

    void run() override;

    unsigned char* GetImageData();
    unsigned char* GetTemplateData();
    int GetImageSize();
    int GetTemplateSize();

signals:
    void FPMessage(int worktype,int retval,unsigned char* data,int size);

public slots:
    void slotFPMessage(int worktype,int retval,unsigned char* data,int size);

private:
    void CaptureImage();
    void CaptureTemplate();
    void EnrolTemplate();

private:
    MainWindow *pMain;
    int m_WorkType;

    int m_TemplateSize;
    int m_ImageSize;

    unsigned char m_TemplateData[1024];
    unsigned char m_ImageData[300000];
};

#endif // FAP20READER_H
