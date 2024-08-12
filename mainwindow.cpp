#include "mainwindow.h"
#include "IMD/FAP20/fap20reader.h"
#include "ui_mainwindow.h"
#include "qgraphicseffect.h"
#include <QtWidgets/QGraphicsView>
#include <QMessageBox>

MainWindow *pMainWindow=0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui_initial_setup();

    pMainWindow=this;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::FPMessage(int worktype,int retval,unsigned char* data,int size)
{
    switch(worktype)
    {
    case FPM_PLACEFINGER:
        status_bar_text("Place Finger ...");
        break;
    case FPM_LIFTFINGER:
        status_bar_text("Lift Finger ...");
        break;
    case FPM_DRAWIMAGE:
        status_bar_text("Draw Finger Image");
        DrawBitmap(data);
        break;
    case FPM_CAPTUREIMAGE:
        if(retval)
            status_bar_text("Capture Image OK");
        else
            status_bar_text("Capture Image Fail");
        break;
    case FPM_ENROLTP:
        if(retval)
        {
            m_EnrolSize=size;
            memcpy(m_EnrolData,data,size);
            status_bar_text("Enrol Template OK");
        }
        else
            status_bar_text("Enrol Template Fail");
        break;
    case FPM_CAPTURETP:
        if(retval)
        {
            m_CaptureSize=size;
            memcpy(m_CaptureData,data,size);
            status_bar_text("Capture Template OK");
            if(m_EnrolSize>0)
            {
                //int sc=MatchTemplate(m_EnrolData,m_EnrolSize,m_CaptureData,m_CaptureSize);
                //int sc=FAP20_MatchTemplate(m_EnrolData,m_EnrolSize,m_CaptureData,m_CaptureSize);
                int sc=FAP30_MatchTemplate(m_EnrolData,m_EnrolSize,m_CaptureData,m_CaptureSize);
                if(sc>50)
                    status_bar_text("Match OK,Scores:"+QString::number(sc));
                else
                    status_bar_text("Match Fail,Scores:"+QString::number(sc));
            }
        }
        else
            status_bar_text("Capture Template Fail");
        break;

    }
}

void MainWindow::DrawBitmap(unsigned char* imagedata)
{
    ImageToBitmap(imagedata,m_ImageWidth,m_ImageHeight,m_BmpData,0);
    m_BmpSize=m_ImageWidth*m_ImageHeight+1078;
    QPixmap * pm = new QPixmap();
    pm->loadFromData(m_BmpData,m_BmpSize,"bmp");
    QGraphicsScene * gs = new QGraphicsScene();
    gs->addPixmap(*pm);
    ui->preview_capture->setScene(gs);
}


void  MainWindow::HotPlugCallBackStub(uint8_t action,uint8_t iSerialNumber,uint16_t idVendor,uint16_t idProduct)
{
    qDebug() << action << iSerialNumber << idVendor << idProduct;

    if(pMainWindow)
        pMainWindow->HotPlugCallBack(action,iSerialNumber,idVendor,idProduct);
}

void  MainWindow::HotPlugCallBack(uint8_t action,uint8_t iSerialNumber,uint16_t idVendor,uint16_t idProduct)
{

    qDebug() << action << iSerialNumber << idVendor << idProduct;
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
        break;
    case 3:
        status_bar_text("Exit Hotplug Thread\n");
        break;
    }
}

// ---------------------------------

void MainWindow::ui_initial_setup()
{
    ui->pushButton_3->setIcon(QIcon(":/img/icon.png")); // IMD Logo

    ui_customhand_setup();
    on_CaptureLiveModeButton_clicked();

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

    QString dbPath = QCoreApplication::applicationDirPath() + "/db";
    /*
    QDir dir(dbPath);
    if (!dir.exists()) {
        if (!dir.mkpath(dbPath)) {
            qDebug() << "No se pudo crear la carpeta 'db'.";
        }
    }
*/
    ui->databasePath_lineEdit->setText(dbPath);

    ui->DisconnectButton->setHidden(true);
    ui->stop_capture_btn->setHidden(true);

    ui->apply_btn->setHidden(true);
    connect(ui->pushButton_12, SIGNAL(clicked()), this, SLOT(on_databaselineedit_clicked()));

}

void MainWindow::ui_customhand_setup()
{
    ui->custom_hand_widget->setBackgroudLabel(ui->label_70);

    ui->capture_hand_widget->setBackgroudLabel(ui->capture_hand_label);
    ui->capture_hand_widget->group1->ChangeExclusive(true);

    ui->authentication_custom_hand->setBackgroudLabel(ui->label_7);
    ui->authentication_custom_hand->group1->ChangeExclusive(true);
}


//---------------------------------->>



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

    status_bar_text("Welcome! Device connected.",4000);

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

    status_bar_text("Device disconnected!",4000);
}

void MainWindow::on_DisconnectButton_clicked()
{
    ui->CaptureTab->click();

    fap20_present = false;
    fap50_present = false;

    //fap20reader->IsConnected = false;

    deviceDisconnected_action();

    ClearCaptureTab();
    ClearEnrollTab();
    ClearAuthTab();
    ClearVerifyTab();
}

void MainWindow::on_ConnectButton_clicked()
{

#define FAP20
#ifdef FAP20

    if(OpenDevice(0,0,0) && LinkDevice(0))
    {
        deviceConnected_action();
        ui->device_combo_box->setCurrentIndex(1);
        enableFAP20layout();
        //fap20reader->receiveDatabasePath(ui->databasePath_lineEdit->text());
        if(ReadDeviceInfo(&m_DevInfo))
        {
            qDebug() << "Fingerprint Capacity: "+QString::number(m_DevInfo.iFPMax)+"\n"+
                                    "Security Level: "+QString::number(m_DevInfo.iSecLevel)+"\n"+
                                    "SerialPort Baud: "+QString::number(m_DevInfo.iBaud*9600)+"\n"+
                                    "SerialPort Package: "+QString::number(32*(1 << m_DevInfo.iPackSize))+"\n"+
                                    "...";
        }
        fap_controller = E_FAP20;
        return;
    }
#elif FAP50
    deviceConnected_action();
    ui->device_combo_box->setCurrentIndex(2);
    enableFAP50layout();
    if(!OpenDataBase())template_popup(E_POPUP_TYPE_WARNING,"Error","Failed to open or create database.");
    fap_controller = E_FAP50;

    return;
#endif

    fap_controller = E_NONE;
    template_popup(E_POPUP_TYPE_WARNING,"Error","Could not be connected");
}

//--------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------
// FAP20 and FAP30: Layouts -----------------------------------------------------------------------------------------------------------------------
void MainWindow::enableFAP20layout(){
    ui->widget_184->setHidden(true);

    // Capture Tab
    ui->groupBox_2->setHidden(true);

    ui->capture_hand_widget->fingerRightIndex->click();
    //ui->finger_capture_label->setText(fap50reader->finger_position_string_map[get_choose_capture_finger()]);

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
    ui->authentication_custom_hand->fingerRightIndex->click();

    ui->cleardb_enroll_button->setEnabled(true);

    //ui->finger_enroll_label->setText(fap50reader->finger_position_string_map[get_choose_enroll_finger()]);

}

void MainWindow::enableFAP50layout(){
    ui->widget_47->setHidden(false);
    ui->widget_184->setHidden(false);

    // Capture Tab
    ui->groupBox_2->setHidden(false);

    ui->capture_hand_widget->fingerRightIndex->click();
    //ui->finger_capture_label->setText(fap50reader->finger_position_string_map[get_choose_capture_finger()]);

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


//------------------>>


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

//-----------------<<



//------------------------------>>


void MainWindow::ClearCaptureTab()
{
    ui->capture_sampling_label->clear();
    //ui->preview_capture->
    ui->capture_score_label->setText("0");
    ui->score_label->setText("0");

    ui->save_button->setEnabled(false);
    ui->save_button->setStyleSheet(ui->save_button->styleSheet() + "QPushButton{color: #CFD6E4;background-color: rgb(239, 242, 245);}");
}

void MainWindow::ClearEnrollTab()
{
    //fap50ClearEnrollFingers();
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

//------------------------------<<


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
        margin-left:7px;\
        margin-right:7px;\
        border:0px solid black;";

        ui->statusbar->clearMessage();
    ui->statusbar->setStyleSheet(style);
    ui->statusbar->showMessage(text,timeout);
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
