#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include <iostream>
#include <QFileDialog>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    mouseDrag=0;
    isotopeUpdateAsked.resize(6,0);
    elementUpdateAsked=0;
    updateAsked=0;
    counter =0;
    ui->setupUi(this);
    elementCalculator.SetUpPlotter(ui->plot,6);
    elementCalculator.LoadBasicSettings();

    LoadStartSettings();
    LoadSettings();
    LoadElements();
    UpdateIsotopes();

    connect(&onlineTimer, SIGNAL(timeout()), this, SLOT(realtimeSlot()));
    connect(ui->plot, SIGNAL(mousePress(QMouseEvent*)), SLOT(SetMouseDrag(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseMove(QMouseEvent*)), SLOT(UpdateDipoleCurrent(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseRelease(QMouseEvent*)), SLOT(ReleaseMouseDrag(QMouseEvent*)));

    onlineTimer.start(10); // Interval 0 means to refresh as fast as possible

    ui->color_0->setStyleSheet("QLabel { background-color : blue;}");
    ui->color_1->setStyleSheet("QLabel { background-color : red;}");
    ui->color_2->setStyleSheet("QLabel { background-color : green;}");
    ui->color_3->setStyleSheet("QLabel { background-color : magenta;}");
    ui->color_4->setStyleSheet("QLabel { background-color : cyan;}");
    ui->color_5->setStyleSheet("QLabel { background-color : yellow;}");


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Element_0_currentIndexChanged(int index)
{
    AskIsotopeUpdate(0);
}

void MainWindow::on_Element_1_currentIndexChanged(int index)
{
    AskIsotopeUpdate(1);
}

void MainWindow::on_Element_2_currentIndexChanged(int index)
{
    AskIsotopeUpdate(2);
}

void MainWindow::on_Element_3_currentIndexChanged(int index)
{
    AskIsotopeUpdate(3);
}

void MainWindow::on_Element_4_currentIndexChanged(int index)
{
    AskIsotopeUpdate(4);
}

void MainWindow::on_Element_5_currentIndexChanged(int index)
{
    AskIsotopeUpdate(5);
}

void MainWindow::Update(void)
{
    if(ui->plotAll->isChecked())
    {
        elementCalculator.plotAll=1;
    }
    else
    {
        elementCalculator.plotAll=0;
    }


    elementCalculator.plotYlimits[0]=ui->Ymin->value();
    elementCalculator.plotYlimits[1]=ui->yMax->value();

    counter++;
    vector <int> plot;
    plot.push_back((int) ui->plot_0->isChecked());
    plot.push_back((int) ui->plot_1->isChecked());
    plot.push_back((int) ui->plot_2->isChecked());
    plot.push_back((int) ui->plot_3->isChecked());
    plot.push_back((int) ui->plot_4->isChecked());
    plot.push_back((int) ui->plot_5->isChecked());

    vector <int> elements;
    elements.push_back(ui->Element_0->currentIndex());
    elements.push_back(ui->Element_1->currentIndex());
    elements.push_back(ui->Element_2->currentIndex());
    elements.push_back(ui->Element_3->currentIndex());
    elements.push_back(ui->Element_4->currentIndex());
    elements.push_back(ui->Element_5->currentIndex());

    vector <int> isotopes;
    isotopes.push_back(ui->Isotope_0->currentIndex());
    isotopes.push_back(ui->Isotope_1->currentIndex());
    isotopes.push_back(ui->Isotope_2->currentIndex());
    isotopes.push_back(ui->Isotope_3->currentIndex());
    isotopes.push_back(ui->Isotope_4->currentIndex());
    isotopes.push_back(ui->Isotope_5->currentIndex());

    vector <double> chargeFWHMs;
    if (ui->chargeDistribution->isChecked())
    {
        chargeFWHMs.push_back(ui->FMHW_0->value());
        chargeFWHMs.push_back(ui->FMHW_1->value());
        chargeFWHMs.push_back(ui->FMHW_2->value());
        chargeFWHMs.push_back(ui->FMHW_3->value());
        chargeFWHMs.push_back(ui->FMHW_4->value());
        chargeFWHMs.push_back(ui->FMHW_5->value());
    }
    else
    {
        chargeFWHMs.resize(6,1000);
    }
    vector <double> intensities;
    intensities.push_back(ui->MaxCurrent_0->value());
    intensities.push_back(ui->MaxCurrent_1->value());
    intensities.push_back(ui->MaxCurrent_2->value());
    intensities.push_back(ui->MaxCurrent_3->value());
    intensities.push_back(ui->MaxCurrent_4->value());
    intensities.push_back(ui->MaxCurrent_5->value());


    vector <double> averageCharges;
    averageCharges.push_back(ui->MeanChargState_0->value());
    averageCharges.push_back(ui->MeanChargState_1->value());
    averageCharges.push_back(ui->MeanChargState_2->value());
    averageCharges.push_back(ui->MeanChargState_3->value());
    averageCharges.push_back(ui->MeanChargState_4->value());
    averageCharges.push_back(ui->MeanChargState_5->value());



    elementCalculator.dipoleA=ui->A->value();
    elementCalculator.dipoleB=ui->B->value();
    elementCalculator.dipoleC=ui->C->value();
    elementCalculator.dipoleD=ui->D->value();
    elementCalculator.dipoleE=ui->E->value();
    elementCalculator.Vacc=ui->Vacc->value();
    elementCalculator.peakFWHM=ui->peakWidth->value();
    elementCalculator.Ioffset=ui->Ioffset->value();

    elementCalculator.PlotGraphs(ui->xMin->value(), ui->xMax->value(), plot, elements, isotopes,averageCharges,chargeFWHMs, intensities);
    vector <double> charges;

    elementCalculator.CalculateCurrentCharges(ui->Icurrent->value(),charges,plot,elements,isotopes);

    if(plot[0]>0)
    {
        ui->current_Q_0->setText(QString::number(charges[0],'f',2));
    }
    else
    {
        ui->current_Q_0->setText("");
    }
    if(plot[1]>0)
    {
        ui->current_Q_1->setText(QString::number(charges[1],'f',2));
    }
    else
    {
        ui->current_Q_1->setText("");
    }
    if(plot[2]>0)
    {
        ui->current_Q_2->setText(QString::number(charges[2],'f',2));
    }
    else
    {
        ui->current_Q_2->setText("");
    }
    if(plot[3]>0)
    {
        ui->current_Q_3->setText(QString::number(charges[3],'f',2));
    }
    else
    {
        ui->current_Q_3->setText("");
    }
    if(plot[4]>0)
    {
        ui->current_Q_4->setText(QString::number(charges[4],'f',2));
    }
    else
    {
        ui->current_Q_4->setText("");
    }
    if(plot[5]>0)
    {
        ui->current_Q_5->setText(QString::number(charges[5],'f',2));
    }
    else
    {
        ui->current_Q_5->setText("");
    }



}

void MainWindow::on_Isotope_4_currentIndexChanged(int index)
{
    AskUpdate();
}

void MainWindow::on_Isotope_0_currentIndexChanged(int index)
{
    AskUpdate();
}

void MainWindow::on_Isotope_1_currentIndexChanged(int index)
{
    AskUpdate();
}

void MainWindow::on_Isotope_2_currentIndexChanged(int index)
{
    AskUpdate();
}

void MainWindow::on_Isotope_3_currentIndexChanged(int index)
{
    AskUpdate();
}

void MainWindow::on_Isotope_5_currentIndexChanged(int index)
{
    AskUpdate();
}

void MainWindow::on_MeanChargState_0_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_MeanChargState_1_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_MeanChargState_2_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_MeanChargState_3_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_MeanChargState_4_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_MeanChargState_5_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_FMHW_0_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_FMHW_1_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_FMHW_2_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_FMHW_3_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_FMHW_4_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_FMHW_5_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_MaxCurrent_0_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_MaxCurrent_1_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_MaxCurrent_2_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_MaxCurrent_3_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_MaxCurrent_4_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_MaxCurrent_5_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::AskUpdate(void)
{
    updateAsked=1;

}

void MainWindow::realtimeSlot()
{
    if(settingsUpdateAsked==1)
    {
        settingsUpdateAsked=0;
        LoadSettings();
    }

    if(elementUpdateAsked==1)
    {
        elementUpdateAsked=0;
        LoadElements();
    }
    int tempIsotopeUpdate=0;
    for(int i=0;i<isotopeUpdateAsked.size();i++)
    {
        if(isotopeUpdateAsked[i]==1)
        {
            tempIsotopeUpdate=1;
        }
    }
    if(tempIsotopeUpdate==1)
    {
        UpdateIsotopes();
    }


    if(updateAsked==1)
    {
        updateAsked=0;
        Update();
    }
}

void MainWindow::on_plot_1_stateChanged(int arg1)
{
    AskUpdate();
}

void MainWindow::on_plot_0_stateChanged(int arg1)
{
    AskUpdate();
}

void MainWindow::on_plot_2_stateChanged(int arg1)
{
    AskUpdate();
}


void MainWindow::on_plot_3_stateChanged(int arg1)
{
    AskUpdate();
}


void MainWindow::on_plot_4_stateChanged(int arg1)
{
    AskUpdate();
}


void MainWindow::on_plot_5_stateChanged(int arg1)
{
    AskUpdate();
}

void MainWindow::on_xMin_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_xMax_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_Vacc_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_Icurrent_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_yMax_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_Ymin_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::UpdateDipoleCurrent(QMouseEvent *event)
{
    if(mouseDrag==1)
    {
        ui->Icurrent->setValue(ui->plot->xAxis->pixelToCoord(event->x()));
        AskUpdate();
    }
}

void MainWindow::SetMouseDrag(QMouseEvent *event)
{
    mouseDrag=1;
}

void MainWindow::ReleaseMouseDrag(QMouseEvent *event)
{
    UpdateDipoleCurrent(event);
    mouseDrag=0;
}


void MainWindow::LoadElements(void)
{

    if(ui->elements_0->isChecked())
    {
        elementCalculator.LoadElements(elementCalculator.defaultElementsFiles[0]);
    }
    if(ui->elements_1->isChecked())
    {
        elementCalculator.LoadElements(elementCalculator.defaultElementsFiles[1]);
    }
    if(ui->elements_2->isChecked())
    {
        elementCalculator.LoadElements(elementCalculator.defaultElementsFiles[2]);
    }
    else
    {

    }

    elementCalculator.AddElements(0,ui->Element_0);
    elementCalculator.AddElements(1,ui->Element_1);
    elementCalculator.AddElements(2,ui->Element_2);
    elementCalculator.AddElements(3,ui->Element_3);
    elementCalculator.AddElements(4,ui->Element_4);
    elementCalculator.AddElements(5,ui->Element_5);
    AskUpdate();
}

void MainWindow::on_plotAll_stateChanged(int arg1)
{
    AskUpdate();
}

void MainWindow::on_chargeDistribution_stateChanged(int arg1)
{
    AskUpdate();
}

void MainWindow::AskElementUpdate(void)
{
    elementUpdateAsked=1;
}

void MainWindow::LoadSettings(void)
{
    if(ui->settings_0->isChecked())
    {
        elementCalculator.LoadSettings(elementCalculator.defaultSettingsFiles[0]);
    }
    else if(ui->settings_1->isChecked())
    {
        elementCalculator.LoadSettings(elementCalculator.defaultSettingsFiles[1]);
    }
    else if(ui->settings_2->isChecked())
    {
        elementCalculator.LoadSettings(elementCalculator.defaultSettingsFiles[2]);
    }
    else
    {

    }

    ui->A->setValue(elementCalculator.dipoleA);
    ui->B->setValue(elementCalculator.dipoleB);
    ui->C->setValue(elementCalculator.dipoleC);
    ui->D->setValue(elementCalculator.dipoleD);
    ui->E->setValue(elementCalculator.dipoleE);
    ui->peakWidth->setValue(elementCalculator.peakFWHM);


    ui->Icurrent->setValue(elementCalculator.defaultDipoleCurrent);
    ui->Icurrent->setSingleStep(elementCalculator.defaultDipoleCurrentStep);

    ui->Ioffset->setValue(elementCalculator.defaultDipoleOffset);
    ui->Ioffset->setSingleStep(elementCalculator.defaultDipoleOffsetStep);

    ui->Vacc->setValue(elementCalculator.defaultSourceBias);
    ui->Vacc->setSingleStep(elementCalculator.defaultSourceBiasStep);
    ui->plotAll->setChecked(elementCalculator.defaultPlotSum);
    ui->chargeDistribution->setChecked(elementCalculator.defaultPlotDistribution);
    ui->labelBiasUnit->setText(QString::fromStdString(elementCalculator.defaultSourceBiasUnit));

    ui->labelDipoleName->setText(QString::fromStdString(elementCalculator.dipoleName));
}

void MainWindow::LoadStartSettings()
{
    ui->xMin->setValue(elementCalculator.defaultPlotXmin);
    ui->xMax->setValue(elementCalculator.defaultPlotXmax);
    ui->Ymin->setValue(elementCalculator.defaultPlotYmin);
    ui->yMax->setValue(elementCalculator.defaultPlotYmax);

    ui->settings_0->setText(QString::fromStdString(elementCalculator.defaultSettingsNames[0]));
    ui->settings_1->setText(QString::fromStdString(elementCalculator.defaultSettingsNames[1]));
    ui->settings_2->setText(QString::fromStdString(elementCalculator.defaultSettingsNames[2]));

    if(elementCalculator.defaultSettingsSelected==0)
    {
        ui->settings_0->setChecked(1);
    }
    else if(elementCalculator.defaultSettingsSelected==1)
    {
        ui->settings_1->setChecked(1);
    }
    else
    {
        ui->settings_2->setChecked(1);
    }

    ui->elements_0->setText(QString::fromStdString(elementCalculator.defaultElementsNames[0]));
    ui->elements_1->setText(QString::fromStdString(elementCalculator.defaultElementsNames[1]));
    ui->elements_2->setText(QString::fromStdString(elementCalculator.defaultElementsNames[2]));

    if(elementCalculator.defaultElementsSelected==0)
    {
        ui->elements_0->setChecked(1);
    }
    else if(elementCalculator.defaultElementsSelected==1)
    {
        ui->elements_1->setChecked(1);
    }
    else
    {
        ui->elements_2->setChecked(1);
    }

}
void MainWindow::AskSettingsUpdate(void)
{
    settingsUpdateAsked=1;
}

void MainWindow::UpdateIsotopes()
{
    if(isotopeUpdateAsked[0]==1)
    {
        isotopeUpdateAsked[0]=0;
        ui->Isotope_0->clear();
        elementCalculator.AddIsotopes(ui->Element_0->currentIndex(),ui->Isotope_0,ui->MeanChargState_0,ui->FMHW_0);
    }
    if(isotopeUpdateAsked[1]==1)
    {
        isotopeUpdateAsked[1]=0;
        ui->Isotope_1->clear();
        elementCalculator.AddIsotopes(ui->Element_1->currentIndex(),ui->Isotope_1,ui->MeanChargState_1,ui->FMHW_1);
    }
    if(isotopeUpdateAsked[2]==1)
    {
        isotopeUpdateAsked[2]=0;
        ui->Isotope_2->clear();
        elementCalculator.AddIsotopes(ui->Element_2->currentIndex(),ui->Isotope_2,ui->MeanChargState_2,ui->FMHW_2);
    }
    if(isotopeUpdateAsked[3]==1)
    {
        isotopeUpdateAsked[3]=0;
        ui->Isotope_3->clear();
        elementCalculator.AddIsotopes(ui->Element_3->currentIndex(),ui->Isotope_3,ui->MeanChargState_3,ui->FMHW_3);
    }
    if(isotopeUpdateAsked[4]==1)
    {
        isotopeUpdateAsked[4]=0;
        ui->Isotope_4->clear();
        elementCalculator.AddIsotopes(ui->Element_4->currentIndex(),ui->Isotope_4,ui->MeanChargState_4,ui->FMHW_4);
    }
    if(isotopeUpdateAsked[5]==1)
    {
        isotopeUpdateAsked[5]=0;
        ui->Isotope_5->clear();
        elementCalculator.AddIsotopes(ui->Element_5->currentIndex(),ui->Isotope_5,ui->MeanChargState_5,ui->FMHW_5);
    }
    AskUpdate();
}

void MainWindow::AskIsotopeUpdate(int index)
{
    if(index==-1)
    {
        for(int i=0;i<isotopeUpdateAsked.size();i++)
        {
            isotopeUpdateAsked[i]=1;
        }
    }
    else
    {
        isotopeUpdateAsked[index]=1;
    }
}

void MainWindow::on_Ioffset_valueChanged(double arg1)
{
    AskUpdate();
}

void MainWindow::on_settings_0_clicked()
{
    AskSettingsUpdate();
}

void MainWindow::on_settings_1_clicked()
{
    AskSettingsUpdate();
}

void MainWindow::on_settings_2_clicked()
{
    AskSettingsUpdate();
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->yMax->setValue(0);
    ui->Ymin->setValue(0);
}



void MainWindow::on_DipoleFileButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select file"), "./",tr("Ini Files (*.ini)"));
    ui->dipoleSettingsFilename->setText(file);
}

void MainWindow::on_ElementFileButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select file"), "./",tr("Ini Files (*.txt)"));
    ui->elementSettingsFilename->setText(file);
}

void MainWindow::on_LoadDipoleFileButton_clicked()
{
    if(ui->settingsOther->isChecked() && ui->dipoleSettingsFilename->text().length()>0)
    {
        elementCalculator.LoadSettings(ui->dipoleSettingsFilename->text().toStdString());
        LoadSettings();
    }
}

void MainWindow::on_elements_0_clicked()
{
    AskElementUpdate();
}

void MainWindow::on_elements_1_clicked()
{
    AskElementUpdate();
}

void MainWindow::on_elements_2_clicked()
{
    AskElementUpdate();
}

void MainWindow::on_settingsOther_clicked()
{

}

void MainWindow::on_loadElementsFileButton_clicked()
{
    if(ui->elementsOther->isChecked() && ui->elementSettingsFilename->text().length()>0)
        {
            elementCalculator.LoadElements(ui->elementSettingsFilename->text().toStdString());
            LoadElements();
        }
}

void MainWindow::on_saveButton_clicked()
{
    elementCalculator.SaveToFile("testi");
}
