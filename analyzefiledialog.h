#ifndef ANALYZEFILEDIALOG_H
#define ANALYZEFILEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class AnalyzeFileDialog;
}

class AnalyzeFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnalyzeFileDialog(QWidget *parent = 0);
    ~AnalyzeFileDialog();

    QString getAccFileName();
    QString getGyroFileName();
    QString getMagFileName();
    QString getPressureFileName();
    QString getTempFileName();
private slots:
    void on_accButton_clicked();

    void on_gyroButton_clicked();

    void okClicked();

    void on_magButton_clicked();

    void on_pressureButton_clicked();

    void on_tempButton_clicked();

private:
    Ui::AnalyzeFileDialog *ui;
};

#endif // ANALYZEFILEDIALOG_H
