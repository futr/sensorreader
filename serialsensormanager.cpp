#include "serialsensormanager.h"

SerialSensorManager::SerialSensorManager(QObject *parent) : QObject(parent),
    baudRate( 9600 )
{
    // Register types for SIGNAL/SLOT communication
    // qRegisterMetaType<QVector<double>>("QVector<double>");

    connect( &port, SIGNAL(readyRead()), this, SLOT(readData()), Qt::QueuedConnection );
}

SerialSensorManager::~SerialSensorManager()
{
    closePort();
    deleteFilters();
}

QStringList SerialSensorManager::getPortNames()
{
    // Get serial port name list
    auto list = QSerialPortInfo::availablePorts();
    QStringList ret;

    for ( auto &elem : list ) {
        ret << elem.portName();
    }

    return ret;
}

bool SerialSensorManager::openPort( QString portName )
{
    // Open serial port
    QSerialPortInfo portInfo( portName );

    // Setting
    port.setPort( portInfo );
    port.setBaudRate( baudRate );

    // Reset
    resetCommunication();
    handshaked = false;

    // Open
    return port.open( QIODevice::ReadWrite );
}

void SerialSensorManager::closePort()
{
    port.close();
}

void SerialSensorManager::resetCommunication()
{
    status = WaitHead;
}

void SerialSensorManager::sendStartTransmit()
{
    // Start
    char data = TRANSMIT_START;

    handshaked = false;

    port.write( &data, 1 );
}

void SerialSensorManager::sendStopTransmit()
{
    // Stop
    char data = TRANSMIT_STOP;

    port.write( &data, 1 );
}

void SerialSensorManager::sendHandshake()
{
    // Handshake
    char data = TRANSMIT_HANDSHAKE;

    port.write( &data, 1 );
}

void SerialSensorManager::flush()
{
    port.flush();
    port.waitForBytesWritten( 1000 );
}

void SerialSensorManager::readData()
{
    // Reading process

    // qDebug() << port.bytesAvailable();

    while ( true ) {
        if ( status == WaitHead ) {
            unsigned char data;

            if ( port.bytesAvailable() >= 1 ) {
                port.read( (char *)&data, 1 );
            } else {
                return;
            }

            if ( data == LOG_SIGNATURE ) {
                // Data start
                status = HeadReading;
            } else if ( data == TRANSMIT_HANDSHAKE_ACK ) {
                // Handshake
                handshaked = true;
                emit handshakeACK();
            }
        } else if ( status == HeadReading ) {
            // Wait all data received
            if ( port.bytesAvailable() < HeadSize ) {
                return;
            }

            // Read header
            QByteArray header = port.read( HeadSize );

            step      = qFromLittleEndian<quint32>( (uchar *)header.data() );
            devID     = (unsigned char)header[4];
            dataSize  = (unsigned char)header[5];

            // Next
            status = DataReading;
            continue;
        } else if ( status == DataReading ) {
            // Wait all data
            if ( port.bytesAvailable() < dataSize ) {
                return;
            }

            // Read data
            QByteArray data = port.read( dataSize );

            // Select filter
            QList<AbstractDataFilter *> filterList;

            filterList = filterMap[devID];

            // 対応するフィルターがなかった
            if ( 0 == filterList.count() ) {
                // Next data
                status = WaitHead;
                return;
            }

            // Use the first filter
            AbstractDataFilter *filter = filterList[0];
            QVector<double> v = filter->parseToVector( step, data );
            v[0] = v[0] * param.xUnit;

            // Emit recved data
            switch ( devID ) {
            case ID_MPU9150_ACC:
                emit newAcceleration( v );
                break;
            case ID_MPU9150_GYRO:
                emit newAngularVelocity( v );
                break;
            case ID_MPU9150_TEMP:
                emit newTemperature( v );
                break;
            case ID_AK8975:
                emit newMagneticField( v );
                break;
            case ID_LPS331AP:
                emit newPressure( v );
                break;
            default:
                break;
            }

            // Clear fifo if need
            if ( port.bytesAvailable() > 200 ) {
                port.readAll();
            }

            // Next data
            status = WaitHead;
            return;
        }
    }
}

void SerialSensorManager::deleteFilters()
{
    foreach ( QList<AbstractDataFilter *> filterList, filterMap ) {
        foreach ( AbstractDataFilter *filter,  filterList ) {
            delete filter;
        }
    }
}
SensorParameters SerialSensorManager::getParameters() const
{
    return parameters;
}

bool SerialSensorManager::isOpen()
{
    return port.isOpen();
}

bool SerialSensorManager::getHandshaked() const
{
    return handshaked;
}


QString SerialSensorManager::getSensorName() const
{
    return sensorName;
}

void SerialSensorManager::setSensorName(const QString &value)
{
    sensorName = value;

    param = parameters.getParameter( value.toStdString() );

    deleteFilters();

    filterMap[ID_MPU9150_ACC] = QList<AbstractDataFilter *>() << new AccDataFilter();
    filterMap[ID_MPU9150_GYRO] = QList<AbstractDataFilter *>() << new GyroDataFilter();
    filterMap[ID_AK8975] = QList<AbstractDataFilter *>() << new MagDataFilter();
    filterMap[ID_MPU9150_TEMP] = QList<AbstractDataFilter *>() << new TempDataFilter();
    filterMap[ID_LPS331AP] = QList<AbstractDataFilter *>() << new PressDataFilter();

    // 解像度設定
    dynamic_cast<AccDataFilter *>( filterMap[ID_MPU9150_ACC].last() )->setResolution( param.accelerationResolution );
    dynamic_cast<GyroDataFilter *>( filterMap[ID_MPU9150_GYRO].last() )->setResolution( param.angularVelocityResolution );
    dynamic_cast<MagDataFilter *>( filterMap[ID_AK8975].last() )->setResolution( param.magneticFieldResolution );

}


qint32 SerialSensorManager::getBaudRate() const
{
    return baudRate;
}

void SerialSensorManager::setBaudRate(const qint32 &value)
{
    baudRate = value;
}

