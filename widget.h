#ifndef WIDGET_H
#define WIDGET_H

#include <QTranslator>
#include <QLibraryInfo>

#include <QWidget>
#include <QStatusBar>
#include "serialsensormanager.h"
#include "connectdialog.h"
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QFileDialog>
#include <QFileInfo>
#include <QList>
#include <QVector>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QVector2D>
#include <QtMath>
#include "logicaldrivedialog.h"
#include "progressdialog.h"
#include "fsfileselectdialog.h"
#include "writefileworker.h"
#include "csvwritefileworker.h"
#include "wavegraphwidget.h"
#include "micomfs.h"
#include "tabledatareader.h"
#include "dataselectdialog.h"
#include "analyzefiledialog.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    int getGraphQueueSize() const;
    void setGraphQueueSize(int value);

private slots:
    void on_quitButton_clicked();

    void on_connectButton_clicked();

    void on_disconnectButton_clicked();

    void on_startReadButton_clicked();

    void on_stopReadButton_clicked();

    void on_resetReadButton_clicked();

    void enableSerialButtons( bool enable = true );
    void clearGraph();

    void enableAnalyzedGraph( bool enable = true );

    void on_readCardButton_clicked();

    void setXScale( int scale );
    void setXGrid( int width );
    void headUpdated( double rawX );
    void setDefaultUpdateHead( bool val );

    void on_analyzeFileButton_clicked();

private:
    QString saveLogFile( QString dirName );

    void createWaveList();
    void setupDefaultWaveParams();

    void showAnalyzedLogFiles(QString accFileName, QString gyroFileName, QString magFileName, QString pressureFileName, QString tempFileName, QString analyzedFileName , double rawXUnit, double analyzedXUnit);

    // For analysis
    bool analyzeLog(QString dirName, QString accFileName, QString gyroFileName, QString analyzedFileName, double xUnit);

    QVector3D rowToVec3D( QVector< double > row );
    QVector<double> getColumnVector( const QList<QVector3D> &list, int column, int startRow, int count );
    QVector<double> getColumnVector( const QVector<double> &vector, int startRow, int count );
    double vectorLength( const QVector<double> &vector );
    QVector<QVector3D> groundAxisFromG(QVector3D g );
    double calcArea( QVector2D p1, QVector2D p2 );

private:
    Ui::Widget *ui;
    SerialSensorManager *sensor;
    QStatusBar *bar;

    QList<WaveGraphWidget *>waveList;

    int graphQueueSize;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *);
};

#endif // WIDGET_H
