#ifndef WIDGET_H
#define WIDGET_H

#include <QTranslator>
#include <QLibraryInfo>

#include <QWidget>
#include <QPicture>
#include <QtSvg/QSvgGenerator>
#include <QtSvg/QSvgRenderer>
#include <QBuffer>
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
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QPdfWriter>
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
#include "analyzeworker.h"
#include "openfiledialog.h"

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
    void setPrintMode( bool enable );

    void enableAnalyzedGraph( bool enable = true );

    void on_readCardButton_clicked();

    void setXScale( int scale );
    void setXGrid( double width );
    void headUpdated( double rawX );
    void setDefaultUpdateHead( bool val );
    void setMinHeight( int height );

    void on_analyzeFileButton_clicked();

    void on_printButton_clicked();

    void setTitleDirName( QString dir );


    void on_showFileButton_clicked();

    void on_pdfButton_clicked();

private:
    QString saveLogFile( QString dirName );

    void createWaveList();
    void setupDefaultWaveParams();

    void showAnalyzedLogFiles(QString dirName, QString accFileName, QString gyroFileName, QString magFileName, QString pressureFileName, QString tempFileName, QString analyzedFileName , double rawXUnit, double analyzedXUnit);

    // For analysis
    bool analyzeLog(QString dirName, QString accFileName, QString gyroFileName, QString magFileName, QString pressureFileName, QString tempFileName, QString analyzedFileName, double xUnit);



private:
    Ui::Widget *ui;
    SerialSensorManager *sensor;
    QStatusBar *bar;

    QList<WaveGraphWidget *>waveList;
    QList<Graph *>graphWidgetList;

    int graphQueueSize;

    double defaultLabelPoint;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *);
};

#endif // WIDGET_H
