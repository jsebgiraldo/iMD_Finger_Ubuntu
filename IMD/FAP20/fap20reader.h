#ifndef FAP20READER_H
#define FAP20READER_H

#include "IMD/common/devicediscover.h"
#include "IMD/common/fingersList.h"
#include "IMD/common/databasemanager.h"
#include "fap20controller.h"
#include "IMD/common/modes.h"

#include "fingerprint.h"
#include <QDateTime>
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>
#include <QByteArray>
#include <QTimer>
#include <QEvent>

#define WORK_CAPTUREIMAGE   1
#define WORK_ENROLTP        2
#define WORK_CAPTURETP      3

#define FPM_PLACEFINGER     0x01
#define FPM_LIFTFINGER      0x02
#define FPM_DRAWIMAGE       0x03
#define FPM_CAPTUREIMAGE    0x04
#define FPM_ENROLTP         0x05
#define FPM_CAPTURETP       0x06

class Fap20Reader : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void FPMessage(int worktype,int retval,unsigned char* data,int size);

    void fap20readerSignal_send_state(int state);
    void fap20readerSignal_send_message_to_mainwindow(const QString message);
    void fap20readerSignal_send_captureReport_to_mainwindow(const QString message, QString buttons);
    void fap20readerSignal_send_enrollReport_to_mainwindow(E_MODE_ENROLLSTATUS enrollStatus, QString message, QString name, QString ID);
    void fap20readerSignal_send_authReport_to_mainwindow(QString name, QString id, E_POPUP_BUTTON_TYPE buttons, E_POPUP_TYPE messageType,int matchscore = 0);
    void fap20readerSignal_send_verificationReport_to_mainwindow(QString name, QString id, E_POPUP_BUTTON_TYPE buttons, E_POPUP_TYPE messageType,int score = 0);
    void fap20readerSignal_sig_ImageReady();
    void fap20readerSignal_placeSamplingLabel(QString name, QString id, QString finger, int records);
public:
    E_TAB_TYPE CaptureMode;
    E_FINGER_POSITION current_finger;
    Fingerprint * Finger = nullptr;
    QList<UserTemplate> RefList;

    QString userID;
    QString userName;

    E_MODE_TYPE enrollMode;
    E_MODE_ENROLLSTATUS enrollStatus;
    bool startLiveVerification = false;
    int Fingers;
    int threshold;
    int matchThreshold = 50;
    bool IsCaptureForced;
    bool IsCapturing;
    bool keepRunning;
    bool forceCapture = false;

    Fap20Reader(QObject *parent = nullptr);

    bool Connect();
    bool Disconnect();

    void StartCapture(bool _keepRunning, int _threshold, E_FINGER_POSITION finger, E_MODE_TYPE mode);
    void StartEnroll(int _threshold, E_FINGER_POSITION finger, E_MODE_TYPE mode, QString username, QString userid);
    void StartAuth(E_MODE_TYPE mode);
    void StartVerification(E_MODE_TYPE mode, QString id, E_FINGER_POSITION finger);
    void Stop();

    void receiveThreshold(int value);
    void receiveForceCapture(bool _forceCapture);

    void VerifyIDFingerprint(const QString id, E_FINGER_POSITION finger);
    void EnrollFingerprintLive(int score, E_FINGER_POSITION finger, QString name, QString id);

    void AuthenticateFingerprint();
    void set_minimum_score(int _threshold);
    void set_current_finger(E_FINGER_POSITION finger);
    void set_match_threshold(int _threshold);

    void GetImageCapture(unsigned char* data);

    void handleOverwriteDecision(bool overwrite);
    std::byte* GenerateTemplate(Fingerprint fingerprint);
    bool IsConnected = false;
    bool m_stop;
    bool bAutoCapture = false;

    void EnrollFingerprint();

public slots:
    void slotFPMessage(int worktype,int retval,unsigned char* data,int size);

private:
    DeviceDiscover *Discover = nullptr;
    Fap20Controller *Controller = nullptr;
    QByteArray fingerprintTemplate;

    QString pendingUserID;
    E_FINGER_POSITION pendingFingerPosition;

    QMutex m_mutex;
    QQueue<QString> m_queue;
    QWaitCondition m_condition;

    QTimer captureTimer;

    QElapsedTimer stopwatch;
    BYTE RefBuf[TEMPLATE_SIZE];

    QString GetFingerString(E_FINGER_POSITION fingerPos);
    void SearchInDB();

    //---------------------------------------------------//
    int m_ImageWidth;
    int m_ImageHeight;

    int m_EnrolSize;
    int m_CaptureSize;
    int m_ImageSize;
    int m_BmpSize;

    unsigned char m_EnrolData[1024];
    unsigned char m_CaptureData[1024];
    unsigned char m_ImageData[300000];
    unsigned char m_BmpData[300000];

};

#endif // FAP20READER_H
