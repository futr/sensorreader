#include "tempdatafilter.h"

TempDataFilter::TempDataFilter(QObject *parent) :
    AbstractDataFilter(parent)
{
    m_fileName   = "temperature.csv";
    m_filterName = "TempFilter";
    m_filterID   = ID_MPU9150_TEMP;
}

bool TempDataFilter::parseAndSave(unsigned int step, QByteArray data)
{
    // データー処理
    QTextStream out( &m_file );
    QVector<double> v = parseToVector( step, data );

    out << (unsigned int)v[0] << ","
        << QString().sprintf( "%.6e", v[1] ) << "\r\n";

    return true;
}

QVector<double> TempDataFilter::parseToVector(unsigned int step, QByteArray data)
{
    // Parse to vector
    qint16 val;
    QVector<double> ret;

    val = qFromLittleEndian<qint16>( (uchar *)data.data() );

    ret << step << (double)val / 340 + 35;

    return ret;
}

void TempDataFilter::makeHeader()
{
    // ヘッダ作成
    QTextStream out( &m_file );

    out << "time[100us],temperature[C]\r\n";

}
