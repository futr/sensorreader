#ifndef OPENFILEDIALOG_H
#define OPENFILEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class OpenFileDialog;
}

class OpenFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenFileDialog(QWidget *parent = 0);
    ~OpenFileDialog();

    QString getAccFileName();
    QString getGyroFileName();
    QString getMagFileName();
    QString getPressureFileName();
    QString getTempFileName();
    QString getAnalyzedFileName();

    QString getDirName();
private slots:
    void on_accButton_clicked();

    void on_gyroButton_clicked();

    void okClicked();

    void on_magButton_clicked();

    void on_pressureButton_clicked();

    void on_tempButton_clicked();
    void on_analyzedButton_clicked();

private:
    Ui::OpenFileDialog *ui;
};

#endif // OpenFileDialog_H
