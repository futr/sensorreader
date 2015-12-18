#ifndef GYRODATAFILTER_H
#define GYRODATAFILTER_H

#include "abstractdatafilter.h"
#include <QTextStream>
#include <QtEndian>

/*
 * 250dps : 131 LSB/dps 0.007633587
 * 500    : 65.5        0.015267175
 * 1000   : 32.8        0.030487804
 * 2000   : 16.4        0.060975709
 */

class GyroDataFilter : public AbstractDataFilter
{
    Q_OBJECT
public:
    explicit GyroDataFilter(QObject *parent = 0);

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

#endif // GYRODATAFILTER_H
