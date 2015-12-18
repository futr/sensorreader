#ifndef TEMPDATAFILTER_H
#define TEMPDATAFILTER_H

#include "abstractdatafilter.h"
#include <QTextStream>
#include <QtEndian>

class TempDataFilter : public AbstractDataFilter
{
    Q_OBJECT
public:
    explicit TempDataFilter(QObject *parent = 0);

    bool parseAndSave(unsigned int step, QByteArray data);
    virtual QVector<double> parseToVector(unsigned int step, QByteArray data);

private:
    void makeHeader();

signals:

public slots:

};

#endif // TEMPDATAFILTER_H
