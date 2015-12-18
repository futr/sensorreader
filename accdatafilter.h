#ifndef ACCDATAFILTER_H
#define ACCDATAFILTER_H

#include "abstractdatafilter.h"
#include <QTextStream>
#include <QtEndian>

/*
 * 2g  : 16384 LSB/mg 0.061035156
 * 4g  : 8192         0.122070312
 * 8g  : 4096         0.244140625
 * 16g : 2048         0.48828125
 */

class AccDataFilter : public AbstractDataFilter
{
    Q_OBJECT
public:
    explicit AccDataFilter(QObject *parent = 0);

    bool parseAndSave(unsigned int step, QByteArray data);
    virtual QVector<double> parseToVector(unsigned int step, QByteArray data);
    void setResolution( double resolution );

private:
    void makeHeader();

private:
    double m_resolution;

signals:

public slots:
};

#endif // ACCDATAFILTER_H
