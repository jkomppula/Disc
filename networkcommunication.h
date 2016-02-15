#ifndef NETWORKCOMMUNICATION_H
#define NETWORKCOMMUNICATION_H

#include <QObject>
#include <QVector>
#include <QSettings>
#include <QTimer>
#include "qcodesysnetvarudp.h"
#include "qcustomplot.h"

class NetworkCommunication : public QObject
{
    Q_OBJECT
    QCoDeSysNetVarUDP* telegramHandler;
    QCoDeSysNVT* telegramData;
    QCoDeSysNVT* telegramTrigger;
    QCoDeSysNVT* telegramStatus;

    QCustomPlot* plotter;

    QTimer* timerHeartBeat;
    QTimer* timerTimeOut;

    quint16 UDPport;

    quint16 variableIndexIonCurrent;
    quint16 variableIndexMagneticField;

    quint16 triggerTimeOut;
    quint16 heartBeatTimeInterval;
    quint16 scanTimeOut;

    int heartBeatCounter;

    int graphNumber;

    quint16 variableIndexTriggerReceived;
    quint16 variableIndexScanCompleted;

    quint16 variableIndexScanFrom;
    quint16 variableIndexScanTo;
    quint16 variableIndexScanTime;

    quint16 variableIndexSolFrom;
    quint16 variableIndexSolTo;

    double ionCurrentCoefA;
    double ionCurrentCoefB;
    double ionCurrentCoefC;

    double magneticFieldCoefA;
    double magneticFieldCoefB;
    double magneticFieldCoefC;

    QVector <double> dataMagneticField;
    QVector <double> dataIonCurrent;

    void SendHeartBeat(void);
    bool CheckIfSettingsExist(QSettings* file, QString parameter);
    void InitializeScan(void);
    void EndScan(void);

    bool scanRunning;
    bool scanTriggered;

    void AddData(double x, double y);

public:
    float currentMagneticField;
    float currentIonCurrent;

    double maxDipoleCurrent;
    double minDipoleCurrent;
    double minIonCurrent;
    double maxIonCurrent;

    explicit NetworkCommunication(QObject *parent = 0);

    int LoadNetworkSettings(QString filename);
    int TriggerScan(void);
    void setPlotter(QCustomPlot* plotter, int graphNumber);
    void ClearData(void);
    int SaveData(QString datafile);

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

signals:
    void DataReady(void);
    void WaitingRepplyToTrigger(void);
    void ScanInProgress(void);
    void updateUI(void);

public slots:
    void HearBeat(void);
    void DataReceived(void);
    void StatusReceived(void);

};

#endif // NETWORKCOMMUNICATION_H
