#include "mainwindow.h"
#include "qgraphicseffect.h"
#include "ui_mainwindow.h"

#include <QStackedWidget>
#include <QListWidget>
#include <QMessageBox>
#include <QThread>
#include <QPainter>

// SECTIONS:
// FAP20 and FAP30: Layouts
// FAP20: Fingers ComboBox callbacks
// FAP20: Functions to start capture, enroll, authentication or validation
// FAP20: SECTION Callbacks from fap20 to display image or messages
// FAP20: SECTION Callbacks from fap20 to display QMessageBox
// SECTION START/STOP PUSHBUTTON
// SECTION Callbacks to capture/enroll/authentication/validation
// SECTION Score thresholds callbacks
// SECTION "Auto" and "Live" pushbutton callbacks
// SECTION Pushbutton to expand and close calibration settings
// SECTION Buttons on the right-hand tab widget
// SECTION Clear buttons on the right-hand side tabwidget

MainWindow *pMainWindow=0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pMainWindow=this;

    if(fpstate_start(HotPlugCallBackStub))
    {
        status_bar_text("Error to register USB callback\n");
    }

    on_CaptureLiveModeButton_clicked();

    ui->pushButton_3->setIcon(QIcon(":/img/icon.png"));

    ui->tabWidget_2->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(0);

    ui->tabWidget_2->tabBar()->setHidden(true);
    ui->match_score_widget->setHidden(true);

    //Capture Tab
    ui->choose_finger_widget_2->setHidden(true);
    ui->capture_tab_option_widget->setHidden(!ui->capture_tab_option_widget->isHidden());
    ui->groupBox_2->setHidden(true);

    ui->save_button->setEnabled(false);

    //Entool Tab
    ui->widget_68->setHidden(true);
    ui->groupBox->setHidden(true);

    ui->enroll_pushButton->setEnabled(false);
    ui->enroll_pushButton_fap50->setEnabled(false);

    ui->identification_group_box->setHidden(!ui->identification_group_box->isHidden());
    ui->capture_score_groupbox->setHidden(!ui->capture_score_groupbox->isHidden());
    ui->auth_score_groupbox->setHidden(!ui->auth_score_groupbox->isHidden());
    ui->enroll_score_groupbox->setHidden(!ui->enroll_score_groupbox->isHidden());
    on_pushButton_clicked();


    // Authentication Tab

    QString dbPath = QCoreApplication::applicationDirPath() + "/db";
    QDir dir(dbPath);
    if (!dir.exists()) {
        if (!dir.mkpath(dbPath)) {
            qDebug() << "No se pudo crear la carpeta 'db'.";
        }
    }
    ui->databasePath_lineEdit->setText(dbPath);

    pt_fingerprint = new class Fingerprint();

    fap20reader = new class Fap20Reader();
    fap50reader = new class Fap50Reader();


    //HandWidget = new class CustomHand(ui->label_70);
    ui->custom_hand_widget->setBackgroudLabel(ui->label_70);
    ui->capture_hand_widget->setBackgroudLabel(ui->capture_hand_label);

    ui->capture_hand_widget->group1->ChangeExclusive(true);

    ui->authentication_custom_hand->setBackgroudLabel(ui->label_7);

    ui->authentication_custom_hand->group1->ChangeExclusive(true);


    ui->capture_hand_widget->fingerRightIndex->setChecked(false);
    ui->capture_hand_widget->fingerRightLittle->setChecked(false);
    ui->capture_hand_widget->fingerRightMiddle->setChecked(false);
    ui->capture_hand_widget->fingerRightRing->setChecked(false);
    ui->capture_hand_widget->fingerRightThumb->setChecked(false);

    ui->capture_hand_widget->fingerLeftIndex->setChecked(false);
    ui->capture_hand_widget->fingerLeftLittle->setChecked(false);
    ui->capture_hand_widget->fingerLeftMiddle->setChecked(false);
    ui->capture_hand_widget->fingerLeftRing->setChecked(false);
    ui->capture_hand_widget->fingerLeftThumb->setChecked(false);

    ui->authentication_custom_hand->fingerRightIndex->setChecked(false);
    ui->authentication_custom_hand->fingerRightLittle->setChecked(false);
    ui->authentication_custom_hand->fingerRightMiddle->setChecked(false);
    ui->authentication_custom_hand->fingerRightRing->setChecked(false);
    ui->authentication_custom_hand->fingerRightThumb->setChecked(false);

    ui->authentication_custom_hand->fingerLeftIndex->setChecked(false);
    ui->authentication_custom_hand->fingerLeftLittle->setChecked(false);
    ui->authentication_custom_hand->fingerLeftMiddle->setChecked(false);
    ui->authentication_custom_hand->fingerLeftRing->setChecked(false);
    ui->authentication_custom_hand->fingerLeftThumb->setChecked(false);


    ui->DisconnectButton->setHidden(true);
    ui->stop_capture_btn->setHidden(true);

    ui->apply_btn->setHidden(true);

    connect(fap20reader, &Fap20Reader::fap20readerSignal_sig_ImageReady, this, &MainWindow::onImageReadyFap20);
    connect(fap20reader, &Fap20Reader::fap20readerSignal_placeSamplingLabel, this, &MainWindow::onFap20SetSamplingLabel);
    connect(fap20reader, &Fap20Reader::fap20readerSignal_send_message_to_mainwindow, this, &MainWindow::onMessageFromFap20);
    connect(fap20reader, &Fap20Reader::fap20readerSignal_send_captureReport_to_mainwindow, this, &MainWindow::onCaptureFromFap20);
    connect(fap20reader, &Fap20Reader::fap20readerSignal_send_enrollReport_to_mainwindow, this, &MainWindow::onEnrollFromFap20);
    connect(fap20reader, &Fap20Reader::fap20readerSignal_send_authReport_to_mainwindow, this, &MainWindow::onAuthFromFap20);
    connect(fap20reader, &Fap20Reader::fap20readerSignal_send_verificationReport_to_mainwindow, this, &MainWindow::onVerificationFromFap20);

    connect(fap50reader, &Fap50Reader::sig_ImageReady, this, &MainWindow::onImageReadyFap50);
    connect(fap50reader, &Fap50Reader::sig_samplingdone, this, &MainWindow::onSamplingDoneFap50);

    connect(fap50reader, &Fap50Reader::sig_NoImage, this, &MainWindow::onNoImageFap50);
    connect(fap50reader, &Fap50Reader::sig_fpCount, this, &MainWindow::onFpCountFap50);
    connect(fap50reader, &Fap50Reader::sig_SendMessage, this, &MainWindow::status_bar_text);

    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(on_databaselineedit_clicked()));
}


MainWindow::~MainWindow()
{
    delete ui;
}



void  MainWindow::HotPlugCallBackStub(uint8_t action,uint8_t iSerialNumber,uint16_t idVendor,uint16_t idProduct)
{
    if(pMainWindow)
        pMainWindow->HotPlugCallBack(action,iSerialNumber,idVendor,idProduct);
}

void  MainWindow::HotPlugCallBack(uint8_t action,uint8_t iSerialNumber,uint16_t idVendor,uint16_t idProduct)
{
    char status[128];
    switch(action)
    {
    case 0:
        status_bar_text("Start Hotplug Thread\n");
        break;
    case 1:
        sprintf(status,"Add USB device: VID(0x%x) PID(0x%x) SerialNumber(0x%x)\n", idVendor, idProduct,iSerialNumber);
        status_bar_text(status);
        break;
    case 2:
        sprintf(status,"Remove USB device: VID(0x%x) PID(0x%x) SerialNumber(0x%x)\n", idVendor, idProduct,iSerialNumber);
        status_bar_text(status);
        if(fap20_present == true)fap20reader->Disconnect();
        on_DisconnectButton_clicked();
        break;
    case 3:
        status_bar_text("Exit Hotplug Thread\n");
        break;
    }
}



void MainWindow::on_databaselineedit_clicked(){
    QString documentsPath = ui->databasePath_lineEdit->text();
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                          documentsPath,
                                                          QFileDialog::ShowDirsOnly
                                                              | QFileDialog::DontResolveSymlinks);
    if (!directory.isEmpty()) {
        ui->databasePath_lineEdit->setText(directory);
        //qDebug() << "Directory selected:" << directory;
    }
}

void MainWindow::status_bar_text(QString text, int timeout)
{
    QString style =    "font-size:18px;"
                    "background-color: rgb(255, 255, 255);"
                    "top: 1027px; \
        left: 72px; \
        width: 126px;   \
        height: 21px;   \
        /* UI Properties */ \
        letter-spacing: var(--unnamed-character-spacing-0); \
        text-align: left;   \
        letter-spacing: 0px;    \
        color: #616E85; \
        opacity: 1;\
        padding-left:7px;\
        padding-right:7px;\
        border:0px solid black;";

        ui->statusbar->clearMessage();
    ui->statusbar->setStyleSheet(style);
    ui->statusbar->showMessage(text);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------
// FAP20 and FAP30: Layouts -----------------------------------------------------------------------------------------------------------------------
void MainWindow::enableFAP20layout(){
    ui->widget_184->setHidden(true);

    // Capture Tab
    ui->groupBox_2->setHidden(true);

    ui->capture_hand_widget->fingerRightIndex->click();

    // Enroll Tab
    ui->fap50_finger_option_combobox->setHidden(true);
    ui->choose_finger_widget->setHidden(true);

    ui->choose_finger_widget_verification->setHidden(true);

    ui->custom_hand_widget->group1->ChangeExclusive(true);


    ui->custom_hand_widget->fingerRightIndex->setChecked(false);
    ui->custom_hand_widget->fingerRightLittle->setChecked(false);
    ui->custom_hand_widget->fingerRightMiddle->setChecked(false);
    ui->custom_hand_widget->fingerRightRing->setChecked(false);
    ui->custom_hand_widget->fingerRightThumb->setChecked(false);

    ui->custom_hand_widget->fingerLeftIndex->setChecked(false);
    ui->custom_hand_widget->fingerLeftLittle->setChecked(false);
    ui->custom_hand_widget->fingerLeftMiddle->setChecked(false);
    ui->custom_hand_widget->fingerLeftRing->setChecked(false);
    ui->custom_hand_widget->fingerLeftThumb->setChecked(false);

    // DEFAULT ON
    ui->custom_hand_widget->fingerRightIndex->click();
    ui->authentication_custom_hand->fingerRightIndex->click();

    ui->cleardb_enroll_button->setEnabled(true);

    ui->finger_enroll_label->setText(pt_fingerprint->finger_position_string_map[get_choose_enroll_finger()]);

}

void MainWindow::enableFAP50layout(){
    ui->widget_47->setHidden(false);
    ui->widget_184->setHidden(false);

    // Capture Tab
    ui->groupBox_2->setHidden(false);

    ui->capture_hand_widget->fingerRightIndex->click();

    // Enroll Tab
    ui->fap50_finger_option_combobox->setHidden(false);
    ui->custom_hand_widget->group1->ChangeExclusive(false);

    ui->custom_hand_widget->fingerRightIndex->setChecked(true);
    ui->custom_hand_widget->fingerRightLittle->setChecked(true);
    ui->custom_hand_widget->fingerRightMiddle->setChecked(true);
    ui->custom_hand_widget->fingerRightRing->setChecked(true);
    ui->custom_hand_widget->fingerRightThumb->setChecked(true);

    ui->custom_hand_widget->fingerLeftIndex->setChecked(true);
    ui->custom_hand_widget->fingerLeftLittle->setChecked(true);
    ui->custom_hand_widget->fingerLeftMiddle->setChecked(true);
    ui->custom_hand_widget->fingerLeftRing->setChecked(true);
    ui->custom_hand_widget->fingerLeftThumb->setChecked(true);

    ui->authentication_custom_hand->fingerRightIndex->click();

    ui->groupBox->setHidden(true);

    ui->choose_finger_widget->setHidden(true);
    ui->choose_finger_widget_verification->setHidden(true);

    ui->cleardb_enroll_button->setEnabled(true);
}

//------------------------------------------------------------------------------------------------------------------------------------------------->> END FAP20 and FAP30: Layouts
// FAP20: Fingers ComboBox callbacks --------------------------------------------------------------------------------------------------------------


void MainWindow::on_finger_fap20_verification_combobox_currentTextChanged(const QString &arg1)
{

    ui->finger_verif_label->setText(arg1);

    if(fap_controller == E_FAP20){
        fap20reader->set_current_finger(select_finger(arg1));
    }else if(fap_controller == E_FAP50){

    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------->> END FAP20: Fingers ComboBox callbacks
// FAP20: Functions to start capture, enroll, authentication or validation ---------------------------------------------------------------------------------
bool MainWindow::fap20StartCaptureProcess(){
    if(currentMode == E_MODE_TYPE::E_MODE_AUTO){
        if(ui->databasePath_lineEdit->text() == ""){
            template_popup(E_POPUP_TYPE_WARNING,"Error", "Database path is empty.");
            return false;
        }else{
            QFileInfo checkDir(ui->databasePath_lineEdit->text());
            if (!checkDir.exists() || !checkDir.isDir()) {
                template_popup(E_POPUP_TYPE_WARNING,"Error", "Database path is not valid.");
                return false;
            }
        }
    }

    fap20reader->StartCapture(true, ui->scoreThreshold_capture_spinBox->value(),get_choose_capture_finger() , currentMode);



    return true;
}

bool MainWindow::fap20StartEnrollProcess(){
    QString name = ui->name_enroll_lineedit->text();
    QString id = ui->id_enroll_lineedit->text();
    fap20reader->StartEnroll(ui->scoreThreshold_enroll_spinBox->value(), get_choose_enroll_finger(), currentMode, name, id);
    return true;
}

void MainWindow::fap20StartAuthProcess()
{
    fap20reader->set_match_threshold(ui->matchThreshold_auth_spinBox->value());
    fap20reader->StartAuth(currentMode);
}

bool MainWindow::fap20StartVerificationProcess()
{
    fap20reader->set_match_threshold(ui->matchThreshold_verification_spinBox->value());
    fap20reader->StartVerification(currentMode, ui->id_verification_label->text(), get_choose_verification_finger());
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------->> END FAP20: Functions to start capture, enroll, authentication or validation
// FAP20: SECTION Callbacks from fap20 to display image or messages -----------------------------------------------------------------------------------
void MainWindow::onImageReadyFap20()
{
    QTransform transform;
    transform.rotate(180);

    QImage originalImage = fap20reader->Finger->pixmap.toImage();
    QImage rotatedImage = originalImage.transformed(transform);

    QPixmap rotatedPixmap = QPixmap::fromImage(rotatedImage);

    if(currentTab == E_TAB_TYPE_CAPTURE){
        ui->preview_capture->setPixmap(rotatedPixmap.scaled(ui->preview_capture->width(), ui->preview_capture->height(), Qt::KeepAspectRatio));
    }else if(currentTab == E_TAB_TYPE_ENROLL){
        ui->preview_enroll->setPixmap(rotatedPixmap.scaled(ui->preview_enroll->width(), ui->preview_enroll->height(), Qt::KeepAspectRatio));
    }else if(currentTab == E_TAB_TYPE_AUTH){
        ui->preview_auth->setPixmap(rotatedPixmap.scaled(ui->preview_auth->width(), ui->preview_auth->height(), Qt::KeepAspectRatio));
    }else{
        ui->preview_verification->setPixmap(rotatedPixmap.scaled(ui->preview_verification->width(), ui->preview_verification->height(), Qt::KeepAspectRatio));
    }

    ui->score_label->setText(QString::number(fap20reader->Finger->Score));
}

void MainWindow::onFap20SetSamplingLabel(QString name = "N/A", QString id = "N/A", QString finger = "N/A", int records = 0){

    ui->capture_sample_widget->setEnabled(true);

    StopButtonWidgets();

    QTransform transform;
    transform.rotate(180);

    QImage originalImage = fap20reader->Finger->pixmap.toImage();
    QImage rotatedImage = originalImage.transformed(transform);

    QPixmap rotatedPixmap = QPixmap::fromImage(rotatedImage);

    if(currentTab == E_TAB_TYPE_CAPTURE){
        ui->save_button->setEnabled(true);
        ui->save_button->setStyleSheet(ui->save_button->styleSheet() + "QPushButton{color: #FFFFFF;background-color: rgb(97, 110, 133);}");
        ui->preview_capture->clear();
        ui->capture_sampling_label->setPixmap(rotatedPixmap.scaled(ui->capture_sampling_label->width(),ui->capture_sampling_label->height(),Qt::KeepAspectRatio));
        ui->capture_score_label->setText(QString::number(fap20reader->Finger->Score));
    }else if(currentTab == E_TAB_TYPE_ENROLL){
        ui->enroll_pushButton->setEnabled(true);
        ui->preview_enroll->clear();
        ui->enroll_pushButton->setStyleSheet(ui->enroll_pushButton->styleSheet() + "QPushButton{color: #FFFFFF;background-color: rgb(97, 110, 133);}");
        ui->enroll_sampling_label->setPixmap(rotatedPixmap.scaled(ui->enroll_sampling_label->width(),ui->enroll_sampling_label->height(),Qt::KeepAspectRatio));
        ui->records_enroll_label->setText(QString::number(records));
        ui->finger_enroll_label->setText(finger);
        ui->score_enroll_label->setText(QString::number(fap20reader->Finger->Score));
        ui->choose_finger_btn->setEnabled(true);
    }else if(currentTab == E_TAB_TYPE_AUTH){
        ui->cleardb_auth_button->setEnabled(true);
        ui->cleardb_auth_button->setStyleSheet(ui->cleardb_auth_button->styleSheet() + "QPushButton{color: #FFFFFF;background-color: rgb(97, 110, 133);}");
        ui->auth_sampling_label->setPixmap(rotatedPixmap.scaled(ui->auth_sampling_label->width(),ui->auth_sampling_label->height(),Qt::KeepAspectRatio));
        ui->finger_auth_label->setText(finger);
        ui->score_auth_label->setText(QString::number(fap20reader->Finger->Score));
        ui->id_auth_lineEdit->setText(id);
        ui->name_auth_lineEdit->setText(name);
        ui->preview_auth->clear();
    }else if(currentTab == E_TAB_TYPE_VERIFICATION){
        ui->preview_verification->clear();
        ui->verify_button->setEnabled(true);
        ui->verify_button->setStyleSheet(ui->verify_button->styleSheet() + "QPushButton{color: #FFFFFF;background-color: rgb(97, 110, 133);}");
        ui->verification_sampling_label->setPixmap(rotatedPixmap.scaled(ui->verification_sampling_label->width(),ui->verification_sampling_label->height(),Qt::KeepAspectRatio));
        ui->score_verification_label->setText(QString::number(fap20reader->Finger->Score));
        fap20reader->Stop();
    }
}

void MainWindow::onMessageFromFap20(QString message){
    status_bar_text(message);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------->> END FAP20: SECTION Callbacks from fap20 to display image or messages
// FAP20: SECTION Callbacks from fap20 to display QMessageBox ----------------------------------------------------------------------------------------
void MainWindow::onCaptureFromFap20(QString message, QString buttons)
{
    QMessageBox box;
    box.setText(message);
    box.setWindowTitle("Message");

    if (buttons == "ok") {
        ui->stop_capture_btn->setHidden(true);
        ui->start_capture->setHidden(false);
        fap20reader->Stop();
        box.setStandardButtons(QMessageBox::Ok);
        box.exec();
    } else if (buttons == "yes/no") {
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int result = box.exec();

        if (result == QMessageBox::Yes) {

        } else if (result == QMessageBox::No) {
            fap20reader->Stop();
            StopButtonWidgets();
            qDebug() << "User clicked No";
        }
    }
}

void MainWindow::onAuthFromFap20(QString name, QString id, E_POPUP_BUTTON_TYPE buttons, E_POPUP_TYPE messageType, int score)
{

    if(messageType == E_POPUP_TYPE_WARNING)
    {
        ui->preview_auth->clear();
        ui->match_auth_label->setText(QString::number(score));
        ui->math_global_score_label->setText(QString::number(score));

        template_popup(E_POPUP_TYPE_WARNING,"Error","No person found",E_POPUP_BUTTON_TYPE_OK);
    }
    else if(messageType == E_POPUP_TYPE_SUCCESS)
    {
        ui->match_auth_label->setText(QString::number(score));
        ui->math_global_score_label->setText(QString::number(score));


        authentification_popup(E_POPUP_TYPE_SUCCESS,"Sucess",{name,id,"Person found"},E_POPUP_BUTTON_TYPE_OK);
    }

}

void MainWindow::onEnrollFromFap20(E_MODE_ENROLLSTATUS enrollStatus, QString message, QString name, QString ID)
{
    qDebug() << enrollStatus << message << name << ID;
    int res;
    switch(enrollStatus){
    case E_NEW_USER_NEW_FINGERPRINT_OK:
        authentification_popup(E_POPUP_TYPE_SUCCESS,"Sucess",{name,ID,message},E_POPUP_BUTTON_TYPE_OK);
        break;
    case E_NEW_USER_CONFLICT_WITH_ANOTHER_USER:
        template_popup(E_POPUP_TYPE_WARNING,"Error","Fingerprint already exists for different ID",E_POPUP_BUTTON_TYPE_OK);
        break;
    case E_OLD_USER_NEW_FINGERPRINT_OK:
        authentification_popup(E_POPUP_TYPE_SUCCESS,"Sucess",{name,ID,"User already exists."},E_POPUP_BUTTON_TYPE_OK);
        break;
    case E_OLD_USER_REPEATED_FINGERPRINT:
        res = (int)template_popup(E_POPUP_TYPE_INFO,"Info",message,E_POPUP_BUTTON_TYPE_YES_NO);
        fap20reader->handleOverwriteDecision(res);
        break;
    case E_OVERWRITE_SUCESS:
        authentification_popup(E_POPUP_TYPE_SUCCESS,"Sucess",{name,ID,message},E_POPUP_BUTTON_TYPE_OK);
        break;
    case E_DATABASE_ERROR:

        break;
    case E_NONE_STATUS:
        break;
    }
}



void MainWindow::onVerificationFromFap20(QString name, QString id, E_POPUP_BUTTON_TYPE buttons, E_POPUP_TYPE messageType,int score)
{

    if(messageType == E_POPUP_TYPE_WARNING)
    {
        ui->stop_capture_btn->click();

        ui->preview_verification->clear();
        ui->math_verifi_label->setText(QString::number(score));
        ui->math_global_score_label->setText(QString::number(score));

        template_popup(E_POPUP_TYPE_WARNING,"Error","Fingerprint does not match the captured fingerprint",E_POPUP_BUTTON_TYPE_OK);

    }
    else if(messageType == E_POPUP_TYPE_SUCCESS)
    {
        ui->math_verifi_label->setText(QString::number(score));
        ui->math_global_score_label->setText(QString::number(score));

        authentification_popup(E_POPUP_TYPE_SUCCESS,"Sucess",{name,id,"Fingerprint verified successfully"},E_POPUP_BUTTON_TYPE_OK);
    }
}
//------------------------------------------------------------------------------------------------------------------------------------>> END  FAP20: SECTION Callbacks from fap20 to display QMessageBox
void MainWindow::fap50ClearEnrollFingers()
{
    QList<QLabel*> allLabels = ui->enroll_fingers_widget->findChildren<QLabel*>();
    foreach(QLabel* label, allLabels)
    {
        if(!label->pixmap().isNull())label->clear();
        if(label->objectName().contains("score"))label->clear();
    }
}


void MainWindow::deviceConnected_action()
{

    ui->radio_status_devices->setChecked(true);
    ui->radio_status_devices->setText("Connected");


    ui->ConnectButton->setHidden(true);
    ui->DisconnectButton->setHidden(false);

    ui->widget_options->setEnabled(true);
    ui->widget_13->setEnabled(true);
    ui->capture_calibration_widget->setEnabled(true);

    ui->start_capture->setStyleSheet(ui->start_capture->styleSheet() + "QPushButton{color: #FFFFFF;background-color: rgb(97, 110, 133);}");
    ui->widget_56->setStyleSheet(ui->widget_56->styleSheet() + "QRadioButton::checked {color: #000000;border-bottom:3px solid; border-bottom-color: #FF0000;}");

    //status_bar_text("Welcome! Device connected.",4000);

}

void MainWindow::deviceDisconnected_action()
{
    ui->radio_status_devices->setChecked(false);

    ui->radio_status_devices->setText("Disconnected");

    ui->DisconnectButton->setHidden(true);
    ui->ConnectButton->setHidden(false);

    ui->widget_options->setEnabled(false);
    ui->widget_13->setEnabled(false);

    ui->device_combo_box->setCurrentIndex(0);

    ui->start_capture->setStyleSheet(ui->start_capture->styleSheet() + "QPushButton{color: #CFD6E4;background-color: rgb(239, 242, 245);}");
    ui->widget_56->setStyleSheet(ui->widget_56->styleSheet() + "QRadioButton::checked {color: #616E85;border-bottom:0px solid;}");
}

void MainWindow::on_DisconnectButton_clicked()
{
    ui->CaptureTab->click();

    fap20_present = false;
    fap50_present = false;

    deviceDisconnected_action();

    ClearCaptureTab();
    ClearEnrollTab();
    ClearAuthTab();
    ClearVerifyTab();

    //status_bar_text("Device disconnected!",4000);
}

void MainWindow::on_ConnectButton_clicked()
{

    if(OpenDevice(0,0,0) == 1 && LinkDevice(0) == 1)
    {
        if(GetSensorType()==0x50)
        {
            fap50_present = true;
            fap_controller = E_FAP50;
        }
        else if(GetSensorType()==0x30)
        {
            fap20_present = true;
            fap_controller = E_FAP20;
        }
        else if(GetSensorType()==0x20)
        {
            fap20_present = true;
            fap_controller = E_FAP20;
        }

    }else
    {
        fap_controller = E_NONE;
        fap20_present = false;
        fap50_present = false;
        template_popup(E_POPUP_TYPE_WARNING,"Error","No devices found.");
        return;
    }


    if(fap20_present && fap20reader->Connect())
    {
        if(!OpenDataBase())template_popup(E_POPUP_TYPE_WARNING,"Error","Failed to open or create database.");
        deviceConnected_action();
        ui->device_combo_box->setCurrentIndex(1);
        enableFAP20layout();
        return;
    }
    else if(fap50_present && fap50reader->Connect())
    {
        if(!OpenDataBase())template_popup(E_POPUP_TYPE_WARNING,"Error","Failed to open or create database.");
        deviceConnected_action();
        ui->device_combo_box->setCurrentIndex(2);
        enableFAP50layout();
        return;
    }

    fap_controller = E_NONE;
    template_popup(E_POPUP_TYPE_WARNING,"Error","Could not be connected");
}

//-----------------------------------------------------------------------------------------------------------------------------
// SECTION START/STOP PUSHBUTTON ---------------------------------------------------------------------------------------------------
void MainWindow::on_start_capture_clicked()
{
    qDebug() << "on_start_capture_clicked";
    StopButtonWidgets();

    m_stop = false;

    ui->start_capture->setHidden(true);
    ui->stop_capture_btn->setHidden(false);

    ui->CaptureTab->setEnabled(ui->stackedWidget->currentIndex() == 0 ? true : false);
    ui->EnrollTab->setEnabled(ui->stackedWidget->currentIndex() == 1 ? true : false);
    ui->IdentificationTab->setEnabled(ui->stackedWidget->currentIndex() == 2 ? true : false);
    ui->AuthenticationTab->setEnabled(ui->stackedWidget->currentIndex() == 3 ? true : false);

    if(currentMode == E_MODE_LIVE)
    {
        status_bar_text("Image capture, Press Force Capture for record it!.");
        ui->force_capture_btn->setEnabled(true);
        ui->force_capture_btn->setStyleSheet(ui->force_capture_btn->styleSheet() + "QPushButton{color: #FFFFFF;background-color: rgb(97, 110, 133);}");
    }

    if(currentTab == E_TAB_TYPE_CAPTURE){
        qDebug() << "Start Capture";
        on_start_tab_capture();
    }else if(currentTab == E_TAB_TYPE_ENROLL){
        qDebug() << "Start Enroll";
        on_start_tab_enroll();
    }else if(currentTab == E_TAB_TYPE_AUTH){
        qDebug() << "Start Authentication";
        on_start_tab_auth();

    }else if(currentTab == E_TAB_TYPE_VERIFICATION){
        qDebug() << "Start Verification";
        on_start_tab_verification();
    }

}

void MainWindow::on_stop_capture_btn_clicked()
{

    m_stop = true;

    if(fap_controller == E_FAP20){
        fap20reader->Stop();
    }else if(fap_controller == E_FAP50){
        fap50reader->m_stop = true;
    }

    StopButtonWidgets();
}

//----------------------------------------------------------------------------------------------------------------------------->> END SECTION START/STOP PUSHBUTTON
// SECTION Callbacks to capture/enroll/authentication/validation --------------------------------------------------------------
void MainWindow::on_start_tab_capture()
{
    ui->widget_125->setEnabled(false);
    ui->scoreThreshold_capture_spinBox->setEnabled(false);
    ui->choose_finge_capture_btn->setEnabled(false);
    ui->groupBox_2->setEnabled(false);

    ClearCaptureTab();
    if(fap_controller == E_FAP20){
        if(!fap20StartCaptureProcess())StopButtonWidgets();
    }else if(fap_controller == E_FAP50)
    {
        fap50reader->m_stop = false;
        fap50reader->m_WorkType = WORK_CAPTUREIMAGE;
        fap50reader->start();
    }
}

void MainWindow::on_start_tab_enroll()
{
    ui->widget_108->setEnabled(false);

    ui->fap50_finger_option_combobox->setEnabled(false);
    ui->groupBox->setEnabled(false);

    ui->scoreThreshold_enroll_spinBox->setEnabled(false);

    ui->choose_finger_btn->setEnabled(false);

    ClearEnrollTab();

    if(fap_controller == E_FAP20){
        if(!fap20StartEnrollProcess())StopButtonWidgets();

    }else if(fap_controller == E_FAP50){
        fap50reader->m_stop = false;
        fap50reader->m_WorkType = WORK_CAPTUREIMAGE;
        fap50reader->start();
    }

}

void MainWindow::on_start_tab_auth()
{
    ui->widget_73->setEnabled(false);

    ui->scoreThreshold_auth_spinBox->setEnabled(false);
    ui->matchThreshold_auth_spinBox->setEnabled(false);

    ClearAuthTab();
    if(fap_controller == E_FAP20){
        fap20StartAuthProcess();
    }else if(fap_controller == E_FAP50){
        fap50reader->m_stop = false;
        fap50reader->m_WorkType = WORK_CAPTUREIMAGE;
        fap50reader->start();
    }
}

void MainWindow::on_start_tab_verification()
{
    ui->widget_81->setEnabled(false);
    ui->scoreThreshold_verification_spinBox->setEnabled(false);
    ui->matchThreshold_verification_spinBox->setEnabled(false);

    ui->preview_verification->clear();

    ui->choose_finger_verification_btn->setEnabled(false);

    ClearVerifyTab();
    if(fap_controller == E_FAP20){
        if(!fap20StartVerificationProcess())StopButtonWidgets();
    }else if(fap_controller == E_FAP50){

    }
}

//----------------------------------------------------------------------------------------------------------------------------->> END SECTION Callbacks to capture/enroll/authentication/validation

void MainWindow::StopButtonWidgets(){



    ui->CaptureTab->setEnabled(true );
    ui->EnrollTab->setEnabled(true);
    ui->IdentificationTab->setEnabled(true);
    ui->AuthenticationTab->setEnabled(true);

    ui->widget_73->setEnabled(true);
    ui->widget_108->setEnabled(true);
    ui->widget_81->setEnabled(true);
    ui->widget_125->setEnabled(true);

    ui->stop_capture_btn->setHidden(true);
    ui->start_capture->setHidden(false);
    ui->enroll_pushButton->setEnabled(false);


    ui->matchThreshold_verification_spinBox->setEnabled(true);
    ui->matchThreshold_auth_spinBox->setEnabled(true);


    ui->scoreThreshold_auth_spinBox->setEnabled(true);
    ui->scoreThreshold_verification_spinBox->setEnabled(true);
    ui->scoreThreshold_enroll_spinBox->setEnabled(true);
    ui->scoreThreshold_capture_spinBox->setEnabled(true);

    ui->name_enroll_lineedit->setEnabled(true);
    ui->id_enroll_lineedit->setEnabled(true);

    ui->fap50_finger_option_combobox->setEnabled(true);

    ui->groupBox->setEnabled(true);
    ui->groupBox_2->setEnabled(true);

    ui->right_fingers_led->setStyleSheet(ui->right_fingers_led->styleSheet() + "QPushButton{background-color: rgb(239, 242, 245);border: 1px solid rgb(239, 242, 245);}");
    ui->left_fingers_led->setStyleSheet(ui->left_fingers_led->styleSheet() + "QPushButton{background-color: rgb(239, 242, 245);border: 1px solid rgb(239, 242, 245);}");
    ui->thumbs_fingers_led->setStyleSheet(ui->thumbs_fingers_led->styleSheet() + "QPushButton{background-color: rgb(239, 242, 245);border: 1px solid rgb(239, 242, 245);}");
    ui->any_finger_led->setStyleSheet(ui->any_finger_led->styleSheet() + "QPushButton{background-color: rgb(239, 242, 245);border: 1px solid rgb(239, 242, 245);}");

    ui->force_capture_btn->setEnabled(false);
    ui->force_capture_btn->setStyleSheet(ui->force_capture_btn->styleSheet() + "QPushButton{color: #CFD6E4;background-color: rgb(239, 242, 245);}");

    ui->choose_finger_btn->setEnabled(true);
    ui->choose_finge_capture_btn->setEnabled(true);
    ui->choose_finger_verification_btn->setEnabled(true);

    ui->preview_auth->clear();
    ui->preview_capture->clear();
    ui->preview_enroll->clear();
    ui->preview_verification->clear();

    statusBar()->clearMessage();

}


void MainWindow::on_device_combo_box_activated(int index)
{
    if(index == 1){
        if(fap20_present && fap20reader->Connect())
        {
            deviceConnected_action();
            ui->device_combo_box->setCurrentIndex(1);
            enableFAP20layout();
            fap_controller = E_FAP20;
            return;
        }
    }
    else if(index == 2){

    }
}


void MainWindow::on_tabWidget_currentChanged(E_TAB_TYPE index)
{

    ClearCaptureTab();
    ClearEnrollTab();
    ClearAuthTab();
    ClearVerifyTab();

    currentTab = (E_TAB_TYPE)index;
    ui->stackedWidget->setCurrentIndex(currentTab);

    if(index == E_TAB_TYPE_CAPTURE){ui->match_score_widget->setHidden(true);}
    else if(index == E_TAB_TYPE_ENROLL)
    {
        int records = dbManager->getDatabaseRecords();
        ui->enroll_records_fap50_label->setText(QString::number(records));
        ui->records_enroll_label->setText(QString::number(records));
        ui->match_score_widget->setHidden(true);
        if(fap_controller == E_FAP20)ui->tabWidget_2->setCurrentIndex(4);
        else if(fap_controller == E_FAP50)ui->tabWidget_2->setCurrentIndex(index);
        return;
    }
    else if(index == E_TAB_TYPE_AUTH){ui->match_score_widget->setHidden(false);}
    else if(index == E_TAB_TYPE_VERIFICATION){ui->match_score_widget->setHidden(false);}

    ui->tabWidget_2->setCurrentIndex(index);


}

E_FINGER_POSITION MainWindow::select_finger(QString text){
    qDebug() << "Text in the finger comboBox is: " << text;
    if(text == "Right thumb")
        return E_FINGER_POSITION::FINGER_POSITION_RIGHT_THUMB;
    else if(text == "Right index")
        return E_FINGER_POSITION::FINGER_POSITION_RIGHT_INDEX;
    else if(text == "Right middle")
        return E_FINGER_POSITION::FINGER_POSITION_RIGHT_MIDDLE;
    else if(text == "Right ring")
        return E_FINGER_POSITION::FINGER_POSITION_RIGHT_RING;
    else if(text == "Right little")
        return E_FINGER_POSITION::FINGER_POSITION_RIGHT_LITTLE;
    else if(text == "Left thumb")
        return E_FINGER_POSITION::FINGER_POSITION_LEFT_THUMB;
    else if(text == "Left index")
        return E_FINGER_POSITION::FINGER_POSITION_LEFT_INDEX;
    else if(text == "Left middle")
        return E_FINGER_POSITION::FINGER_POSITION_LEFT_MIDDLE;
    else if(text == "Left ring")
        return E_FINGER_POSITION::FINGER_POSITION_LEFT_RING;
    else if(text == "Left little")
        return E_FINGER_POSITION::FINGER_POSITION_LEFT_LITTLE;
    else
        return E_FINGER_POSITION::FINTER_POSITION_UNKNOW_FINGER;
}

//-----------------------------------------------------------------------------------------------------------------------------
// SECTION Score thresholds callbacks -----------------------------------------------------------------------------------------
void MainWindow::on_scoreThreshold_capture_spinBox_valueChanged(int arg1)
{
    if(fap_controller == E_FAP20){
        fap20reader->set_minimum_score(arg1);
    }else if(fap_controller == E_FAP50){

        score_threshold = arg1;
    }
}

void MainWindow::on_scoreThreshold_enroll_spinBox_valueChanged(int arg1)
{
    if(fap_controller == E_FAP20){
        fap20reader->set_minimum_score(arg1);
    }else if(fap_controller == E_FAP50){

        score_threshold = arg1;
    }
}

void MainWindow::on_scoreThreshold_auth_spinBox_valueChanged(int arg1)
{
    if(fap_controller == E_FAP20){
        fap20reader->set_minimum_score(arg1);
    }else if(fap_controller == E_FAP50){

    }
}


void MainWindow::on_scoreThreshold_verification_spinBox_valueChanged(int arg1)
{
    if(fap_controller == E_FAP20){
        fap20reader->set_minimum_score(arg1);
    }else if(fap_controller == E_FAP50){

    }
}

//----------------------------------------------------------------------------------------------------------------------------------------->> END SECTION Score thresholds callbacks
// SECTION "Auto" and "Live" pushbutton callbacks -----------------------------------------------------------------------------------------
void MainWindow::clearBackgroundColor(QPushButton* button) {
    static const QRegularExpression regex("background-color:\\s*rgb\\((\\d{1,3}),\\s*(\\d{1,3}),\\s*(\\d{1,3})\\);?");
    QString styleSheet = button->styleSheet();
    button->setStyleSheet(styleSheet.remove(regex));
}


void MainWindow::on_CaptureLiveModeButton_clicked()
{
    //ui->save_button->setEnabled(true);
    //ui->enroll_pushButton->setEnabled(true);
    //ui->verify_button->setEnabled(true);

    currentMode = E_MODE_TYPE::E_MODE_LIVE;
    //Enroll
    clearBackgroundColor(ui->CaptureLiveModeButton);
    clearBackgroundColor(ui->AutoCaptureModeButton);
    ui->CaptureLiveModeButton->setStyleSheet(ui->CaptureLiveModeButton->styleSheet() + "background-color: rgb(255, 255, 255);color: #616E85;");
    ui->AutoCaptureModeButton->setStyleSheet(ui->AutoCaptureModeButton->styleSheet() + "background-color: #EFF2F5;color: #616E85;");

    // Capture Tab
    clearBackgroundColor(ui->CaptureLiveModeButton_2);
    clearBackgroundColor(ui->AutoCaptureModeButton_2);
    ui->CaptureLiveModeButton_2->setStyleSheet(ui->CaptureLiveModeButton_2->styleSheet() + "background-color: rgb(255, 255, 255);color: #616E85;");
    ui->AutoCaptureModeButton_2->setStyleSheet(ui->AutoCaptureModeButton_2->styleSheet() + "background-color: #EFF2F5;color: #616E85;");



    // Authentication Tab
    clearBackgroundColor(ui->CaptureLiveModeButton_3);
    clearBackgroundColor(ui->AutoCaptureModeButton_3);
    ui->CaptureLiveModeButton_3->setStyleSheet(ui->CaptureLiveModeButton_3->styleSheet() + "background-color: rgb(255, 255, 255);color: #616E85;");
    ui->AutoCaptureModeButton_3->setStyleSheet(ui->AutoCaptureModeButton_3->styleSheet() + "background-color: #EFF2F5;color: #616E85;");




    // Verification Tab
    clearBackgroundColor(ui->CaptureLiveModeButton_4);
    clearBackgroundColor(ui->AutoCaptureModeButton_4);
    ui->CaptureLiveModeButton_4->setStyleSheet(ui->CaptureLiveModeButton_4->styleSheet() + "background-color: rgb(255, 255, 255);color: #616E85;");
    ui->AutoCaptureModeButton_4->setStyleSheet(ui->AutoCaptureModeButton_4->styleSheet() + "background-color: #EFF2F5;color: #616E85;");

    ui->capture_score_groupbox->setHidden(true);
    ui->enroll_score_groupbox->setHidden(true);
    ui->identification_group_box->setHidden(true);
    ui->auth_score_groupbox->setHidden(true);
}

void MainWindow::on_AutoCaptureModeButton_clicked()
{
    //ui->save_button->setEnabled(false);
    //ui->enroll_pushButton->setEnabled(false);
    //ui->verify_button->setEnabled(false);

    currentMode = E_MODE_TYPE::E_MODE_AUTO;
    clearBackgroundColor(ui->CaptureLiveModeButton);
    clearBackgroundColor(ui->AutoCaptureModeButton);
    ui->AutoCaptureModeButton->setStyleSheet(ui->AutoCaptureModeButton->styleSheet() + "background-color: rgb(255, 255, 255);color: #616E85;");
    ui->CaptureLiveModeButton->setStyleSheet(ui->CaptureLiveModeButton->styleSheet() + "background-color: #EFF2F5;color: #616E85;");

    clearBackgroundColor(ui->CaptureLiveModeButton_2);
    clearBackgroundColor(ui->AutoCaptureModeButton_2);
    ui->AutoCaptureModeButton_2->setStyleSheet(ui->AutoCaptureModeButton_2->styleSheet() + "background-color: rgb(255, 255, 255);color: #616E85;");
    ui->CaptureLiveModeButton_2->setStyleSheet(ui->CaptureLiveModeButton_2->styleSheet() + "background-color: #EFF2F5;color: #616E85;");

    clearBackgroundColor(ui->CaptureLiveModeButton_3);
    clearBackgroundColor(ui->AutoCaptureModeButton_3);
    ui->AutoCaptureModeButton_3->setStyleSheet(ui->AutoCaptureModeButton_3->styleSheet() + "background-color: rgb(255, 255, 255);color: #616E85;");
    ui->CaptureLiveModeButton_3->setStyleSheet(ui->CaptureLiveModeButton_3->styleSheet() + "background-color: #EFF2F5;color: #616E85;");

    clearBackgroundColor(ui->CaptureLiveModeButton_4);
    clearBackgroundColor(ui->AutoCaptureModeButton_4);
    ui->AutoCaptureModeButton_4->setStyleSheet(ui->AutoCaptureModeButton_4->styleSheet() + "background-color: rgb(255, 255, 255);color: #616E85;");
    ui->CaptureLiveModeButton_4->setStyleSheet(ui->CaptureLiveModeButton_4->styleSheet() + "background-color: #EFF2F5;color: #616E85;");

    ui->capture_score_groupbox->setHidden(false);
    ui->enroll_score_groupbox->setHidden(false);
    ui->identification_group_box->setHidden(false);
    ui->auth_score_groupbox->setHidden(false);
}

void MainWindow::on_CaptureLiveModeButton_2_clicked()
{
    on_CaptureLiveModeButton_clicked();
}

void MainWindow::on_AutoCaptureModeButton_2_clicked()
{
    on_AutoCaptureModeButton_clicked();
}

void MainWindow::on_CaptureLiveModeButton_3_clicked()
{
    on_CaptureLiveModeButton_clicked();
}

void MainWindow::on_AutoCaptureModeButton_3_clicked()
{
    on_AutoCaptureModeButton_clicked();
}

void MainWindow::on_CaptureLiveModeButton_4_clicked()
{
    on_CaptureLiveModeButton_clicked();
}

void MainWindow::on_AutoCaptureModeButton_4_clicked()
{
    on_AutoCaptureModeButton_clicked();
}

//----------------------------------------------------------------------------------------------------------------------------------------->> END SECTION "Auto" and "Live" pushbutton callbacks
// SECTION Pushbutton to expand and close calibration settings ----------------------------------------------------------------------------
void MainWindow::on_pushButton_clicked()
{
    ui->capture_calibration_widget->setHidden(!ui->capture_calibration_widget->isHidden());
    ui->enroll_calibration_widget->setHidden(!ui->enroll_calibration_widget->isHidden());
    ui->enroll_calibration_widget_2->setHidden(!ui->enroll_calibration_widget_2->isHidden());
    ui->enroll_calibration_widget_3->setHidden(!ui->enroll_calibration_widget_3->isHidden());


    if(ui->centralwidget->size().width() < 1920 && ui->centralwidget->size().height() < 980)
    {
        ui->widget_68->setHidden(!ui->widget_68->isHidden());
        ui->capture_tab_option_widget->setHidden(!ui->capture_tab_option_widget->isHidden());

        ui->capture_score_groupbox->setHidden(!ui->capture_score_groupbox->isHidden());
        ui->identification_group_box->setHidden(!ui->identification_group_box->isHidden());
        ui->enroll_score_groupbox->setHidden(!ui->enroll_score_groupbox->isHidden());
        ui->auth_score_groupbox->setHidden(!ui->auth_score_groupbox->isHidden());
    }



    if(ui->capture_calibration_widget->isHidden())
    {
        ui->pushButton_10->setIcon(QIcon(":/img/arrow/icons8-arrows-24.png"));
        ui->widget_172->setStyleSheet(ui->widget_172->styleSheet() + "border-bottom: 0px solid #EFF2F5;");

        ui->pushButton_8->setIcon(QIcon(":/img/arrow/icons8-arrows-24.png"));
        ui->widget_160->setStyleSheet(ui->widget_160->styleSheet() + "border-bottom: 0px solid #EFF2F5;");

        ui->pushButton_4->setIcon(QIcon(":/img/arrow/icons8-arrows-24.png"));
        ui->widget_148->setStyleSheet(ui->widget_148->styleSheet() + "border-bottom: 0px solid #EFF2F5;");

        ui->pushButton->setIcon(QIcon(":/img/arrow/icons8-arrows-24.png"));
        ui->widget_189->setStyleSheet(ui->widget_189->styleSheet() + "border-bottom: 0px solid #EFF2F5;");
    }
    else
    {
        ui->pushButton_10->setIcon(QIcon(":/img/arrow/icons8-arrow-24.png"));
        ui->widget_172->setStyleSheet(ui->widget_172->styleSheet() + "border-bottom: 2px solid #EFF2F5;");

        ui->pushButton_8->setIcon(QIcon(":/img/arrow/icons8-arrow-24.png"));
        ui->widget_160->setStyleSheet(ui->widget_160->styleSheet() + "border-bottom: 2px solid #EFF2F5;");

        ui->pushButton_4->setIcon(QIcon(":/img/arrow/icons8-arrow-24.png"));
        ui->widget_148->setStyleSheet(ui->widget_148->styleSheet() + "border-bottom: 2px solid #EFF2F5;");

        ui->pushButton->setIcon(QIcon(":/img/arrow/icons8-arrow-24.png"));
        ui->widget_189->setStyleSheet(ui->widget_189->styleSheet() + "border-bottom: 2px solid #EFF2F5;");
    }
}
void MainWindow::on_pushButton_4_clicked()
{
    on_pushButton_clicked();
}

void MainWindow::on_pushButton_8_clicked()
{
    on_pushButton_clicked();
}


void MainWindow::on_pushButton_10_clicked()
{
    on_pushButton_clicked();
}

//----------------------------------------------------------------------------------------------------------------------------------------->> END SECTION Pushbutton to expand and close calibration settings


// ??????????????????
void MainWindow::on_force_capture_btn_clicked()
{

    if(fap_controller == E_FAP20)
    {
        if(currentTab == E_TAB_TYPE_AUTH)
        {
            fap20reader->receiveForceCapture(true);
            return;
        }
        onFap20SetSamplingLabel("N/A","N/A",pt_fingerprint->finger_position_string_map[get_choose_enroll_finger()]);
        fap20reader->Stop();
        StopButtonWidgets();
    }else if(fap_controller == E_FAP50){

        switch (currentTab) {
        case E_TAB_TYPE_CAPTURE:
            if(fpcount_fap50 == 1)
            {
                fap50reader->force_capture = true;
            }
            break;
        case E_TAB_TYPE_ENROLL:
            if(fpcount_fap50 == 4)
            {
                fap50reader->force_capture = true;
            }
                break;
        default:
            break;
        }
    }


}
//-----------------------------------------------------------------------------------------------------------------------------
// SECTION Buttons on the right-hand tab widget -------------------------------------------------------------------------------
void MainWindow::on_save_button_clicked()
{
    if(ui->databasePath_lineEdit->text() == ""){
        template_popup(E_POPUP_TYPE_WARNING,"Error","Database path is empty.");
        return;
    }else{
        QFileInfo checkDir(ui->databasePath_lineEdit->text());
        if (!checkDir.exists() || !checkDir.isDir()) {
            template_popup(E_POPUP_TYPE_WARNING,"Error","Database path is not valid.");
            return;
        }
    }

    if(get_choose_capture_finger() == FINTER_POSITION_UNKNOW_FINGER)
    {
        template_popup(E_POPUP_TYPE_WARNING,"Error","First select a finger.");
        return;
    }


    QPixmap pixmap = ui->capture_sampling_label->pixmap();
    QDateTime dateTime = QDateTime::currentDateTime();

    QString fileName;

    // Abrir un cuadro de diálogo para seleccionar el path del archivo

    fileName = QFileDialog::getSaveFileName(this, tr("Save image"), ui->databasePath_lineEdit->text() + "/captures/" + pt_fingerprint->finger_path_map[get_choose_capture_finger()] +
                                                                        "_" +  ui->capture_score_label->text() +
                                                                        "_" + dateTime.toString("yyyyMMdd_HHmmss"), tr("Images (*.png)"));

    if (!fileName.isEmpty()) {
        // Guardar el QPixmap en el archivo seleccionado
        pixmap.save(fileName, "PNG");
    }

}

void MainWindow::on_enroll_pushButton_clicked()
{
    QString name = ui->name_enroll_lineedit->text();
    QString id = ui->id_enroll_lineedit->text();

    if(name == ""){
        template_popup(E_POPUP_TYPE_WARNING,"Error","Name is not valid.");
        return;
    }
    if(id == ""){
        template_popup(E_POPUP_TYPE_WARNING,"Error","ID is not valid.");
        return;
    }

    if(ui->databasePath_lineEdit->text() == ""){
        template_popup(E_POPUP_TYPE_WARNING,"Error","Database path is empty.");
        return;
    }else{
        QFileInfo checkDir(ui->databasePath_lineEdit->text());
        if (!checkDir.exists() || !checkDir.isDir()) {
            template_popup(E_POPUP_TYPE_WARNING,"Error","Database path is not valid.");
            return;
        }
    }
    ui->name_enroll_lineedit->setEnabled(false);
    ui->id_enroll_lineedit->setEnabled(false);
    fap20reader->EnrollFingerprintLive(ui->scoreThreshold_enroll_spinBox->value(), get_choose_enroll_finger(), ui->name_enroll_lineedit->text(), ui->id_enroll_lineedit->text());
    fap20reader->EnrollFingerprint();
}

void MainWindow::on_verify_button_clicked()
{

    if(ui->databasePath_lineEdit->text() == ""){
        template_popup(E_POPUP_TYPE_WARNING,"Error", "Database path is empty.");
        return;
    }else{
        QFileInfo checkDir(ui->databasePath_lineEdit->text());
        if (!checkDir.exists() || !checkDir.isDir()) {
            template_popup(E_POPUP_TYPE_WARNING,"Error", "Database path is not valid.");
            return;
        }
    }

    if(fap_controller == E_FAP20){
        fap20reader->VerifyIDFingerprint(ui->id_verification_label->text(), get_choose_verification_finger());
    }else if(fap_controller == E_FAP50){

    }
}

//----------------------------------------------------------------------------------------------------------------------------->> END SECTION Buttons on the right-hand tab widget
// SECTION Clear buttons on the right-hand side tabwidget ---------------------------------------------------------------------
//TODO remove "db" from widget name
void MainWindow::on_cleardb_auth_button_clicked()
{
    ui->id_auth_lineEdit->clear();
    ui->id_auth_lineEdit->setPlaceholderText("N/A");
    ui->name_auth_lineEdit->clear();
    ui->name_auth_lineEdit->setPlaceholderText("N/A");
    ui->auth_sampling_label->clear();

    ui->score_auth_label->setText("0");
    ui->finger_auth_label->clear();
    ui->match_auth_label->setText("0");
}


void MainWindow::on_cleardb_enroll_button_clicked()
{
    if(ui->databasePath_lineEdit->text() == ""){
        template_popup(E_POPUP_TYPE_WARNING,"Error", "Database path is empty.");
        return;
    }else{
        QFileInfo checkDir(ui->databasePath_lineEdit->text());
        if (!checkDir.exists() || !checkDir.isDir()) {
            template_popup(E_POPUP_TYPE_WARNING,"Error", "Database path is not valid.");
            return;
        }
    }


    int result = template_popup(E_POPUP_TYPE_INFO,"Info","Database is about to be cleaned. Are you sure?",E_POPUP_BUTTON_TYPE_YES_NO);

    if (result == true) {
        QMessageBox box;
        if(dbManager->clearDatabase()){
            template_popup(E_POPUP_TYPE_SUCCESS,"Success","Database cleaned successfully.");
            ui->records_enroll_label->setText("0");
        }else{
            template_popup(E_POPUP_TYPE_WARNING,"Error","Database could not be cleaned.");
        }
    } else if (result == false) {
        return;
    }

}

//----------------------------------------------------------------------------------------------------------------------->> END SECTION Clear buttons on the right-hand side tabwidget

void MainWindow::ClearCaptureTab()
{
    ui->capture_sampling_label->clear();
    ui->preview_capture->clear();
    ui->capture_score_label->setText("0");
    ui->score_label->setText("0");

    ui->save_button->setEnabled(false);
    ui->save_button->setStyleSheet(ui->save_button->styleSheet() + "QPushButton{color: #CFD6E4;background-color: rgb(239, 242, 245);}");
}

void MainWindow::ClearEnrollTab()
{
    fap50ClearEnrollFingers();
    ui->enroll_sampling_label->clear();
    ui->score_enroll_label->setText("0");
    ui->score_label->setText("0");


    ui->enroll_pushButton->setEnabled(false);
    ui->enroll_pushButton->setStyleSheet(ui->enroll_pushButton->styleSheet() + "QPushButton{color: #CFD6E4;background-color: rgb(239, 242, 245);}");

    ui->enroll_pushButton_fap50->setEnabled(false);
    ui->enroll_pushButton_fap50->setStyleSheet(ui->enroll_pushButton->styleSheet() + "QPushButton{color: #CFD6E4;background-color: rgb(239, 242, 245);}");

}

void MainWindow::ClearAuthTab()
{
    ui->auth_sampling_label->clear();
    ui->score_auth_label->setText("0");
    ui->finger_auth_label->clear();
    ui->match_auth_label->setText("0");

    ui->score_label->setText("0");
    ui->math_global_score_label->setText("0");

    ui->name_auth_lineEdit->clear();
    ui->id_auth_lineEdit->clear();

    ui->cleardb_auth_button->setEnabled(false);
    ui->cleardb_auth_button->setStyleSheet(ui->cleardb_auth_button->styleSheet() + "QPushButton{color: #CFD6E4;background-color: rgb(239, 242, 245);}");
}

void MainWindow::ClearVerifyTab()
{
    ui->verification_sampling_label->clear();
    ui->score_verification_label->setText("0");
    ui->math_verifi_label->setText("0");

    ui->score_label->setText("0");
    ui->math_global_score_label->setText("0");

    ui->verify_button->setEnabled(false);
    ui->verify_button->setStyleSheet(ui->verify_button->styleSheet() + "QPushButton{color: #CFD6E4;background-color: rgb(239, 242, 245);}");
}

void MainWindow::on_apply_btn_clicked()
{

    if(currentTab == E_TAB_TYPE_CAPTURE)
    {
        on_choose_finge_capture_btn_clicked();
        ui->finger_capture_label->setText(pt_fingerprint->finger_position_string_map[get_choose_capture_finger()]);
    }else if(currentTab == E_TAB_TYPE_ENROLL)
    {
        on_choose_finger_btn_clicked();
        ui->finger_enroll_label->setText(pt_fingerprint->finger_position_string_map[get_choose_enroll_finger()]);
    }else if(currentTab == E_TAB_TYPE_VERIFICATION){
        on_choose_finger_verification_btn_clicked();
        ui->finger_enroll_label->setText(pt_fingerprint->finger_position_string_map[get_choose_verification_finger()]);
    }

    qDebug() << get_choose_capture_finger();

    ui->CaptureTab->setEnabled(true);
    ui->EnrollTab->setEnabled(true);
    ui->IdentificationTab->setEnabled(true);
    ui->AuthenticationTab->setEnabled(true);
}


int MainWindow::template_popup(E_POPUP_TYPE mode,QString title, QString body, E_POPUP_BUTTON_TYPE btn_type)
{
    // Pop-up personalizado
    QDialog popUp(nullptr);
    popUp.setModal(true);
    //popUp.setFixedSize(350, 210);
    popUp.setStyleSheet("background-color: #ffffff; border: 0px solid black;color:#000000;");


    // Agregar sombra
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(100); // Ajusta el radio de desenfoque
    shadow->setOffset(10); // Ajusta el desplazamiento
    popUp.setGraphicsEffect(shadow);

    // Máscara semi-transparente
    QWidget mask(this);
    mask.resize(this->size());
    mask.setStyleSheet("background-color: #616E85;");
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect;
    effect->setOpacity(0.5); // Ajusta la opacidad
    mask.setGraphicsEffect(effect);
    mask.show();

    // Layout principal
    QVBoxLayout *layout = new QVBoxLayout(&popUp);
    // Ícono
    QLabel *iconLabel = new QLabel;
    if(mode == E_POPUP_TYPE_INFO) iconLabel->setPixmap(QIcon(":/img/messagebox/icons8-info-64.png").pixmap(64, 64)); // Ajusta la ruta al ícono
    else if(mode == E_POPUP_TYPE_WARNING) iconLabel->setPixmap(QIcon(":/img/messagebox/icons8-warning-96.png").pixmap(64, 64)); // Ajusta la ruta al ícono
    else if(mode == E_POPUP_TYPE_SUCCESS) iconLabel->setPixmap(QIcon(":/img/messagebox/icons8-success-80.png").pixmap(64, 64)); // Ajusta la ruta al ícono
    iconLabel->setStyleSheet("margin-top:20px;");
    layout->addWidget(iconLabel, 0, Qt::AlignCenter); // Alineación central


    QString style_title = "   \
                          font-size:24px;\
        font: var(--unnamed-font-style-normal) normal var(--unnamed-font-weight-bold) var(--unnamed-font-size-24)/21.6px var(--unnamed-font-family-helvetica-neue); \
        letter-spacing: var(--unnamed-character-spacing-0); \
        color: var(--unnamed-color-000000); \
        text-align: center; \
        font: normal normal bold 24px/22px Helvetica Neue; \
        letter-spacing: 0px; \
        color: #000000; \
        margin-bottom:10px;\
        margin-top:7px;\
        opacity: 1;";

        QLabel *idVerLabel = new QLabel(title);
    idVerLabel->setAlignment(Qt::AlignCenter);
    idVerLabel->setStyleSheet(style_title);
    layout->addWidget(idVerLabel);


    QString style_info = "/* UI Properties */ \
                         font-size:16px;\
        letter-spacing: var(--unnamed-character-spacing-0);\
        color: var(--unnamed-color-000000);\
        text-align: left;\
        letter-spacing: 0px;\
        font: normal normal 16px/14px Helvetica; \
        margin-left:40px;\
        margin-right:40px;\
        color: #000000;\
        opacity: 1;";

        // Etiquetas y campos de texto
        QHBoxLayout *legal_name_layout = new QHBoxLayout(&popUp);
    QLabel *nameLabel = new QLabel(body);
    nameLabel->setStyleSheet(style_info);
    legal_name_layout->addWidget(nameLabel,0, Qt::AlignCenter); // Alineación central
    layout->addLayout(legal_name_layout);


    QString button_style = "QPushButton{\
                           font-size : 18px;\
        /* UI Properties */\
        width: 454px;\
        height: 36px;\
        text-align: center;\
        letter-spacing: 0px;\
        opacity: 1;\
        /* UI Properties */\
        background: #EFF2F5;\
        border: 1px solid #EFF2F5;\
        border-radius: 16px;\
        color: #616E85;\
        opacity: 1;\
            font: normal normal 18px/16px HelveticaNeue-Medium; \
        margin-top:20px;\
        margin-bottom:20px;\
        margin-left:20px;\
        margin-right:20px;\
}\
                        \
    QPushButton:hover{\
        background-color: #D6D9DC\
}";

    QHBoxLayout *buttons_layouts = new QHBoxLayout(&popUp);
// Botón
if(btn_type == E_POPUP_BUTTON_TYPE_OK)
{
    QPushButton *okButton = new QPushButton("OK");
    okButton->setStyleSheet(button_style);
    buttons_layouts->addWidget(okButton, 0, Qt::AlignCenter); // Alineación central
    layout->addLayout(buttons_layouts);

    QObject::connect(okButton, &QPushButton::clicked, &popUp, &QDialog::reject);
    QObject::connect(okButton, &QPushButton::clicked, &mask, &QWidget::close);

}
else if(btn_type == E_POPUP_BUTTON_TYPE_YES_NO)
{
    QPushButton *yesButton = new QPushButton("Yes");
    QPushButton *noButton = new QPushButton("No");

    yesButton->setStyleSheet(button_style + "QPushButton{ width: 227px;;}");
    noButton->setStyleSheet(button_style + "QPushButton{ width: 227px;}");

    buttons_layouts->addWidget(yesButton, 0, Qt::AlignCenter); // Alineación central
    buttons_layouts->addWidget(noButton, 0, Qt::AlignCenter); // Alineación central

    layout->addLayout(buttons_layouts);

    QObject::connect(yesButton, &QPushButton::clicked, &mask, &QDialog::close);
    QObject::connect(noButton, &QPushButton::clicked, &mask, &QDialog::close);

    QObject::connect(yesButton, &QPushButton::clicked, &popUp, &QDialog::accept);
    QObject::connect(noButton, &QPushButton::clicked, &popUp, &QDialog::reject);

}


// Mostrar el pop-up
popUp.setWindowFlags(Qt::FramelessWindowHint);
popUp.move((this->width() / 2) - (popUp.width() / 2.5), (this->height() / 2) - (popUp.height() / 4) );
return popUp.exec();
}

int MainWindow::authentification_popup(E_POPUP_TYPE mode,QString title, QList<QString> body, E_POPUP_BUTTON_TYPE btn_type)
{
    // Pop-up personalizado
    QDialog popUp(nullptr);
    popUp.setModal(true);
    //popUp.setFixedSize(350, 210);
    popUp.setStyleSheet("background-color: #ffffff; border: 0px solid black;color:#000000;");


    // Agregar sombra
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(100); // Ajusta el radio de desenfoque
    shadow->setOffset(10); // Ajusta el desplazamiento
    popUp.setGraphicsEffect(shadow);

    // Máscara semi-transparente
    QWidget mask(this);
    mask.resize(this->size());
    mask.setStyleSheet("background-color: #616E85;");
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect;
    effect->setOpacity(0.5); // Ajusta la opacidad
    mask.setGraphicsEffect(effect);
    mask.show();

    // Layout principal
    QVBoxLayout *layout = new QVBoxLayout(&popUp);
    // Ícono
    QLabel *iconLabel = new QLabel;
    if(mode == E_POPUP_TYPE_INFO) iconLabel->setPixmap(QIcon(":/img/messagebox/icons8-info-64.png").pixmap(64, 64)); // Ajusta la ruta al ícono
    else if(mode == E_POPUP_TYPE_WARNING) iconLabel->setPixmap(QIcon(":/img/messagebox/icons8-warning-96.png").pixmap(64, 64)); // Ajusta la ruta al ícono
    else if(mode == E_POPUP_TYPE_SUCCESS) iconLabel->setPixmap(QIcon(":/img/messagebox/icons8-success-80.png").pixmap(64, 64)); // Ajusta la ruta al ícono
    iconLabel->setStyleSheet("margin-top:20px;");
    layout->addWidget(iconLabel, 0, Qt::AlignCenter); // Alineación central


    QString style_title = "   \
                          font-size:24px;\
        font: var(--unnamed-font-style-normal) normal var(--unnamed-font-weight-bold) var(--unnamed-font-size-24)/21.6px var(--unnamed-font-family-helvetica-neue); \
        letter-spacing: var(--unnamed-character-spacing-0); \
        color: var(--unnamed-color-000000); \
        text-align: center; \
        font: normal normal bold 24px/22px Helvetica Neue; \
        letter-spacing: 0px; \
        color: #000000; \
        margin-bottom:10px;\
        margin-top:7px;\
        opacity: 1;";

        QLabel *idVerLabel = new QLabel(title);
    idVerLabel->setAlignment(Qt::AlignCenter);
    idVerLabel->setStyleSheet(style_title);
    layout->addWidget(idVerLabel);


    QString style_info = "/* UI Properties */ \
                         font-size:16px;\
        letter-spacing: var(--unnamed-character-spacing-0);\
        color: var(--unnamed-color-000000);\
        text-align: left;\
        letter-spacing: 0px;\
        font: normal normal 16px/14px Helvetica; \
        margin-left:40px;\
        margin-right:40px;\
        color: #000000;\
        opacity: 1;";

        // Etiquetas y campos de texto
        QHBoxLayout *legal_name_layout = new QHBoxLayout(&popUp);
    QLabel *nameLabel = new QLabel("Legal Name:");
    QLabel *nameLineEdit = new QLabel(body[0]);
    ui->name_auth_lineEdit->setText(body[0]);

    nameLabel->setStyleSheet(style_info);
    nameLineEdit->setStyleSheet(style_info + "font: normal normal bold 16px/14px Helvetica;");

    nameLineEdit->setAlignment(Qt::AlignRight);
    legal_name_layout->addWidget(nameLabel);
    legal_name_layout->addWidget(nameLineEdit);
    layout->addLayout(legal_name_layout);

    QHBoxLayout *national_id_layout = new QHBoxLayout(&popUp);
    QLabel *idLabel = new QLabel("National identification number:");
    QLabel *idLineEdit = new QLabel(body[1]);
    ui->id_auth_lineEdit->setText(body[1]);

    idLabel->setStyleSheet(style_info);
    idLineEdit->setStyleSheet(style_info + "font: normal normal bold 16px/14px Helvetica;");

    idLineEdit->setAlignment(Qt::AlignRight);
    national_id_layout->addWidget(idLabel);
    national_id_layout->addWidget(idLineEdit);
    layout->addLayout(national_id_layout);


    QHBoxLayout *debug_message_layout = new QHBoxLayout(&popUp);
    QLabel *message = new QLabel(body[2]);
    message->setAlignment(Qt::AlignHCenter);
    message->setStyleSheet(style_info + "margin-top:10px;");
    debug_message_layout->addWidget(message);
    layout->addLayout(debug_message_layout);



    QString button_style = "QPushButton{\
                           font-size : 18px;\
        /* UI Properties */\
        width: 454px;\
        height: 36px;\
        text-align: center;\
        letter-spacing: 0px;\
        opacity: 1;\
        /* UI Properties */\
        background: #EFF2F5;\
        border: 1px solid #EFF2F5;\
        border-radius: 16px;\
        color: #616E85;\
        opacity: 1;\
            font: normal normal 18px/16px HelveticaNeue-Medium; \
        margin-top:20px;\
        margin-bottom:20px;\
        margin-left:20px;\
        margin-right:20px;\
}\
                        \
    QPushButton:hover{\
        background-color: #D6D9DC\
}";

    QHBoxLayout *buttons_layouts = new QHBoxLayout(&popUp);
// Botón
if(btn_type == E_POPUP_BUTTON_TYPE_OK)
{
    QPushButton *okButton = new QPushButton("OK");
    okButton->setStyleSheet(button_style);
    buttons_layouts->addWidget(okButton, 0, Qt::AlignCenter); // Alineación central
    layout->addLayout(buttons_layouts);

    QObject::connect(okButton, &QPushButton::clicked, &popUp, &QDialog::reject);
    QObject::connect(okButton, &QPushButton::clicked, &mask, &QWidget::close);

}
else if(btn_type == E_POPUP_BUTTON_TYPE_YES_NO)
{
    QPushButton *yesButton = new QPushButton("Yes");
    QPushButton *noButton = new QPushButton("No");

    yesButton->setStyleSheet(button_style + "QPushButton{ width: 227px;;}");
    noButton->setStyleSheet(button_style + "QPushButton{ width: 227px;}");

    buttons_layouts->addWidget(yesButton, 0, Qt::AlignCenter); // Alineación central
    buttons_layouts->addWidget(noButton, 0, Qt::AlignCenter); // Alineación central

    layout->addLayout(buttons_layouts);

    QObject::connect(yesButton, &QPushButton::clicked, &mask, &QDialog::close);
    QObject::connect(noButton, &QPushButton::clicked, &mask, &QDialog::close);

    QObject::connect(yesButton, &QPushButton::clicked, &popUp, &QDialog::accept);
    QObject::connect(noButton, &QPushButton::clicked, &popUp, &QDialog::reject);

}


// Mostrar el pop-up
popUp.setWindowFlags(Qt::FramelessWindowHint);
//popUp.move((this->width() / 2) - (popUp.width() / 2.5), (this->height() / 2) - (popUp.height() / 4) );
return popUp.exec();
}



void MainWindow::on_choose_finger_btn_clicked()
{
    ui->preview_enroll_widget->setHidden(ui->choose_finger_widget->isHidden());
    ui->choose_finger_widget->setHidden(!ui->choose_finger_widget->isHidden());
    ui->force_capture_btn->setHidden(!ui->force_capture_btn->isHidden());
    ui->apply_btn->setHidden(!ui->apply_btn->isHidden());
    ui->choose_finger_btn->setEnabled(!ui->choose_finger_btn->isEnabled());

    ui->CaptureTab->setEnabled(ui->stackedWidget->currentIndex() == 0 ? true : false);
    ui->EnrollTab->setEnabled(ui->stackedWidget->currentIndex() == 1 ? true : false);
    ui->IdentificationTab->setEnabled(ui->stackedWidget->currentIndex() == 2 ? true : false);
    ui->AuthenticationTab->setEnabled(ui->stackedWidget->currentIndex() == 3 ? true : false);

    if(ui->choose_finger_widget->isHidden())
    {
        ui->choose_finger_btn->setStyleSheet(ui->choose_finger_btn->styleSheet() + "QPushButton{background-color: #EFF2F5;color: #616E85;}QPushButton:hover{background-color: #D6D9DC}");
        ui->choose_finger_btn->setIcon(QIcon(":/img/@3x/ic_hand_normal@3x.png"));

        ui->start_capture->setStyleSheet(ui->start_capture->styleSheet() + "QPushButton{background-color: #616E85;color: #FFFFFF;}QPushButton:hover{background-color: #3D526D}");
        ui->start_capture->setEnabled(true);
    }
    else
    {
        ui->choose_finger_btn->setStyleSheet(ui->choose_finger_btn->styleSheet() + "QPushButton{background-color: #616E85;color: #FFFFFF;}QPushButton:hover{background-color: #3D526D}");
        ui->choose_finger_btn->setIcon(QIcon(":/img/@3x/ic_hand_select@3x.png"));

        ui->start_capture->setStyleSheet(ui->start_capture->styleSheet() + "QPushButton{background-color: #EFF2F5;color: #616E85;}QPushButton:hover{background-color: #D6D9DC}");
        ui->start_capture->setEnabled(false);
    }

}

void MainWindow::on_choose_finge_capture_btn_clicked()
{
    ui->preview_capture_widget->setHidden(!ui->preview_capture_widget->isHidden());
    ui->choose_finger_widget_2->setHidden(!ui->choose_finger_widget_2->isHidden());
    ui->force_capture_btn->setHidden(!ui->force_capture_btn->isHidden());
    ui->apply_btn->setHidden(!ui->apply_btn->isHidden());
    ui->choose_finge_capture_btn->setEnabled(!ui->choose_finge_capture_btn->isEnabled());

    ui->CaptureTab->setEnabled(ui->stackedWidget->currentIndex() == 0 ? true : false);
    ui->EnrollTab->setEnabled(ui->stackedWidget->currentIndex() == 1 ? true : false);
    ui->IdentificationTab->setEnabled(ui->stackedWidget->currentIndex() == 2 ? true : false);
    ui->AuthenticationTab->setEnabled(ui->stackedWidget->currentIndex() == 3 ? true : false);

    if(ui->choose_finger_widget_2->isHidden())
    {
        ui->choose_finge_capture_btn->setStyleSheet(ui->choose_finge_capture_btn->styleSheet() + "QPushButton{background-color: #EFF2F5;color: #616E85;}QPushButton:hover{background-color: #D6D9DC}");
        ui->choose_finge_capture_btn->setIcon(QIcon(":/img/@3x/ic_hand_normal@3x.png"));

        ui->start_capture->setStyleSheet(ui->start_capture->styleSheet() + "QPushButton{background-color: #616E85;color: #FFFFFF;}QPushButton:hover{background-color: #3D526D}");
        ui->start_capture->setEnabled(true);
    }
    else
    {
        ui->choose_finge_capture_btn->setStyleSheet(ui->choose_finge_capture_btn->styleSheet() + "QPushButton{background-color: #616E85;color: #FFFFFF;}QPushButton:hover{background-color: #3D526D}");
        ui->choose_finge_capture_btn->setIcon(QIcon(":/img/@3x/ic_hand_select@3x.png"));

        ui->start_capture->setStyleSheet(ui->start_capture->styleSheet() + "QPushButton{background-color: #EFF2F5;color: #616E85;}QPushButton:hover{background-color: #D6D9DC}");
        ui->start_capture->setEnabled(false);
    }
}

E_FINGER_POSITION MainWindow::get_choose_enroll_finger(){

    QAbstractButton *buttons;
    E_FINGER_POSITION pos;
    foreach (buttons, ui->custom_hand_widget->group1->buttons()) {
        if(buttons->isChecked())
        {
            pos = (E_FINGER_POSITION)buttons->group()->id(buttons);
        }
    }

    return pos;
}

std::vector<E_FINGER_POSITION> MainWindow::get_choose_enroll_finger_442()
{
    std::vector<E_FINGER_POSITION> pos;
    QAbstractButton *buttons;
    foreach (buttons, ui->custom_hand_widget->group1->buttons()) {
        if(buttons->isChecked())
        {
            pos.push_back((E_FINGER_POSITION)buttons->group()->id(buttons));
        }
    }
    return pos;

}

E_FINGER_POSITION MainWindow::get_choose_capture_finger(){

    QAbstractButton *buttons;
    E_FINGER_POSITION pos;
    foreach (buttons, ui->capture_hand_widget->group1->buttons()) {
        if(buttons->isChecked())
        {
            pos = (E_FINGER_POSITION)buttons->group()->id(buttons);
        }
    }

    return pos;
}

E_FINGER_POSITION MainWindow::get_choose_verification_finger(){

    QAbstractButton *buttons;
    E_FINGER_POSITION pos;
    foreach (buttons, ui->authentication_custom_hand->group1->buttons()) {
        if(buttons->isChecked())
        {
            pos = (E_FINGER_POSITION)buttons->group()->id(buttons);
        }
    }

    return pos;
}

void MainWindow::on_matchThreshold_auth_spinBox_valueChanged(int arg1)
{
    if(fap_controller == E_FAP20){
        fap20reader->set_match_threshold(arg1);
    }else if(fap_controller == E_FAP50)
    {
        score_threshold = arg1;
    }
}


void MainWindow::on_matchThreshold_verification_spinBox_valueChanged(int arg1)
{
    if(fap_controller == E_FAP20){
        fap20reader->set_match_threshold(arg1);
    }else if(fap_controller == E_FAP50)
    {
        score_threshold = arg1;
    }
}

void MainWindow::on_enroll_pushButton_fap50_clicked()
{

}

void MainWindow::on_fap50_finger_option_combobox_activated(int index)
{
    if(index == 0)
    {

        ui->flat_check->setEnabled(false);
        ui->roll_check->setEnabled(false);
        ui->groupBox->setHidden(true);

        ui->custom_hand_widget->group1->ChangeExclusive(false);

        ui->custom_hand_widget->fingerRightIndex->setChecked(true);
        ui->custom_hand_widget->fingerRightLittle->setChecked(true);
        ui->custom_hand_widget->fingerRightMiddle->setChecked(true);
        ui->custom_hand_widget->fingerRightRing->setChecked(true);
        ui->custom_hand_widget->fingerRightThumb->setChecked(true);

        ui->custom_hand_widget->fingerLeftIndex->setChecked(true);
        ui->custom_hand_widget->fingerLeftLittle->setChecked(true);
        ui->custom_hand_widget->fingerLeftMiddle->setChecked(true);
        ui->custom_hand_widget->fingerLeftRing->setChecked(true);
        ui->custom_hand_widget->fingerLeftThumb->setChecked(true);

        //ui->finger_enroll_groupBox_3->setHidden(true);
    }else if(index == 1)
    {

        ui->flat_check->setEnabled(true);
        ui->roll_check->setEnabled(true);
        ui->groupBox->setHidden(false);

        ui->custom_hand_widget->group1->ChangeExclusive(true);


        ui->custom_hand_widget->fingerRightIndex->setChecked(false);
        ui->custom_hand_widget->fingerRightLittle->setChecked(false);
        ui->custom_hand_widget->fingerRightMiddle->setChecked(false);
        ui->custom_hand_widget->fingerRightRing->setChecked(false);
        ui->custom_hand_widget->fingerRightThumb->setChecked(false);

        ui->custom_hand_widget->fingerLeftIndex->setChecked(false);
        ui->custom_hand_widget->fingerLeftLittle->setChecked(false);
        ui->custom_hand_widget->fingerLeftMiddle->setChecked(false);
        ui->custom_hand_widget->fingerLeftRing->setChecked(false);
        ui->custom_hand_widget->fingerLeftThumb->setChecked(false);

        // DEFAULT ON
        ui->custom_hand_widget->fingerRightIndex->click();

        //ui->finger_enroll_groupBox_3->setHidden(false);
    }
}

// ------------------------------------------------------ //

bool MainWindow::OpenDataBase()
{
    QString DBPath = ui->databasePath_lineEdit->text();
    if (dbManager) {
        delete dbManager;
        dbManager = nullptr;
    }

    dbManager = new databasemanager(DBPath);
    if (!dbManager->isDatabaseOpen()) {
        delete dbManager;
        dbManager = nullptr;
        return false;
    }
    return true;

}

void MainWindow::on_pushButton_7_clicked()
{
    if(ui->databasePath_lineEdit->text() == ""){
        template_popup(E_POPUP_TYPE_WARNING,"Error", "Database path is empty.");
        return;
    }else{
        QFileInfo checkDir(ui->databasePath_lineEdit->text());
        if (!checkDir.exists() || !checkDir.isDir()) {
            template_popup(E_POPUP_TYPE_WARNING,"Error", "Database path is not valid.");
            return;
        }
    }

    if(!OpenDataBase()){
        template_popup(E_POPUP_TYPE_WARNING,"Error", "Database could not be processed.");
        return;
    }

    int result = template_popup(E_POPUP_TYPE_INFO,"Info","Database is about to be cleaned. Are you sure?",E_POPUP_BUTTON_TYPE_YES_NO);

    if (result == true) {
        if(dbManager->clearDatabase()){
            template_popup(E_POPUP_TYPE_SUCCESS,"Success","Database cleaned successfully.");
            ui->enroll_records_fap50_label->setText("0");
        }else{
            template_popup(E_POPUP_TYPE_WARNING,"Error","Database could not be cleaned.");
        }
    } else if (result == false) {
        return;
    }
}


void MainWindow::on_CaptureTab_clicked()
{
    on_tabWidget_currentChanged(E_TAB_TYPE_CAPTURE);
}


void MainWindow::on_EnrollTab_clicked()
{
    on_tabWidget_currentChanged(E_TAB_TYPE_ENROLL);
}


void MainWindow::on_IdentificationTab_clicked()
{
    on_tabWidget_currentChanged(E_TAB_TYPE_AUTH);
}


void MainWindow::on_AuthenticationTab_clicked()
{
    on_tabWidget_currentChanged(E_TAB_TYPE_VERIFICATION);
}

void MainWindow::on_choose_finger_verification_btn_clicked()
{
    ui->preview_verification_widget->setHidden(!ui->preview_verification_widget->isHidden());
    ui->choose_finger_widget_verification->setHidden(!ui->choose_finger_widget_verification->isHidden());
    ui->force_capture_btn->setHidden(!ui->force_capture_btn->isHidden());
    ui->apply_btn->setHidden(!ui->apply_btn->isHidden());
    ui->choose_finger_verification_btn->setEnabled(!ui->choose_finger_verification_btn->isEnabled());

    ui->CaptureTab->setEnabled(ui->stackedWidget->currentIndex() == 0 ? true : false);
    ui->EnrollTab->setEnabled(ui->stackedWidget->currentIndex() == 1 ? true : false);
    ui->IdentificationTab->setEnabled(ui->stackedWidget->currentIndex() == 2 ? true : false);
    ui->AuthenticationTab->setEnabled(ui->stackedWidget->currentIndex() == 3 ? true : false);

    if(ui->choose_finger_widget_verification->isHidden())
    {
        ui->choose_finger_verification_btn->setStyleSheet(ui->choose_finger_verification_btn->styleSheet() + "QPushButton{background-color: #EFF2F5;color: #616E85;}QPushButton:hover{background-color: #D6D9DC}");
        ui->choose_finger_verification_btn->setIcon(QIcon(":/img/@3x/ic_hand_normal@3x.png"));

        ui->start_capture->setStyleSheet(ui->start_capture->styleSheet() + "QPushButton{background-color: #616E85;color: #FFFFFF;}QPushButton:hover{background-color: #3D526D}");
        ui->start_capture->setEnabled(true);
    }
    else
    {
        ui->choose_finger_verification_btn->setStyleSheet(ui->choose_finger_verification_btn->styleSheet() + "QPushButton{background-color: #616E85;color: #FFFFFF;}QPushButton:hover{background-color: #3D526D}");
        ui->choose_finger_verification_btn->setIcon(QIcon(":/img/@3x/ic_hand_select@3x.png"));

        ui->start_capture->setStyleSheet(ui->start_capture->styleSheet() + "QPushButton{background-color: #EFF2F5;color: #616E85;}QPushButton:hover{background-color: #D6D9DC}");
        ui->start_capture->setEnabled(false);
    }
}



void MainWindow::onImageReadyFap50(QPixmap pm)
{


    if(currentTab == E_TAB_TYPE_CAPTURE){
        ui->preview_capture->setPixmap(pm.scaled(ui->preview_capture->width(),ui->preview_capture->height(),Qt::KeepAspectRatio));
    }else if(currentTab == E_TAB_TYPE_ENROLL){
        ui->preview_enroll->setPixmap(pm.scaled(ui->preview_enroll->width(),ui->preview_enroll->height(),Qt::KeepAspectRatio));
    }else if(currentTab == E_TAB_TYPE_AUTH){
        ui->preview_auth->setPixmap(pm.scaled(ui->preview_auth->width(),ui->preview_auth->height(),Qt::KeepAspectRatio));
    }else if(currentTab == E_TAB_TYPE_VERIFICATION){
        ui->preview_verification->setPixmap(pm.scaled(ui->preview_verification->width(),ui->preview_verification->height(),Qt::KeepAspectRatio));
    }

}

void MainWindow::onNoImageFap50()
{
    if(currentTab == E_TAB_TYPE_CAPTURE){
        ui->preview_capture->clear();
    }else if(currentTab == E_TAB_TYPE_ENROLL){
        ui->preview_enroll->clear();
    }else if(currentTab == E_TAB_TYPE_AUTH){
        ui->preview_auth->clear();
    }else if(currentTab == E_TAB_TYPE_VERIFICATION){
        ui->preview_verification->clear();
    }
}

void MainWindow::onFpCountFap50(int count)
{
    fpcount_fap50 = count;
}

void MainWindow::onSamplingDoneFap50(FourFingers res)
{
    if(currentTab != E_TAB_TYPE_ENROLL)StopButtonWidgets();

    if(currentTab == E_TAB_TYPE_CAPTURE){
        fap50CaptureSamplingDone(*res.finger1);
    }else if(currentTab == E_TAB_TYPE_ENROLL){
        fap50EnrollSamplingDone(res);
    }else if(currentTab == E_TAB_TYPE_AUTH){
        //fap50AuthSamplingDone(res);
    }else if(currentTab == E_TAB_TYPE_VERIFICATION){
    }

    ui->capture_sample_widget->setEnabled(true);

    ui->save_button->setEnabled(true);
    ui->enroll_pushButton_fap50->setEnabled(true);

}


void MainWindow::fap50CaptureSamplingDone(QPixmap res)
{
    ui->capture_sampling_label->setPixmap(res.scaled(ui->capture_sampling_label->width(),ui->capture_sampling_label->height(),Qt::KeepAspectRatio));

    ui->capture_score_label->setText(QString::number(100));

    ui->stop_capture_btn->setHidden(true);
    ui->start_capture->setHidden(false);

    ui->preview_capture->clear();

    ui->save_button->setEnabled(true);
    ui->save_button->setStyleSheet(ui->save_button->styleSheet() + "QPushButton{color: #FFFFFF;background-color: rgb(97, 110, 133);}");

    fap50reader->m_stop = true;
    StopButtonWidgets();

    status_bar_text("Capture sampling done!",3000);
}

void MainWindow::fap50EnrollSamplingDone(FourFingers res)
{
    ui->score_label->setText(QString::number(100));

    ui->enroll_pushButton_fap50->setEnabled(true);
    ui->enroll_pushButton_fap50->setStyleSheet(ui->enroll_pushButton_fap50->styleSheet() + "QPushButton{color: #FFFFFF;background-color: rgb(97, 110, 133);}");

    ui->r_index_label->setPixmap((res.finger1->scaled(ui->r_index_label->width(),ui->r_index_label->height(),Qt::KeepAspectRatio)));
    ui->r_middle_label->setPixmap((res.finger2->scaled(ui->r_middle_label->width(),ui->r_middle_label->height(),Qt::KeepAspectRatio)));
    ui->r_ring_label->setPixmap((res.finger3->scaled(ui->r_ring_label->width(),ui->r_ring_label->height(),Qt::KeepAspectRatio)));
    ui->r_little_label->setPixmap((res.finger4->scaled(ui->r_little_label->width(),ui->r_little_label->height(),Qt::KeepAspectRatio)));

    fap50reader->m_stop = true;
    StopButtonWidgets();

    status_bar_text("Capture sampling done!",3000);

}
