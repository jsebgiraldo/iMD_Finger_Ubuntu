#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "./libs/fpcore.h"
#include "./libs/fpdevice.h"
#include "./libs/fpstate.h"

#include "IMD/common/databasemanager.h"
#include "IMD/common/modes.h"
#include "qpushbutton.h"

enum E_SAMPLING_TYPE {
    E_SAMPLING_TYPE_ERROR,
    E_SAMPLING_TYPE_FLAT_442,
    E_SAMPLING_TYPE_FLAT_442_ROLL,
    E_SAMPLING_TYPE_FLAT_ANY_FINGER,
    E_SAMPLING_TYPE_FLAT,
    E_SAMPLING_TYPE_ROLL,
    E_SAMPLING_TYPE_CAPTURE,
};

enum E_FAP_TYPE{
    E_FAP20,
    E_FAP50,
    E_NONE
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    static void  HotPlugCallBackStub(uint8_t action,uint8_t iSerialNumber,uint16_t idVendor,uint16_t idProduct);
    void  HotPlugCallBack(uint8_t action,uint8_t iSerialNumber,uint16_t idVendor,uint16_t idProduct);

private slots:
    void on_ConnectButton_clicked();
    void on_DisconnectButton_clicked();

    void on_pushButton_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_10_clicked();

    void on_CaptureTab_clicked();
    void on_EnrollTab_clicked();
    void on_IdentificationTab_clicked();
    void on_AuthenticationTab_clicked();

    void on_tabWidget_currentChanged(E_TAB_TYPE index);

    void on_CaptureLiveModeButton_clicked();
    void on_AutoCaptureModeButton_clicked();
    void on_CaptureLiveModeButton_2_clicked();
    void on_AutoCaptureModeButton_2_clicked();
    void on_CaptureLiveModeButton_3_clicked();
    void on_AutoCaptureModeButton_3_clicked();
    void on_CaptureLiveModeButton_4_clicked();
    void on_AutoCaptureModeButton_4_clicked();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void enableFAP20layout();
    void enableFAP50layout();

public: //fap20
    void DrawBitmap(unsigned char* imagedata);
    void FPMessage(int worktype,int retval,unsigned char* data,int size);

private:
    void ui_initial_setup();
    void ui_customhand_setup();

    void deviceConnected_action();
    void deviceDisconnected_action();

    void ClearCaptureTab();
    void ClearEnrollTab();
    void ClearAuthTab();
    void ClearVerifyTab();

    void clearBackgroundColor(QPushButton* button);

    void status_bar_text(QString text, int timeout=1000);
    int template_popup(E_POPUP_TYPE mode, QString title, QString body,E_POPUP_BUTTON_TYPE btn_type = E_POPUP_BUTTON_TYPE_OK);

private:
    bool fap20_present = false;
    bool fap50_present = false;

    E_FAP_TYPE fap_controller = E_FAP_TYPE::E_NONE;

    E_MODE_TYPE currentMode = E_MODE_TYPE::E_MODE_LIVE;
    E_TAB_TYPE currentTab = E_TAB_TYPE::E_TAB_TYPE_CAPTURE;

private: //fap20
    bool m_isOpen;
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
private:
    Ui::MainWindow *ui;
    FPDEVINFO m_DevInfo;
    databasemanager *dbManager = nullptr;
};
#endif // MAINWINDOW_H
