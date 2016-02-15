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
    initialized=0;


    chargeStateListWindow = NULL;
    mouseDrag=0;
    isotopeUpdateAsked.resize(6,0);
    elementUpdateAsked=0;
    updateAsked=0;
    dipoleUpdateAsked=0;
    counter =0;
    ui->setupUi(this);

    elementCalculator.SetUpPlotter(ui->plot,6);
    if(elementCalculator.LoadBasicSettings())
    {
        close();
    }

    scanner=new DipoleScanner();
    scanner->setPlotter(ui->plot,ui->plot->graphCount()-2,ui->plot->graphCount()-3,ui->plot->graphCount()-4);

    connect(scanner,SIGNAL(errorMessage(QString,QString,QString,QString,QString)),this,SLOT(popupMessage(QString,QString,QString,QString,QString)));

    LoadStartSettings();
    LoadSettings();
    LoadElements();
    UpdateIsotopes();

    connect(&onlineTimer, SIGNAL(timeout()), this, SLOT(realtimeSlot()));
    connect(ui->plot, SIGNAL(mousePress(QMouseEvent*)), SLOT(SetMouseDrag(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseMove(QMouseEvent*)), SLOT(UpdateDipoleCurrent(QMouseEvent*)));
    connect(ui->plot, SIGNAL(mouseRelease(QMouseEvent*)), SLOT(ReleaseMouseDrag(QMouseEvent*)));
    connect(scanner,SIGNAL(updateUI()),this,SLOT(updateUIfromScanner()));
    connect(scanner,SIGNAL(recentValues(double,double)),this,SLOT(updateRecentValues(double,double)));
    connect(scanner,SIGNAL(scanStatus(int)),this,SLOT(scanStatus(int)));

    onlineTimer.start(40); // Interval 0 means to refresh as fast as possible

    ui->color_0->setStyleSheet("QLabel { background-color : blue;}");
    ui->color_1->setStyleSheet("QLabel { background-color : red;}");
    ui->color_2->setStyleSheet("QLabel { background-color : green;}");
    ui->color_3->setStyleSheet("QLabel { background-color : magenta;}");
    ui->color_4->setStyleSheet("QLabel { background-color : cyan;}");
    ui->color_5->setStyleSheet("QLabel { background-color : yellow;}");
    initialized=1;

    ui->plot->axisRect()->setAutoMargins(QCP::msNone);
    ui->plot->axisRect()->setMargins(QMargins(90,45,10,50));

    Update(1);

}

void MainWindow::updateUIfromScanner(void)
{

    double maxIonCurrent=scanner->maxIonCurrent;
    if(maxIonCurrent<0 || maxIonCurrent==0)
    {
        maxIonCurrent=1.0;
    }
    double difference=maxIonCurrent-scanner->minIonCurrent;


    double minI=scanner->minIonCurrent-0.1*difference;
    double maxI=maxIonCurrent+0.1*difference;



    ui->Ymin->setValue(minI);
    ui->yMax->setValue(maxI);
    ui->xMin->setValue(scanner->minDipoleCurrent);
    ui->xMax->setValue(scanner->maxDipoleCurrent);

    ui->MaxCurrent_0->setValue(maxIonCurrent);
    ui->MaxCurrent_1->setValue(maxIonCurrent);
    ui->MaxCurrent_2->setValue(maxIonCurrent);
    ui->MaxCurrent_3->setValue(maxIonCurrent);
    ui->MaxCurrent_4->setValue(maxIonCurrent);
    ui->MaxCurrent_5->setValue(maxIonCurrent);

}


MainWindow::~MainWindow()
{
    delete ui;
    delete popupWindow;
}

void MainWindow::popupMessage(QString label1, QString label2, QString label3, QString label4, QString label5)
{
    popupWindow = new PopUp(); // Be sure to destroy you window somewhere
    popupWindow->ClearAll();
    popupWindow->SetLabel1(label1);
    popupWindow->SetLabel2(label2);
    popupWindow->SetLabel3(label3);
    popupWindow->SetLabel4(label4);
    popupWindow->SetLabel5(label5);
    popupWindow->show();
}

void MainWindow::updateRecentValues(double B, double Ibeam)
{
    QString B_text;
    B_text.append(QString::number(B,'g',2));
    B_text.append(" mT");
    ui->label_current_B->setText(B_text);

    QString Ibeam_text;
    Ibeam_text.append(QString::number(Ibeam,'g',2));
    Ibeam_text.append(" uA");
    ui->label_current_Ibeam->setText(Ibeam_text);
}

void MainWindow::scanStatus(int status)
{
    if(status==1)
    {
        ui->scanButton->setText("Scan\r\n triggered");
    }

    else if(status==2)
    {
        ui->scanButton->setText("Scan\r\n running");
    }
    else
    {
        ui->scanButton->setText("Scan");
    }

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

void MainWindow::Update(int all)
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



    //elementCalculator.dipoleA=ui->A->value();
    //elementCalculator.dipoleB=ui->B->value();
    //elementCalculator.dipoleC=ui->C->value();
    //elementCalculator.dipoleD=ui->D->value();
    //elementCalculator.dipoleE=ui->E->value();
    elementCalculator.Vacc=ui->Vacc->value();
    //elementCalculator.peakFWHM=ui->peakWidth->value();
    elementCalculator.Ioffset=ui->Ioffset->value();

    if(all==1)
    {
        elementCalculator.PlotGraphs(ui->xMin->value(), ui->xMax->value(), plot, elements, isotopes,averageCharges,chargeFWHMs, intensities);
        QString chargeStateList;
        elementCalculator.ChargeStateList(chargeStateList,plot, elements, isotopes);
        scanner->updateX2axis();

        //cout << "All graphs: " << endl << chargeStateList.toStdString() << endl;
        if (chargeStateListWindow != NULL)
        {
            if (chargeStateListWindow->isVisible())
            {
                chargeStateListWindow->setText(chargeStateList);
            }
        }


    }
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

void MainWindow::AskDipoleUpdate()
{
    dipoleUpdateAsked=1;
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
        dipoleUpdateAsked=0;
        Update(1);
    }

    if(dipoleUpdateAsked==1)
    {
        dipoleUpdateAsked=0;
        Update(0);
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
    AskDipoleUpdate();
}

void MainWindow::on_yMax_valueChanged(double arg1)
{
    ui->yMax->setSingleStep(ui->yMax->value()*0.1);
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
        //AskDipoleUpdate();
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
        if(elementCalculator.LoadElements(elementCalculator.defaultElementsFiles[0]))
        {
            close();
        }
    }
    if(ui->elements_1->isChecked())
    {
        if(elementCalculator.LoadElements(elementCalculator.defaultElementsFiles[1]))
        {
            close();
        }
    }
    if(ui->elements_2->isChecked())
    {
        if(elementCalculator.LoadElements(elementCalculator.defaultElementsFiles[2]))
        {
            close();
        }
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
        if(!scanner->loadSettings(elementCalculator.defaultSettingsFiles[0]))
        {
            qDebug() << "Socket will be opened";
            scanner->openSocket();
        }
        else
        {
            qDebug() << "Error while loading settings";
        }
    }
    else if(ui->settings_1->isChecked())
    {
        elementCalculator.LoadSettings(elementCalculator.defaultSettingsFiles[1]);
        if(!scanner->loadSettings(elementCalculator.defaultSettingsFiles[1]))
        {
            //qDebug() << "Socket will be opened";
            scanner->openSocket();
        }
        else
        {
            qDebug() << "Error while loading settings";
        }
    }
    else if(ui->settings_2->isChecked())
    {
        elementCalculator.LoadSettings(elementCalculator.defaultSettingsFiles[2]);
        if(!scanner->loadSettings(elementCalculator.defaultSettingsFiles[2]))
        {
            //qDebug() << "Socket will be opened";
            scanner->openSocket();
        }
        else
        {
            qDebug() << "Error while loading settings";
        }
    }
    else
    {

    }
    ui->spinBox_ScanFrom->setValue(scanner->valueScanFrom());
    ui->spinBox_ScanTo->setValue(scanner->valueScanTo());
    ui->spinBox_ScanTime->setValue(scanner->valueScanTime());
    ui->spinBox_SolenoidFrom->setValue(scanner->valueSolenoidFrom());
    ui->spinBox_SolenoidTo->setValue(scanner->valueSolenoidTo());

    //ui->A->setValue(elementCalculator.dipoleA);
    //ui->B->setValue(elementCalculator.dipoleB);
    //ui->C->setValue(elementCalculator.dipoleC);
    //ui->D->setValue(elementCalculator.dipoleD);
    //ui->E->setValue(elementCalculator.dipoleE);
    //ui->peakWidth->setValue(elementCalculator.peakFWHM);


    ui->Icurrent->setValue(elementCalculator.defaultDipoleCurrent);
    ui->Icurrent->setSingleStep(elementCalculator.defaultDipoleCurrentStep);

    ui->Ioffset->setValue(elementCalculator.defaultDipoleOffset);
    ui->Ioffset->setSingleStep(elementCalculator.defaultDipoleOffsetStep);

    ui->Vacc->setValue(elementCalculator.defaultSourceBias);
    ui->Vacc->setSingleStep(elementCalculator.defaultSourceBiasStep);
    ui->plotAll->setChecked(elementCalculator.defaultPlotSum);
    ui->chargeDistribution->setChecked(elementCalculator.defaultPlotDistribution);
    ui->labelBiasUnit->setText(elementCalculator.defaultSourceBiasUnit);

    //ui->labelDipoleName->setText(elementCalculator.dipoleName);
}

void MainWindow::LoadStartSettings()
{
    ui->xMin->setValue(elementCalculator.defaultPlotXmin);
    ui->xMax->setValue(elementCalculator.defaultPlotXmax);
    ui->Ymin->setValue(elementCalculator.defaultPlotYmin);
    ui->yMax->setValue(elementCalculator.defaultPlotYmax);

    ui->settings_0->setText(elementCalculator.defaultSettingsNames[0]);
    ui->settings_1->setText(elementCalculator.defaultSettingsNames[1]);
    ui->settings_2->setText(elementCalculator.defaultSettingsNames[2]);

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

    ui->elements_0->setText(elementCalculator.defaultElementsNames[0]);
    ui->elements_1->setText(elementCalculator.defaultElementsNames[1]);
    ui->elements_2->setText(elementCalculator.defaultElementsNames[2]);

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
        elementCalculator.AddIsotopes(ui->Element_0->currentIndex(),ui->Isotope_0,ui->MeanChargState_0,ui->FMHW_0,ui->MaxCurrent_0);
    }
    if(isotopeUpdateAsked[1]==1)
    {
        isotopeUpdateAsked[1]=0;
        ui->Isotope_1->clear();
        elementCalculator.AddIsotopes(ui->Element_1->currentIndex(),ui->Isotope_1,ui->MeanChargState_1,ui->FMHW_1,ui->MaxCurrent_1);
    }
    if(isotopeUpdateAsked[2]==1)
    {
        isotopeUpdateAsked[2]=0;
        ui->Isotope_2->clear();
        elementCalculator.AddIsotopes(ui->Element_2->currentIndex(),ui->Isotope_2,ui->MeanChargState_2,ui->FMHW_2,ui->MaxCurrent_2);
    }
    if(isotopeUpdateAsked[3]==1)
    {
        isotopeUpdateAsked[3]=0;
        ui->Isotope_3->clear();
        elementCalculator.AddIsotopes(ui->Element_3->currentIndex(),ui->Isotope_3,ui->MeanChargState_3,ui->FMHW_3,ui->MaxCurrent_3);
    }
    if(isotopeUpdateAsked[4]==1)
    {
        isotopeUpdateAsked[4]=0;
        ui->Isotope_4->clear();
        elementCalculator.AddIsotopes(ui->Element_4->currentIndex(),ui->Isotope_4,ui->MeanChargState_4,ui->FMHW_4,ui->MaxCurrent_4);
    }
    if(isotopeUpdateAsked[5]==1)
    {
        isotopeUpdateAsked[5]=0;
        ui->Isotope_5->clear();
        elementCalculator.AddIsotopes(ui->Element_5->currentIndex(),ui->Isotope_5,ui->MeanChargState_5,ui->FMHW_5,ui->MaxCurrent_5);
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
    AskUpdate();
}

void MainWindow::on_settings_1_clicked()
{
    AskSettingsUpdate();
    AskUpdate();
}

void MainWindow::on_settings_2_clicked()
{
    AskSettingsUpdate();
    AskUpdate();
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->yMax->setValue(0);
    ui->Ymin->setValue(0);
}



void MainWindow::on_ElementFileButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select file"), "./",tr("Ini Files (*.txt)"));
    ui->elementSettingsFilename->setText(file);
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


void MainWindow::on_loadElementsFileButton_clicked()
{
    if(ui->elementsOther->isChecked() && ui->elementSettingsFilename->text().length()>0)
        {
            if (elementCalculator.LoadElements(ui->elementSettingsFilename->text()))
            {
                close();
            }
            LoadElements();
        }
}

void MainWindow::on_saveButton_clicked()
{
    QString defaultFilename;
    defaultFilename.append(elementCalculator.defaultDataPath);
    defaultFilename.append("untitled.pdf");

    QString pdfFileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               defaultFilename,tr("PDF files (*.pdf)"));

    if(pdfFileName.length()>0)
    {
    QStringList parsedFilename = pdfFileName.split(".");

    QString rawDataFilename = parsedFilename[parsedFilename.length()-2];
    rawDataFilename.append("_rawdata.txt");

    QString chargeListFilename = parsedFilename[parsedFilename.length()-2];
    chargeListFilename.append("_ChargeStateList.txt");


    scanner->saveData(rawDataFilename);
    qDebug() << "rawDataFilename: " << rawDataFilename;

    elementCalculator.SaveToFile(pdfFileName,chargeListFilename);
    }
}

void MainWindow::on_legend_clicked()
{
    if(ui->legend->isChecked())
    {
        elementCalculator.plotLegend=1;
    }
    else
    {
        elementCalculator.plotLegend=0;
    }
    AskElementUpdate();
}

void MainWindow::on_scanButton_clicked()
{
    scanner->triggerScan();

}

void MainWindow::on_loadButton_clicked()
{

    QString file = QFileDialog::getOpenFileName(this, tr("Select file"), elementCalculator.defaultDataPath,tr("Raw data (*_rawdata.txt)"));
    if(file.length()>0)
    {
        scanner->loadData(file);
    }

    //cout << file.toStdString() << endl;
    //elementCalculator.LoadData(file.toStdString());
    //cout << "Data OK" << endl;

    /*AskIsotopeUpdate(0);
    AskIsotopeUpdate(1);
    AskIsotopeUpdate(2);
    AskIsotopeUpdate(3);
    AskIsotopeUpdate(4);
    AskIsotopeUpdate(5);
    ui->yMax->setValue(elementCalculator.maxCurrent);
    ui->yMax->setSingleStep(ui->yMax->value()/100.0);
    string label;
    label.append("Data file: ");
    label.append(elementCalculator.loadedDataFile);

    ui->label_dataFile->setText(QString::fromStdString(label));
    */
}

void MainWindow::on_clearButton_clicked()
{
    scanner->clearData();

    //elementCalculator.ClearData();
    //ui->label_dataFile->setText("");

}



void MainWindow::on_ChargeStateListButton_clicked()
{
    if(chargeStateListWindow != NULL)
    {
        delete chargeStateListWindow;
    }
    chargeStateListWindow = new ChargeStateListWindow(this);
    chargeStateListWindow->show();
    AskUpdate();
}

void MainWindow::on_spinBox_ScanFrom_valueChanged(int arg1)
{
    if(initialized==1)
    {
        scanner->setScanFrom(arg1);
    }
}

void MainWindow::on_spinBox_ScanTo_valueChanged(int arg1)
{
    if(initialized==1)
    {
        scanner->setScanTo(arg1);
    }
}

void MainWindow::on_spinBox_ScanTime_valueChanged(int arg1)
{
    if(initialized==1)
    {
        scanner->setScanTime(arg1);
    }
}

void MainWindow::on_spinBox_SolenoidFrom_valueChanged(int arg1)
{
    if(initialized==1)
    {
        scanner->setSolenoidFrom(arg1);
    }
}

void MainWindow::on_spinBox_SolenoidTo_valueChanged(int arg1)
{
    if(initialized==1)
    {
        scanner->setSolenoidTo(arg1);
    }
}

void MainWindow::on_pushButton_Go_clicked()
{
    scanner->goTo(ui->Icurrent->value());
}
