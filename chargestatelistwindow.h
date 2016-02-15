#ifndef CHARGESTATELISTWINDOW_H
#define CHARGESTATELISTWINDOW_H

#include <QDialog>

namespace Ui {
class ChargeStateListWindow;
}

class ChargeStateListWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ChargeStateListWindow(QWidget *parent = 0);
    ~ChargeStateListWindow();
    void setText(QString& charges);

private slots:
    void on_pushButton_close_clicked();

private:
    Ui::ChargeStateListWindow *ui;
};

#endif // CHARGESTATELISTWINDOW_H
