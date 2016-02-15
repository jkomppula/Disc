#include "networkcommunication.h"
#include <QFile>

bool NetworkCommunication::CheckIfSettingsExist(QSettings *file, QString parameter)
{
    if (file->contains(parameter))
    {
        return(1);
    }
    else
    {
        qDebug() << "File " << file->fileName() << " does not contain parameter " << parameter;
        return(0);
    }
}

NetworkCommunication::NetworkCommunication(QObject *parent) :
    QObject(parent)
{
    scanRunning=0;
    scanTriggered=0;


    telegramHandler = NULL;

    telegramData = NULL;
    telegramTrigger = NULL;
    telegramStatus = NULL;

    timerHeartBeat = NULL;
    timerTimeOut = NULL;

    plotter=NULL;
}

int NetworkCommunication::LoadNetworkSettings(QString filename)
{
    if(QFile(filename).exists())
    {
        QSettings* settings;
        settings= new QSettings(filename,QSettings::IniFormat);

        if(telegramHandler != NULL)
        {
            delete telegramHandler;
        }

        if(telegramData != NULL)
        {
            delete telegramData;
        }

        if(telegramTrigger != NULL)
        {
            delete telegramTrigger;
        }

        if(telegramStatus != NULL)
        {
            delete telegramStatus;
        }

        if(!CheckIfSettingsExist(settings, "UDPport"))
            return(1);
        UDPport=settings->value("UDPport").toInt();

        telegramHandler = new QCoDeSysNetVarUDP(QHostAddress::Any,UDPport);

        if(!CheckIfSettingsExist(settings, "VariableIndexIonCurrent"))
            return(1);
        variableIndexIonCurrent = settings->value("VariableIndexIonCurrent").toInt();

        if(!CheckIfSettingsExist(settings, "VariableIndexMagneticField"))
            return(1);
        variableIndexMagneticField = settings->value("VariableIndexMagneticField").toInt();

        if(!CheckIfSettingsExist(settings, "DataIonCurrentCoefA"))
            return(1);
        ionCurrentCoefA = settings->value("DataIonCurrentCoefA").toDouble();

        if(!CheckIfSettingsExist(settings, "DataIonCurrentCoefB"))
            return(1);
        ionCurrentCoefB = settings->value("DataIonCurrentCoefB").toDouble();

        if(!CheckIfSettingsExist(settings, "DataIonCurrentCoefC"))
            return(1);
        ionCurrentCoefC = settings->value("DataIonCurrentCoefC").toDouble();


        if(!CheckIfSettingsExist(settings, "VariableIndexTriggerReceived"))
            return(1);
        variableIndexTriggerReceived = settings->value("VariableIndexTriggerReceived").toInt();

        if(!CheckIfSettingsExist(settings, "VariableIndexScanCompleted"))
            return(1);
        variableIndexScanCompleted = settings->value("VariableIndexScanCompleted").toInt();


        if(!CheckIfSettingsExist(settings, "DataMagneticFieldCoefA"))
            return(1);
        magneticFieldCoefA = settings->value("DataMagneticFieldCoefA").toDouble();

        if(!CheckIfSettingsExist(settings, "DataMagneticFieldCoefB"))
            return(1);
        magneticFieldCoefB = settings->value("DataMagneticFieldCoefB").toDouble();

        if(!CheckIfSettingsExist(settings, "DataMagneticFieldCoefC"))
            return(1);
        magneticFieldCoefC = settings->value("DataMagneticFieldCoefC").toDouble();

        telegramData = new QCoDeSysNVT(0);

        if(!CheckIfSettingsExist(settings, "DataTelegramFormat"))
            return(1);
        telegramData->StringToVariableList(settings->value("DataTelegramFormat").toString());

        if(!CheckIfSettingsExist(settings, "DataTelegramCobID"))
            return(1);
        telegramData->setCobId(settings->value("DataTelegramCobID").toInt());

        telegramStatus = new QCoDeSysNVT(0);

        if(!CheckIfSettingsExist(settings, "StatusTelegramFormat"))
            return(1);
        telegramStatus->StringToVariableList(settings->value("StatusTelegramFormat").toString());

        if(!CheckIfSettingsExist(settings, "StatusTelegramCobID"))
            return(1);
        telegramStatus->setCobId(settings->value("StatusTelegramCobID").toInt());


        telegramTrigger = new QCoDeSysNVT(1);

        if(!CheckIfSettingsExist(settings, "TriggerTelegramFormat"))
            return(1);
        telegramTrigger->StringToVariableList(settings->value("TriggerTelegramFormat").toString());

        if(!CheckIfSettingsExist(settings, "TriggerTelegramCobID"))
            return(1);
        telegramTrigger->setCobId(settings->value("TriggerTelegramCobID").toInt());

        if(!CheckIfSettingsExist(settings, "TriggerTelegramValues"))
            return(1);
        telegramTrigger->StringToData(settings->value("TriggerTelegramValues").toString());

        telegramTrigger->setPort(UDPport);
        telegramTrigger->setIP(QHostAddress::Broadcast);

        if(!CheckIfSettingsExist(settings, "VariableIndexScanFrom"))
            return(1);
        variableIndexScanFrom=settings->value("VariableIndexScanFrom").toInt();

        if(!CheckIfSettingsExist(settings, "VariableIndexScanTo"))
            return(1);
        variableIndexScanTo=settings->value("VariableIndexScanTo").toInt();

        if(!CheckIfSettingsExist(settings, "VariableIndexScanTime"))
            return(1);
        variableIndexScanTime=settings->value("VariableIndexScanTime").toInt();


        if(!CheckIfSettingsExist(settings, "VariableIndexSolFrom"))
            return(1);
        variableIndexSolFrom=settings->value("VariableIndexSolFrom").toInt();

        if(!CheckIfSettingsExist(settings, "VariableIndexSolTo"))
            return(1);
        variableIndexSolTo=settings->value("VariableIndexSolTo").toInt();



        if(!CheckIfSettingsExist(settings, "TriggerTimeOut"))
            return(1);
        triggerTimeOut=settings->value("TriggerTimeOut").toInt();



        if(!CheckIfSettingsExist(settings, "ScanTimeOut"))
            return(1);
        scanTimeOut=settings->value("ScanTimeOut").toInt();

        if(!CheckIfSettingsExist(settings, "HeartBeatTimeInterval"))
            return(1);
        heartBeatTimeInterval=settings->value("HeartBeatTimeInterval").toInt();

        connect(telegramData,SIGNAL(updated()),this,SLOT(DataReceived()));
        connect(telegramStatus,SIGNAL(updated()),this,SLOT(StatusReceived()));

        telegramHandler->telegrams.append(telegramStatus);
        telegramHandler->telegrams.append(telegramData);

        return(0);
    }
    else
    {
        qDebug() << "NetworkCommunication::LoadNetworkSettings: File " << filename << " does not exist!";
        return(1);
    }


}

int NetworkCommunication::TriggerScan()
{
    if(telegramHandler != NULL && telegramTrigger != NULL)
    {

        timerHeartBeat = new QTimer(this);
        connect(timerHeartBeat,SIGNAL(timeout()), this, SLOT(HearBeat()));
        timerHeartBeat->start(heartBeatTimeInterval);
        heartBeatCounter=0;

        bool beat=1;
        bool trigger=1;

        scanTriggered=1;

        telegramTrigger->SetData(0,beat);
        telegramTrigger->SetData(1,trigger);

        return(telegramHandler->BroadcastTelegram(telegramTrigger));
    }
    else
    {
        qDebug() << "NetworkCommunication::TriggerScan: Error, settings were not loaded!";
        return(1);
    }
}

void NetworkCommunication::setPlotter(QCustomPlot *plotterPointer, int graphNumberSetValue)
{
    plotter=plotterPointer;
    graphNumber=graphNumberSetValue;
}

void NetworkCommunication::ClearData()
{
    dataMagneticField.clear();
    dataIonCurrent.clear();
    maxIonCurrent=0;
    minIonCurrent=1e6;
    maxDipoleCurrent=0;
    minDipoleCurrent=1e6;


    if (plotter != NULL)
    {
        plotter->graph(graphNumber)->clearData();
        plotter->replot();
    }


}

int NetworkCommunication::SaveData(QString datafile)
{
    if (dataMagneticField.size()==0)
    {
        qDebug() << "NetworkCommunication::SaveData: Data is not available";
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
        qDebug() << "NetworkCommunication::SaveData: File " << datafile << " can not be opened";
        return(1);
    }


}

int NetworkCommunication::loadData(QString datafile)
{
    QFile file(datafile);


    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "NetworkCommunication::LoadData: File " << datafile << "is open";
        ClearData();
        QTextStream in(&file);
        int counter=0;
        while(!in.atEnd()) {
            counter++;
            QString line = in.readLine();
            QStringList fields = line.split(" ",QString::SkipEmptyParts);
            if(fields.length() == 2)
            {
                AddData(fields[0].toDouble(),fields[1].toDouble());
            }
        }
        file.close();
        emit(updateUI());
        return(0);
    }
    else
    {
        qDebug() << "NetworkCommunication::LoadData: File " << datafile << " can not be opened";
        return(1);
    }



}

void NetworkCommunication::setScanFrom(int value)
{
    if(telegramTrigger != NULL)
    {
        u_int16_t scanfrom = (u_int16_t) value;
        telegramTrigger->SetData(variableIndexScanFrom,scanfrom);
    }
}

void NetworkCommunication::setScanTo(int value)
{
    if(telegramTrigger != NULL)
    {
        u_int16_t scanto = (u_int16_t) value;
        telegramTrigger->SetData(variableIndexScanTo,scanto);
    }
}

void NetworkCommunication::setScanTime(int value)
{
    if(telegramTrigger != NULL)
    {
        u_int16_t time = (u_int16_t) value;
        telegramTrigger->SetData(variableIndexScanTime,time);
    }
}

void NetworkCommunication::setSolenoidFrom(int value)
{
    if(telegramTrigger != NULL)
    {
        u_int16_t solfrom = (u_int16_t) value;
        telegramTrigger->SetData(variableIndexSolFrom,solfrom);
    }
}

void NetworkCommunication::setSolenoidTo(int value)
{
    if(telegramTrigger != NULL)
    {
        u_int16_t solto = (u_int16_t) value;
        telegramTrigger->SetData(variableIndexSolTo,solto);
    }
}


int NetworkCommunication::valueScanFrom()
{
    if(telegramTrigger != NULL)
    {
        u_int16_t value;
        telegramTrigger->ReadData(variableIndexScanFrom,value);
        return(value);
    }
}

int NetworkCommunication::valueScanTo()
{
    if(telegramTrigger != NULL)
    {
        u_int16_t value;
        telegramTrigger->ReadData(variableIndexScanTo,value);
        return(value);
    }
}

int NetworkCommunication::valueScanTime()
{
    if(telegramTrigger != NULL)
    {
        u_int16_t value;
        telegramTrigger->ReadData(variableIndexScanTime,value);
        return(value);
    }
}



int NetworkCommunication::valueSolenoidFrom(void)
{
    if(telegramTrigger != NULL)
    {
        u_int16_t value;
        telegramTrigger->ReadData(variableIndexSolFrom,value);
        return(value);
    }
}

int NetworkCommunication::valueSolenoidTo(void)
{
    if(telegramTrigger != NULL)
    {
        u_int16_t value;
        telegramTrigger->ReadData(variableIndexSolTo,value);
        return(value);
    }
}


void NetworkCommunication::HearBeat()
{
    heartBeatCounter++;
    if ((heartBeatCounter * heartBeatTimeInterval) > (1000*triggerTimeOut) && scanRunning == 0 && scanTriggered == 1)
    {
        scanTriggered=0;
        timerHeartBeat->stop();
        qDebug() << "NetworkCommunication::HearBeat(): TimeOut, the scan did not initialize!";
    }
    else if ((heartBeatCounter * heartBeatTimeInterval) > (1000*scanTimeOut) && scanRunning == 1 && scanTriggered == 1)
    {
        scanTriggered=0;
        EndScan();
        timerHeartBeat->stop();
        qDebug() << "NetworkCommunication::HearBeat(): TimeOut, the scan did not end!";
    }

    else if (telegramTrigger != NULL &&  telegramHandler != NULL)
    {
        //qDebug() << "Beat number" << heartBeatCounter << " & scanRunning = " << scanRunning << " & scanTriggered = " << scanTriggered << " Time number: " << heartBeatCounter * heartBeatTimeInterval << " of limit " << 1000*triggerTimeOut;

        u_int8_t beat;
        telegramTrigger->ReadData(0,beat);
        telegramTrigger->SetData(0,!beat);

        //bool bit0;
        //telegramTrigger->ReadData(0,bit0);
        //bool bit1;
        //telegramTrigger->ReadData(1,bit1);
        //qDebug() << "CobID: " << telegramTrigger->readCobId() << " Port: " << telegramTrigger->readPort() << " Bit 0 :" << bit0 << " Bit 1 :" << bit1 << " variables: " << telegramTrigger->dataVariables.length();

        telegramHandler->BroadcastTelegram(telegramTrigger);
    }
    else
    {
        EndScan();
        timerHeartBeat->stop();
        qDebug() << "NetworkCommunication::HearBeat(): Error in sequency";
    }

}

void NetworkCommunication::DataReceived()
{

    if (telegramData != NULL){

        telegramData->ReadData(variableIndexMagneticField,currentMagneticField);
        telegramData->ReadData(variableIndexIonCurrent,currentIonCurrent);
        //qDebug() << "DataReceived: " << currentMagneticField << " " << currentIonCurrent;

        if(scanRunning==1)
        {
            qDebug() << "Scan data: " << currentMagneticField << " " << currentIonCurrent;
            AddData(currentMagneticField,currentIonCurrent);
        }

        /*if(plotter != NULL)
        {
            plotter->graph(graphNumber)->addData(currentMagneticField,currentIonCurrent);
        }*/
    }
}

void NetworkCommunication::StatusReceived()
{
    //qDebug() << "Status received!";

    if(telegramStatus != NULL){
        u_int8_t statusBool;
        telegramStatus->ReadData(variableIndexTriggerReceived,statusBool);
        if(scanTriggered==1 && scanRunning==0 && statusBool ==1)
        {
            qDebug() << "Start";
            InitializeScan();
        }
        else if (scanTriggered==1 && scanRunning==1 && statusBool ==0)
        {
            qDebug() << "Non violant END";
            EndScan();
        }
    }
}

void NetworkCommunication::InitializeScan()
{
    scanRunning=1;
    dataMagneticField.clear();
    dataIonCurrent.clear();
}

void NetworkCommunication::EndScan()
{
    emit(updateUI());
    qDebug() << "THE END!";
    scanRunning=0;
    scanTriggered=0;
    timerHeartBeat->stop();
}

void NetworkCommunication::AddData(double x, double y)
{
    dataMagneticField.append(x);
    dataIonCurrent.append(y);

    if (plotter != NULL)
    {
        double xI=magneticFieldCoefA+magneticFieldCoefB*x+magneticFieldCoefC*x*x;
        double yI=ionCurrentCoefA+ionCurrentCoefB*y+ionCurrentCoefC*y*y;
        if(xI<minDipoleCurrent)
        {
            minDipoleCurrent=xI;
        }
        if(xI>maxDipoleCurrent);
        {
            maxDipoleCurrent=xI;
        }
        if(yI<minIonCurrent)
        {
            minIonCurrent=yI;
        }
        if(yI>maxIonCurrent)
        {
            maxIonCurrent=yI;
        }


        plotter->graph(graphNumber)->addData(xI,yI);
        plotter->yAxis->setRangeLower(minIonCurrent);
        plotter->yAxis->setRangeUpper(maxIonCurrent);
        plotter->xAxis->setRangeLower(minDipoleCurrent);
        plotter->xAxis->setRangeUpper(maxDipoleCurrent);
        plotter->replot();
    }
}

//OLD FUNCTIONS
/*void calculator::LoadData(string filename)
{
        //cout << "Attempt 1... ";

        ifstream inputFile;
        //cout << "Attempt 2... ";
        inputFile.open(filename.c_str());
        //cout << "Attempt... ";
        if(inputFile.is_open())
        {
            //cout << " OK" << endl;

            double ymax=0;
            string Xrow;
            string Yrow;
            Xrow.reserve(1024*1024);
            Yrow.reserve(1024*1024);
            vector <double> x;
            vector <double> y;
            x.clear();
            y.clear();
            x.reserve(1024*1024);
            y.reserve(1024*1024);

            if (getline(inputFile,Xrow))
            {
                //cout << "1st row:" << Xrow << endl;
            }
            else
            {
                cout << "Unable to read first row" << endl;
            }


            if(getline(inputFile,Yrow))
            {
                //cout << "2nd row:" << Yrow << endl;
            }
            else
            {
                cout << "Unable to read second row" << endl;
            }


            stringstream Xstream(Xrow);
            stringstream Ystream(Yrow);



            string tempString;
            while (getline(Xstream,tempString,','))
            {
                x.push_back(analyserA+analyserB*atof(tempString.c_str())+analyserC*atof(tempString.c_str())*atof(tempString.c_str()));
            }
            while (getline(Ystream,tempString,','))
            {
                y.push_back(analyserIcoef*atof(tempString.c_str())+analyserIoffset);
                if(y[y.size()-1]>ymax)
                {
                    ymax=y[y.size()-1];
                }


            }
            for (int i=0;i<x.size();i++)
            {
                cout << analyserIcoef << " " << analyserIoffset << " " << x[i] << " " << y[i] << endl;
            }

            plotter->clearData(plotter->lines-2);
            plotter->AddData(plotter->lines-2,x,y);
            plotter->Yrange[1]=ymax;
            maxCurrent=ymax;

            plotter->Replot();

            loadedDataFile=filename;

        }
}
*/

//void calculator::SaveToFile(string filename)
//{
//    string dataFilename;
//    string plotFilename;
//    plotFilename.append(filename);
//    plotFilename.append(".pdf");
//
//    dataFilename.append(filename);
//    dataFilename.append("_data.pdf");
//
//    plotter->SavePlotToFile(filename,1189,841,1);
//}

//void calculator::ClearData()
//{
//    plotter->clearData(plotter->lines-2);
//    plotter->Replot();
//
//}
