#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->custom_hand_widget->setBackgroudLabel(ui->label_70);

    ui->capture_hand_widget->setBackgroudLabel(ui->capture_hand_label);

    ui->capture_hand_widget->group1->ChangeExclusive(true);

    ui->authentication_custom_hand->setBackgroudLabel(ui->label_7);

    ui->authentication_custom_hand->group1->ChangeExclusive(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
