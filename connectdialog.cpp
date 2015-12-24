#include "connectdialog.h"
#include "ui_connectdialog.h"

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog)
{
    ui->setupUi(this);

    // Setup
    layout()->setSizeConstraint( QLayout::SetFixedSize );
}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}
void ConnectDialog::setSensor(SerialSensorManager *value)
{
    sensor = value;

    ui->portNameBox->addItems( sensor->getPortNames() );
}

QString ConnectDialog::getSelectedName()
{
    return ui->portNameBox->currentText();
}

void ConnectDialog::okClicked()
{
    if ( ui->portNameBox->currentIndex() < 0 || getSelectedName() == "" ) {
        QMessageBox::information( this, tr( "Information" ), tr( "Please select a port" ) );

        return;
    }

    accept();
}
