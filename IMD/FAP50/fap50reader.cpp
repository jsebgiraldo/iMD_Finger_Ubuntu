#include "IMD/FAP50/Fap50reader.h"
#include "mainwindow.h"
#include "qpainter.h"
#include "ui_mainwindow.h"
#include <qcoreapplication.h>
#include <QThread>

Fap50Reader::Fap50Reader()
{
    this->m_BmpData=new unsigned char[FAP50_BMP_SIZE];
    this->m_RawData=new unsigned char[FAP50_RAW_SIZE];

    this->flat1data=new unsigned char[FAP30_RAW_SIZE];
    this->flat2data=new unsigned char[FAP30_RAW_SIZE];
    this->flat3data=new unsigned char[FAP30_RAW_SIZE];
    this->flat4data=new unsigned char[FAP30_RAW_SIZE];
    this->flat1bmp=new unsigned char[FAP30_BMP_SIZE];
    this->flat2bmp=new unsigned char[FAP30_BMP_SIZE];
    this->flat3bmp=new unsigned char[FAP30_BMP_SIZE];
    this->flat4bmp=new unsigned char[FAP30_BMP_SIZE];

    fpCount = 0;
    fpXY = new int[40];

    FAP50_SetImageMode(0);
}

Fap50Reader::~Fap50Reader()
{
}

Fap50Reader::Fap50Reader(int worktype)
{
    m_WorkType=worktype;
}


void Fap50Reader::run()
{
    QPixmap pm = *new QPixmap();
    switch (m_WorkType) {
    case WORK_CAPTUREIMAGE:
        while(m_stop == false)StartCapture();
        emit sig_ImageReady(pm);
        qDebug() << "WORK_CAPTUREIMAGE";
        break;
    case WORK_ENROLTP:
        qDebug() << "WORK_ENROLTP";
        break;
    case WORK_CAPTURETP:
        qDebug() << "WORK_CAPTURETP";
        break;
    default:
        break;
    }
}

bool Fap50Reader::Connect()
{
    m_isOpen=true;
    GetImageSize(&m_ImageWidth,&m_ImageHeight);
    return FAP50_ResetDevice();
}

void Fap50Reader::StartCapture()
{
    if(FAP50_CaptureImage(HW_SCAN_MODE_FLAT,m_RawData,RAW_SIZE)>=0)
    {
        Lightness(m_RawData, RAW_SIZE, 64);
        Contrast(m_RawData, RAW_SIZE, 128);

        FAP50_ImageToBitmap(m_RawData,m_BmpData,IMAGE_WIDTH,IMAGE_HEIGHT,0);
        FAP50_FingerCheck(m_RawData,100,fpXY,&fpCount);
        emit sig_SendMessage("Finger Check Count:"+QString::number(fpCount));

        /*
                FILE *fh= fopen( "./finger.bmp", "wb" );
                if( (fh  ) == NULL )
                    return ;
                fwrite(g_FPBmpData,1,BMP_SIZE,fh);
                fclose(fh);
                */

        QPixmap * pm = new QPixmap();
        pm->loadFromData(m_BmpData,BMP_SIZE,"bmp");
        if(fpCount>0)
        {
            emit sig_fpCount(fpCount);
            QPainter* ps=new QPainter(pm);
            QPen pen;
            pen.setWidth(3);
            pen.setColor(QColor(0,200,0));
            ps->setPen(pen);
            //ps->setPen(Qt::green);
            for(int i=0;i<fpCount;i++)
            {
                int x1 = fpXY[i * 4 + 0];
                int y1 = fpXY[i * 4 + 2];
                int x2 = fpXY[i * 4 + 1];
                int y2 = fpXY[i * 4 + 3];
                ps->drawLine(x1,y1,x2,y1);
                ps->drawLine(x2,y1,x2,y2);
                ps->drawLine(x1,y2,x2,y2);
                ps->drawLine(x1,y1,x1,y2);
            }
            delete ps;

            SplitImages(400,500);
        }
        else
        {
            emit sig_NoImage();
            return;
        }
        emit sig_ImageReady(*pm);
        delete pm;
    }
}


void Fap50Reader::SplitImages(int stWidth, int stHeight)
{
    if (fpCount > 0)
    {
        unsigned char * flatdata=new unsigned char[FAP30_RAW_SIZE];

        //ui->graphicsView1->setScene(nullptr);
        //ui->graphicsView2->setScene(nullptr);
        //ui->graphicsView3->setScene(nullptr);
        //ui->graphicsView4->setScene(nullptr);

        for (int i = 0; i < fpCount; i++)
        {
            int x1 = fpXY[i * 4 + 0];
            int y1 = fpXY[i * 4 + 2];
            int x2 = fpXY[i * 4 + 1];
            int y2 = fpXY[i * 4 + 3];

            for (int j = 0; j < (stWidth * stHeight); j++)
                flatdata[j] = 0xFF;

            int w = (x2 - x1);
            int h = (y2 - y1);
            if (w > stWidth)
            {
                x2 = x1 + stWidth;
                w = stWidth;
            }
            if (h > stHeight)
            {
                y2 = y1 + stHeight;
                h = stHeight;
            }
            int l = (stWidth - w) / 2;
            int t = (stHeight - h) / 2;

            for (int y = 0; y < h; y++)
            {
                for (int x = 0; x < w; x++)
                {
                    flatdata[(y + t) * stWidth + l + x] = m_RawData[FAP50_IMAGE_WIDTH * (y1 + y) + x1 + x];
                }
            }

            if(force_capture == false)
            {
                return;
            }

            switch (i)
            {
            case 0:
            {
                memcpy(flat1data,flatdata,stWidth * stHeight);
                FAP50_ImageToBitmap(flatdata, flat1bmp, stWidth, stHeight, 0);

                QPixmap * pm = new QPixmap();
                pm->loadFromData(flat1bmp,BMP_SIZE,"bmp");
                m_fourfingers.finger1->loadFromData(flat1bmp,BMP_SIZE,"bmp");
                delete pm;
            }
            break;
            case 1:
            {
                memcpy(flat2data,flatdata,stWidth * stHeight);
                FAP50_ImageToBitmap(flatdata, flat2bmp, stWidth, stHeight, 0);

                QPixmap * pm = new QPixmap();
                pm->loadFromData(flat1bmp,BMP_SIZE,"bmp");
                m_fourfingers.finger2->loadFromData(flat1bmp,BMP_SIZE,"bmp");
                delete pm;
            }
            break;
            case 2:
            {
                memcpy(flat3data,flatdata,stWidth * stHeight);
                FAP50_ImageToBitmap(flatdata, flat3bmp, stWidth, stHeight, 0);

                QPixmap * pm = new QPixmap();
                pm->loadFromData(flat1bmp,BMP_SIZE,"bmp");
                m_fourfingers.finger3->loadFromData(flat1bmp,BMP_SIZE,"bmp");
                delete pm;
            }
            break;
            case 3:
            {
                memcpy(flat4data,flatdata,stWidth * stHeight);
                FAP50_ImageToBitmap(flatdata, flat4bmp, stWidth, stHeight, 0);

                QPixmap * pm = new QPixmap();
                pm->loadFromData(flat1bmp,BMP_SIZE,"bmp");
                m_fourfingers.finger4->loadFromData(flat1bmp,BMP_SIZE,"bmp");
                delete pm;
            }
            break;
            }
            force_capture = false;
            emit sig_samplingdone(m_fourfingers);
        }
        delete [] flatdata;
    }
}
