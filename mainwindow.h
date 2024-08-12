#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "IMD/common/databasemanager.h"
#include "IMD/common/modes.h"

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

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void enableFAP20layout();
    void enableFAP50layout();



private:
    void ui_initial_setup();
    void ui_customhand_setup();

    void deviceConnected_action();
    void deviceDisconnected_action();

    void ClearCaptureTab();
    void ClearEnrollTab();
    void ClearAuthTab();
    void ClearVerifyTab();

    void status_bar_text(QString text, int timeout);
    int template_popup(E_POPUP_TYPE mode, QString title, QString body,E_POPUP_BUTTON_TYPE btn_type = E_POPUP_BUTTON_TYPE_OK);

private:
    bool fap20_present = false;
    bool fap50_present = false;

    E_FAP_TYPE fap_controller = E_FAP_TYPE::E_NONE;

    E_MODE_TYPE currentMode = E_MODE_TYPE::E_MODE_LIVE;
    E_TAB_TYPE currentTab = E_TAB_TYPE::E_TAB_TYPE_CAPTURE;

private:
    Ui::MainWindow *ui;

    databasemanager *dbManager = nullptr;
};
#endif // MAINWINDOW_H
