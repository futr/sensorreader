#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::setProgressPos(int pos, int max, int bytesPerSec, QString fileName , bool disableBPS)
{
    // Set progress bar position and label caption
    ui->progressBar->setMaximum( max );
    ui->progressBar->setValue( pos );

    QString label;

    if ( !disableBPS ) {
        label = QString( "%1\n%2 / %3\n%4[MB/s]" ).arg( fileName ).arg( pos ).arg( max ).arg( bytesPerSec / 1024.0 / 1024.0 );
    } else {
        label = QString( "%1\n" ).arg( fileName );
    }

    setLabelCaption( label );
}

void ProgressDialog::setProgressMax(int max)
{
    ui->progressBar->setMaximum( max );
}

void ProgressDialog::setLabelCaption( QString &caption )
{
    // Set label caption
    ui->label->setText( caption );
}
