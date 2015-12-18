#include "magdatafilter.h"

MagDataFilter::MagDataFilter(QObject *parent) :
    AbstractDataFilter(parent)
{
    m_fileName   = "mag.csv";
    m_filterName = "MagFilter";
    m_filterID   = ID_AK8975;

    m_resolution = 0.3;
}

bool MagDataFilter::parseAndSave(unsigned int step, QByteArray data)
{
    // データー処理
    QTextStream out( &m_file );
    QVector<double> v = parseToVector( step, data );

    out << (unsigned int)v[0] << ","
        << QString().sprintf( "%.6e", v[1] ) << ","
        << QString().sprintf( "%.6e", v[2] ) << ","
        << QString().sprintf( "%.6e", v[3] ) << "\r\n";

    return true;
}

QVector<double> MagDataFilter::parseToVector(unsigned int step, QByteArray data)
{
    // Parse to vector
    qint16 x, y, z;
    QVector<double> ret;

    x = qFromLittleEndian<qint16>( (uchar *)data.data() );
    y = qFromLittleEndian<qint16>( (uchar *)data.data() + 2 );
    z = qFromLittleEndian<qint16>( (uchar *)data.data() + 4 );

    ret << step <<
           (double)x * m_resolution <<
           (double)y * m_resolution <<
           (double)z * m_resolution;

    return ret;
}

void MagDataFilter::setResolution(double resolution)
{
    // 解像度を設定
    m_resolution = resolution;
}

void MagDataFilter::makeHeader()
{
    // ヘッダ作成
    QTextStream out( &m_file );

    out << "time[100us],x[uT],y[uT],z[uT]\r\n";
}
