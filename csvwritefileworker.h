#ifndef CSVWRITEFILEWORKER_H
#define CSVWRITEFILEWORKER_H

#include <QObject>
#include <QFile>
#include <QTimer>
#include "device_id.h"
#include "abstractdatafilter.h"
#include <time.h>
#include <QMap>
#include <QtEndian>
#include <QElapsedTimer>

class CSVWriteFileWorker : public QObject
{
    Q_OBJECT
public:
    explicit CSVWriteFileWorker(QObject *parent = 0);

private:
    QFile *m_logFile;
    QMap<int, QList<AbstractDataFilter *> > m_filterMap;
    bool stopFlag;
    bool m_error;
    qint64 m_progress;
    bool m_running;


public:
    bool error( void );
    qint64 getProgress( void );
    bool isRunning( void );

signals:
    void errorOccurred( void );
    void finished( void );
    void progress( int pos, int max, int bytesPerSec, QString name );

public slots:
    void setParameter( QFile *logFile, QMap<int, QList<AbstractDataFilter *> > filterMap );
    void setup();
    void doSaveFile( void );
    void stopSave( void );

};

#endif // CSVWriteFileWorker_H
