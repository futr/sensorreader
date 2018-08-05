#include "accdatafilter.h"

AccDataFilter::AccDataFilter(QObject *parent) :
    AbstractDataFilter(parent)
{
    m_fileName   = "acc.csv";
    m_filterName = "AccFilter";
    m_filterID   = ID_MPU9150_ACC;

    m_resolution = 0.122070312;
}

bool AccDataFilter::parseAndSave(unsigned int step, QByteArray data)
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

QVector<double> AccDataFilter::parseToVector(unsigned int step, QByteArray data)
{
    // Parse to vector
    QVector<double> ret;
    qint16 x, y, z;

    x = qFromLittleEndian<qint16>( (uchar *)data.data() );
    y = qFromLittleEndian<qint16>( (uchar *)data.data() + 2 );
    z = qFromLittleEndian<qint16>( (uchar *)data.data() + 4 );

    ret << step <<
           (double)x * m_resolution / 1000 <<
           (double)y * m_resolution / 1000 <<
           (double)z * m_resolution / 1000;

    return ret;
}

void AccDataFilter::setResolution(double resolution)
{
    m_resolution = resolution;
}

void AccDataFilter::makeHeader()
{
    // ヘッダ作成
    QTextStream out( &m_file );

    out << "time[100us],x[G],y[G],z[G]" << '\n';
}
