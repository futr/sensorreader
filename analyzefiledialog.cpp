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
    ui->accFileEdit->setText( QFileDialog::getOpenFileName( this, tr( "Select a Acceleration log file" ) ) );
}

void AnalyzeFileDialog::on_gyroButton_clicked()
{
    ui->gyroFileEdit->setText( QFileDialog::getOpenFileName( this, tr( "Select a Angular velocity log file" ) ) );
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
    ui->magFileEdit->setText( QFileDialog::getOpenFileName( this, tr( "Select a Magnetic field log file" ) ) );
}

void AnalyzeFileDialog::on_pressureButton_clicked()
{
    ui->pressureFileEdit->setText( QFileDialog::getOpenFileName( this, tr( "Select a Pressure log file" ) ) );
}

void AnalyzeFileDialog::on_tempButton_clicked()
{
    ui->tempFileEdit->setText( QFileDialog::getOpenFileName( this, tr( "Select a Temperature log file" ) ) );
}
