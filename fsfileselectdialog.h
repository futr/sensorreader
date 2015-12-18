#ifndef FSFILESELECTDIALOG_H
#define FSFILESELECTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "micomfs.h"

namespace Ui {
class FSFileSelectDialog;
}

class FSFileSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FSFileSelectDialog(QWidget *parent = 0);
    ~FSFileSelectDialog();

    MicomFSFile *getSelectedFile();

public slots:
    void setFileList(MicomFSFile *value, int fileCount);

private slots:
    void on_openButton_clicked();

private:
    Ui::FSFileSelectDialog *ui;

    MicomFS *fs;
    MicomFSFile *fileList;
    MicomFSFile *selectedFile;
};

#endif // FSFILESELECTDIALOG_H
