#include "popup.h"
#include "ui_popup.h"

PopUp::PopUp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PopUp)
{
    ui->setupUi(this);
}

PopUp::~PopUp()
{
    delete ui;
}

void PopUp::SetLabel1(QString label)
{
    ui->label1->setText(label);
}
void PopUp::SetLabel2(QString label)
{
    ui->label2->setText(label);
}
void PopUp::SetLabel3(QString label)
{
    ui->label3->setText(label);
}
void PopUp::SetLabel4(QString label)
{
    ui->label4->setText(label);
}
void PopUp::SetLabel5(QString label)
{
    ui->label5->setText(label);
}

 void PopUp::ClearLabel1(void)
 {
     ui->label1->clear();
 }
 void PopUp::ClearLabel2(void)
 {
     ui->label2->clear();
 }
 void PopUp::ClearLabel3(void)
 {
     ui->label3->clear();
 }
 void PopUp::ClearLabel4(void)
 {
     ui->label4->clear();
 }
 void PopUp::ClearLabel5(void)
 {
     ui->label5->clear();
 }

 void PopUp::ClearAll(void)
 {
     ClearLabel1();
     ClearLabel2();
     ClearLabel3();
     ClearLabel4();
     ClearLabel5();
 }

void PopUp::on_pushButton_close_clicked()
{
    this->close();
}
