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

    // Title
    QApplication::setApplicationName( tr( "Sensor Reader" ) );

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
    graphQueueSize = 10000;

    createWaveList();
    setupDefaultWaveParams();

    ui->acc->setLabel( tr( "Acceleration" ) );
    ui->acc->wave->setUpSize( 3, graphQueueSize );
    ui->acc->wave->setYGridCount( 1 );
    ui->acc->wave->setAutoUpdateYMax( true );
    ui->acc->wave->setAutoUpdateYMin( true );
    ui->acc->wave->setAutoZeroCenter( true );
    ui->acc->wave->setNames( QStringList() << "x[G]" << "y[G]" << "z[G]" );
    ui->acc->wave->setColors( QList<QColor>() << Qt::red << Qt::darkGreen << Qt::blue );

    ui->gyro->setLabel( tr( "Angular Velocity" ) );
    ui->gyro->wave->setUpSize( 3, graphQueueSize );
    ui->gyro->wave->setYGridCount( 1 );
    ui->gyro->wave->setAutoUpdateYMax( true );
    ui->gyro->wave->setAutoUpdateYMin( true );
    ui->gyro->wave->setAutoZeroCenter( true );
    ui->gyro->wave->setNames( QStringList() << "x[dps]" << "y[dps]" << "z[dps]" );
    ui->gyro->wave->setColors( QList<QColor>() << Qt::red << Qt::darkGreen << Qt::blue );

    ui->mag->setLabel( tr( "Magnetic field" ) );
    ui->mag->wave->setUpSize( 3, graphQueueSize );
    ui->mag->wave->setYGridCount( 1 );
    ui->mag->wave->setAutoUpdateYMax( true );
    ui->mag->wave->setAutoUpdateYMin( true );
    ui->mag->wave->setAutoZeroCenter( true );
    ui->mag->wave->setNames( QStringList() << "x[µT]" << "y[µT]" << "z[µT]" );
    ui->mag->wave->setColors( QList<QColor>() << Qt::red << Qt::darkGreen << Qt::blue );

    ui->temp->setLabel( tr( "Temperature" ) );
    ui->temp->wave->setUpSize( 1, graphQueueSize );
    ui->temp->wave->setYGridCount( 3 );
    ui->temp->wave->setAutoUpdateYMax( true );
    ui->temp->wave->setAutoUpdateYMin( true );
    ui->temp->wave->setNames( QStringList() << "x[℃]" );
    ui->temp->wave->setColors( QList<QColor>() << Qt::blue );

    ui->pressure->setLabel( tr( "Pressure" ) );
    ui->pressure->wave->setUpSize( 1, graphQueueSize );
    ui->pressure->wave->setYGridCount( 3 );
    ui->pressure->wave->setAutoUpdateYMax( true );
    ui->pressure->wave->setAutoUpdateYMin( true );
    ui->pressure->wave->setNames( QStringList() << "x[hPa]" );
    ui->pressure->wave->setColors( QList<QColor>() << Qt::blue );

    ui->pos->setLabel( tr( "Position" ) );
    ui->pos->wave->setUpSize( 3, 100 );
    ui->pos->wave->setYGridCount( 2 );
    ui->pos->wave->setAutoUpdateYMax( true );
    ui->pos->wave->setAutoUpdateYMin( true );
    ui->pos->wave->setAutoZeroCenter( false );
    ui->pos->wave->setNames( QStringList() << "x[m]" << "y[m]" << "z[m]" );
    ui->pos->wave->setColors( QList<QColor>() << Qt::red << Qt::darkGreen << Qt::blue );

    ui->posLen->setLabel( tr( "Position ( Distance )" ) );
    ui->posLen->wave->setUpSize( 1, graphQueueSize );
    ui->posLen->wave->setYGridCount( 3 );
    ui->posLen->wave->setAutoUpdateYMax( true );
    ui->posLen->wave->setAutoUpdateYMin( false );
    ui->posLen->wave->setAutoZeroCenter( false );
    ui->posLen->wave->setYMin( 0 );
    ui->posLen->wave->setNames( QStringList() << "l[m]" );
    ui->posLen->wave->setColors( QList<QColor>() << Qt::blue );

    ui->velocity->setLabel( tr( "Velocity" ) );
    ui->velocity->wave->setUpSize( 3, graphQueueSize );
    ui->velocity->wave->setYGridCount( 2 );
    ui->velocity->wave->setAutoUpdateYMax( true );
    ui->velocity->wave->setAutoUpdateYMin( true );
    ui->velocity->wave->setAutoZeroCenter( false );
    ui->velocity->wave->setNames( QStringList() << "Vx[m/s]" << "Vy[m/s]" << "Vz[m/s]" );
    ui->velocity->wave->setColors( QList<QColor>() << Qt::red << Qt::darkGreen << Qt::blue );

    ui->vLen->setLabel( tr( "Velocity ( Speed )" ) );
    ui->vLen->wave->setUpSize( 1, graphQueueSize );
    ui->vLen->wave->setYGridCount( 3 );
    ui->vLen->wave->setAutoUpdateYMax( true );
    ui->vLen->wave->setAutoUpdateYMin( false );
    ui->vLen->wave->setAutoZeroCenter( false );
    ui->vLen->wave->setYMin( 0 );
    ui->vLen->wave->setNames( QStringList() << "v[m/s]" );
    ui->vLen->wave->setColors( QList<QColor>() << Qt::blue );

    // Disable Analyzed graph
    enableAnalyzedGraph( false );

    // Connect
    connect( ui->xScaleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setXScale(int)) );
    connect( ui->timeGridSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setXGrid(double)) );
    connect( ui->updateHeadCheckBox, SIGNAL(toggled(bool)), this, SLOT(setDefaultUpdateHead(bool)) );
    connect( ui->minHeightBox, SIGNAL(valueChanged(int)), this, SLOT(setMinHeight(int)) );

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

    // Disable analyzed data graph
    enableAnalyzedGraph( false );

    // Clear
    clearGraph();

    // Enable
    enableSerialButtons( true );

    // ウィンドウタイトルクリア
    setTitleDirName( "" );
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
    for ( auto &elem : waveList ) {
        elem->clear();
    }
}

void Widget::setPrintMode(bool enable)
{
    for ( auto &elem : graphWidgetList ) {
        elem->setPrintMode( enable );

        if ( enable ) {
            QFont f = elem->label->font();
            f.setPointSizeF( 11 );
            elem->label->setFont( f );
        } else {
            QFont f = elem->label->font();
            f.setPointSizeF( defaultLabelPoint );
            elem->label->setFont( f );
        }

        elem->update();

        /*
        if ( enable ) {
            elem->wave->setDefaultFontSize( 16 );
            elem->wave->setLegendFontSize( 16 );
        } else {
            elem->wave->setDefaultFontSize( 12 );
            elem->wave->setLegendFontSize( 12 );
        }
        */
    }
}

void Widget::enableAnalyzedGraph(bool enable)
{
    ui->velocity->setVisible( enable );
    ui->vLen->setVisible( enable );
    ui->pos->setVisible( enable );
    ui->posLen->setVisible( enable );
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
    QMessageBox::warning( this, tr( "Warning" ), tr( "SD card reading is partially supported on UNIX like OS" ) );
    // QMessageBox::information( this, tr( "Information" ), tr( "SD card reading is only supported on Windows" ) );
    // return;
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
    progress->setWindowTitle( tr( "Analyzing log file" ) );
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
    filterMap[ID_GPS] = QList<AbstractDataFilter *>() << new GPSDataFilter();

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

    ////////// 解析用、表示用にCSVを読み込み用にファイル名
    QString accFileName  = dirName + "/" + filterMap[ID_MPU9150_ACC][0]->getFileName();
    QString gyroFileName = dirName + "/" + filterMap[ID_MPU9150_GYRO][0]->getFileName();
    QString magFileName = dirName + "/" + filterMap[ID_AK8975][0]->getFileName();
    QString pressureFileName = dirName + "/" + filterMap[ID_LPS331AP][0]->getFileName();
    QString tempFileName = dirName + "/" + filterMap[ID_MPU9150_TEMP][0]->getFileName();
    QString analyzedFileName = dirName + "/" + tr( "output.csv" );

    ////////// 続行確認
    bool analyzed = false;

    if ( QMessageBox::question( this, tr( "Question" ), tr( "Do you want to continue to Analysis step?" ) ) == QMessageBox::Yes ) {
        ////////// 解析開始
        analyzed = analyzeLog( dirName, accFileName, gyroFileName, magFileName, pressureFileName, tempFileName, analyzedFileName, param.xUnit );
    }

    // もういらなので破棄
    deleteAll();

    // Confirm whether to display saved data
    bool showGraph = false;

    if ( QMessageBox::question( this, tr( "Question" ), tr( "Do you want to show graph of saved data?" ) ) == QMessageBox::Yes ) {
        showGraph = true;
    }

    ////////// 保存したデータを表示する
    if ( showGraph ) {
        if ( analyzed ) {
            showAnalyzedLogFiles( dirName, accFileName, gyroFileName, magFileName, pressureFileName, tempFileName, analyzedFileName, param.xUnit, 1 );
        } else {
            showAnalyzedLogFiles( dirName, accFileName, gyroFileName, magFileName, pressureFileName, tempFileName, "", param.xUnit, 1 );
        }
    }

    // すべての処理完了
    QMessageBox::information( this, tr( "Complete" ), tr( "All operation is successfully completed." ) );
}

void Widget::setXScale( int scale )
{
    for ( auto &elem : waveList ) {
        elem->setXScale( scale );
    }
}

void Widget::setXGrid(double width)
{
    for ( auto &elem : waveList ) {
        elem->setXGrid( width );
        elem->update();
    }
}

void Widget::headUpdated(double rawX)
{
    // wave widget invoke this when head is changed
    if ( !ui->syncX->isChecked() ) {
        return;
    }

    for ( auto &elem : waveList ) {
        elem->setHeadFromRawXSmallForce( rawX );
    }
}

void Widget::setDefaultUpdateHead(bool val)
{
    for ( auto &elem : waveList ) {
        elem->setDefaultHeadUpdate( val );
    }
}

void Widget::setMinHeight(int height)
{
    for ( auto &elem : waveList ) {
        elem->setMinimumHeight( height );
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
#else
        // For unix like os
        // Open device
        if ( !micomfs_open_device( &fs, dialog->getSelectedLogicalDriveName().toUtf8().data(), MicomFSDeviceFile, MicomFSDeviceModeRead ) ) {
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
    progress->setWindowTitle( tr( "Saving log file" ) );

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
    /*
    waveList << ui->acc->wave;
    waveList << ui->gyro->wave;
    waveList << ui->mag->wave;
    waveList << ui->pressure->wave;
    waveList << ui->temp->wave;
    waveList << ui->velocity->wave;
    waveList << ui->vLen->wave;
    waveList << ui->pos->wave;
    waveList << ui->posLen->wave;
    */

    graphWidgetList << ui->acc;
    graphWidgetList << ui->gyro;
    graphWidgetList << ui->mag;
    graphWidgetList << ui->pressure;
    graphWidgetList << ui->temp;
    graphWidgetList << ui->velocity;
    graphWidgetList << ui->vLen;
    graphWidgetList << ui->pos;
    graphWidgetList << ui->posLen;

    defaultLabelPoint = ui->acc->label->font().pointSizeF();

    for ( auto &elem : graphWidgetList ) {
        waveList << elem->wave;
    }
}

void Widget::setupDefaultWaveParams()
{
    for ( auto &elem : waveList ) {
        elem->setXScale( ui->xScaleSpinBox->value() );
        elem->setXGrid( ui->timeGridSpinBox->value() );
        elem->setLegendFontSize( 12 );
        elem->setDefaultFontSize( 12 );
        elem->setShowCursor( true );
        elem->setXName( "time[sec]" );
        elem->setForceRequestedRawX( true );
    }
}

void Widget::showAnalyzedLogFiles( QString dirName, QString accFileName, QString gyroFileName, QString magFileName, QString pressureFileName, QString tempFileName, QString analyzedFileName, double rawXUnit, double analyzedXUnit )
{
    // グラフクリア
    clearGraph();

    // CSVみんな開く
    // ログ解析して位置と速度に
    TableDataReader accReader;
    TableDataReader gyroReader;
    TableDataReader magReader;
    TableDataReader pressureReader;
    TableDataReader tempReader;
    TableDataReader analyzedReader;

    // 開いて表示
    if ( accFileName != "" ) {
        if ( !accReader.readFile( accFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a acceleration log file" ) );
        } else {
            ui->acc->wave->setQueueDataFromList( accReader.tableData, rawXUnit );
        }
    }

    if ( gyroFileName != "" ) {
        if ( !gyroReader.readFile( gyroFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a angular velocity log file" ) );
        } else {
            ui->gyro->wave->setQueueDataFromList( gyroReader.tableData, rawXUnit );
        }
    }

    if ( magFileName != "" ) {
        if ( !magReader.readFile( magFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a Magnetic field log file" ) );
        } else {
            ui->mag->wave->setQueueDataFromList( magReader.tableData, rawXUnit );
        }
    }

    if ( pressureFileName != "" ) {
        if ( !pressureReader.readFile( pressureFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a pressure log file" ) );
        } else {
            ui->pressure->wave->setQueueDataFromList( pressureReader.tableData, rawXUnit );
        }
    }

    if ( tempFileName != "" ) {
        if ( !tempReader.readFile( tempFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a temperature log file" ) );
        } else {
            ui->temp->wave->setQueueDataFromList( tempReader.tableData, rawXUnit );
        }
    }

    if ( analyzedFileName != "" ) {
        if ( !analyzedReader.readFile( analyzedFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a Analyzed log file" ) );
            return;
        }
    } else {
        return;
    }

    // 解析済みログファイルを要素ごとのリストに分ける
    QList<QVector<double > > posList;
    QList<QVector<double > > vList;
    QList<QVector<double > > posLenList;
    QList<QVector<double > > vLenList;

    // ハードコーディングだからおかしいかもしんない
    double startTime = ( *analyzedReader.tableData.begin() )[0];
    double endTime = ( *( analyzedReader.tableData.end() - 1 ) )[0];

    for ( auto it = analyzedReader.tableData.begin(); it != analyzedReader.tableData.end(); ++it ) {
        // 振り分け
        double time = (*it)[0];
        QVector<double> pos, posLen;
        QVector<double> v, vLen;

        pos << time << it->mid( 1, 3 );
        posLen << time << it->mid( 4, 1 );
        v << time << it->mid( 5, 3 );
        vLen << time << it->mid( 8, 1 );

        posList << pos;
        posLenList << posLen;
        vList << v;
        vLenList << vLen;
    }

    ui->velocity->wave->setQueueDataFromList( vList, analyzedXUnit );
    ui->vLen->wave->setQueueDataFromList( vLenList, analyzedXUnit );
    ui->pos->wave->setQueueDataFromList( posList, analyzedXUnit );
    ui->posLen->wave->setQueueDataFromList( posLenList, analyzedXUnit );

    // 色フィルターで有効範囲を表示
    ui->acc->wave->addColorFilter( startTime, endTime, QColor( 0, 0, 255, 25 ) );
    ui->gyro->wave->addColorFilter( startTime, endTime, QColor( 0, 0, 255, 25 ) );
    ui->mag->wave->addColorFilter( startTime, endTime, QColor( 0, 0, 255, 25 ) );
    ui->pressure->wave->addColorFilter( startTime, endTime, QColor( 0, 0, 255, 25 ) );
    ui->temp->wave->addColorFilter( startTime, endTime, QColor( 0, 0, 255, 25 ) );

    // 処理結果表示イネーブル
    enableAnalyzedGraph( true );

    // ヘッド位置が合うようにする
    ui->velocity->wave->moveHeadToHead( true, false );

    // ウィンドウタイトルかえる
    setTitleDirName( QDir( dirName ).dirName() );
}

bool Widget::analyzeLog( QString dirName, QString accFileName, QString gyroFileName, QString magFileName, QString pressureFileName, QString tempFileName, QString analyzedFileName, double xUnit)
{
    Q_UNUSED( dirName )

    // ログ解析して位置と速度に
    // 使わないログも多いけど、一応全部読む
    // 行ごとの時刻がずれている場合を考量してないけど、TableDataReader自体は対応しているので書き換えたほうが良い
    // ジャイロ補正もしたい
    TableDataReader accReader;
    TableDataReader gyroReader;
    TableDataReader magReader;
    TableDataReader pressureReader;
    TableDataReader tempReader;
    TableDataReader analyzedReader;

    bool useMag = false;
    bool usePressure = false;
    bool useTemp = false;

    // 開く
    if ( accFileName != "" && !accReader.readFile( accFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a acceleration log file" ) );
        return false;
    }

    if ( gyroFileName != "" && !gyroReader.readFile( gyroFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a angular velocity log file" ) );
        return false;
    }

    if ( magFileName != "" ) {
        if ( !( useMag = magReader.readFile( magFileName, TableDataReader::Comma, TableDataReader::Title ) ) ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a Magnetic field log file" ) );
            useMag = false;
        }
    } else {
        useMag = false;
    }

    if ( pressureFileName != "" ) {
        if ( !( usePressure = pressureReader.readFile( pressureFileName, TableDataReader::Comma, TableDataReader::Title ) ) ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a pressure log file" ) );
            usePressure = false;
        }
    } else {
        usePressure = false;
    }

    if ( tempFileName != "" ) {
        if ( !( useTemp = tempReader.readFile( tempFileName, TableDataReader::Comma, TableDataReader::Title ) ) ) {
            QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a temperature log file" ) );
            useTemp = false;
        }
    } else {
        useTemp = false;
    }

    // データ範囲を指定する
    DataSelectDialog *dataDialog = new DataSelectDialog( this );
    dataDialog->setTable( &accReader, xUnit );
    dataDialog->setZeroCount( 40 );
    dataDialog->setG( 9.8 );
    dataDialog->deleteLater();

    if ( dataDialog->exec() != QDialog::Accepted ) {
        return false;
    }

    // Open save file
    QFile analyzedFile;
    analyzedFile.setFileName( analyzedFileName );

    if ( !analyzedFile.open( QIODevice::WriteOnly ) ) {
        // 開けなかった
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a new file to save analyzed data" ) );
        return false;
    }

    // 処理ワーカー準備
    QThread *analyzeThread = new QThread();
    AnalyzeWorker *worker  = new AnalyzeWorker();
    ProgressDialog *progress = new ProgressDialog( this );

    // ワーカー設定
    worker->moveToThread( analyzeThread );
    worker->setParameter( &accReader, &gyroReader, &magReader, &pressureReader, &tempReader,
                          &analyzedFile,
                          useMag,
                          usePressure,
                          useTemp,
                          xUnit,
                          dataDialog->getZeroCount(),
                          dataDialog->getZeroStartTime(),
                          dataDialog->getZeroEndTime(),
                          dataDialog->getEndTime(),
                          dataDialog->getG() );
    worker->setup();

    // シグナル接続
    connect( worker,          SIGNAL(finished()),    progress, SLOT(accept()) );
    connect( progress,        SIGNAL(finished(int)), worker,   SLOT(stopWork()), Qt::DirectConnection );
    connect( analyzeThread,   SIGNAL(started()),     worker,   SLOT(doWork()) );
    connect( worker, SIGNAL(progress(int,int,int,QString,bool)), progress, SLOT(setProgressPos(int,int,int,QString,bool)), Qt::QueuedConnection );

    // 開始
    analyzeThread->start();

    // プログレスダイアログ表示
    progress->deleteLater();
    progress->setWindowTitle( tr( "Calculating velocity and position" ) );
    progress->setWindowFlags( Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog );
    progress->exec();

    // スレッドまち
    analyzeThread->quit();
    analyzeThread->wait();

    // 問題が起きていれば告知
    if ( worker->error() ) {
        QMessageBox::warning( this, tr( "Warning" ), tr( "Some errors are occured in calculating process.\nSaved file will be broken." ) );
    }

    // ファイルを閉じる
    analyzedFile.close();

    return true;
}

int Widget::getGraphQueueSize() const
{
    return graphQueueSize;
}

void Widget::setGraphQueueSize(int value)
{
    graphQueueSize = value;
}


void Widget::on_analyzeFileButton_clicked()
{
    // ログファイルから位置と速度を解析

    // センサーと接続されてたら実行できない
    if ( sensor->isOpen() ) {
        QMessageBox::information( this, tr( "Information" ), tr( "Can't open card during connected to sensor" ) );

        return;
    }

    SensorParameters params;
    SensorParameters::Parameter param = params.getParameter( ui->sensorNameBox->currentText().toStdString() );
    AnalyzeFileDialog *dialog = new AnalyzeFileDialog( this );
    dialog->deleteLater();

    if ( dialog->exec() != QDialog::Accepted ) {
        return;
    }

    QString dirName = QFileInfo( dialog->getAccFileName() ).canonicalPath();
    QString analyzedFileName = dirName + "/" + tr( "output.csv" );

    if( !analyzeLog( dirName, dialog->getAccFileName(), dialog->getGyroFileName(), "", "", "", analyzedFileName, param.xUnit ) ) {
        return;
    }

    showAnalyzedLogFiles( dirName, dialog->getAccFileName(), dialog->getGyroFileName(), dialog->getMagFileName(), dialog->getPressureFileName(), dialog->getTempFileName(), analyzedFileName, param.xUnit, 1 );
}

void Widget::on_printButton_clicked()
{
    // 印刷する
    QPrinter printer;
    printer.setOutputFormat( QPrinter::NativeFormat );

    QPrintDialog *dialog = new QPrintDialog( &printer, this );
    dialog->deleteLater();
    dialog->setWindowTitle( tr( "Print graph" ) );

    if ( dialog->exec() != QDialog::Accepted ) {
        return;
    }

    QWidget *widget = ui->scrollAreaWidgetContents;

    // Print widgets as is into svg
    QBuffer buffer;
    QSvgGenerator svg;

    // Create svg buffer
    buffer.open( QIODevice::ReadWrite );
    svg.setOutputDevice( &buffer );

    // Draw widget
    QPainter picPainter( &svg );

    setPrintMode( true );
    widget->adjustSize();
    widget->render( &picPainter );
    setPrintMode( false );

    picPainter.end();

    // Prin
    QPainter painter( &printer );
    QSvgRenderer render( buffer.data() );

    // 大きさ情報計算
    QRectF r = painter.window();
    double ratio = widget->height() / double( widget->width() );
    double height = r.width() * ratio;

    if ( height < r.height() ) {
        // スケーリング防止はとりあえずしない
        // r.setHeight( height );
    }

    render.render( &painter, r );
}

void Widget::setTitleDirName(QString dir)
{
    // ウィンドウのタイトルにファイル名を表示する
    QString title = tr( "Sensor Reader" );

    if ( dir != "" ) {
        title = dir + " - " + title;
    }

    setWindowTitle( title );
    // QApplication::setApplicationDisplayName( title );
}

void Widget::on_showFileButton_clicked()
{
    // ログファイルを表示

    // センサーと接続されてたら実行できない
    if ( sensor->isOpen() ) {
        QMessageBox::information( this, tr( "Information" ), tr( "Can't open card during connected to sensor" ) );

        return;
    }

    SensorParameters params;
    SensorParameters::Parameter param = params.getParameter( ui->sensorNameBox->currentText().toStdString() );

    OpenFileDialog *dialog = new OpenFileDialog( this );
    dialog->deleteLater();

    if ( dialog->exec() != QDialog::Accepted ) {
        return;
    }

    QString dirName = dialog->getDirName();

    showAnalyzedLogFiles( dirName, dialog->getAccFileName(), dialog->getGyroFileName(), dialog->getMagFileName(), dialog->getPressureFileName(), dialog->getTempFileName(), dialog->getAnalyzedFileName(), param.xUnit, 1 );

    setTitleDirName( QDir( dirName ).dirName() );
}

void Widget::on_pdfButton_clicked()
{
    // PDFに印刷する
    QString fileName;

    if ( ( fileName = QFileDialog::getSaveFileName( this, tr( "Save to PDF" ), "*.pdf", tr( "PDF (*.pdf)" ) ) ) == "" ) {
        return;
    }

    QWidget *widget = ui->scrollAreaWidgetContents;

    // Print widgets as is into svg
    QBuffer buffer;
    QSvgGenerator svg;

    // Create svg buffer
    buffer.open( QIODevice::ReadWrite );
    svg.setOutputDevice( &buffer );

    // Draw widget
    QPainter picPainter( &svg );

    setPrintMode( true );
    widget->adjustSize();
    widget->render( &picPainter );
    setPrintMode( false );

    picPainter.end();

    // Prin
    QPdfWriter writer( fileName );
    QPainter painter( &writer );
    QSvgRenderer render( buffer.data() );

    // 大きさ情報計算
    QRectF r = painter.window();
    double ratio = widget->height() / double( widget->width() );
    double height = r.width() * ratio;

    if ( height < r.height() ) {
        // スケーリング防止はとりあえずしない
        // r.setHeight( height );
    }

    render.render( &painter, r );
}

void Widget::on_aboutButton_clicked()
{
    // About box
    // About box
    QString verStr;

    verStr = verStr +
            QApplication::applicationName() + "\n" +
            "BUILD : " + QString("%1\n").arg( __DATE__ ) +
            "(c) 2018 Masato Takahashi\n" +
            "mas@to.email.ne.jp\n\n" +
            "MFSAPI : " + MICOMFS_API_VERSION_CODE;

    QMessageBox::about( this, QApplication::applicationName(), verStr );
}
