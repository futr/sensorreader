#ifndef ANALYZEWORKER_H
#define ANALYZEWORKER_H

#include <QObject>

#include <QFile>
#include <QElapsedTimer>
#include <QTextStream>
#include <QList>
#include <QVector>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QVector2D>
#include <QtMath>
#include "tabledatareader.h"

class AnalyzeWorker : public QObject
{
    Q_OBJECT
public:
    explicit AnalyzeWorker(QObject *parent = 0);

    bool error();
    int getProgress();
    bool isRunning();

signals:
    void errorOccurred();
    void finished();
    void progress( int pos, int max, int bytesPerSec, QString name );
    void progress( int pos, int max, int bytesPerSec, QString name, bool disableBPS );

public slots:
    void setParameter(TableDataReader *accReader, TableDataReader *gyroReader, TableDataReader *magReader, TableDataReader *pressureReader, TableDataReader *tempReader,
                       QFile *analyzedFile, bool useMag, bool usePressure, bool useTemp,
                       double xUnit,
                       int zeroSampleCount,
                       quint32 zeroStartTime,
                       quint32 zeroEndTime,
                       quint32 endTime,
                       double gVal );
    void setup();
    void doWork();
    void stopWork();

private:    // そのうち基底クラスのほうに移動する
    int bytesPerSec;
    int beforePos;
    int interval;
    bool stopFlag;
    bool m_error;
    int m_progress;
    bool m_running;

private:
    TableDataReader *accReader;
    TableDataReader *gyroReader;
    TableDataReader *magReader;
    TableDataReader *pressureReader;
    TableDataReader *tempReader;
    QFile *analyzedFile;

    int zeroSampleCount;
    quint32 zeroStartTime;
    quint32 zeroEndTime;
    quint32 endTime;
    double gVal;
    double xUnit;

    bool useMag, usePressure, useTemp;

private:
    QVector3D rowToVec3D( QVector< double > row );
    QVector<double> getColumnVector( const QList<QVector3D> &list, int column, int startRow, int count );
    QVector<double> getColumnVector( const QVector<double> &vector, int startRow, int count );
    double vectorLength( const QVector<double> &vector );
    QVector<QVector3D> groundAxisFromG(QVector3D g );
    double calcArea( QVector2D p1, QVector2D p2 );
};

#endif // ANALYZEWORKER_H
