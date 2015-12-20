#include "fsfileselectdialog.h"
#include "ui_fsfileselectdialog.h"

FSFileSelectDialog::FSFileSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FSFileSelectDialog)
{
    ui->setupUi(this);

    selectedFile = NULL;
}

FSFileSelectDialog::~FSFileSelectDialog()
{
    delete ui;
}

MicomFSFile *FSFileSelectDialog::getSelectedFile()
{
    return selectedFile;
}
void FSFileSelectDialog::setFileList(MicomFSFile *value, int fileCount )
{
    // Setup file list
    fileList = value;

    ui->fileListWidget->clear();
    ui->fileListWidget->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->fileListWidget->setColumnCount( 2 );
    ui->fileListWidget->setHeaderLabels( QStringList() << tr( "Name" ) << tr( "Size[Bytes]" ) );

    // Show the file list into widget
    for ( int i = 0; i < fileCount; i++ ) {
        QList<QTreeWidgetItem *> items;
        items << new QTreeWidgetItem( QStringList() << fileList[i].name << "" );
        items[0]->setData( 1, Qt::DisplayRole, (quint64)fileList[i].sector_count * 512 );
        items[0]->setData( 2, Qt::UserRole, QVariant::fromValue( (void *)&fileList[i] ) );

        ui->fileListWidget->addTopLevelItems( items );
    }

    // resize
    for ( int i = 0; i < 2; i++ ) {
        ui->fileListWidget->resizeColumnToContents( i );
    }
}

void FSFileSelectDialog::okClicked()
{
    // Open
    QList<QTreeWidgetItem *> items = ui->fileListWidget->selectedItems();

    if ( items.count() == 0 ) {
        QMessageBox::information( this, tr( "Information" ), tr( "File is not selected.\nPlease select a file." ) );

        selectedFile = NULL;

        return;
    }

    selectedFile = (MicomFSFile *)items[0]->data( 2, Qt::UserRole ).value<void *>();

    accept();
}
