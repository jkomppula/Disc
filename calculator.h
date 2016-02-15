#include "element.h"
#include "qtplotter.h"
#include <QVector>
#include <string>
#include <QSettings>
#include <QComboBox>
#include <QDoubleSpinBox>
#include "qcustomplot.h"
#include <math.h>

using namespace std;

#ifndef CALCULATOR_H
#define CALCULATOR_H



class calculator
{   
    vector <Element> elements;
    QtPlotter* plotter;

    double Gaussian(double x, double FWHM);
    double NormGaussian(double x, double FWHM);
    double IterMQ(double dipoleI);
    double dipoleI(double MQ);

    bool CheckIfSettingsExist(QSettings* file, QString parameter);
    QString recentChargeStateList;

public:
    QString defaultDataPath;
    QString loadedDataFile;


    double maxCurrent;

    QVector <QString > defaultElements;
    QVector <QString > defaultSettingsFiles;
    QVector <QString > defaultSettingsNames;
    int defaultSettingsSelected;

    QVector <QString > defaultElementsFiles;
    QVector <QString > defaultElementsNames;
    int defaultElementsSelected;

    double defaultPlotXmin;
    double defaultPlotXmax;
    double defaultPlotYmin;
    double defaultPlotYmax;

    double defaultDipoleCurrent;
    double defaultDipoleCurrentStep;
    double defaultDipoleOffset;
    double defaultDipoleOffsetStep;
    double defaultSourceBias;
    double defaultSourceBiasStep;
    QString defaultSourceBiasUnit;
    double defaultIntensity;

    bool defaultPlotSum;
    bool defaultPlotDistribution;

    bool plotLegend;


    bool plotAll;

    QVector <double> plotYlimits;
    double dipoleA;
    double dipoleB;
    double dipoleC;
    double dipoleD;
    double dipoleE;


    double magneticFiedA;
    double magneticFiedB;
    double magneticFiedC;
    double magneticFiedD;
    double magneticFiedE;

    double peakFWHM;
    double Ioffset;
    QString dipoleName;
    double Vacc;

    calculator();

    void SetUpPlotter(QCustomPlot* plot,int lines);

    void AddElements(int number, QComboBox* box);
    void AddIsotopes(int element,QComboBox* box,QDoubleSpinBox* avgCharge,QDoubleSpinBox* chargeFWHM,QDoubleSpinBox* current);
    void CalculatePointsMQ(vector <double> x, vector <double>& y, int element, int isotope, double avgCharge, double chargeFWHM, double intensity, double peakFWHM);
    void CalculateCurrentCharges(double I, vector<double>& charges, vector <int> plot, vector <int> elements, vector<int> isotopes);

    void PlotGraphs(double fromI, double toI, vector <int> plot, vector <int> elements, vector <int> isotopes,vector <double> avgCharge,vector <double> chargeFWHM, vector <double> intensity);
    void ChargeStateList(QString& list, vector <int> plot, vector <int> elements, vector <int> isotopes);

    int LoadElements(QString file);
    int LoadSettings(QString file);
    int LoadBasicSettings(void);

    void SaveToFile(QString pdfFileName,QString chargeListFilename);


};

#endif // CALCULATOR_H
