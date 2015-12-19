#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Create UI
    // bar = new QStatusBar( this );
    // this->layout()->addWidget( bar );

    // Sensor
    sensor = new SerialSensorManager( this );

    // Setup UI
    QStringList sensorNameList;

    for ( auto elem : sensor->getParameters().sensorNameList() ) {
        sensorNameList << QString::fromStdString( elem );
    }

    ui->sensorNameBox->addItems( sensorNameList );

    ui->scrollAreaWidgetContents->layout()->setSpacing( 0 );
    ui->scrollAreaWidgetContents->layout()->setContentsMargins( 0, 0, 0, 0 );

    enableSerialButtons( false );

    // Setup graph
    createWaveList();

    ui->acc->setLabel( tr( "Acceleration" ) );
    ui->acc->wave->setUpSize( 3, 1000 );
    ui->acc->wave->setXScale( 20 );
    ui->acc->wave->setAutoUpdateYMax( true );
    ui->acc->wave->setAutoUpdateYMin( true );
    ui->acc->wave->setAutoZeroCenter( true );
    ui->acc->wave->setXGrid( 5 );
    ui->acc->wave->setLegendFontSize( 12 );
    ui->acc->wave->setDefaultFontSize( 12 );
    ui->acc->wave->setShowCursor( true );
    ui->acc->wave->setXName( "time[sec]" );
    ui->acc->wave->setNames( QStringList() << "x[G]" << "y[G]" << "z[G]" );
    ui->acc->wave->setColors( QList<QColor>() << Qt::red << Qt::green << Qt::blue );

    ui->gyro->setLabel( tr( "Angular Velocity" ) );
    ui->gyro->wave->setUpSize( 3, 1000 );
    ui->gyro->wave->setXScale( 20 );
    ui->gyro->wave->setAutoUpdateYMax( true );
    ui->gyro->wave->setAutoUpdateYMin( true );
    ui->gyro->wave->setAutoZeroCenter( true );
    ui->gyro->wave->setXGrid( 5 );
    ui->gyro->wave->setLegendFontSize( 12 );
    ui->gyro->wave->setDefaultFontSize( 12 );
    ui->gyro->wave->setShowCursor( true );
    ui->gyro->wave->setXName( "time[sec]" );
    ui->gyro->wave->setNames( QStringList() << "x[dps]" << "y[dps]" << "z[dps]" );
    ui->gyro->wave->setColors( QList<QColor>() << Qt::red << Qt::green << Qt::blue );

    ui->mag->setLabel( tr( "Magnetic field" ) );
    ui->mag->wave->setUpSize( 3, 1000 );
    ui->mag->wave->setXScale( 20 );
    ui->mag->wave->setAutoUpdateYMax( true );
    ui->mag->wave->setAutoUpdateYMin( true );
    ui->mag->wave->setAutoZeroCenter( true );
    ui->mag->wave->setXGrid( 5 );
    ui->mag->wave->setLegendFontSize( 12 );
    ui->mag->wave->setDefaultFontSize( 12 );
    ui->mag->wave->setShowCursor( true );
    ui->mag->wave->setXName( "time[sec]" );
    ui->mag->wave->setNames( QStringList() << "x[µT]" << "y[µT]" << "z[µT]" );
    ui->mag->wave->setColors( QList<QColor>() << Qt::red << Qt::green << Qt::blue );

    ui->temp->setLabel( tr( "Temperature" ) );
    ui->temp->wave->setUpSize( 1, 1000 );
    ui->temp->wave->setXScale( 20 );
    ui->temp->wave->setAutoUpdateYMax( true );
    ui->temp->wave->setAutoUpdateYMin( true );
    ui->temp->wave->setXGrid( 5 );
    ui->temp->wave->setLegendFontSize( 12 );
    ui->temp->wave->setDefaultFontSize( 12 );
    ui->temp->wave->setShowCursor( true );
    ui->temp->wave->setXName( "time[sec]" );
    ui->temp->wave->setNames( QStringList() << "x[℃]" );
    ui->temp->wave->setColors( QList<QColor>() << Qt::blue );

    ui->pressure->setLabel( tr( "Pressure" ) );
    ui->pressure->wave->setUpSize( 1, 1000 );
    ui->pressure->wave->setXScale( 20 );
    ui->pressure->wave->setAutoUpdateYMax( true );
    ui->pressure->wave->setAutoUpdateYMin( true );
    ui->pressure->wave->setXGrid( 5 );
    ui->pressure->wave->setLegendFontSize( 12 );
    ui->pressure->wave->setDefaultFontSize( 12 );
    ui->pressure->wave->setShowCursor( true );
    ui->pressure->wave->setXName( "time[sec]" );
    ui->pressure->wave->setNames( QStringList() << "x[hPa]" );
    ui->pressure->wave->setColors( QList<QColor>() << Qt::blue );

    ui->velocity->setVisible( false );

    // Connect
    connect( ui->xScaleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setXScale(int)) );
    connect( ui->updateHeadCheckBox, SIGNAL(toggled(bool)), this, SLOT(setDefaultUpdateHead(bool)) );

    for ( auto &elem : waveList ) {
        // Have to use QueuedConnection Not using causes atomic error ( bug? non-reentrant? )
        connect( elem, SIGNAL(headChanged(double)), this, SLOT( headUpdated(double) ) );
    }

    // Connect new data signal
    connect( sensor, SIGNAL(newAcceleration(QVector<double>)), ui->acc->wave, SLOT(enqueueData(QVector<double>)) );
    connect( sensor, SIGNAL(newAngularVelocity(QVector<double>)), ui->gyro->wave, SLOT(enqueueData(QVector<double>)) );
    connect( sensor, SIGNAL(newMagneticField(QVector<double>)), ui->mag->wave, SLOT(enqueueData(QVector<double>)) );
    connect( sensor, SIGNAL(newPressure(QVector<double>)), ui->pressure->wave, SLOT(enqueueData(QVector<double>)) );
    connect( sensor, SIGNAL(newTemperature(QVector<double>)), ui->temp->wave, SLOT(enqueueData(QVector<double>)) );
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_quitButton_clicked()
{
    close();
}

void Widget::on_connectButton_clicked()
{
    // Connect USART
    ConnectDialog d( this );
    d.setSensor( sensor );

    // Disable
    enableSerialButtons( false );

    if ( d.exec() != QDialog::Accepted ) {
        return;
    }

    // Set sensor name
    sensor->setSensorName( ui->sensorNameBox->currentText() );

    // Open
    if ( !sensor->openPort( d.getSelectedName() ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Cant open specified port" ) );

        return;
    }

    if ( ui->handshakeCheckBox->isChecked() ) {
        // Try handshake
        QEventLoop loop;
        QTimer timer;

        connect( sensor, SIGNAL(handshakeACK()), &loop, SLOT(quit()), Qt::QueuedConnection );
        connect( &timer, SIGNAL(timeout()), &loop, SLOT(quit()) );

        sensor->sendHandshake();

        timer.start( 2000 );
        loop.exec( QEventLoop::ExcludeUserInputEvents );

        // Check handshakeACK
        if ( !sensor->getHandshaked() ) {
            // Handsake failed
            QMessageBox::critical( this, tr( "Error" ), tr( "Handshake failed" ) );

            sensor->closePort();

            return;
        }
    }

    // Complete
    sensor->sendStartTransmit();

    // Clear
    clearGraph();

    // Enable
    enableSerialButtons( true );
}

void Widget::on_disconnectButton_clicked()
{
    sensor->sendStopTransmit();
    sensor->flush();
    sensor->closePort();
    enableSerialButtons( false );
}

void Widget::on_startReadButton_clicked()
{
    sensor->sendStartTransmit();
}

void Widget::on_stopReadButton_clicked()
{
    sensor->sendStopTransmit();
}

void Widget::on_resetReadButton_clicked()
{
    sensor->resetCommunication();
    clearGraph();
}

void Widget::enableSerialButtons(bool enable)
{
    ui->disconnectButton->setEnabled( enable );
    ui->startReadButton->setEnabled( enable );
    ui->stopReadButton->setEnabled( enable );
    ui->resetReadButton->setEnabled( enable );
    ui->connectButton->setEnabled( !enable );
}

void Widget::clearGraph()
{
    ui->acc->wave->clearQueue();
    ui->mag->wave->clearQueue();
    ui->gyro->wave->clearQueue();
    ui->pressure->wave->clearQueue();
    ui->temp->wave->clearQueue();
}

void Widget::closeEvent(QCloseEvent *)
{
    if ( sensor->isOpen() ) {
        on_disconnectButton_clicked();
    }
}

void Widget::on_readCardButton_clicked()
{
    // Read sd card

    // Windows only
#ifndef __WIN32__
    QMessageBox::information( this, tr( "Information" ), tr( "SD card reading is only supported on Windows" ) );
    return;
#endif

    // Check connected
    if ( sensor->isOpen() ) {
        QMessageBox::information( this, tr( "Information" ), tr( "Can't open card during connected to sensor" ) );

        return;
    }

    // Select save directory
    QString dirName = QFileDialog::getExistingDirectory( this, tr( "Select a directory to save" ) );

    if ( dirName == "" ) {
        return;
    }

    // SDからログ保存
    QString fileName = saveLogFile( dirName );

    if ( fileName == "" ) {
        return;
    }

    ////////// ログファイルからCSVへ

    // ログファイル開く
    QFile logFile;

    logFile.setFileName( fileName );
    logFile.open( QIODevice::ReadOnly );

    // 先頭へ
    logFile.seek( 2 );

    // 保存用クラス実体化
    QThread *writeFileThread = new QThread();
    CSVWriteFileWorker *worker  = new CSVWriteFileWorker();
    ProgressDialog *progress = new ProgressDialog( this );
    progress->setProgressPos( 0, 0, 0, "Null" );
    QMessageBox warningBox;

    // Setup warning box
    warningBox.setWindowTitle( tr( "Warning" ) );
    warningBox.setText( tr( "File writing failed" ) );
    warningBox.setInformativeText( tr( "A problem has occurred on current operation." ) );
    warningBox.setIcon( QMessageBox::Warning );
    warningBox.setStandardButtons( QMessageBox::Ok );

    // フィルター登録
    QMap<int, QList<AbstractDataFilter *> > filterMap;
    SensorParameters params;
    SensorParameters::Parameter param = params.getParameter( ui->sensorNameBox->currentText().toStdString() );

    filterMap[ID_MPU9150_ACC] = QList<AbstractDataFilter *>() << new AccDataFilter();
    filterMap[ID_MPU9150_GYRO] = QList<AbstractDataFilter *>() << new GyroDataFilter();
    filterMap[ID_AK8975] = QList<AbstractDataFilter *>() << new MagDataFilter();
    filterMap[ID_MPU9150_TEMP] = QList<AbstractDataFilter *>() << new TempDataFilter();
    filterMap[ID_LPS331AP] = QList<AbstractDataFilter *>() << new PressDataFilter();

    // 解像度設定
    dynamic_cast<AccDataFilter *>( filterMap[ID_MPU9150_ACC].last() )->setResolution( param.accelerationResolution );
    dynamic_cast<GyroDataFilter *>( filterMap[ID_MPU9150_GYRO].last() )->setResolution( param.angularVelocityResolution );
    dynamic_cast<MagDataFilter *>( filterMap[ID_AK8975].last() )->setResolution( param.magneticFieldResolution );

    // (実験)後始末関数をラムダで作ってみる
    auto deleteAll = [&] {
        foreach ( QList<AbstractDataFilter *> filterList, filterMap ) {
            foreach ( AbstractDataFilter *filter,  filterList ) {
                delete filter;
            }
        }

        delete worker;
        delete writeFileThread;
    };

    // フィルター設定
    bool opened = true;

    // 各フィルターオープン
    foreach ( QList<AbstractDataFilter *> filterList, filterMap ) {
        foreach ( AbstractDataFilter *filter, filterList ) {
            // ファイル名指定 012345_defaultName.csv
            filter->setFileName( QFileInfo( logFile.fileName() ).baseName() + "_" + filter->getFileName() );

            // 開く
            if ( !filter->openFile( dirName ) ) {
                opened = false;
            }
        }
    }

    // オープンエラーチェック
    if ( !opened ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a file to save" ) );

        deleteAll();

        return;
    }

    // ワーカー設定
    worker->moveToThread( writeFileThread );
    worker->setParameter( &logFile, filterMap );
    worker->setup();

    // シグナル接続
    connect( worker,          SIGNAL(finished()),    progress, SLOT(accept()) );
    connect( progress,        SIGNAL(finished(int)), worker,   SLOT(stopSave()), Qt::DirectConnection );
    connect( writeFileThread, SIGNAL(started()),     worker,   SLOT(doSaveFile()) );
    connect( worker, SIGNAL(progress(int,int,int,QString)), progress, SLOT(setProgressPos(int,int,int,QString)), Qt::QueuedConnection );

    // 開始
    writeFileThread->start();

    // プログレスダイアログ表示
    progress->setWindowFlags( Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog );
    progress->exec();

    // Wait Save Thread
    writeFileThread->quit();
    writeFileThread->wait();

    // Check a error
    if ( worker->error() ) {
        warningBox.exec();
    }

    // 破棄
    deleteAll();

    ////////// CSVを読み込み
}

void Widget::setXScale( int scale )
{
    for ( auto &elem : waveList ) {
        elem->setXScale( scale );
    }
}

void Widget::headUpdated(double rawX)
{
    // wave widget invoke this when head is changed
    if ( !ui->syncX->isChecked() ) {
        return;
    }

    for ( auto &elem : waveList ) {
        elem->setHeadFromRawXSmall( rawX );
    }
}

void Widget::setDefaultUpdateHead(bool val)
{
    for ( auto &elem : waveList ) {
        elem->setDefaultHeadUpdate( val );
    }
}

QString Widget::saveLogFile( QString dirName )
{
    // SDからデータを読みこみログとして保存

    // Open Window's logical drive
    MicomFS fs;
    LogicalDriveDialog *dialog = new LogicalDriveDialog();
    dialog->deleteLater();

    // Select a logical drive
    if ( dialog->exec() == QDialog::Accepted ) {
#ifdef __WIN32__
        HANDLE handle;
        VOLUME_DISK_EXTENTS diskExtents;
        DWORD dwSize;
        WCHAR wbuf[1024];
        int ret;

        // Clear array
        for ( int i = 0; i < 1024; i++ ) {
            wbuf[i] = L'\0';
        }

        // Open Logical drive
        QString( "\\\\.\\" + QString( dialog->getSelectedLogicalDriveName() ) + ":" ).toWCharArray( wbuf );

        handle = CreateFileW( wbuf,
                              GENERIC_READ,
                              0,
                              NULL,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL );

        // Check error
        if ( handle == INVALID_HANDLE_VALUE ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open selected drive" ) );

            return "";
        }

        // Get Physical drive name
        ret = DeviceIoControl( handle,
                               IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
                               NULL,
                               0,
                               (LPVOID) &diskExtents,
                               (DWORD) sizeof(diskExtents),
                               (LPDWORD) &dwSize,
                               NULL );

        // Check error
        if ( ret == 0 ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't get physical drive name" ) );

            return "";
        }

        // Close
        CloseHandle( handle );

        // Open device ( 権限が不要なので論理ドライブ名のままで実行 )
        if ( !micomfs_open_device( &fs, (char *)wbuf, MicomFSDeviceWinDrive, MicomFSDeviceModeReadWrite ) ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open device" ) );

            return "";
        }

        // Open Filesystem
        if ( !micomfs_init_fs( &fs ) ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open FileSystem" ) );

            micomfs_close_device( &fs );

            return "";
        }
#endif
    } else {
        return "";
    }

    // Create file list
    MicomFSFile *fileList;
    MicomFSFile *fp;
    uint16_t fileCount;

    // Update file list
    micomfs_get_file_list( &fs, &fileList, &fileCount );

    // Open file select dialog
    FSFileSelectDialog *listDialog = new FSFileSelectDialog( this );
    listDialog->deleteLater();
    listDialog->setFileList( fileList, fileCount );

    if ( listDialog->exec() != QDialog::Accepted ) {
        free( fileList );
        micomfs_close_device( &fs );

        return "";
    }

    // Get file pointer
    fp = listDialog->getSelectedFile();

    // Size warning
    if ( fp->start_sector + fp->sector_count >= fs.sector_count ) {
        QMessageBox::warning( this, tr( "Warning" ),
                              tr( "This file seems to be broken\n\n" ) +
                              tr( "File saving process will take very long time because reading entire of SD card.\n" ) +
                              tr( "Please save a file about 1 or 2 minutes, and then cancel saving.\n" ) +
                              tr( "If the saved file is not enough, Please try again and save the file in more long time." ) );
    }

    // Save a file
    QFile file;
    QThread *saveFileThread;
    WriteFileWorker *saveFileWorker;
    QMessageBox warningBox;

    // Setup warning box
    warningBox.setWindowTitle( tr( "Warning" ) );
    warningBox.setText( tr( "File writing failed" ) );
    warningBox.setInformativeText( tr( "A problem has occurred on current operation." ) );
    warningBox.setIcon( QMessageBox::Warning );
    warningBox.setStandardButtons( QMessageBox::Ok );

    // Set file name
    file.setFileName( dirName + "/" + fp->name );

    // 上書き確認
    if ( file.exists() ) {
        QMessageBox::StandardButton btn = QMessageBox::question( this, tr( "Overwrite" ), QString( tr( "%1 is already existing.\nOverwrite it?" ) ).arg( file.fileName() ), QMessageBox::Yes | QMessageBox::No );

        if ( btn == QMessageBox::No ) {
            free( fileList );
            micomfs_close_device( &fs );

            return "";
        }
    }

    // Open
    file.open( QIODevice::WriteOnly );

    // Write file
    // Create progress
    ProgressDialog *progress = new ProgressDialog( this );
    progress->deleteLater();
    progress->setProgressPos( 0, 0, 0, "Null" );
    progress->setProgressMax( fp->sector_count );

    // Start file access
    micomfs_start_fread( fp, 0 );

    // Start Save File Thread
    saveFileThread = new QThread( this );
    saveFileWorker = new WriteFileWorker();

    // Setup
    saveFileWorker->moveToThread( saveFileThread );
    saveFileWorker->setParameter( &file, fp );
    saveFileWorker->setup();

    connect( saveFileWorker, SIGNAL(finished()),    progress,       SLOT(accept()) );
    connect( progress,       SIGNAL(finished(int)), saveFileWorker, SLOT(stopSave()), Qt::DirectConnection );
    connect( saveFileThread, SIGNAL(started()),     saveFileWorker, SLOT(doSaveFile()) );
    connect( saveFileWorker, SIGNAL(progress(int,int,int,QString)), progress, SLOT(setProgressPos(int,int,int,QString)) );

    // Exec
    saveFileThread->start();

    // Showmodal progress
    progress->setWindowFlags( Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog );
    progress->exec();

    // Wait Save Thread
    saveFileThread->quit();
    saveFileThread->wait();

    // Check a error
    if ( saveFileWorker->error() ) {
        warningBox.exec();
    }

    // Delete
    delete saveFileWorker;
    delete saveFileThread;

    // 閉じる
    free( fileList );
    micomfs_fclose( fp );
    micomfs_close_device( &fs );
    file.close();

    return file.fileName();
}

void Widget::createWaveList()
{
    waveList << ui->acc->wave;
    waveList << ui->gyro->wave;
    waveList << ui->mag->wave;
    waveList << ui->pressure->wave;
    waveList << ui->temp->wave;
}
