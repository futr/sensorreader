#include "gpsdatafilter.h"

GPSDataFilter::GPSDataFilter(QObject *parent) :
    AbstractDataFilter(parent)
{
    m_fileName   = "gps.nmea";
    m_filterName = "GPSFilter";
    m_filterID   = ID_GPS;
}

bool GPSDataFilter::parseAndSave(unsigned int step, QByteArray data)
{
    // データー処理
    Q_UNUSED(step)
    QTextStream out( &m_file );

    out << data;

    return true;
}

QVector<double> GPSDataFilter::parseToVector(unsigned int step, QByteArray data)
{
    Q_UNUSED(data)
    QVector<double> ret;

    ret << step << 0;

    return ret;
}

void GPSDataFilter::makeHeader()
{
    // ヘッダ作成
}
