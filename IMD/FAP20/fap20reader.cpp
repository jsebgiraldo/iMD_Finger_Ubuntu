#include "fap20reader.h"
#include "IMD/FAP20/fap20thread.h"
#include "libs/fpcore.h"
#include "qapplication.h"
#include <QMessageBox>
#include <QThread>
#include <QLabel>
#include <QVBoxLayout>

Fap20Reader::Fap20Reader(QObject *parent) : QObject(parent)
{
    Discover = new DeviceDiscover();
    Controller = new Fap20Controller();

    connect(this,SIGNAL(FPMessage(int,int,unsigned char *,int)),this,SLOT(slotFPMessage(int,int,unsigned char *,int)));
}


void Fap20Reader::slotFPMessage(int worktype,int retval,unsigned char* data,int size)
{
    switch(worktype)
    {
    case FPM_PLACEFINGER:
        emit fap20readerSignal_send_message_to_mainwindow("Place Finger ...");
        break;
    case FPM_LIFTFINGER:
        emit fap20readerSignal_send_message_to_mainwindow("Lift Finger ...");
        break;
    case FPM_DRAWIMAGE:
        emit fap20readerSignal_send_message_to_mainwindow("Draw Finger Image");
        //DrawBitmap(data);
        break;
    case FPM_CAPTUREIMAGE:
        if(retval)
            emit fap20readerSignal_send_message_to_mainwindow("Capture Image OK");
        else
            emit fap20readerSignal_send_message_to_mainwindow("Capture Image Fail");
        break;
    case FPM_ENROLTP:
        if(retval)
        {
            m_EnrolSize=size;
            memcpy(m_EnrolData,data,size);
            emit fap20readerSignal_send_message_to_mainwindow("Enrol Template OK");
        }
        else
            emit fap20readerSignal_send_message_to_mainwindow("Enrol Template Fail");
        break;
    case FPM_CAPTURETP:
        if(retval)
        {
            m_CaptureSize=size;
            memcpy(m_CaptureData,data,size);
            emit fap20readerSignal_send_message_to_mainwindow("Capture Template OK");
            if(m_EnrolSize>0)
            {
                //int sc=MatchTemplate(m_EnrolData,m_EnrolSize,m_CaptureData,m_CaptureSize);
                //int sc=FAP20_MatchTemplate(m_EnrolData,m_EnrolSize,m_CaptureData,m_CaptureSize);
                int sc=FAP30_MatchTemplate(m_EnrolData,m_EnrolSize,m_CaptureData,m_CaptureSize);
                if(sc>50)
                    emit fap20readerSignal_send_message_to_mainwindow("Match OK,Scores:"+QString::number(sc));
                else
                    emit fap20readerSignal_send_message_to_mainwindow("Match Fail,Scores:"+QString::number(sc));
            }
        }
        else
            emit fap20readerSignal_send_message_to_mainwindow("Capture Template Fail");
        break;

    }
}

bool Fap20Reader::fap20clearDatabase(){
    bool result = dbManager->clearDatabase();
    return result;
}

void Fap20Reader::GetImageCapture()
{

}

void Fap20Reader::GetImageTemplate()
{

}

bool Fap20Reader::Detect(){
    return true;
}

bool Fap20Reader::Connect(){

    if(IsConnected)
    {
        return true;
    }

    Controller->SafeCloseDevice();

    if (Controller->SafeOpenDevice() == 1 && Controller->SafeLinkDevice(0) == 1)
    {
        IsConnected = true;
        Controller->SafeGetImageSize(&m_ImageWidth,&m_ImageHeight);
    }
    else
    {
        Controller->SafeCloseDevice();
        IsConnected = false;
    }

    return IsConnected;
}

bool Fap20Reader::Disconnect(){
    return Controller->SafeCloseDevice();
}

bool Fap20Reader::receiveDatabasePath(QString dbpath) {
    DBPath = dbpath;
    if (dbManager) {
        delete dbManager;
        dbManager = nullptr;
    }

    dbManager = new databasemanager(DBPath);
    if (!dbManager->isDatabaseOpen()) {
        delete dbManager;
        dbManager = nullptr;
        emit fap20readerSignal_send_message_to_mainwindow("Failed to open or create database");
        return false;
    }
    return true;
}

void Fap20Reader::receiveThreshold(int value){
    threshold = value;
}

void Fap20Reader::Stop()
{
    forceCapture = false;
    startLiveVerification = false;
    keepRunning = false;
    Controller->SafeResetDevice();
    return;
}

void Fap20Reader::set_minimum_score(int _threshold){
    qDebug() << "threshold changed to: " << _threshold;
    threshold = _threshold;
}

void Fap20Reader::set_current_finger(E_FINGER_POSITION finger){
    current_finger = finger;
    qDebug() << "current_finger changed to: " << GetFingerString(current_finger);
}

void Fap20Reader::receiveForceCapture(bool _forceCapture){
    forceCapture = _forceCapture;
}

void Fap20Reader::StartCapture(bool _keepRunning, int _threshold, E_FINGER_POSITION finger, E_MODE_TYPE mode, QString dbpath){
    CaptureMode = E_TAB_TYPE::E_TAB_TYPE_CAPTURE;
    current_finger = finger;
    enrollMode = mode;
    keepRunning = _keepRunning;
    threshold = _threshold;
    DBPath = dbpath;

    if(IsConnected)
    {
        Fap20Thread * fpwork=new Fap20Thread(this,WORK_CAPTURETP);
        fpwork->start();
    }


}

void Fap20Reader::StartEnroll(int _threshold, E_FINGER_POSITION finger, E_MODE_TYPE mode, QString username, QString userid){
    CaptureMode = E_TAB_TYPE::E_TAB_TYPE_ENROLL;
    threshold = _threshold;
    keepRunning = true;
    current_finger = finger;
    userName = username;
    userID = userid;
    enrollMode = mode;

    if(enrollMode == E_MODE_TYPE::E_MODE_AUTO){
        qDebug() << "Start enrolling with folowwing parameters: ";
        qDebug() << "userID: " << userID;
        qDebug() << "userName: " << userName;
        qDebug() << "threshold: " << threshold;
        qDebug() << "current_finger: " << GetFingerString(current_finger);
    }
    Controller->SafeEnrollTemplate();
}

void Fap20Reader::StartAuth(E_MODE_TYPE mode){
    CaptureMode = E_TAB_TYPE::E_TAB_TYPE_AUTH;
    keepRunning = true;
    enrollMode = mode;
    Controller->SafeEnrollTemplate();
}

void Fap20Reader::StartVerification(E_MODE_TYPE mode, QString id, E_FINGER_POSITION finger){
    CaptureMode = E_TAB_TYPE::E_TAB_TYPE_VERIFICATION;
    keepRunning = true;
    enrollMode = mode;
    userID = id;
    current_finger = finger;
    Controller->SafeEnrollTemplate();
}

void Fap20Reader::EnrollFingerprintLive(int score, E_FINGER_POSITION finger, QString name, QString id) {
    threshold = score;
    current_finger = finger;
    userName = name;
    userID = id;
    qDebug() << "Start enrolling with folowwing parameters: ";
    qDebug() << "userID: " << userID;
    qDebug() << "userName: " << userName;
    qDebug() << "threshold: " << threshold;
    qDebug() << "current_finger: " << GetFingerString(current_finger);
}

void Fap20Reader::SearchInDB(){
    keepRunning = false;
    qDebug() << "SearchInDB";
    keepRunning = false;
    int fingerprintCount;

    QString msgToLabel;
    if (dbManager->userExists(userID)) {
        QMap<E_FINGER_POSITION, QByteArray> fingerprints = dbManager->getFingerprintsByPositionAndUserID(userID);

        if (fingerprints.contains(current_finger)) {
            QByteArray storedTemplate = fingerprints.value(current_finger);
            std::vector<std::byte> storedTemplateBytes(storedTemplate.size());
            std::copy(storedTemplate.begin(), storedTemplate.end(), reinterpret_cast<char*>(storedTemplateBytes.data()));

            int match_score = Controller->SafeMatchTemplate(
                reinterpret_cast<std::byte*>(capturedTemplate.data()),
                storedTemplateBytes.data());

            if (match_score >= threshold) {


                emit fap20readerSignal_send_message_to_mainwindow(QString::number(match_score));

                pendingFingerPosition = current_finger;
                msgToLabel = "Fingerprint already exists for this user";
                enrollStatus = E_OLD_USER_REPEATED_FINGERPRINT;
                fingerprintCount = dbManager->getFingerprintCountForUser(userID);
                emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
                emit fap20readerSignal_placeSamplingLabel("", "", GetFingerString(current_finger), fingerprintCount);
                return;
            }
        }
        fingerprintCount = dbManager->getFingerprintCountForUser(userID);
        msgToLabel = "New fingeprint for registered user";
        enrollStatus = E_OLD_USER_NEW_FINGERPRINT_OK;
        emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
        emit fap20readerSignal_placeSamplingLabel("", "", GetFingerString(current_finger), fingerprintCount);
    } else {
        // User ID is not registered. Comparing current_finger with all the current_finger in the database
        QList<QPair<QString, QByteArray>> fingerprints = dbManager->getFingerprintsByPosition(current_finger);
        for (const auto& fingerprint : fingerprints) {
            QString userId = fingerprint.first;
            QByteArray storedTemplate = fingerprint.second;
            std::vector<std::byte> storedTemplateBytes(storedTemplate.size());
            std::copy(storedTemplate.begin(), storedTemplate.end(), reinterpret_cast<char*>(storedTemplateBytes.data()));

            int match_score = Controller->SafeMatchTemplate(
                reinterpret_cast<std::byte*>(capturedTemplate.data()),
                storedTemplateBytes.data());

            if (match_score >= threshold) {


                emit fap20readerSignal_send_message_to_mainwindow(QString::number(match_score));

                pendingUserID = userId;
                fingerprintCount = dbManager->getFingerprintCountForUser(pendingUserID);
                msgToLabel = "Fingerprint already exists for different id: " + pendingUserID;
                enrollStatus = E_NEW_USER_CONFLICT_WITH_ANOTHER_USER;
                emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
                emit fap20readerSignal_placeSamplingLabel("", "", GetFingerString(current_finger), fingerprintCount);
                return;
            }
        }
        fingerprintCount = dbManager->getFingerprintCountForUser(userID);
        msgToLabel = "New fingerprint for new user";
        enrollStatus = E_NEW_USER_NEW_FINGERPRINT_OK;
        emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
        emit fap20readerSignal_placeSamplingLabel("", "", GetFingerString(current_finger), fingerprintCount);
    }
}

void Fap20Reader::EnrollFingerprint(){
    keepRunning = false;
    qDebug() << "EnrollFingerprint";
    QString msgToBox;
    Stop();

    int fingerprintCount;

    QString msgToLabel;
    if (dbManager->userExists(userID)) {
        QMap<E_FINGER_POSITION, QByteArray> fingerprints = dbManager->getFingerprintsByPositionAndUserID(userID);

        if (fingerprints.contains(current_finger)) {
            QByteArray storedTemplate = fingerprints.value(current_finger);
            std::vector<std::byte> storedTemplateBytes(storedTemplate.size());
            std::copy(storedTemplate.begin(), storedTemplate.end(), reinterpret_cast<char*>(storedTemplateBytes.data()));

            if (Controller->SafeMatchTemplate(
                    reinterpret_cast<std::byte*>(capturedTemplate.data()),
                    storedTemplateBytes.data()) >= threshold) {

                pendingFingerPosition = current_finger;
                msgToLabel = "Fingerprint already exists for this user";
                enrollStatus = E_OLD_USER_REPEATED_FINGERPRINT;
                fingerprintCount = dbManager->getFingerprintCountForUser(userID);
                emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
                emit fap20readerSignal_placeSamplingLabel("", "", GetFingerString(current_finger), fingerprintCount);

                msgToBox = "Current fingerprint already exists for this user.\
                            \nWould you like to overwrite the existing fingerprint?";
                emit fap20readerSignal_send_enrollReport_to_mainwindow(enrollStatus, msgToBox, dbManager->getUserNameByID(userID), userID);
                return;
            }
        }

        if (!dbManager->addFingerprint_to_registeredUser(userID, current_finger, capturedTemplate)) {
            qDebug() << "Failed to store fingerprint for current user";
            emit fap20readerSignal_send_message_to_mainwindow("Failed to store fingerprint for current user");
            emit fap20readerSignal_send_enrollReport_to_mainwindow(E_MODE_ENROLLSTATUS::E_DATABASE_ERROR, "Database error", dbManager->getUserNameByID(userID), userID);
        }

        fingerprintCount = dbManager->getFingerprintCountForUser(userID);
        msgToLabel = "New fingeprint for registered user";
        enrollStatus = E_OLD_USER_NEW_FINGERPRINT_OK;
        emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
        emit fap20readerSignal_placeSamplingLabel("", "", GetFingerString(current_finger), fingerprintCount);
        emit fap20readerSignal_send_enrollReport_to_mainwindow(enrollStatus, msgToBox, dbManager->getUserNameByID(userID), userID);
    } else {
        // User ID is not registered. Comparing current_finger with all the current_finger in the database
        QList<QPair<QString, QByteArray>> fingerprints = dbManager->getFingerprintsByPosition(current_finger);
        for (const auto& fingerprint : fingerprints) {
            QString userId = fingerprint.first;
            QByteArray storedTemplate = fingerprint.second;
            std::vector<std::byte> storedTemplateBytes(storedTemplate.size());
            std::copy(storedTemplate.begin(), storedTemplate.end(), reinterpret_cast<char*>(storedTemplateBytes.data()));

            if (Controller->SafeMatchTemplate(
                    reinterpret_cast<std::byte*>(capturedTemplate.data()),
                    storedTemplateBytes.data()) >= threshold) {
                pendingUserID = userId;
                fingerprintCount = dbManager->getFingerprintCountForUser(pendingUserID);
                msgToLabel = "Fingerprint already exists for different id: " + pendingUserID;
                enrollStatus = E_NEW_USER_CONFLICT_WITH_ANOTHER_USER;
                emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
                emit fap20readerSignal_placeSamplingLabel("", "", GetFingerString(current_finger), fingerprintCount);

                msgToBox = "Fingerprint " + GetFingerString(current_finger) + " is already enrolled for a different user with id: " + pendingUserID + " and name: " + dbManager->getUserNameByID(pendingUserID);
                emit fap20readerSignal_send_enrollReport_to_mainwindow(enrollStatus, msgToBox, dbManager->getUserNameByID(pendingUserID), pendingUserID);
                return;
            }
        }
        if (!dbManager->addNewUserWithFingerprint(userID, userName, current_finger, capturedTemplate)) {
            msgToLabel = "Failed to create new user and store fingerprint";
            emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
            emit fap20readerSignal_send_enrollReport_to_mainwindow(E_MODE_ENROLLSTATUS::E_DATABASE_ERROR, "Database error", userName, userID);
        }

        fingerprintCount = dbManager->getFingerprintCountForUser(userID);
        msgToLabel = "New fingerprint for new user";
        enrollStatus = E_NEW_USER_NEW_FINGERPRINT_OK;
        emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
        emit fap20readerSignal_send_enrollReport_to_mainwindow(enrollStatus, msgToBox, userName, userID);
        emit fap20readerSignal_placeSamplingLabel("", "", GetFingerString(current_finger), fingerprintCount);
    }
}

void Fap20Reader::handleOverwriteDecision(bool overwrite) {
    if (overwrite) {
        QString msgToBox;
        if (!dbManager->deleteFingerprint(pendingUserID, pendingFingerPosition)) {
            emit fap20readerSignal_send_message_to_mainwindow("Failed to overwrite fingerprint for current user");
            emit fap20readerSignal_send_enrollReport_to_mainwindow(E_MODE_ENROLLSTATUS::E_DATABASE_ERROR, "Database error", dbManager->getUserNameByID(userID), userID);
            return;
        }

        if (!dbManager->addFingerprint_to_registeredUser(pendingUserID, pendingFingerPosition, capturedTemplate)) {
            qDebug() << "Failed to store fingerprint for current user";
            emit fap20readerSignal_send_message_to_mainwindow("Failed to store fingerprint for current user");
            emit fap20readerSignal_send_enrollReport_to_mainwindow(E_MODE_ENROLLSTATUS::E_DATABASE_ERROR, "Database error", dbManager->getUserNameByID(userID), userID);
            return;
        }

        msgToBox = "Fingerprint overwritten successfully";
        QString msgToLabel = "Fingerprint overwritten successfully";
        emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);

        emit fap20readerSignal_send_enrollReport_to_mainwindow(E_MODE_ENROLLSTATUS::E_OVERWRITE_SUCESS, msgToBox, dbManager->getUserNameByID(userID), userID);
    }
    QApplication::processEvents();
}

void Fap20Reader::set_match_threshold(int _threshold){
    matchThreshold = _threshold;
}

QString Fap20Reader::GetFingerString(E_FINGER_POSITION fingerPos){
    if(fingerPos == E_FINGER_POSITION::FINGER_POSITION_RIGHT_THUMB)
        return "Right thumb";
    else if(fingerPos == E_FINGER_POSITION::FINGER_POSITION_RIGHT_INDEX)
        return "Right index";
    else if(fingerPos == E_FINGER_POSITION::FINGER_POSITION_RIGHT_MIDDLE)
        return "Right middle";
    else if(fingerPos == E_FINGER_POSITION::FINGER_POSITION_RIGHT_RING)
        return "Right ring";
    else if(fingerPos == E_FINGER_POSITION::FINGER_POSITION_RIGHT_LITTLE)
        return "Right little";
    else if(fingerPos == E_FINGER_POSITION::FINGER_POSITION_LEFT_THUMB)
        return "Left thumb";
    else if(fingerPos == E_FINGER_POSITION::FINGER_POSITION_LEFT_INDEX)
        return "Left index";
    else if(fingerPos == E_FINGER_POSITION::FINGER_POSITION_LEFT_MIDDLE)
        return "Left middle";
    else if(fingerPos == E_FINGER_POSITION::FINGER_POSITION_LEFT_RING)
        return "Left ring";
    else if(fingerPos == E_FINGER_POSITION::FINGER_POSITION_LEFT_LITTLE)
        return "Left little";
    else
        return "Unknown";
}

void Fap20Reader::AuthenticateFingerprint() {
    keepRunning = false;
    Stop();
    std::vector<std::byte> capturedBuf(TEMPLATE_SIZE);
    int capturedSize = 0;

    // Capturar la huella digital
    if (!Controller->SafeGetTemplateByEnl(capturedBuf.data(), &capturedSize)) {
        qDebug() << "Failed to capture fingerprint template";
        emit fap20readerSignal_send_message_to_mainwindow("Failed to capture fingerprint template");
        return;
    }
    Stop();
    QByteArray capturedTemplate = QByteArray(reinterpret_cast<char*>(capturedBuf.data()), capturedSize);

    // Lista de posiciones de los dedos que sí son manejadas
    QList<E_FINGER_POSITION> validFingerPositions = {
        FINGER_POSITION_RIGHT_THUMB,
        FINGER_POSITION_RIGHT_INDEX,
        FINGER_POSITION_RIGHT_MIDDLE,
        FINGER_POSITION_RIGHT_RING,
        FINGER_POSITION_RIGHT_LITTLE,
        FINGER_POSITION_LEFT_THUMB,
        FINGER_POSITION_LEFT_INDEX,
        FINGER_POSITION_LEFT_MIDDLE,
        FINGER_POSITION_LEFT_RING,
        FINGER_POSITION_LEFT_LITTLE
    };

    QString msgToBox;
    QString msgToLabel;

    // Iterar a través de todas las posiciones válidas de los dedos
    for (E_FINGER_POSITION fingerPosition : validFingerPositions) {
        QList<QPair<QString, QByteArray>> fingerprints = dbManager->getFingerprintsByPosition(fingerPosition);

        for (const auto& fingerprint : fingerprints) {
            QString userId = fingerprint.first;
            QByteArray storedTemplate = fingerprint.second;
            std::vector<std::byte> storedTemplateBytes(storedTemplate.size());
            std::copy(storedTemplate.begin(), storedTemplate.end(), reinterpret_cast<char*>(storedTemplateBytes.data()));
            int matchScore = Controller->SafeMatchTemplate(
                reinterpret_cast<std::byte*>(capturedTemplate.data()),
                storedTemplateBytes.data());
            if (matchScore >= matchThreshold) {
                QString userName = dbManager->getUserNameByID(userId);
                msgToLabel = QString::number(matchScore);
                emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
                emit fap20readerSignal_placeSamplingLabel(userName, userId, GetFingerString(fingerPosition), 0);
                msgToBox = "Fingerprint is recognized for user with id: " + userId + " and name: " + userName;
                msgToBox = msgToBox + "\nReading again.";
                emit fap20readerSignal_send_authReport_to_mainwindow(userName, userId, E_POPUP_BUTTON_TYPE::E_POPUP_BUTTON_TYPE_OK, E_POPUP_TYPE::E_POPUP_TYPE_SUCCESS,matchScore);
                return;
            }
        }
    }

    msgToLabel = "Fingerprint not recognized.";
    emit fap20readerSignal_placeSamplingLabel("N/A","N/A","N/A",0);
    emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);

    emit fap20readerSignal_send_authReport_to_mainwindow("", "", E_POPUP_BUTTON_TYPE::E_POPUP_BUTTON_TYPE_OK, E_POPUP_TYPE::E_POPUP_TYPE_WARNING,0);

}

void Fap20Reader::VerifyIDFingerprint(const QString id, E_FINGER_POSITION finger) {
    keepRunning = false;
    std::vector<std::byte> capturedBuf(TEMPLATE_SIZE);
    int capturedSize = 0;

    if (!Controller->SafeGetTemplateByEnl(capturedBuf.data(), &capturedSize)) {
        qDebug() << "Failed to capture fingerprint template";
        emit fap20readerSignal_send_message_to_mainwindow("Failed to capture fingerprint template");
        return;
    }

    QByteArray capturedTemplate = QByteArray(reinterpret_cast<char*>(capturedBuf.data()), capturedSize);

    QMap<E_FINGER_POSITION, QByteArray> fingerprintsMap = dbManager->getFingerprintsByPositionAndUserID(id);

    if (fingerprintsMap.contains(finger)) {
        QByteArray storedTemplate = fingerprintsMap.value(finger);
        std::vector<std::byte> storedTemplateBytes(storedTemplate.size());
        std::copy(storedTemplate.begin(), storedTemplate.end(), reinterpret_cast<char*>(storedTemplateBytes.data()));

        int match_score = Controller->SafeMatchTemplate(
            reinterpret_cast<std::byte*>(capturedTemplate.data()),
            storedTemplateBytes.data());

        if (match_score >= matchThreshold) {
            QString userName = dbManager->getUserNameByID(id);
            //QString msgToBox = "Fingerprint for user ID: " + id + " and name: " + userName + " at position " + GetFingerString(finger) + " verified successfully.";
            QString msgToLabel = "Fingerprint verified";
            emit fap20readerSignal_placeSamplingLabel(userName, id, GetFingerString(current_finger), 0);
            emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
            emit fap20readerSignal_send_verificationReport_to_mainwindow(userName, id, E_POPUP_BUTTON_TYPE::E_POPUP_BUTTON_TYPE_OK, E_POPUP_TYPE::E_POPUP_TYPE_SUCCESS,match_score);

            return;
        } else {
            //QString msgToBox = "Fingerprint for user ID: " + id + " at position " + GetFingerString(finger) + " does not match the captured fingerprint.";
            QString msgToLabel = "Fingerprint mismatch";
            emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
            emit fap20readerSignal_send_verificationReport_to_mainwindow(userName, id, E_POPUP_BUTTON_TYPE::E_POPUP_BUTTON_TYPE_OK, E_POPUP_TYPE::E_POPUP_TYPE_WARNING,match_score);

            return;
        }
    } else {
        QString msgToBox = "No fingerprint found for user ID: " + id + " at position " + GetFingerString(finger) + ".";
        QString msgToLabel = "Fingerprint not found";
        emit fap20readerSignal_send_message_to_mainwindow(msgToLabel);
        emit fap20readerSignal_send_verificationReport_to_mainwindow(userName, id, E_POPUP_BUTTON_TYPE::E_POPUP_BUTTON_TYPE_OK, E_POPUP_TYPE::E_POPUP_TYPE_WARNING);

        return;
    }
}
