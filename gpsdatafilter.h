#ifndef GPSDATAFILTER_H
#define GPSDATAFILTER_H

#include <QObject>
#include <QTextStream>
#include "abstractdatafilter.h"

class GPSDataFilter : public AbstractDataFilter
{
    Q_OBJECT
public:
    explicit GPSDataFilter(QObject *parent = 0);

    bool parseAndSave(unsigned int step, QByteArray data);
    QVector<double> parseToVector(unsigned int step, QByteArray data) override;

private:
    void makeHeader();
};

#endif // GPSDATAFILTER_H
