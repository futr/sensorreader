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
