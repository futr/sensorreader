#include "pressdatafilter.h"

PressDataFilter::PressDataFilter(QObject *parent) :
    AbstractDataFilter(parent)
{
    m_fileName   = "pressure.csv";
    m_filterName = "PressFilter";
    m_filterID   = ID_LPS331AP;
}

bool PressDataFilter::parseAndSave(unsigned int step, QByteArray data)
{
    // データー処理
    QTextStream out( &m_file );
    QVector<double> v = parseToVector( step, data );

    out << (unsigned int)v[0] << ","
        << QString().sprintf( "%.6e", v[1] ) << "\r\n";

    return true;
}

QVector<double> PressDataFilter::parseToVector(unsigned int step, QByteArray data)
{
    // Parse to vector
    qint32 val;
    QVector<double> ret;

    val = qFromLittleEndian<qint32>( (uchar *)data.data() );

    ret << step << (double)val / 4096;

    return ret;
}

void PressDataFilter::makeHeader()
{
    // ヘッダ作成
    QTextStream out( &m_file );

    out << "time[100us],pressure[hPa]\r\n";
}
