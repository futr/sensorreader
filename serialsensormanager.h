#ifndef SERIALSENSORMANAGER_H
#define SERIALSENSORMANAGER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QVector>
#include <QList>
#include <QStringList>
#include <QtEndian>
#include <abstractdatafilter.h>
#include <accdatafilter.h>
#include <gyrodatafilter.h>
#include <magdatafilter.h>
#include <pressdatafilter.h>
#include <tempdatafilter.h>
#include <QMap>
#include <QDebug>
#include "device_id.h"
#include "sensorparameters.h"

class SerialSensorManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialSensorManager(QObject *parent = 0);
    ~SerialSensorManager();

    QStringList getPortNames();

    qint32 getBaudRate() const;
    void setBaudRate(const qint32 &value);

    QString getSensorName() const;
    void setSensorName(const QString &value);

    bool getHandshaked() const;

    SensorParameters getParameters() const;

    bool isOpen();

private:
    enum RecvStatus {
        WaitHead,
        HeadReading,
        DataReading,
    };

    enum Sizes {
        HeadSize = 6,
    };

signals:
    void newAcceleration( QVector<double> );
    void newMagneticField( QVector<double> );
    void newAngularVelocity( QVector<double> );
    void newPressure( QVector<double> );
    void newTemperature( QVector<double> );
    void handshakeACK();

public slots:
    bool openPort( QString portName );
    void closePort();
    void resetCommunication();

    void sendStartTransmit();
    void sendStopTransmit();
    void sendHandshake();
    void flush();

private slots:
    void readData();

private:
    void deleteFilters();

private:
    qint32 baudRate;
    QSerialPort port;
    bool handshaked;

    RecvStatus status;

    QString sensorName;
    SensorParameters parameters;
    SensorParameters::Parameter param;
    QMap<int, QList<AbstractDataFilter *> > filterMap;

    int devID;
    int dataSize;
    quint32 step;
};

#endif // SERIALSENSORMANAGER_H
