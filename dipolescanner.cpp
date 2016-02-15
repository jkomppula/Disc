#include "dipolescanner.h"
#include <QFile>

bool DipoleScanner::checkIfSettingsExist(QSettings *file, QString parameter)
{
    if (file->contains(parameter))
    {
        return(1);
    }
    else
    {
        qDebug() << "File " << file->fileName() << " does not contain parameter " << parameter;
        emit errorMessage("Error!","Settings file",file->fileName(),"does not include parameter:",parameter);
        return(0);
    }
}

DipoleScanner::DipoleScanner(QObject *parent) :
    QObject(parent)
{
    scanRunning=0;
    scanTriggered=0;


    telegramSocket = NULL;

    telegramData = NULL;
    telegramTrigger = NULL;
    telegramStatus = NULL;

    timerHeartBeat = NULL;
    timerTimeOut = NULL;
    timerGoto = NULL;

    plotter=NULL;
}

int DipoleScanner::loadSettings(QString filename)
{
    if(QFile(filename).exists())
    {
        QSettings* settings;
        settings= new QSettings(filename,QSettings::IniFormat);

        if(!checkIfSettingsExist(settings, "UDPport"))
            return(1);
        UDPport=settings->value("UDPport").toInt();

        if(!checkIfSettingsExist(settings, "VariableIndexIonCurrent"))
            return(1);
        variableIndexIonCurrent = settings->value("VariableIndexIonCurrent").toInt();

        if(!checkIfSettingsExist(settings, "VariableIndexMagneticField"))
            return(1);
        variableIndexMagneticField = settings->value("VariableIndexMagneticField").toInt();

        if(!checkIfSettingsExist(settings, "DataIonCurrentCoefA"))
            return(1);
        ionCurrentCoefA = settings->value("DataIonCurrentCoefA").toDouble();

        if(!checkIfSettingsExist(settings, "DataIonCurrentCoefB"))
            return(1);
        ionCurrentCoefB = settings->value("DataIonCurrentCoefB").toDouble();

        if(!checkIfSettingsExist(settings, "DataIonCurrentCoefC"))
            return(1);
        ionCurrentCoefC = settings->value("DataIonCurrentCoefC").toDouble();


        if(!checkIfSettingsExist(settings, "VariableIndexTriggerReceived"))
            return(1);
        variableIndexTriggerReceived = settings->value("VariableIndexTriggerReceived").toInt();

        if(!checkIfSettingsExist(settings, "VariableIndexScanCompleted"))
            return(1);
        variableIndexScanCompleted = settings->value("VariableIndexScanCompleted").toInt();


        if(!checkIfSettingsExist(settings, "DataMagneticFieldCoefA"))
            return(1);
        magneticFieldCoefA = settings->value("DataMagneticFieldCoefA").toDouble();

        if(!checkIfSettingsExist(settings, "DataMagneticFieldCoefB"))
            return(1);
        magneticFieldCoefB = settings->value("DataMagneticFieldCoefB").toDouble();

        if(!checkIfSettingsExist(settings, "DataMagneticFieldCoefC"))
            return(1);
        magneticFieldCoefC = settings->value("DataMagneticFieldCoefC").toDouble();



        if(!checkIfSettingsExist(settings, "DataTelegramFormat"))
            return(1);
        telegramDataVariableList=settings->value("DataTelegramFormat").toString();

        if(!checkIfSettingsExist(settings, "DataTelegramCobID"))
            return(1);
        telegramDataCobId=settings->value("DataTelegramCobID").toInt();



        if(!checkIfSettingsExist(settings, "StatusTelegramFormat"))
            return(1);
        telegramStatusVariableList=settings->value("StatusTelegramFormat").toString();

        if(!checkIfSettingsExist(settings, "StatusTelegramCobID"))
            return(1);
        telegramStatusCobId=settings->value("StatusTelegramCobID").toInt();




        if(!checkIfSettingsExist(settings, "TriggerTelegramFormat"))
            return(1);
        telegramTriggerVariableList=settings->value("TriggerTelegramFormat").toString();

        if(!checkIfSettingsExist(settings, "TriggerTelegramCobID"))
            return(1);
        telegramTriggerCobId=settings->value("TriggerTelegramCobID").toInt();

        if(!checkIfSettingsExist(settings, "TriggerTelegramValues"))
            return(1);
        telegramTriggerDefaultData=settings->value("TriggerTelegramValues").toString();



        if(!checkIfSettingsExist(settings, "VariableIndexHeartBeat"))
            return(1);
        variableIndexHeartBeat=settings->value("VariableIndexHeartBeat").toInt();

        if(!checkIfSettingsExist(settings, "VariableIndexScanEnable"))
            return(1);
        variableIndexScanEnable=settings->value("VariableIndexScanEnable").toInt();

        if(!checkIfSettingsExist(settings, "VariableIndexScanFrom"))
            return(1);
        variableIndexScanFrom=settings->value("VariableIndexScanFrom").toInt();

        if(!checkIfSettingsExist(settings, "VariableIndexScanTo"))
            return(1);
        variableIndexScanTo=settings->value("VariableIndexScanTo").toInt();

        if(!checkIfSettingsExist(settings, "VariableIndexScanTime"))
            return(1);
        variableIndexScanTime=settings->value("VariableIndexScanTime").toInt();


        if(!checkIfSettingsExist(settings, "VariableIndexSolFrom"))
            return(1);
        variableIndexSolFrom=settings->value("VariableIndexSolFrom").toInt();

        if(!checkIfSettingsExist(settings, "VariableIndexSolTo"))
            return(1);
        variableIndexSolTo=settings->value("VariableIndexSolTo").toInt();





        if(!checkIfSettingsExist(settings, "VariableIndexGoToEnable"))
            return(1);
        variableIndexGoToEnable=settings->value("VariableIndexGoToEnable").toInt();

        if(!checkIfSettingsExist(settings, "VariableIndexGoToValue"))
            return(1);
        variableIndexGoToValue=settings->value("VariableIndexGoToValue").toInt();

        if(!checkIfSettingsExist(settings, "GoToCommandTimeOut"))
            return(1);
        goToCommandTimeOut=settings->value("GoToCommandTimeOut").toInt();


        if(!checkIfSettingsExist(settings, "TriggerTimeOut"))
            return(1);
        triggerTimeOut=settings->value("TriggerTimeOut").toInt();



        if(!checkIfSettingsExist(settings, "ScanTimeOut"))
            return(1);
        scanTimeOut=settings->value("ScanTimeOut").toInt();

        if(!checkIfSettingsExist(settings, "HeartBeatTimeInterval"))
            return(1);
        heartBeatTimeInterval=settings->value("HeartBeatTimeInterval").toInt();

        return(0);
    }
    else
    {
        qDebug() << "DipoleScanner::LoadNetworkSettings: File " << filename << " does not exist!";
        emit errorMessage("Error!","Settings file",filename,"does not exist!","");
        return(1);
    }


}

int DipoleScanner::openSocket()
{

    if(telegramSocket != NULL)
    {
        delete telegramSocket;
    }

    telegramSocket = new QCodesysNVSocket(QHostAddress::Any,UDPport);
    //qDebug() << "Port: " << UDPport;


    if(telegramData != NULL)
    {
        delete telegramData;
    }
    telegramData = new QCodesysNVTelegram(0);
    telegramData->setVariableTypes(telegramDataVariableList);
    telegramData->setCobId(telegramDataCobId);
    //qDebug() << "Data variables: " << telegramDataVariableList;
    //qDebug() << "Data CobID: " << telegramDataCobId;


    if(telegramStatus != NULL)
    {
        delete telegramStatus;
    }
    telegramStatus = new QCodesysNVTelegram(0);
    telegramStatus->setVariableTypes(telegramStatusVariableList);
    telegramStatus->setCobId(telegramStatusCobId);



    if(telegramTrigger != NULL)
    {
        delete telegramTrigger;
    }

    telegramTrigger = new QCodesysNVTelegram(1);
    telegramTrigger->setVariableTypes(telegramTriggerVariableList);
    telegramTrigger->setCobId(telegramTriggerCobId);
    telegramTrigger->stringToData(telegramTriggerDefaultData);
    telegramTrigger->setPort(UDPport);
    telegramTrigger->setIP(QHostAddress::Broadcast);

    connect(telegramData,SIGNAL(updated()),this,SLOT(dataReceived()));
    connect(telegramStatus,SIGNAL(updated()),this,SLOT(statusReceived()));
    //connect(telegramSocket,SIGNAL(udpPackageReceived(QString,uint,uint)),this,SLOT(UDPreceived(QString,uint,uint)));
    //qDebug() << "Connected";

    telegramSocket->addTelegram(telegramStatus);
    telegramSocket->addTelegram(telegramData);

}


void DipoleScanner::UDPreceived(QString IP,uint port,uint bytes)
{
    //qDebug() << "UDP received";
}

int DipoleScanner::triggerScan()
{
    if(telegramSocket != NULL && telegramTrigger != NULL)
    {

        if (timerHeartBeat != NULL)
        {
            delete timerHeartBeat;
        }
        timerHeartBeat = new QTimer(this);
        connect(timerHeartBeat,SIGNAL(timeout()), this, SLOT(heartBeat()));
        timerHeartBeat->start(heartBeatTimeInterval);
        heartBeatCounter=0;

        bool beat=1;
        bool trigger=1;

        clearData();

        scanTriggered=1;
        scanRunning=0;
        telegramTrigger->setData(variableIndexHeartBeat,beat);
        telegramTrigger->setData(variableIndexScanEnable,trigger);

        return(telegramSocket->broadcastTelegram(telegramTrigger));
    }
    else
    {
        qDebug() << "DipoleScanner::TriggerScan: Error, settings were not loaded!";
        emit errorMessage("Error!","Scan can not be triggered.","Scanner settings are not loaded.","","");
        return(1);
    }
}

void DipoleScanner::setPlotter(QCustomPlot *plotter, int plotGraph, int realTimeBDataGraph, int realTimeBeamDataGraph)
{
    this->realTimeBDataGraph=realTimeBDataGraph;
    this->plotter=plotter;
    this->plotGraph=plotGraph;
    this->realTimeBeamDataGraph = realTimeBeamDataGraph;

    plotter->graph(realTimeBeamDataGraph)->setPen(QPen(Qt::blue));
    plotter->graph(realTimeBeamDataGraph)->setLineStyle(QCPGraph::lsNone);
    plotter->graph(realTimeBeamDataGraph)->setScatterStyle(QCPScatterStyle::ssDisc);

    plotter->xAxis2->setLabel("Magnetic field [mT]");
    plotter->xAxis2->setRangeReversed(1);
    plotter->xAxis2->setVisible(true);
    plotter->xAxis2->setTickLabels(true);

}

void DipoleScanner::clearData()
{
    dataMagneticField.clear();
    dataIonCurrent.clear();

    if (plotter != NULL)
    {
        plotter->graph(plotGraph)->clearData();
        plotter->replot();
    }


}

int DipoleScanner::saveData(QString datafile)
{
    if (dataMagneticField.size()==0)
    {
        qDebug() << "DipoleScanner::SaveData: Data is not available";
        emit errorMessage("Error!","Data can not be saved.","No data available!","","");
        return(1);
    }

    QFile file(datafile);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {

        QTextStream out(&file);

        for (int i=0;i<dataMagneticField.size();i++)
        {
            out << dataMagneticField[i] << "  " << dataIonCurrent[i] << "\r\n";
        }
        file.close();
        return(0);
    }
    else
    {
        qDebug() << "DipoleScanner::SaveData: File " << datafile << " can not be opened";
        emit errorMessage("Error!","File ",datafile,"can not be opened","");
        return(1);
    }


}

int DipoleScanner::loadData(QString datafile)
{
    QFile file(datafile);


    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "DipoleScanner::LoadData: File " << datafile << "is open";
        clearData();
        QTextStream in(&file);
        int counter=0;


        double tempDip;
        double tempX;
        double tempY;
        double tempCorrectedBeam;

        QVector <double> dataDipoleCurrents;
        QVector <double> dataCorrectedBeams;

        while(!in.atEnd()) {
            counter++;
            QString line = in.readLine();
            QStringList fields = line.split(" ",QString::SkipEmptyParts);
            if(fields.length() == 2)
            {

                tempX=fields[0].toDouble();
                tempY=fields[1].toDouble();
                tempDip=magneticFieldToDipoleCurrent(tempX);
                tempCorrectedBeam=measuredBeamToCorrectedBeam(tempY);

                if(dataMagneticField.size()==0)
                {
                    minDipoleCurrent=tempDip;
                    minMagneticField=tempX;
                    maxDipoleCurrent=tempDip;
                    maxMagneticField=tempX;
                    minIonCurrent=tempCorrectedBeam;
                    maxIonCurrent=tempCorrectedBeam;
                }

                dataMagneticField.append(tempX);
                dataIonCurrent.append(tempY);

                if(tempDip<minDipoleCurrent)
                {
                    minDipoleCurrent=tempDip;
                    minMagneticField=tempX;
                }
                if(tempX>maxDipoleCurrent)
                {
                    maxDipoleCurrent=tempDip;
                    maxMagneticField=tempX;
                }
                if(tempCorrectedBeam<minIonCurrent)
                {
                    minIonCurrent=tempCorrectedBeam;
                }
                if(tempCorrectedBeam>maxIonCurrent)
                {
                    maxIonCurrent=tempCorrectedBeam;
                }
                dataDipoleCurrents.append(tempDip);
                dataCorrectedBeams.append(tempCorrectedBeam);
            }
        }
        file.close();

        plotter->graph(plotGraph)->addData(dataDipoleCurrents,dataCorrectedBeams);
        plotter->yAxis->setRangeLower(minIonCurrent);
        plotter->yAxis->setRangeUpper(maxIonCurrent);
        plotter->xAxis->setRangeLower(minDipoleCurrent);
        plotter->xAxis->setRangeUpper(maxDipoleCurrent);
        plotter->xAxis2->setRangeLower(minMagneticField);
        plotter->xAxis2->setRangeUpper(maxMagneticField);

        emit(updateUI());
        return(0);
    }
    else
    {
        qDebug() << "DipoleScanner::LoadData: File " << datafile << " can not be opened";
        emit errorMessage("Error!","Data can not be loaded.","File",datafile,"can not be opened");
        return(1);
    }



}

void DipoleScanner::setScanFrom(int value)
{
    if(telegramTrigger != NULL)
    {
        quint16 scanfrom = (quint16) value;
        telegramTrigger->setData(variableIndexScanFrom,scanfrom);
    }
}

void DipoleScanner::setScanTo(int value)
{
    if(telegramTrigger != NULL)
    {
        quint16 scanto = (quint16) value;
        telegramTrigger->setData(variableIndexScanTo,scanto);
    }
}

void DipoleScanner::setScanTime(int value)
{
    if(telegramTrigger != NULL)
    {
        quint16 time = (quint16) value;
        telegramTrigger->setData(variableIndexScanTime,time);
    }
}

void DipoleScanner::setSolenoidFrom(int value)
{
    if(telegramTrigger != NULL)
    {
       quint16 solfrom = (quint16) value;
        telegramTrigger->setData(variableIndexSolFrom,solfrom);
    }
}

void DipoleScanner::setSolenoidTo(int value)
{
    if(telegramTrigger != NULL)
    {
        quint16 solto = (quint16) value;
        telegramTrigger->setData(variableIndexSolTo,solto);
    }
}


int DipoleScanner::valueScanFrom()
{
    if(telegramTrigger != NULL)
    {
        quint16 value;
        telegramTrigger->readData(variableIndexScanFrom,value);
        return(value);
    }
}

int DipoleScanner::valueScanTo()
{
    if(telegramTrigger != NULL)
    {
        quint16 value;
        telegramTrigger->readData(variableIndexScanTo,value);
        return(value);
    }
}

int DipoleScanner::valueScanTime()
{
    if(telegramTrigger != NULL)
    {
        quint16 value;
        telegramTrigger->readData(variableIndexScanTime,value);
        return(value);
    }
}



int DipoleScanner::valueSolenoidFrom(void)
{
    if(telegramTrigger != NULL)
    {
        quint16 value;
        telegramTrigger->readData(variableIndexSolFrom,value);
        return(value);
    }
}

int DipoleScanner::valueSolenoidTo(void)
{
    if(telegramTrigger != NULL)
    {
        quint16 value;
        telegramTrigger->readData(variableIndexSolTo,value);
        return(value);
    }
}

void DipoleScanner::updateX2axis()
{
    if(plotter!=NULL)
    {
        QCPRange currentRange=plotter->xAxis->range();
        double dipImin=currentRange.lower;
        double dipImax=currentRange.upper;

        double minB=dipoleCurrentToMagneticField(dipImin);
        double maxB=dipoleCurrentToMagneticField(dipImax);

        plotter->xAxis2->setRangeLower(minB);
        plotter->xAxis2->setRangeUpper(maxB);

    }
}

void DipoleScanner::goTo(double dipoleCurrent)
{
    if(scanTriggered==0 && scanRunning==0)
    {
        if(telegramSocket != NULL && telegramTrigger != NULL)
        {

            goToBeatCounter=0;
            if (timerGoto!=NULL)
            {
                delete timerGoto;
            }

            timerGoto = new QTimer(this);
            connect(timerGoto,SIGNAL(timeout()), this, SLOT(goToBeat()));
            timerGoto->start(heartBeatTimeInterval);
            heartBeatCounter=0;

            telegramTrigger->setData(variableIndexHeartBeat,1);
            telegramTrigger->setData(variableIndexGoToEnable,1);
            telegramTrigger->setData(variableIndexGoToValue,dipoleCurrent);

            telegramSocket->broadcastTelegram(telegramTrigger);
        }
        else
        {
            qDebug() << "DipoleScanner::goTo(double): Error, settings are not loaded!";
            emit errorMessage("Error!","Command can not be broadcasted.","Scanner settings are not loaded.","","");
           //return(1);
        }
    }
    else
    {
        qDebug() << "DipoleScanner::goTo(double): Error, scan in progress!";
        emit errorMessage("Error!","Command can not be broadcasted.","Scan in progress","","");
        //return(1);
    }


}

void DipoleScanner::goToBeat()
{
    if(((double) heartBeatTimeInterval)*((double) goToBeatCounter) < goToCommandTimeOut*1000.0 )
    {
        goToBeatCounter++;

        quint8 beat;
        telegramTrigger->readData(variableIndexHeartBeat,beat);
        telegramTrigger->setData(variableIndexHeartBeat,!beat);

        telegramSocket->broadcastTelegram(telegramTrigger);
    }
    else
    {
        timerGoto->stop();
        telegramTrigger->setData(variableIndexHeartBeat,0);
        telegramTrigger->setData(variableIndexGoToEnable,0);
        telegramSocket->broadcastTelegram(telegramTrigger);
    }

}

double DipoleScanner::dipoleCurrentToMagneticField(double dipoleCurrent)
{
    double limit=1e-4;
    int maxIterations=1000;

    double lower=0;
    double upper=10000;
    double counter=0;
    double tempI=0;
    double tempB=0;

    while((upper-lower)/((upper+lower)/2)>limit && counter<maxIterations)
    {
        counter++;
        tempB=(upper+lower)/2.0;
        tempI=magneticFieldToDipoleCurrent(tempB);

        if(tempI>=dipoleCurrent)
        {
            upper=(upper+lower)/2.0;
        }
        else
        {
            lower=(upper+lower)/2.0;
        }
    }
    return(lower);
}

double DipoleScanner::magneticFieldToDipoleCurrent(double magneticField)
{
    double Idip=magneticFieldCoefA+magneticFieldCoefB*magneticField+magneticFieldCoefC*magneticField*magneticField;
    return(Idip);
}

double DipoleScanner::measuredBeamToCorrectedBeam(double measurement)
{
    double Icorrect=ionCurrentCoefA+ionCurrentCoefB*measurement+ionCurrentCoefC*measurement*measurement;
    return(Icorrect);
}

void DipoleScanner::heartBeat()
{
    heartBeatCounter++;
    if ((heartBeatCounter * heartBeatTimeInterval) > (1000*triggerTimeOut) && scanRunning == 0 && scanTriggered == 1)
    {
        scanTriggered=0;
        timerHeartBeat->stop();
        qDebug() << "DipoleScanner::HearBeat(): TimeOut, the scan was not initialized.";
        endScan(0);
        emit errorMessage("Error!","Trigger time out.","The scan was not initialized.","","");
    }
    else if ((heartBeatCounter * heartBeatTimeInterval) > (1000*scanTimeOut) && scanRunning == 1 && scanTriggered == 1)
    {
        scanTriggered=0;
        endScan(0);
        timerHeartBeat->stop();
        qDebug() << "DipoleScanner::HearBeat(): TimeOut, the scan did not end!";
        emit errorMessage("Error!","Scan time out","The scan did not end within time limit.","","");
    }

    else if (telegramTrigger != NULL &&  telegramSocket != NULL)
    {
        //qDebug() << "Beat number" << heartBeatCounter << " & scanRunning = " << scanRunning << " & scanTriggered = " << scanTriggered << " Time number: " << heartBeatCounter * heartBeatTimeInterval << " of limit " << 1000*triggerTimeOut;

        if(scanRunning==1)
        {
            emit scanStatus(2);
        }
        else
        {
            emit scanStatus(1);
        }

        quint8 beat;
        telegramTrigger->readData(variableIndexHeartBeat,beat);
        telegramTrigger->setData(variableIndexHeartBeat,!beat);

        //bool bit0;
        //telegramTrigger->ReadData(0,bit0);
        //bool bit1;
        //telegramTrigger->ReadData(1,bit1);
        //qDebug() << "CobID: " << telegramTrigger->readCobId() << " Port: " << telegramTrigger->readPort() << " Bit 0 :" << bit0 << " Bit 1 :" << bit1 << " variables: " << telegramTrigger->dataVariables.length();

        telegramSocket->broadcastTelegram(telegramTrigger);
    }
    else
    {
        endScan(0);
        timerHeartBeat->stop();
        qDebug() << "DipoleScanner::HearBeat(): Error in sequency";
    }

}

void DipoleScanner::dataReceived()
{

    //qDebug() << "Dataa!!!";
    if (telegramData != NULL){

        telegramData->readData(variableIndexMagneticField,currentMagneticField);
        telegramData->readData(variableIndexIonCurrent,currentIonCurrent);
        emit recentValues(currentMagneticField,currentIonCurrent);

        //qDebug() << "DataReceived: " << currentMagneticField << " " << currentIonCurrent;
        currentDipoleCurrent=magneticFieldToDipoleCurrent(currentMagneticField);
        currentCorrectedBeam=measuredBeamToCorrectedBeam(currentIonCurrent);
        if(scanRunning==1)
        {
            //qDebug() << "Scan data: " << currentMagneticField << " " << currentIonCurrent;
            addData(currentMagneticField,currentIonCurrent,currentDipoleCurrent,currentCorrectedBeam);
        }
        else if(plotter != NULL)
        {
            //qDebug() << "Plot";
            plotter->graph(realTimeBDataGraph)->clearData();
            QCPRange currentRange=plotter->yAxis->range();
            plotter->graph(realTimeBDataGraph)->addData(currentDipoleCurrent,currentRange.lower);
            plotter->graph(realTimeBDataGraph)->addData(currentDipoleCurrent+0.0001*currentDipoleCurrent,currentRange.upper);
            plotter->graph(realTimeBeamDataGraph)->clearData();
            plotter->graph(realTimeBeamDataGraph)->addData(currentDipoleCurrent, currentCorrectedBeam);
            plotter->replot();
        }
    }
}

void DipoleScanner::statusReceived()
{
    //qDebug() << "Status received!";

    if(telegramStatus != NULL){
        quint8 statusBool;
        telegramStatus->readData(variableIndexTriggerReceived,statusBool);
        if(scanTriggered==1 && scanRunning==0 && statusBool ==1)
        {
            qDebug() << "Start";
            initializeScan();
        }
        else if (scanTriggered==1 && scanRunning==1 && statusBool ==0)
        {
            qDebug() << "Non violant END";
            endScan(1);
        }
    }
}

void DipoleScanner::initializeScan()
{
    scanRunning=1;
    dataMagneticField.clear();
    dataIonCurrent.clear();
}

void DipoleScanner::endScan(int nonviolent)
{

    emit scanStatus(0);
    scanRunning=0;
    scanTriggered=0;
    timerHeartBeat->stop();
    telegramTrigger->setData(variableIndexHeartBeat,0);
    telegramTrigger->setData(variableIndexScanEnable,0);
    telegramSocket->broadcastTelegram(telegramTrigger);

    if(nonviolent==1)
    {
        //qDebug() << "Data parsing";
        QVector<double> tempCorrectedBeam;
        QVector<double> tempDip;
        QVector<double> tempX;
        QVector<double> tempY;


        int start=0;
        double prevMax=1e6;

        for(int i=0;i<dataMagneticField.size();i++)
        {
            if (dataMagneticField[i]<prevMax && start==0)
            {
                prevMax=dataMagneticField[i];
            }
            else
            {
                start=1;
            }
            if(start==1)
            {
                tempX.append(dataMagneticField[i]);
                tempY.append(dataIonCurrent[i]);
                tempDip.append(magneticFieldToDipoleCurrent(dataMagneticField[i]));
                tempCorrectedBeam.append(measuredBeamToCorrectedBeam(dataIonCurrent[i]));
          }

        }
        if (start==0)
        {
            qDebug() << "No valid data";
        }
        dataMagneticField=tempX;
        dataIonCurrent=tempY;

        plotter->graph(plotGraph)->clearData();
        plotter->graph(plotGraph)->addData(tempDip,tempCorrectedBeam);
        plotter->replot();
        emit(updateUI());
    }

}

void DipoleScanner::addData(double B, double Ibeam,double Idip,double correctedBeam)
{

    //qDebug() << "Data: " << B << Ibeam << Idip << correctedBeam;
    if (dataMagneticField.size()==0)
    {
        minDipoleCurrent=Idip;
        minMagneticField=B;
        maxDipoleCurrent=Idip+0.01*sqrt(Idip*Idip);
        maxMagneticField=B+0.01*sqrt(B*B);
        minIonCurrent=correctedBeam;
        maxIonCurrent=correctedBeam+0.01*sqrt(correctedBeam*correctedBeam);

    }

    dataMagneticField.append(B);
    dataIonCurrent.append(Ibeam);

    if (plotter != NULL)
    {

        if(Idip<minDipoleCurrent)
        {
            minDipoleCurrent=Idip;
            minMagneticField=B;
        }
        if(Idip>maxDipoleCurrent)
        {
            maxDipoleCurrent=Idip;
            maxMagneticField=B;
            //qDebug() << "Max dipole current set to " << maxDipoleCurrent << maxMagneticField;
        }
        if(correctedBeam<minIonCurrent)
        {
            minIonCurrent=correctedBeam;
        }
        if(correctedBeam>maxIonCurrent)
        {
            //qDebug() << "Max ion current set to " << maxIonCurrent;
            maxIonCurrent=correctedBeam;
        }

        //qDebug() << minIonCurrent << maxIonCurrent << minDipoleCurrent << maxDipoleCurrent << minMagneticField << maxMagneticField;
        plotter->graph(plotGraph)->addData(Idip,correctedBeam);
        plotter->yAxis->setRangeLower(minIonCurrent);
        plotter->yAxis->setRangeUpper(maxIonCurrent);
        plotter->xAxis->setRangeLower(minDipoleCurrent);
        plotter->xAxis->setRangeUpper(maxDipoleCurrent);
        plotter->xAxis2->setRangeLower(minMagneticField);
        plotter->xAxis2->setRangeUpper(maxMagneticField);

        plotter->replot();
    }
}
