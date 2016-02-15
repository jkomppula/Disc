#ifndef POPUP_H
#define POPUP_H

#include <QDialog>

namespace Ui {
class PopUp;
}

class PopUp : public QDialog
{
    Q_OBJECT

public:
    explicit PopUp(QWidget *parent = 0);
    ~PopUp();
    void SetLabel1(QString label);
    void SetLabel2(QString label);
    void SetLabel3(QString label);
    void SetLabel4(QString label);
    void SetLabel5(QString label);

    void ClearLabel1(void);
    void ClearLabel2(void);
    void ClearLabel3(void);
    void ClearLabel4(void);
    void ClearLabel5(void);
    void ClearAll(void);

private slots:

    void on_pushButton_close_clicked();

private:
    Ui::PopUp *ui;
};

#endif // POPUP_H
