#ifndef FAP50READER_H
#define FAP50READER_H

#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>
#include <QByteArray>
#include <QTimer>

struct FourFingers {
    QPixmap *finger1;
    QPixmap *finger2;
    QPixmap *finger3;
    QPixmap *finger4;
};

class Fap50Reader : public QThread
{
    Q_OBJECT

Q_SIGNALS:
    void sig_NoImage();
    void sig_fpCount(const int &fpCount);
    void sig_ImageReady(const QPixmap &pm);
    void sig_samplingdone(const FourFingers &pm);
    void sig_SendMessage(const QString message,const int timeout=0);
public:
    typedef struct{
        int id;
        int devtype;
        int fp1size;
        int fp2size;
        int fp3size;
        int fp4size;
        unsigned char fp1data[2048];
        unsigned char fp2data[2048];
        unsigned char fp3data[2048];
        unsigned char fp4data[2048];
    }FPITEM;
public:
    Fap50Reader();
    Fap50Reader(int worktype);
    ~Fap50Reader();
    void run() override;

public:
    bool Connect();

    void StartCapture();

    void SplitImages(int stWidth, int stHeight);

public slots:
private:
    bool m_isOpen;
    int m_ImageWidth;
    int m_ImageHeight;
    int m_ImageSize;
    int m_BmpSize;

    unsigned char * m_RawData;
    unsigned char * m_BmpData;

    unsigned char * flat1data;
    unsigned char * flat2data;
    unsigned char * flat3data;
    unsigned char * flat4data;

    unsigned char * flat1bmp;
    unsigned char * flat2bmp;
    unsigned char * flat3bmp;
    unsigned char * flat4bmp;

    int fpCount;
    int* fpXY;

    QList<FPITEM> fpList;
public:
    int m_WorkType;
    bool force_capture = false;
    bool m_stop;

    FourFingers m_fourfingers;
};

#endif // FAP50READER_H
