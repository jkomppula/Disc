#include "qcodesysnetvarudp.h"
#include <QDataStream>
#include <QStringList>
#include <iostream>

const int QCoDeSysNVT::VAR_BYTES[] = {1,1,2,4,1,1,2,2,4,4,4,8};

const int QCoDeSysNVT::VAR_BOOL=0;
const int QCoDeSysNVT::VAR_BYTE=1;
const int QCoDeSysNVT::VAR_WORD=2;
const int QCoDeSysNVT::VAR_DWORD=3;
const int QCoDeSysNVT::VAR_SINT=4;
const int QCoDeSysNVT::VAR_USINT=5;
const int QCoDeSysNVT::VAR_INT=6;
const int QCoDeSysNVT::VAR_UINT=7;
const int QCoDeSysNVT::VAR_DINT=8;
const int QCoDeSysNVT::VAR_UDINT=9;
const int QCoDeSysNVT::VAR_REAL=10;
const int QCoDeSysNVT::VAR_LREAL=11;

QCoDeSysNetVarUDP::QCoDeSysNetVarUDP(QHostAddress incomingIP,quint16 localPort,QObject *parent) :
    QObject(parent)
{
    telegramIdentity.resize(4);
    telegramIdentity[3]='3';
    telegramIdentity[2]='S';
    telegramIdentity[1]='-';
    telegramIdentity[0]=0;

    IP=incomingIP;
    port=localPort;
    // create a QUDP socket
    socket = new QUdpSocket(this);

    // The most common way to use QUdpSocket class is
    // to bind to an address and port using bind()
    // bool QAbstractSocket::bind(const QHostAddress & address,
    //     quint16 port = 0, BindMode mode = DefaultForPlatform)
    socket->bind(incomingIP, localPort);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

}

int QCoDeSysNetVarUDP::BroadcastTelegram(QCoDeSysNVT *telegram)
{
    if(telegram->isWritable())
    {
        QByteArray tempData;
        QHostAddress address;
        quint16 port;
        telegram->readTelegram(tempData);
        address=telegram->readIP();
        port=telegram->readPort();

        if (socket->writeDatagram(tempData,address,port)>0)
        //if(socket->write(tempData))
        {
            //qDebug() << "QCoDeSysNetVarUDP::BroadcastTelegram: Data write OK!";
            //qDebug() << "QCoDeSysNetVarUDP::BroadcastTelegram: Message " << tempData.length() << " bytes";
            return(0);
        }
        else
        {
            qDebug() << "QCoDeSysNetVarUDP::BroadcastTelegram: Error in broadcasting!";
            qDebug() << "QCoDeSysNetVarUDP::BroadcastTelegram: Message " << tempData.length() << " bytes";
            return(1);
        }


    }
    else
    {
        qDebug() << "QCoDeSysNetVarUDP::BroadcastTelegram: Telegram is not writable";
        return(1);
    }


}

int QCoDeSysNetVarUDP::BroadcastTelegram(unsigned int index)
{
    BroadcastTelegram(telegrams[index]);
}

void QCoDeSysNetVarUDP::readyRead()
{
    // when data comes in
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());

    QHostAddress senderIP;
    quint16 senderPort;

    // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize,
    //                 QHostAddress * address = 0, quint16 * port = 0)
    // Receives a datagram no larger than maxSize bytes and stores it in data.
    // The sender's host address and port is stored in *address and *port
    // (unless the pointers are 0).

    while (socket->readDatagram(buffer.data(), buffer.size(),&senderIP, &senderPort)>0)
    {
        emit receivedUDP(senderIP.toString(),senderPort,buffer.length());

        bool CODESYStelegram=1;
        for (int i=0;i<telegramIdentity.size();i++)
        {
            if (telegramIdentity[i]!=buffer[i])
            {
                CODESYStelegram=0;
                cout << "Wrong character " << i << endl;
                break;
            }
        }

        if(CODESYStelegram==1)
        {
            uint16_t incomingCobID=QCoDeSysNVT::ConvertBytesRead<uint16_t>(buffer.mid(8,2));
            emit receivedCODESYS(senderIP.toString(),senderPort,buffer.length(),incomingCobID);



            if(buffer.size()>20 && telegrams.size()>0)
            {

                for (int i=0;i<telegrams.size();i++)
                {
                    if(telegrams[i]->readCobId()==incomingCobID)
                    {
                        telegrams[i]->ParseTelegram(buffer, senderIP, senderPort);
                        break;
                    }
                }

            }
        }
        buffer.clear();
        buffer.resize(socket->pendingDatagramSize());
    }
}

QCoDeSysNVT::QCoDeSysNVT(bool telegramWritable, QObject *parent)
{
    dataUpdated=0;
    writable = telegramWritable;
    identity.resize(4);
    identity[0]='3';
    identity[1]='S';
    identity[2]='-';
    identity[3]=0;

    id=0;
    cob_id=65535;
    subindex=0;
    items=0;
    length=0;
    counter=0;
    flags=0;
    checksum=0;

}

void QCoDeSysNVT::RecalculateDataPositions()
{
    totalDataLength=0;
    for (int i=0;i<dataVariables.size();i++)
    {
        dataLengths.push_back(VAR_BYTES[dataVariables[i]]);
        dataPositions.push_back(totalDataLength);
        totalDataLength=totalDataLength+VAR_BYTES[dataVariables[i]];
    }
    length=totalDataLength+20;
    items=dataVariables.size();

    InitializeTelegram();

}

void QCoDeSysNVT::ParseTelegram(QByteArray telegram, QHostAddress IPin, u_int16_t portIn)
{
    if(telegram.length()==(totalDataLength+20))
    {
        data=telegram;
        IP=IPin;
        port=portIn;
    }

    dataUpdated=1;
    emit updated();

}

u_int32_t QCoDeSysNVT::readId()
{
    return(ConvertBytesRead<u_int32_t>(data.mid(4,4)));
}

u_int16_t QCoDeSysNVT::readCobId()
{
    return(ConvertBytesRead<u_int16_t>(data.mid(8,2)));
}

u_int16_t QCoDeSysNVT::readSubIndex()
{
    return(ConvertBytesRead<u_int16_t>(data.mid(10,2)));
}

u_int16_t QCoDeSysNVT::readItems()
{
    return(ConvertBytesRead<u_int16_t>(data.mid(12.2)));
}

u_int16_t QCoDeSysNVT::readLength()
{
    return(ConvertBytesRead<u_int16_t>(data.mid(14,2)));
}

u_int16_t QCoDeSysNVT::readCounter()
{
    return(ConvertBytesRead<u_int16_t>(data.mid(16,2)));
}

u_int8_t QCoDeSysNVT::readFlags()
{
    return(ConvertBytesRead<u_int8_t>(data.mid(18,1)));
}

u_int8_t QCoDeSysNVT::readChecksum()
{
    return(ConvertBytesRead<u_int8_t>(data.mid(19,1)));
}

void QCoDeSysNVT::readTelegram(QByteArray &dataOut)
{
    dataOut=data;
}

int QCoDeSysNVT::setIP(QHostAddress IPset)
{
    if (isWritable())
    {
        IP=IPset;
        InitializeTelegram();
        return(0);
    }
    else
    {
        qDebug() << "QCoDeSysNVT::setIP: Telegram is not writable";
        return(1);
    }
}

int QCoDeSysNVT::setPort(u_int16_t portSet)
{
    if (isWritable())
    {
        port=portSet;
        InitializeTelegram();
        return(0);
    }
    else
    {
        qDebug() << "QCoDeSysNVT::setPort: Telegram is not writable";
        return(1);
    }
}

int QCoDeSysNVT::setId(u_int32_t IDset)
{
    if (isWritable())
    {
        id=IDset;
        InitializeTelegram();
        return(0);
    }
    else
    {
        qDebug() << "QCoDeSysNVT::setId: Telegram is not writable";
        return(1);
    }
}

void QCoDeSysNVT::setCobId(u_int16_t cobIDset)
{
        cob_id=cobIDset;
        InitializeTelegram();
}

int QCoDeSysNVT::setSubIndex(u_int16_t subIndexSet)
{
    if (isWritable())
    {
        subindex=subIndexSet;
        InitializeTelegram();
        return(0);
    }
    else
    {
        qDebug() << "QCoDeSysNVT::setSubIndex: Telegram is not writable";
        return(1);
    }

}

int QCoDeSysNVT::setCounter(u_int16_t counterSet)
{
    if (isWritable())
    {
        counter=counterSet;
        InitializeTelegram();
        return(0);
    }
    else
    {
        qDebug() << "QCoDeSysNVT::setCounter: Telegram is not writable";
        return(1);
    }

}

int QCoDeSysNVT::setFlags(u_int8_t flagsSet )
{
    if (isWritable())
    {
        flags=flagsSet;
        InitializeTelegram();
        return(0);
    }
    else
    {
        qDebug() << "QCoDeSysNVT::setFlags: Telegram is not writable";
        return(1);
    }

}

int QCoDeSysNVT::setChecksum(u_int8_t checksumSet)
{
    if (isWritable())
    {
        checksum=checksumSet;
        InitializeTelegram();
        return(0);
    }
    else
    {
        qDebug() << "QCoDeSysNVT::setChecksum: Telegram is not writable";
        return(1);
    }

}

bool QCoDeSysNVT::isWritable()
{
    return(writable);
}

bool QCoDeSysNVT::isUpdated()
{
    if(dataUpdated==1)
    {
        dataUpdated=0;
        return(1);
    }
    return(0);
}

QHostAddress QCoDeSysNVT::readIP()
{
    return(IP);
}

u_int16_t QCoDeSysNVT::readPort()
{
    return(port);
}

int QCoDeSysNVT::StringToVariableList(QString variables)
{
    variables = variables.simplified();
    variables.replace( " ", "" );

    QStringList variableList;
    variableList.clear();
    variableList=variables.split(",");

    dataVariables.clear();
    for(int i=0;i<variableList.length();i++)
    {

        if(variableList[i].compare("BOOL")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_BOOL);
        }
        else if(variableList[i].compare("BYTE")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_BYTE);
        }
        else if(variableList[i].compare("WORD")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_WORD);
        }
        else if(variableList[i].compare("DWORD")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_DWORD);
        }
        else if(variableList[i].compare("SINT")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_SINT);
        }
        else if(variableList[i].compare("USINT")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_USINT);
        }
        else if(variableList[i].compare("INT")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_INT);
        }
        else if(variableList[i].compare("UINT")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_UINT);
        }
        else if(variableList[i].compare("DINT")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_DINT);
        }
        else if(variableList[i].compare("UDINT")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_UDINT);
        }
        else if(variableList[i].compare("REAL")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_REAL);
        }
        else if(variableList[i].compare("LREAL")==0)
        {
            dataVariables.push_back(QCoDeSysNVT::VAR_LREAL);
        }
        else
        {
            dataVariables.clear();
            qDebug() << "QCoDeSysNVT::StringToVariableList: Typo in variable list!";
            return(1);
        }
    }
    RecalculateDataPositions();
    return(0);
}
void QCoDeSysNVT::VariableListToSring(QString& dataString){
    dataString.clear();
    if(dataVariables.size()>0)
    {
        for(int i=0;i<dataVariables.size();i++)
        {
            if(dataVariables[i] == QCoDeSysNVT::VAR_BOOL)
            {
                dataString.append("BOOL");
            }
            else if(dataVariables[i] == QCoDeSysNVT::VAR_BYTE)
            {
                dataString.append("BYTE");
            }
            else if(dataVariables[i] == QCoDeSysNVT::VAR_WORD)
            {
                dataString.append("WORD");
            }
            else if(dataVariables[i] == QCoDeSysNVT::VAR_DWORD)
            {
                dataString.append("DWORD");
            }
            else if(dataVariables[i] == QCoDeSysNVT::VAR_SINT)
            {
                dataString.append("SINT");
            }
            else if(dataVariables[i] == QCoDeSysNVT::VAR_USINT)
            {
                dataString.append("USINT");
            }
            else if(dataVariables[i] == QCoDeSysNVT::VAR_INT)
            {
                dataString.append("INT");
            }
            else if(dataVariables[i] == QCoDeSysNVT::VAR_UINT)
            {
                dataString.append("UINT");
            }
            else if(dataVariables[i] == QCoDeSysNVT::VAR_DINT)
            {
                dataString.append("DINT");
            }
            else if(dataVariables[i] == QCoDeSysNVT::VAR_UDINT)
            {
                dataString.append("UDINT");
            }
            else if(dataVariables[i] == QCoDeSysNVT::VAR_REAL)
            {
                dataString.append("REAL");
            }
            else if(dataVariables[i] == QCoDeSysNVT::VAR_LREAL)
            {
                dataString.append("LREAL");
            }

            if (i<(dataVariables.length()-1))
            {
                dataString.append(", ");
            }
        }
    }

}

void QCoDeSysNVT::DataVarToString(int index,QString& variableString)
{
    variableString.clear();
    if(dataVariables[index]==VAR_BOOL)
    {
        u_int8_t data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_BYTE)
    {
        u_int8_t data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_WORD)
    {
        u_int16_t data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_DWORD)
    {
        u_int32_t data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_SINT)
    {
        int8_t data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_USINT)
    {
        u_int8_t data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_INT)
    {
        int16_t data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_UINT)
    {
        u_int16_t data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_DINT)
    {
        int32_t data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_UDINT)
    {
        u_int32_t data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_REAL)
    {
        float data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_LREAL)
    {
        double data;
        ReadData(index,data);
        variableString.append(QString::number(data));
        SetData(index,data);
    }
}

void QCoDeSysNVT::StringToDataVar(int index,QString variableString)
{
    if(dataVariables[index]==VAR_BOOL)
    {
        u_int8_t data;
        data=variableString.toUShort();
        //quint8 tempVar=variableString.toUShort();
        //QDataStream tempStream(tempVar);
        //tempStream >> data;
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_BYTE)
    {
        u_int8_t data;
        data=variableString.toUShort();
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_WORD)
    {
        u_int16_t data;
        data=variableString.toUInt();
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_DWORD)
    {
        u_int32_t data;
        data=variableString.toULong();
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_SINT)
    {
        int8_t data;
        data=variableString.toUShort();
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_USINT)
    {
        u_int8_t data;
        data=variableString.toUShort();
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_INT)
    {
        int16_t data;
        data=variableString.toInt();
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_UINT)
    {
        u_int16_t data;
        data=variableString.toUInt();
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_DINT)
    {
        int32_t data;
        data=variableString.toLong();
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_UDINT)
    {
        u_int32_t data;
        data=variableString.toULong();
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_REAL)
    {
        float data;
        data=variableString.toFloat();
        SetData(index,data);
    }
    else if(dataVariables[index]==VAR_LREAL)
    {
        double data;
        data=variableString.toDouble();
        SetData(index,data);
    }
}

int QCoDeSysNVT::StringToData(QString dataString){
    dataString= dataString.simplified();
    dataString.replace(" ", "");

    QStringList dataList;
    dataList.clear();
    dataList=dataString.split(",");

    if (dataVariables.length()>0 && dataVariables.length()==dataList.length())
    {
        for(int i=0;i<dataVariables.length();i++)
        {
            StringToDataVar(i,dataList[i]);
        }
    }
    else
    {
        qDebug() << "QCoDeSysNVT::StringToData: Wrong number of variables!";
        return(1);
    }

    return(0);
}

int QCoDeSysNVT::StringListToData(QStringList &dataStrings)
{

    if (dataVariables.length()>0 && dataVariables.length()==dataStrings.length())
    {
        for(int i=0;i<dataVariables.length();i++)
        {
            StringToDataVar(i,dataStrings[i]);
        }
    }
    else
    {
        qDebug() << "QCoDeSysNVT::StringListToData: Wrong number of variables!";
        return(1);
    }

    return(0);
}

void QCoDeSysNVT::DataToString(QString& dataString)
{
    dataString.clear();
    if (dataVariables.length()>0)
    {
        for(int i=0;i<dataVariables.length();i++)
        {
            QString tempString;
            DataVarToString(i,tempString);
            dataString.append(tempString);

            if(i<(dataVariables.length()-1))
            {
                dataString.append(", ");
            }


        }

    }
}

void QCoDeSysNVT::DataToStringList(QStringList &dataStrings)
{
    dataStrings.clear();
    if (dataVariables.length()>0)
    {
        for(int i=0;i<dataVariables.length();i++)
        {
            QString tempString;
            DataVarToString(i,tempString);
            dataStrings.push_back(tempString);
        }

    }
}

void QCoDeSysNVT::InitializeTelegram(void)
{
        data.resize(length);

        QByteArray buffer;
        QDataStream bufferStream(identity);
        u_int32_t identityInt;
        bufferStream >> identityInt;

        ConvertBytesSet(identityInt,buffer);
        data.replace(0,4,buffer);

        buffer.clear();
        ConvertBytesSet(id,buffer);
        data.replace(4,4,buffer);

        buffer.clear();
        ConvertBytesSet(cob_id,buffer);
        data.replace(8,2,buffer);

        buffer.clear();
        ConvertBytesSet(subindex,buffer);
        data.replace(10,2,buffer);

        buffer.clear();
        ConvertBytesSet(items,buffer);
        data.replace(12,2,buffer);

        buffer.clear();
        ConvertBytesSet(length,buffer);
        data.replace(14,2,buffer);

        buffer.clear();
        ConvertBytesSet(counter,buffer);
        data.replace(16,2,buffer);

        buffer.clear();
        ConvertBytesSet(flags,buffer);
        data.replace(18,1,buffer);

        buffer.clear();
        ConvertBytesSet(checksum,buffer);
        data.replace(19,1,buffer);


}
