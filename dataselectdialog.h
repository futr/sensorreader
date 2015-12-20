#ifndef DATASELECTDIALOG_H
#define DATASELECTDIALOG_H

#include <QDialog>
#include "graph.h"
#include "tabledatareader.h"
#include <QPair>
#include <QMessageBox>

namespace Ui {
class DataSelectDialog;
}

class DataSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataSelectDialog(QWidget *parent = 0);
    ~DataSelectDialog();

    void setTable(TableDataReader *accReader, double xUnit );

    quint32 getZeroStartTime();
    quint32 getZeroEndTime();
    quint32 getEndTime();

    int getZeroCount() const;
    void setZeroCount(int value);

    double getG() const;
    void setG(double value);

private:
    Ui::DataSelectDialog *ui;

    TableDataReader *accReader;
    QPair<int, QVector<double> > cursorValue;
    QPair<int, QVector<double> > rightCursorValue;

    int zeroCount;

    double G;
    double xUnit;
    double endXValue;

private:
    void updateFilter();

private slots:
    void cursorChanged( QPair<int, QVector<double> > data );
    void rightCursorChanged(QPair<int, QVector<double> > data);
    void okClicked();
    void on_zeroCountSpinBox_valueChanged(int arg1);
    void on_gEdit_textChanged(const QString &arg1);
};

#endif // DATASELECTDIALOG_H
