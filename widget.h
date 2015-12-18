#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QStatusBar>
#include "serialsensormanager.h"
#include "connectdialog.h"
#include <QMessageBox>
#include <QThread>
#include <QTimer>
#include <QFileDialog>
#include "logicaldrivedialog.h"
#include "progressdialog.h"
#include "fsfileselectdialog.h"
#include "writefileworker.h"
#include "micomfs.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_quitButton_clicked();

    void on_connectButton_clicked();

    void on_disconnectButton_clicked();

    void on_startReadButton_clicked();

    void on_stopReadButton_clicked();

    void on_resetReadButton_clicked();

    void enableSerialButtons( bool enable = true );
    void clearGraph();

    void on_readCardButton_clicked();

private:
    Ui::Widget *ui;
    SerialSensorManager *sensor;
    QStatusBar *bar;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *);
};

#endif // WIDGET_H
