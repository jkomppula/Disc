#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMouseEvent>
#include <vector>;
#include "calculator.h"
#include "chargestatelistwindow.h"
#include "dipolescanner.h"
#include "popup.h"
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void popupMessage(QString label1,QString label2,QString label3,QString label4,QString label5);
    void updateRecentValues(double B,double Ibeam);
    void scanStatus(int status);

    void realtimeSlot();
    void updateUIfromScanner(void);

    void on_Element_0_currentIndexChanged(int index);

    void on_Element_1_currentIndexChanged(int index);

    void on_Element_2_currentIndexChanged(int index);

    void on_Element_3_currentIndexChanged(int index);

    void on_Element_4_currentIndexChanged(int index);

    void on_Element_5_currentIndexChanged(int index);

    void on_Isotope_4_currentIndexChanged(int index);

    void on_Isotope_0_currentIndexChanged(int index);

    void on_Isotope_1_currentIndexChanged(int index);

    void on_Isotope_2_currentIndexChanged(int index);

    void on_Isotope_3_currentIndexChanged(int index);

    void on_Isotope_5_currentIndexChanged(int index);

    void on_MeanChargState_0_valueChanged(double arg1);

    void on_MeanChargState_1_valueChanged(double arg1);

    void on_MeanChargState_2_valueChanged(double arg1);

    void on_MeanChargState_3_valueChanged(double arg1);

    void on_MeanChargState_4_valueChanged(double arg1);

    void on_MeanChargState_5_valueChanged(double arg1);

    void on_FMHW_0_valueChanged(double arg1);

    void on_FMHW_1_valueChanged(double arg1);

    void on_FMHW_2_valueChanged(double arg1);

    void on_FMHW_3_valueChanged(double arg1);

    void on_FMHW_4_valueChanged(double arg1);

    void on_FMHW_5_valueChanged(double arg1);

    void on_MaxCurrent_0_valueChanged(double arg1);

    void on_MaxCurrent_1_valueChanged(double arg1);

    void on_MaxCurrent_2_valueChanged(double arg1);

    void on_MaxCurrent_3_valueChanged(double arg1);

    void on_MaxCurrent_4_valueChanged(double arg1);

    void on_MaxCurrent_5_valueChanged(double arg1);

    void on_plot_1_stateChanged(int arg1);

    void on_plot_0_stateChanged(int arg1);

    void on_plot_2_stateChanged(int arg1);

    void on_plot_3_stateChanged(int arg1);

    void on_plot_4_stateChanged(int arg1);

    void on_plot_5_stateChanged(int arg1);

    void on_xMin_valueChanged(double arg1);

    void on_xMax_valueChanged(double arg1);

    void on_Vacc_valueChanged(double arg1);

    void on_Icurrent_valueChanged(double arg1);

    void on_yMax_valueChanged(double arg1);

    void on_Ymin_valueChanged(double arg1);

    void UpdateDipoleCurrent(QMouseEvent *event);
    void SetMouseDrag(QMouseEvent *event);
    void ReleaseMouseDrag(QMouseEvent *event);

    void on_plotAll_stateChanged(int arg1);

    void on_chargeDistribution_stateChanged(int arg1);

    void on_Ioffset_valueChanged(double arg1);

    void on_settings_0_clicked();

    void on_settings_1_clicked();

    void on_settings_2_clicked();

    void on_pushButton_3_clicked();

    void on_ElementFileButton_clicked();

    void on_elements_0_clicked();

    void on_elements_1_clicked();

    void on_elements_2_clicked();

    void on_loadElementsFileButton_clicked();

    void on_saveButton_clicked();

    void on_legend_clicked();

    void on_scanButton_clicked();

    void on_loadButton_clicked();

    void on_clearButton_clicked();


    void on_ChargeStateListButton_clicked();

    void on_spinBox_ScanFrom_valueChanged(int arg1);

    void on_spinBox_ScanTo_valueChanged(int arg1);

    void on_spinBox_ScanTime_valueChanged(int arg1);

    void on_spinBox_SolenoidFrom_valueChanged(int arg1);

    void on_spinBox_SolenoidTo_valueChanged(int arg1);

    void on_pushButton_Go_clicked();

private:
    bool initialized;

    bool mouseDrag;

    int counter;
    void Update(int);
    void UpdateIsotopes(void);
    void AskUpdate(void);
    void AskDipoleUpdate(void);
    void AskElementUpdate(void);
    void AskIsotopeUpdate(int index);
    void AskSettingsUpdate(void);

    int updateAsked;
    int dipoleUpdateAsked;
    vector <int> isotopeUpdateAsked;
    int elementUpdateAsked;
    int settingsUpdateAsked;


    void LoadElements(void);
    void LoadSettings(void);
    void LoadStartSettings(void);

    Ui::MainWindow *ui;
    PopUp *popupWindow;
    ChargeStateListWindow *chargeStateListWindow;

    QTimer onlineTimer;
    calculator elementCalculator;

    DipoleScanner* scanner;


};

#endif // MAINWINDOW_H
