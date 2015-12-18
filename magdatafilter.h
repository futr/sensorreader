#ifndef MAGDATAFILTER_H
#define MAGDATAFILTER_H

#include "abstractdatafilter.h"
#include <QTextStream>
#include <QtEndian>


class MagDataFilter : public AbstractDataFilter
{
    Q_OBJECT
public:
    explicit MagDataFilter(QObject *parent = 0);

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

#endif // MAGDATAFILTER_H
