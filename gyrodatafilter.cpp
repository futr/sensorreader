#include "gyrodatafilter.h"

GyroDataFilter::GyroDataFilter(QObject *parent) :
    AbstractDataFilter(parent)
{
    m_fileName   = "gyro.csv";
    m_filterName = "GyroFilter";
    m_filterID   = ID_MPU9150_GYRO;

    m_resolution = 0.01526;
}

bool GyroDataFilter::parseAndSave(unsigned int step, QByteArray data)
{
    // データー処理
    QTextStream out( &m_file );
    QVector<double> v = parseToVector( step, data );

    out << (unsigned int)v[0] << ","
        << QString().sprintf( "%.6e", v[1] ) << ","
        << QString().sprintf( "%.6e", v[2] ) << ","
        << QString().sprintf( "%.6e", v[3] ) << '\n';

    return true;
}

QVector<double> GyroDataFilter::parseToVector(unsigned int step, QByteArray data)
{
    // Parse to vector
    QVector<double> ret;
    qint16 x, y, z;

    x = qFromLittleEndian<qint16>( (uchar *)data.data() );
    y = qFromLittleEndian<qint16>( (uchar *)data.data() + 2 );
    z = qFromLittleEndian<qint16>( (uchar *)data.data() + 4 );

    ret << step <<
           (double)x * m_resolution <<
           (double)y * m_resolution <<
           (double)z * m_resolution;

    return ret;
}

void GyroDataFilter::setResolution(double resolution)
{
    // 解像度設定
    m_resolution = resolution;
}

void GyroDataFilter::makeHeader()
{
    // ヘッダ作成
    QTextStream out( &m_file );

    out << "time[100us],x[dps],y[dps],z[dps]" << '\n';
}
