#ifndef QCODESYSNETVARUDP_H
#define QCODESYSNETVARUDP_H

#include <QList>
#include <QVector>
#include <vector>
#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <type_traits>
#include <QDataStream>

#if defined(_WIN32)
#include <stdint.h>
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;
#endif


using namespace std;

class QCoDeSysNetVarUDP;
class QCoDeSysNVT;

class QCoDeSysNetVarUDP : public QObject
{
    Q_OBJECT


public:

    explicit QCoDeSysNetVarUDP(QHostAddress IP,quint16 localPort,QObject *parent = 0);
    QList <QCoDeSysNVT*> telegrams;

    int BroadcastTelegram(QCoDeSysNVT* telegram);
    int BroadcastTelegram(unsigned int index);

signals:    
    void receivedUDP(QString IP, unsigned int port, unsigned int bytes);
    void receivedCODESYS(QString IP, unsigned int port, unsigned int bytes, unsigned int cobid);
public slots:
    void readyRead();
private:
    QByteArray telegramIdentity;
    QUdpSocket *socket;
    QHostAddress IP;
    quint16 port;
};




class QCoDeSysNVT : public QObject
{
    Q_OBJECT
public:

    static const int VAR_BOOL;
    static const int VAR_BYTE;
    static const int VAR_WORD;
    static const int VAR_DWORD;
    static const int VAR_SINT;
    static const int VAR_USINT;
    static const int VAR_INT;
    static const int VAR_UINT;
    static const int VAR_DINT;
    static const int VAR_UDINT;
    static const int VAR_REAL;
    static const int VAR_LREAL;

    QList<int> dataVariables;

    explicit QCoDeSysNVT(bool telegramWritable = 0, QObject *parent = 0);
    void RecalculateDataPositions();
    void InitializeTelegram(void);

    int StringToVariableList(QString variables);
    void VariableListToSring(QString& dataString);

    int StringToData(QString dataString);
    void DataToString(QString& dataString);

    int StringListToData(QStringList& dataStrings);
    void DataToStringList(QStringList& dataStrings);

    void ParseTelegram(QByteArray telegram, QHostAddress IP, u_int16_t port);



    QHostAddress readIP(void);
    u_int16_t readPort(void);
    QString readIdentity(void);
    u_int32_t readId(void);
    u_int16_t readCobId(void);
    u_int16_t readSubIndex(void);
    u_int16_t readItems(void);
    u_int16_t readLength(void);
    u_int16_t readCounter(void);
    u_int8_t readFlags(void);
    u_int8_t readChecksum(void);
    void readTelegram(QByteArray& data);


    int setIP(QHostAddress IP);
    int setPort(u_int16_t port);
    int setId(u_int32_t ID);
    void setCobId(u_int16_t cobID);
    int setSubIndex(u_int16_t subIndex);
    int setCounter(u_int16_t counter);
    int setFlags(u_int8_t flags);
    int setChecksum(u_int8_t checksum);



    bool isWritable(void);
    bool isUpdated(void);

    template <typename Type>
    void ReadData(int index, Type &variable);

    template <typename Type>
    static Type ConvertBytesRead(QByteArray input);

    template <typename Type>
    void SetData(int index, Type variable);

    template <typename Type>
    void ConvertBytesSet(Type value, QByteArray &buffer);


private:
    QHostAddress IP;
    u_int16_t port;
    QByteArray identity;
    u_int32_t id;
    u_int16_t cob_id;
    u_int16_t subindex;
    u_int16_t items;
    u_int16_t length;
    u_int16_t counter;
    u_int8_t flags;
    u_int8_t checksum;

    u_int8_t totalDataLength;

    static const int VAR_BYTES[];
    QList<int> dataPositions;
    QList<int> dataLengths;

    bool writable;

    bool dataUpdated;

    QByteArray data;

    void DataVarToString(int index,QString& variableString);
    void StringToDataVar(int index,QString variableString);


signals:
    void updated(void);
};

template <typename Type>
Type QCoDeSysNVT::ConvertBytesRead(QByteArray input) {
    QDataStream temp(input);
    temp.setByteOrder(QDataStream::LittleEndian);

    if (is_same<float,Type>::value)
    {
        temp.setFloatingPointPrecision(QDataStream::SinglePrecision);
    }
    else if(is_same<double,Type>::value)
    {
        temp.setFloatingPointPrecision(QDataStream::DoublePrecision);
    }
    Type out;
    temp >> out;
    return(out);
}
//    void ConvertBytesSet(Type value, QByteArray &buffer);
template <typename Type>
void QCoDeSysNVT::ConvertBytesSet(Type value, QByteArray &buffer) {
    QDataStream temp(&buffer, QIODevice::WriteOnly);
    temp.setByteOrder(QDataStream::LittleEndian);

    if (is_same<float,Type>::value)
    {
        temp.setFloatingPointPrecision(QDataStream::SinglePrecision);
    }
    else if(is_same<double,Type>::value)
    {
        temp.setFloatingPointPrecision(QDataStream::DoublePrecision);
    }
    temp << value;
}


template <typename Type>
void QCoDeSysNVT::ReadData(int index, Type &variable){
    variable=ConvertBytesRead<Type>(data.mid((20+dataPositions[index]),dataLengths[index]));
}
template <typename Type>
void QCoDeSysNVT::SetData(int index, Type variable){
    QByteArray buffer;
    ConvertBytesSet(variable,buffer);

    data.replace((20+dataPositions[index]),dataLengths[index],buffer);
}



#endif // QCODESYSNETVARUDP_H
