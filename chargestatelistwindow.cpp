#include "chargestatelistwindow.h"
#include "ui_chargestatelistwindow.h"

ChargeStateListWindow::ChargeStateListWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChargeStateListWindow)
{
    ui->setupUi(this);
}

ChargeStateListWindow::~ChargeStateListWindow()
{
    delete ui;
}

void ChargeStateListWindow::setText(QString& charges){

    ui->chargeStateBrowser->setText(charges);
}

void ChargeStateListWindow::on_pushButton_close_clicked()
{
    this->close();
}
