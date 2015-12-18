#ifndef ABSTRACTDATAFILTER_H
#define ABSTRACTDATAFILTER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QVector>
#include "device_id.h"

class AbstractDataFilter : public QObject
{
    Q_OBJECT
public:
    explicit AbstractDataFilter(QObject *parent = 0);
    ~AbstractDataFilter();

    QString getFileName();
    void setFileName( QString name );
    virtual bool parseAndSave( unsigned int step, QByteArray data ) = 0;   // 定義しなければならない関数
    virtual QVector<double> parseToVector( unsigned int step, QByteArray data );
    bool openFile( QString path );
    void closeFile();
    QString getFilterName();
    int getFilterID();

private:
    virtual void makeHeader();

protected:
    QString m_fileName;     // コンストラクターで設定する必要がある
    QString m_filterName;   // コンストラクターで設定する必要がある
    int m_filterID;         // コンストラクターで設定する必要がある
    QFile m_file;

signals:

public slots:

};

#endif // ABSTRACTDATAFILTER_H
