#include "openfiledialog.h"
#include "ui_openfiledialog.h"

OpenFileDialog::OpenFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenFileDialog)
{
    ui->setupUi(this);
}

OpenFileDialog::~OpenFileDialog()
{
    delete ui;
}

QString OpenFileDialog::getAccFileName()
{
    return ui->accFileEdit->text();
}

QString OpenFileDialog::getGyroFileName()
{
    return ui->gyroFileEdit->text();
}

QString OpenFileDialog::getMagFileName()
{
    return ui->magFileEdit->text();
}

QString OpenFileDialog::getPressureFileName()
{
    return ui->pressureFileEdit->text();
}

QString OpenFileDialog::getTempFileName()
{
    return ui->tempFileEdit->text();
}

QString OpenFileDialog::getAnalyzedFileName()
{
    return ui->analyzedFileEdit->text();
}

QString OpenFileDialog::getDirName()
{
    if ( getAccFileName() != "" ) return QFileInfo( getAccFileName() ).canonicalPath();
    if ( getGyroFileName() != "" ) return QFileInfo( getGyroFileName() ).canonicalPath();
    if ( getMagFileName() != "" ) return QFileInfo( getMagFileName() ).canonicalPath();
    if ( getPressureFileName() != "" ) return QFileInfo( getPressureFileName() ).canonicalPath();
    if ( getTempFileName() != "" ) return QFileInfo( getTempFileName() ).canonicalPath();
    if ( getAnalyzedFileName() != "" ) return QFileInfo( getAnalyzedFileName() ).canonicalPath();

    return "";
}

void OpenFileDialog::on_accButton_clicked()
{
    auto str = QFileDialog::getOpenFileName( this, tr( "Select a Acceleration log file" ) );

    if ( str != "" ) {
        ui->accFileEdit->setText( str );
    }
}

void OpenFileDialog::on_gyroButton_clicked()
{
    auto str = QFileDialog::getOpenFileName( this, tr( "Select a Angular velocity log file" ) );

    if ( str != "" ) {
        ui->gyroFileEdit->setText( str );
    }
}

void OpenFileDialog::okClicked()
{
    if ( getDirName() == "" ) {
        QMessageBox::information( this, tr( "Information" ), tr( "Please select log files" ) );

        return;
    }

    accept();
}

void OpenFileDialog::on_magButton_clicked()
{
    auto str = QFileDialog::getOpenFileName( this, tr( "Select a Magnetic field log file" ) );

    if ( str != "" ) {
        ui->magFileEdit->setText( str );
    }
}

void OpenFileDialog::on_pressureButton_clicked()
{
    auto str = QFileDialog::getOpenFileName( this, tr( "Select a Pressure log file" ) );

    if ( str != "" ) {
        ui->pressureFileEdit->setText( str );
    }
}

void OpenFileDialog::on_tempButton_clicked()
{
    auto str = QFileDialog::getOpenFileName( this, tr( "Select a Temperature log file" ) );

    if ( str != "" ) {
        ui->tempFileEdit->setText( str );
    }
}

void OpenFileDialog::on_analyzedButton_clicked()
{
    auto str = QFileDialog::getOpenFileName( this, tr( "Select a Analyzed data log file" ) );

    if ( str != "" ) {
        ui->analyzedFileEdit->setText( str );
    }
}
