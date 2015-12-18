#ifndef PRESSDATAFILTER_H
#define PRESSDATAFILTER_H

#include "abstractdatafilter.h"
#include <QTextStream>
#include <QtEndian>

class PressDataFilter : public AbstractDataFilter
{
    Q_OBJECT
public:
    explicit PressDataFilter(QObject *parent = 0);

    bool parseAndSave(unsigned int step, QByteArray data);
    virtual QVector<double> parseToVector(unsigned int step, QByteArray data);

private:
    void makeHeader();

signals:

public slots:

};

#endif // PRESSDATAFILTER_H
