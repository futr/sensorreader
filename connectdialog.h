#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "serialsensormanager.h"

namespace Ui {
class ConnectDialog;
}

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectDialog(QWidget *parent = 0);
    ~ConnectDialog();

    void setSensor(SerialSensorManager *value);
    QString getSelectedName();

private slots:
    void okClicked();

private:
    Ui::ConnectDialog *ui;
    SerialSensorManager *sensor;
};

#endif // CONNECTDIALOG_H
