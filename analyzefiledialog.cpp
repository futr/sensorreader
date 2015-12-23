#include "analyzefiledialog.h"
#include "ui_analyzefiledialog.h"

AnalyzeFileDialog::AnalyzeFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnalyzeFileDialog)
{
    ui->setupUi(this);
}

AnalyzeFileDialog::~AnalyzeFileDialog()
{
    delete ui;
}

QString AnalyzeFileDialog::getAccFileName()
{
    return ui->accFileEdit->text();
}

QString AnalyzeFileDialog::getGyroFileName()
{
    return ui->gyroFileEdit->text();
}

QString AnalyzeFileDialog::getMagFileName()
{
    return ui->magFileEdit->text();
}

QString AnalyzeFileDialog::getPressureFileName()
{
    return ui->pressureFileEdit->text();
}

QString AnalyzeFileDialog::getTempFileName()
{
    return ui->tempFileEdit->text();
}

void AnalyzeFileDialog::on_accButton_clicked()
{
    auto str = QFileDialog::getOpenFileName( this, tr( "Select a Acceleration log file" ) );

    if ( str != "" ) {
        ui->accFileEdit->setText( str );
    }
}

void AnalyzeFileDialog::on_gyroButton_clicked()
{
    auto str = QFileDialog::getOpenFileName( this, tr( "Select a Angular velocity log file" ) );

    if ( str != "" ) {
        ui->gyroFileEdit->setText( str );
    }
}

void AnalyzeFileDialog::okClicked()
{
    if ( ui->accFileEdit->text() == "" || ui->gyroFileEdit->text() == "" ) {
        QMessageBox::information( this, tr( "Information" ), tr( "Please select log files" ) );

        return;
    }

    accept();
}

void AnalyzeFileDialog::on_magButton_clicked()
{
    auto str = QFileDialog::getOpenFileName( this, tr( "Select a Magnetic field log file" ) );

    if ( str != "" ) {
        ui->magFileEdit->setText( str );
    }
}

void AnalyzeFileDialog::on_pressureButton_clicked()
{
    auto str = QFileDialog::getOpenFileName( this, tr( "Select a Pressure log file" ) );

    if ( str != "" ) {
        ui->pressureFileEdit->setText( str );
    }
}

void AnalyzeFileDialog::on_tempButton_clicked()
{
    auto str = QFileDialog::getOpenFileName( this, tr( "Select a Temperature log file" ) );

    if ( str != "" ) {
        ui->tempFileEdit->setText( str );
    }
}
