#ifndef DIPOLESCANNER_H
#define DIPOLESCANNER_H

#include <QObject>
#include <QVector>
#include <QSettings>
#include <QTimer>
#include "qcodesysnv.h"
#include "qcustomplot.h"

class DipoleScanner : public QObject
{
    Q_OBJECT

    QCodesysNVSocket* telegramSocket;
    QCodesysNVTelegram* telegramData;
    QString telegramDataVariableList;
    quint32 telegramDataCobId;

    QCodesysNVTelegram* telegramStatus;
    QString telegramStatusVariableList;
    quint32 telegramStatusCobId;

    QCodesysNVTelegram* telegramTrigger;
    QString telegramTriggerVariableList;
    QString telegramTriggerDefaultData;
    quint32 telegramTriggerCobId;
    quint32 telegramTriggerPort;


    QCustomPlot* plotter;

    QTimer* timerGoto;
    QTimer* timerHeartBeat;
    QTimer* timerTimeOut;

    quint16 UDPport;

    quint16 variableIndexIonCurrent;
    quint16 variableIndexMagneticField;

    quint16 triggerTimeOut;
    quint16 heartBeatTimeInterval;
    quint16 scanTimeOut;

    int heartBeatCounter;

    int plotGraph;
    int realTimeBDataGraph;
    int realTimeBeamDataGraph;

    quint16 variableIndexTriggerReceived;
    quint16 variableIndexScanCompleted;

    quint16 variableIndexHeartBeat;
    quint16 variableIndexScanEnable;
    quint16 variableIndexScanTime;

    quint16 variableIndexScanFrom;
    quint16 variableIndexScanTo;

    quint16 variableIndexSolFrom;
    quint16 variableIndexSolTo;

    quint16 variableIndexGoToEnable;
    quint16 variableIndexGoToValue;

    double goToCommandTimeOut;

    int goToBeatCounter;

    double ionCurrentCoefA;
    double ionCurrentCoefB;
    double ionCurrentCoefC;

    double magneticFieldCoefA;
    double magneticFieldCoefB;
    double magneticFieldCoefC;

    QVector <double> dataMagneticField;
    QVector <double> dataIonCurrent;

    bool checkIfSettingsExist(QSettings* file, QString parameter);
    void initializeScan(void);
    void endScan(int nonviolent);

    bool scanRunning;
    bool scanTriggered;

    void addData(double B, double Ibeam, double Idip, double scaledIbeam);

    double dipoleCurrentToMagneticField(double dipoleCurrent);
    double magneticFieldToDipoleCurrent(double magneticField);
    double measuredBeamToCorrectedBeam(double measurement);


public:
    double currentMagneticField;
    double currentIonCurrent;
    double currentDipoleCurrent;
    double currentCorrectedBeam;


    double maxMagneticField;
    double minMagneticField;
    double maxDipoleCurrent;
    double minDipoleCurrent;
    double minIonCurrent;
    double maxIonCurrent;

    explicit DipoleScanner(QObject *parent = 0);

    int loadSettings(QString filename);
    int openSocket(void);


    int triggerScan(void);
    void setPlotter(QCustomPlot *plotter, int plotGraph, int realTimeDataGraph, int realTimeBeamGraph);
    void clearData(void);
    int saveData(QString datafile);

    int loadData(QString datafile);

    void setScanFrom(int value);
    void setScanTo(int value);
    void setScanTime(int value);

    void setSolenoidFrom(int value);
    void setSolenoidTo(int value);

    int valueScanFrom(void);
    int valueScanTo(void);
    int valueScanTime(void);

    int valueSolenoidFrom(void);
    int valueSolenoidTo(void);

    void updateX2axis(void);

    void goTo(double dipoleCurrent);

signals:
    void recentValues(double,double);
    void dataReady(void);
    void waitingRepplyToTrigger(void);
    void scanInProgress(void);
    void updateUI(void);
    void errorMessage(QString, QString, QString, QString, QString);

    void scanStatus(int);

public slots:
    void goToBeat(void);
    void heartBeat(void);
    void dataReceived(void);
    void statusReceived(void);
    void UDPreceived(QString IP,uint port,uint bytes);
};

#endif // DIPOLESCANNER_H
