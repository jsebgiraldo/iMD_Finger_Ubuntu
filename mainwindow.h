#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QStandardPaths>

#include "IMD/FAP20/fap20reader.h"
#include "IMD/common/modes.h"
#include "IMD/FAP20/fingerprint.h"

#include <QMainWindow>
#include <QTimer>

#include "./libs/fpcore.h"
#include "./libs/fpdevice.h"
#include "./libs/fpstate.h"


#define WM_NCLMESSAGE (WM_USER + 120)


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
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ConnectButton_clicked();

    void on_DisconnectButton_clicked();

    void on_CaptureLiveModeButton_clicked();

    void on_start_capture_clicked();

    void on_AutoCaptureModeButton_clicked();

    void on_stop_capture_btn_clicked();

    void on_device_combo_box_activated(int index);

    void on_tabWidget_currentChanged(E_TAB_TYPE index);

    void on_scoreThreshold_capture_spinBox_valueChanged(int arg1);

    void on_CaptureLiveModeButton_2_clicked();

    void on_AutoCaptureModeButton_2_clicked();

    void on_CaptureLiveModeButton_3_clicked();

    void on_AutoCaptureModeButton_3_clicked();

    void on_CaptureLiveModeButton_4_clicked();

    void on_AutoCaptureModeButton_4_clicked();

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_10_clicked();
    void on_databaselineedit_clicked();

    void on_verify_button_clicked();

    void on_enroll_pushButton_clicked();

    void on_save_button_clicked();

    void on_force_capture_btn_clicked();

    void on_cleardb_auth_button_clicked();

    void on_cleardb_enroll_button_clicked();

    void on_scoreThreshold_enroll_spinBox_valueChanged(int arg1);


    void on_scoreThreshold_auth_spinBox_valueChanged(int arg1);

    void on_scoreThreshold_verification_spinBox_valueChanged(int arg1);

    void on_finger_fap20_verification_combobox_currentTextChanged(const QString &arg1);

    void on_choose_finger_btn_clicked();

    void on_apply_btn_clicked();

    void on_choose_finge_capture_btn_clicked();

    void on_matchThreshold_auth_spinBox_valueChanged(int arg1);

    void on_matchThreshold_verification_spinBox_valueChanged(int arg1);

    void on_enroll_pushButton_fap50_clicked();

    void on_fap50_finger_option_combobox_activated(int index);

    void on_pushButton_7_clicked();

    void on_CaptureTab_clicked();

    void on_EnrollTab_clicked();

    void on_IdentificationTab_clicked();

    void on_AuthenticationTab_clicked();


    void on_choose_finger_verification_btn_clicked();

private:
    Ui::MainWindow *ui;

    Fap20Reader *fap20reader = nullptr;

    bool fap20_present = false;
    bool fap50_present = false;

    E_FAP_TYPE fap_controller = E_FAP_TYPE::E_NONE;

    E_MODE_TYPE currentMode = E_MODE_TYPE::E_MODE_LIVE;
    E_TAB_TYPE currentTab = E_TAB_TYPE::E_TAB_TYPE_CAPTURE;

    void enableFAP20layout();
    void enableFAP50layout();
    void onImageReadyFap20();
    void onFap20SetSamplingLabel(QString name,QString id,QString finger,int records);
    void onMessageFromFap20(QString message);
    void onCaptureFromFap20(QString message, QString buttons);
    void onEnrollFromFap20(E_MODE_ENROLLSTATUS enrollStatus, QString message, QString name, QString ID);
    void onAuthFromFap20(QString name, QString id, E_POPUP_BUTTON_TYPE buttons, E_POPUP_TYPE messageType,int score);
    void onVerificationFromFap20(QString name, QString id, E_POPUP_BUTTON_TYPE buttons, E_POPUP_TYPE messageType, int score);

    void onStatusFromFap20(int);

    void onSamplingDoneFap50(ImageProperty res);
    void onSamplingCustomDoneFap50(ImageProperty res, std::vector< E_FINGER_POSITION> vec);
    void onCancelSamplingFap50();
    void onCaptureInfoFap50(E_FINGER_POSITION pos);
    void onScoreFap50(ImageProperty score);

    void fap50enrollFinish();
    void fap50ClearEnrollFingers();

    void clearBackgroundColor(QPushButton* button);

    bool fap20StartCaptureProcess();
    bool fap20StartEnrollProcess();
    void fap20StartAuthProcess();
    bool fap20StartVerificationProcess();

    void StopButtonWidgets();


    void on_start_tab_capture();
    void on_start_tab_enroll();
    void on_start_tab_auth();
    void on_start_tab_verification();

    void deviceConnected_action();
    void deviceDisconnected_action();

    void ClearCaptureTab();
    void ClearEnrollTab();
    void ClearAuthTab();
    void ClearVerifyTab();

    int authentification_popup(E_POPUP_TYPE mode,QString title, QList<QString> body, E_POPUP_BUTTON_TYPE btn_type = E_POPUP_BUTTON_TYPE_OK);
    int template_popup(E_POPUP_TYPE mode, QString title, QString body,E_POPUP_BUTTON_TYPE btn_type = E_POPUP_BUTTON_TYPE_OK);

    //Database Functions
    bool OpenDataBase();

    E_FINGER_POSITION get_choose_capture_finger();
    E_FINGER_POSITION get_choose_enroll_finger();
    E_FINGER_POSITION get_choose_verification_finger();
    std::vector<E_FINGER_POSITION> get_choose_enroll_finger_442();

    void status_bar_text(QString text, int timeout = 0);
    E_FINGER_POSITION select_finger(QString text);

    static void  HotPlugCallBackStub(uint8_t action,uint8_t iSerialNumber,uint16_t idVendor,uint16_t idProduct);
    void  HotPlugCallBack(uint8_t action,uint8_t iSerialNumber,uint16_t idVendor,uint16_t idProduct);

private:

    Fingerprint *pt_fingerprint = nullptr;

    databasemanager *dbManager = nullptr;
    t_enroll_struct enroll_temp[FINGER_POSITION_SIZE];

    FPDEVINFO m_DevInfo;

    BYTE templateBytes[TEMPLATE_SIZE];
    int templateSize;

    int score_threshold = 50;
};
#endif // MAINWINDOW_H
