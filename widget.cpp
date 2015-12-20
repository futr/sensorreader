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
    ui->acc->wave->setUpSize( 3, 10000 );
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
    ui->acc->wave->setColors( QList<QColor>() << Qt::red << Qt::darkGreen << Qt::blue );

    ui->gyro->setLabel( tr( "Angular Velocity" ) );
    ui->gyro->wave->setUpSize( 3, 10000 );
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
    ui->gyro->wave->setColors( QList<QColor>() << Qt::red << Qt::darkGreen << Qt::blue );

    ui->mag->setLabel( tr( "Magnetic field" ) );
    ui->mag->wave->setUpSize( 3, 10000 );
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
    ui->mag->wave->setColors( QList<QColor>() << Qt::red << Qt::darkGreen << Qt::blue );

    ui->temp->setLabel( tr( "Temperature" ) );
    ui->temp->wave->setUpSize( 1, 10000 );
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
    ui->pressure->wave->setUpSize( 1, 10000 );
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

    ui->pos->setLabel( tr( "Position" ) );
    ui->pos->wave->setUpSize( 3, 10000 );
    ui->pos->wave->setXScale( 20 );
    ui->pos->wave->setAutoUpdateYMax( true );
    ui->pos->wave->setAutoUpdateYMin( true );
    ui->pos->wave->setAutoZeroCenter( true );
    ui->pos->wave->setXGrid( 5 );
    ui->pos->wave->setLegendFontSize( 12 );
    ui->pos->wave->setDefaultFontSize( 12 );
    ui->pos->wave->setShowCursor( true );
    ui->pos->wave->setXName( "time[sec]" );
    ui->pos->wave->setNames( QStringList() << "x[m]" << "y[m]" << "z[m]" );
    ui->pos->wave->setColors( QList<QColor>() << Qt::red << Qt::darkGreen << Qt::blue );

    ui->posLen->setLabel( tr( "Position ( Length from start position )" ) );
    ui->posLen->wave->setUpSize( 1, 10000 );
    ui->posLen->wave->setXScale( 20 );
    ui->posLen->wave->setAutoUpdateYMax( true );
    ui->posLen->wave->setAutoUpdateYMin( false );
    ui->posLen->wave->setAutoZeroCenter( true );
    ui->posLen->wave->setYMin( 0 );
    ui->posLen->wave->setXGrid( 5 );
    ui->posLen->wave->setLegendFontSize( 12 );
    ui->posLen->wave->setDefaultFontSize( 12 );
    ui->posLen->wave->setShowCursor( true );
    ui->posLen->wave->setXName( "time[sec]" );
    ui->posLen->wave->setNames( QStringList() << "l[m]" );
    ui->posLen->wave->setColors( QList<QColor>() << Qt::blue );

    ui->velocity->setLabel( tr( "Velocity" ) );
    ui->velocity->wave->setUpSize( 3, 10000 );
    ui->velocity->wave->setXScale( 20 );
    ui->velocity->wave->setAutoUpdateYMax( true );
    ui->velocity->wave->setAutoUpdateYMin( true );
    ui->velocity->wave->setAutoZeroCenter( true );
    ui->velocity->wave->setXGrid( 5 );
    ui->velocity->wave->setLegendFontSize( 12 );
    ui->velocity->wave->setDefaultFontSize( 12 );
    ui->velocity->wave->setShowCursor( true );
    ui->velocity->wave->setXName( "time[sec]" );
    ui->velocity->wave->setNames( QStringList() << "Vx[m/s]" << "Vy[m/s]" << "Vz[m/s]" );
    ui->velocity->wave->setColors( QList<QColor>() << Qt::red << Qt::darkGreen << Qt::blue );

    ui->vLen->setLabel( tr( "Velocity ( Length )" ) );
    ui->vLen->wave->setUpSize( 1, 10000 );
    ui->vLen->wave->setXScale( 20 );
    ui->vLen->wave->setAutoUpdateYMax( true );
    ui->vLen->wave->setAutoUpdateYMin( false );
    ui->vLen->wave->setAutoZeroCenter( true );
    ui->vLen->wave->setYMin( 0 );
    ui->vLen->wave->setXGrid( 5 );
    ui->vLen->wave->setLegendFontSize( 12 );
    ui->vLen->wave->setDefaultFontSize( 12 );
    ui->vLen->wave->setShowCursor( true );
    ui->vLen->wave->setXName( "time[sec]" );
    ui->vLen->wave->setNames( QStringList() << "v[m/s]" );
    ui->vLen->wave->setColors( QList<QColor>() << Qt::blue );

    // Disable Analyzed graph
    enableAnalyzedGraph( false );

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

    // Disable analyzed data graph
    enableAnalyzedGraph( false );

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
    for ( auto &elem : waveList ) {
        elem->clear();
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

    ////////// 続行確認
    if ( QMessageBox::question( this, tr( "Question" ), tr( "Do you want to continue to Analysis step?" ) ) != QMessageBox::Yes ) {
        // 破棄
        deleteAll();

        return;
    }

    ////////// 解析用、表示用にCSVを読み込み用にファイル名
    QString accFileName  = dirName + "/" + filterMap[ID_MPU9150_ACC][0]->getFileName();
    QString gyroFileName = dirName + "/" + filterMap[ID_MPU9150_GYRO][0]->getFileName();
    QString magFileName = dirName + "/" + filterMap[ID_AK8975][0]->getFileName();
    QString pressureFileName = dirName + "/" + filterMap[ID_LPS331AP][0]->getFileName();
    QString tempFileName = dirName + "/" + filterMap[ID_MPU9150_TEMP][0]->getFileName();
    QString analyzedFileName = dirName + "/" + tr( "output.csv" );

    // もういらなので破棄
    deleteAll();

    ////////// 解析開始
    if ( !analyzeLog( dirName, accFileName, gyroFileName, analyzedFileName, param.xUnit ) ) {
        return;
    }

    ////////// 保存したデータを表示する
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

    // 開く
    if ( !accReader.readFile( accFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a acceleration log file" ) );
        return;
    }

    if ( !gyroReader.readFile( gyroFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a angular velocity log file" ) );
        return;
    }

    if ( !magReader.readFile( magFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a Magnetic field log file" ) );
        return;
    }

    if ( !pressureReader.readFile( pressureFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a pressure log file" ) );
        return;
    }

    if ( !tempReader.readFile( tempFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a temperature log file" ) );
        return;
    }

    if ( !analyzedReader.readFile( analyzedFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a Analyzed log file" ) );
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

    // 表示
    ui->acc->wave->setQueueDataFromList( accReader.tableData, param.xUnit );
    ui->gyro->wave->setQueueDataFromList( gyroReader.tableData, param.xUnit );
    ui->mag->wave->setQueueDataFromList( magReader.tableData, param.xUnit );
    ui->pressure->wave->setQueueDataFromList( pressureReader.tableData, param.xUnit );
    ui->temp->wave->setQueueDataFromList( tempReader.tableData, param.xUnit );

    ui->velocity->wave->setQueueDataFromList( vList, 1 );
    ui->vLen->wave->setQueueDataFromList( vLenList, 1 );
    ui->pos->wave->setQueueDataFromList( posList, 1 );
    ui->posLen->wave->setQueueDataFromList( posLenList, 1 );

    // 色フィルターで有効範囲を表示
    ui->acc->wave->addColorFilter( startTime, endTime, QColor( 0, 0, 255, 20 ) );
    ui->gyro->wave->addColorFilter( startTime, endTime, QColor( 0, 0, 255, 20 ) );
    ui->mag->wave->addColorFilter( startTime, endTime, QColor( 0, 0, 255, 20 ) );
    ui->pressure->wave->addColorFilter( startTime, endTime, QColor( 0, 0, 255, 20 ) );
    ui->temp->wave->addColorFilter( startTime, endTime, QColor( 0, 0, 255, 20 ) );

    // 処理結果表示イネーブル
    enableAnalyzedGraph( true );

    // すべての処理完了
    QMessageBox::information( this, tr( "Complete" ), tr( "All operation is successfully completed." ) );
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
    waveList << ui->velocity->wave;
    waveList << ui->vLen->wave;
    waveList << ui->pos->wave;
    waveList << ui->posLen->wave;
}

bool Widget::analyzeLog( QString dirName, QString accFileName, QString gyroFileName, QString analyzedFileName, double xUnit)
{
    Q_UNUSED( dirName )

    // ログ解析して位置と速度に
    TableDataReader accReader;
    TableDataReader gyroReader;

    // Open log
    if ( !accReader.readFile( accFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a acceleration log file" ) );
        return false;
    }

    if ( !gyroReader.readFile( gyroFileName, TableDataReader::Comma, TableDataReader::Title ) ) {
        QMessageBox::critical( this, tr( "Error" ), tr( "Can't open a angular velocity log file" ) );
        return false;
    }

    // データ範囲を指定する
    DataSelectDialog *dataDialog = new DataSelectDialog( this );
    dataDialog->setTable( &accReader, xUnit );
    dataDialog->setZeroCount( 20 );
    dataDialog->setG( 9.80665 );
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

    ////// 解析開始
    QTextStream stream( &analyzedFile );

    // Calculation
    bool exist;
    int zeroSampleCount = dataDialog->getZeroCount();
    // double threshold    = 0.006;
    // double thresholdLen = 0.02;
    quint32 zeroStartTime = dataDialog->getZeroStartTime();
    quint32 zeroEndTime   = dataDialog->getZeroEndTime();
    quint32 endTime     = dataDialog->getEndTime();
    double gVal = dataDialog->getG();

    QVector< double > rowData;
    QVector3D avrZeroGyro;
    QVector3D avrZeroAcc;
    QVector<double> timeVector;
    QVector<double> matchLevel;
    QVector<double> offsetedAccLength;
    QVector<double> modifiedAccLength;
    QVector<QVector3D> rotateAxis( 3 );
    QVector<QVector3D> minusRotateAxis( 3 );
    QList<QVector3D> unitX;
    QList<QVector3D> unitY;
    QList<QVector3D> unitZ;
    QList<QVector3D> dGyro;
    QList<QVector3D> rotatedG;
    QList<QVector3D> offsetedAcc;
    QList<QVector3D> modifiedAcc;
    QList<QVector3D> velocity;
    QList<QVector3D> position;
    QList<QMatrix4x4> dTranslate;
    QList<QMatrix4x4> dMinusTranslate;
    QList<QQuaternion> qList;
    QVector<double> zeroAccX( zeroSampleCount );
    QVector<double> zeroAccY( zeroSampleCount );
    QVector<double> zeroAccZ( zeroSampleCount );
    QVector<double> zeroOffsetedAccLength( zeroSampleCount );

    int dataStartRow;
    int dataEndRow;
    int zeroStartRow;
    int zeroEndRow;
    int dataCount;

    // Calc zeroGyro
    gyroReader.getRowFromColumnValue( 0, zeroStartTime, TableDataReader::ProportionalValue, &exist, &zeroStartRow );
    gyroReader.getRowFromColumnValue( 0, zeroEndTime, TableDataReader::ProportionalValue, &exist, &zeroEndRow );

    for ( int i = zeroStartRow; i <= zeroEndRow; i++ ) {
        avrZeroGyro += rowToVec3D( gyroReader.getRow( i ) );
    }

    avrZeroGyro /= ( zeroEndRow - zeroStartRow + 1 );

    // Calc zeroAcc
    accReader.getRowFromColumnValue( 0, zeroStartTime, TableDataReader::ProportionalValue, &exist, &zeroStartRow );
    accReader.getRowFromColumnValue( 0, zeroEndTime, TableDataReader::ProportionalValue, &exist, &zeroEndRow );

    for ( int i = zeroStartRow; i <= zeroEndRow; i++ ) {
        avrZeroAcc += rowToVec3D( accReader.getRow( i ) );
    }

    avrZeroAcc /= ( zeroEndRow - zeroStartRow + 1 );

    // make zeroAcc
    zeroAccX = accReader.getColumnVector( 1, zeroStartRow, zeroSampleCount );
    zeroAccY = accReader.getColumnVector( 2, zeroStartRow, zeroSampleCount );
    zeroAccZ = accReader.getColumnVector( 3, zeroStartRow, zeroSampleCount );

    // Start integral
    // DEBUG Change data start row to zeroStartRow
    dataStartRow = zeroStartRow;
    accReader.getRowFromColumnValue( 0, endTime, TableDataReader::ProportionalValue, &exist, &dataEndRow );
    // dataEndRow   = accReader.rowCount() - 1;
    dataCount    = dataEndRow - dataStartRow + 1;

    // Create time vector;
    timeVector.resize( dataCount );

    for ( int i = dataStartRow; i <= dataEndRow; i++ ) {
        timeVector[i - dataStartRow] = gyroReader.getRow( i )[0] * 0.0001;
    }

    // calc dgyro
    for ( int i = dataStartRow; i <= dataEndRow; i++ ) {
        if ( i == dataStartRow ) {
            dGyro.append( QVector3D( 0, 0, 0 ) );
        } else {
            double time;

            time = ( gyroReader.getRow( i )[0] - gyroReader.getRow( i - 1 )[0] ) * 0.0001;

            dGyro.append( ( QVector3D( rowToVec3D( gyroReader.getRow( i - 1 ) ) ) - avrZeroGyro ) * time );
        }
    }

    // クオータニオンをつかってみる
    for ( int i = dataStartRow; i <= dataEndRow; i++ ) {
        if ( i == dataStartRow ) {
            qList.append( QQuaternion() );
        } else {
            // クオータニオンの計算
            // 角速度ベクトルを作る( 角度に変換済みのを角速度に戻してラジアンに )
            // 本来4x3行列だけど，めんどくさいので4x4を使う
            double time;
            QVector4D omega( ( rowToVec3D( gyroReader.getRow( i - 1 ) ) - avrZeroGyro ) / 180.0 * M_PI );
            QMatrix4x4 E;
            QQuaternion dq, q;

            q = qList[i - dataStartRow - 1];

            E( 0, 0 ) = q.scalar();
            E( 0, 1 ) = -q.z();
            E( 0, 2 ) = q.y();

            E( 1, 0 ) = q.z();
            E( 1, 1 ) = q.scalar();
            E( 1, 2 ) = -q.x();

            E( 2, 0 ) = -q.y();
            E( 2, 1 ) = q.x();
            E( 2, 2 ) = q.scalar();

            E( 3, 0 ) = -q.x();
            E( 3, 1 ) = -q.y();
            E( 3, 2 ) = -q.z();

            // クオターニオンの微分を作る
            dq = QQuaternion( 1.0 / 2 * E * omega );

            // 積分
            time = ( gyroReader.getRow( i )[0] - gyroReader.getRow( i - 1 )[0] ) * 0.0001;

            q = q + ( dq * time );
            q.normalize();

            qList.append( q );
        }
    }

    // 回転する単位ベクトル軸を，初期重力加速度を元に地面を推定して回転
    rotateAxis = groundAxisFromG( avrZeroAcc );
    minusRotateAxis = rotateAxis;

    // DEBUG クオータニオン

    // calc translate matrix
    // DEBUG invalid index sequence?
    // for ( int i = dataStartRow; i <= dataEndRow; i++ ) {
    for ( int i = 0; i < dataCount; i++ ) {
        // Create translate matrix
        QMatrix4x4 trans, minusTrans;

        trans.setToIdentity();
        trans.rotate( dGyro[i].x(), 1, 0, 0 );
        trans.rotate( dGyro[i].y(), 0, 1, 0 );
        trans.rotate( dGyro[i].z(), 0, 0, 1 );
        dTranslate.append( trans );

        /*
        minusTrans.setToIdentity();
        minusTrans.rotate( -dGyro[i].x(), 1, 0, 0 );
        minusTrans.rotate( -dGyro[i].y(), 0, 1, 0 );
        minusTrans.rotate( -dGyro[i].z(), 0, 0, 1 );
        dMinusTranslate.append( minusTrans );
        */

        // DEBUG use rotated rotate matrix

        minusTrans.setToIdentity();

        /*
        minusTrans.rotate( -dGyro[i].x(), rotateAxis[0] );
        minusTrans.rotate( -dGyro[i].y(), rotateAxis[1] );
        minusTrans.rotate( -dGyro[i].z(), rotateAxis[2] );
        */

        minusTrans.rotate( -dGyro[i].x(), 1, 0, 0 );
        minusTrans.rotate( -dGyro[i].y(), 0, 1, 0 );
        minusTrans.rotate( -dGyro[i].z(), 0, 0, 1 );


        minusRotateAxis[0] = minusTrans * minusRotateAxis[0];
        minusRotateAxis[1] = minusTrans * minusRotateAxis[1];
        minusRotateAxis[2] = minusTrans * minusRotateAxis[2];

        if ( i == 0 ) {
            rotatedG.append( avrZeroAcc );
        } else {
            rotatedG.append( minusTrans * rotatedG[i - 1] );
        }

        // Rotate and update rotate axis
        QMatrix4x4 transRotateAxis;

        // DEBUG 返還前のとあとのどちらがよいのか
        /*
        unitX.append( rotateAxis[0] );
        unitY.append( rotateAxis[1] );
        unitZ.append( rotateAxis[2] );
        */

        if ( i == 0 ) {
            unitX.append( rotateAxis[0] );
            unitY.append( rotateAxis[1] );
            unitZ.append( rotateAxis[2] );
        } else {
            QQuaternion q;

            q = qList[i];

            transRotateAxis.setToIdentity();
            transRotateAxis.rotate( q );





            QVector3D rx = transRotateAxis * QVector3D( 1, 0, 0 );
            QVector3D ry = transRotateAxis * QVector3D( 0, 1, 0 );
            QVector3D rz = transRotateAxis * QVector3D( 0, 0, 1 );

            /*
            QVector3D rx = qList[i].rotatedVector( QVector3D( 1, 0, 0 ) );
            QVector3D ry = qList[i].rotatedVector( QVector3D( 0, 1, 0 ) );
            QVector3D rz = qList[i].rotatedVector( QVector3D( 0, 0, 1 ) );
            */

            QVector3D ux = rotateAxis[0] * rx.x() + rotateAxis[1] * rx.y() + rotateAxis[2] * rx.z();
            QVector3D uy = rotateAxis[0] * ry.x() + rotateAxis[1] * ry.y() + rotateAxis[2] * ry.z();
            QVector3D uz = rotateAxis[0] * rz.x() + rotateAxis[1] * rz.y() + rotateAxis[2] * rz.z();
//            QVector3D ux=rx;
//            QVector3D uy=ry;
//            QVector3D uz=rz;

            /*
            QVector3D ux = rx * rotateAxis[0].x() + ry * rotateAxis[0].y() + rz * rotateAxis[0].z();
            QVector3D uy = rx * rotateAxis[1].x() + ry * rotateAxis[1].y() + rz * rotateAxis[1].z();
            QVector3D uz = rx * rotateAxis[2].x() + ry * rotateAxis[2].y() + rz * rotateAxis[2].z();
            */

            /*
            ux.normalize();
            uy.normalize();
            uz.normalize();
            */

            unitX.append( ux );
            unitY.append( uy );
            unitZ.append( uz );
        }
    }

    // calc offseted acc
    for ( int i = dataStartRow; i <= dataEndRow; i++ ) {
        QVector3D acc( rowToVec3D( accReader.getRow( i ) ) );

        // DEBUG Do not offset
        ///////// offsetedAcc.append( acc - rotatedG[i - dataStartRow] );
        offsetedAcc.append( acc );
        // offsetedAcc.append( acc - rotatedG[i - dataStartRow] );
    }

    // Calc offseted acc length
    offsetedAccLength.resize( dataCount );

    for ( int i = 0; i < dataCount; i++ ) {
        offsetedAccLength[i] = offsetedAcc[i].length();
    }

    // calc modified acc
    modifiedAcc = offsetedAcc;

    // Calc modified acc length
    modifiedAccLength.resize( dataCount );

    for ( int i = 0; i < dataCount; i++ ) {
        modifiedAccLength[i] = modifiedAcc[i].length();
    }


    // DEBUG : Create velocity and position by rotated acc vector
    // calc velocity
    velocity.append( QVector3D( 0, 0, 0 ) );

    for ( int i = 1; i < dataCount; i++ ) {
        // QVector3D acc( offsetedAcc[i - 1] );
        QVector3D acc1( modifiedAcc[i - 1] );
        QVector3D acc2( modifiedAcc[i] );
        // QVector3D acc( modifiedAcc[i] );
        // QVector3D rotatedAvrAcc( 0, 0, avrZeroAcc.length() );
        QVector3D rotatedAvrAcc( rotateAxis[0] * avrZeroAcc.x() + rotateAxis[1] * avrZeroAcc.y() + rotateAxis[2] * avrZeroAcc.z() );

        acc1 = unitX[i - 1] * acc1.x() + unitY[i - 1] * acc1.y() + unitZ[i - 1] * acc1.z();
        acc2 = unitX[i] * acc2.x() + unitY[i] * acc2.y() + unitZ[i] * acc2.z();
        // acc = ( unitX[i] + unitY[i] + unitZ[i] );

        QVector3D a1 = gVal * ( acc1 - rotatedAvrAcc );
        QVector3D a2 = gVal * ( acc2 - rotatedAvrAcc );
        double t1 = timeVector[i - 1];
        double t2 = timeVector[i];
        QVector3D v1 = velocity[i - 1];
        QVector3D v2;
        v2.setX( calcArea( QVector2D( t1, a1.x() ), QVector2D( t2, a2.x() ) ) );
        v2.setY( calcArea( QVector2D( t1, a1.y() ), QVector2D( t2, a2.y() ) ) );
        v2.setZ( calcArea( QVector2D( t1, a1.z() ), QVector2D( t2, a2.z() ) ) );

        v2 = v2 + v1;

        velocity.append( v2 );


        // velocity.append( 9.80665 * ( acc + rotatedG[i] ) * ( timeVector[i] - timeVector[i - 1] ) + velocity[i - 1] );
        //velocity.append(  * ( timeVector[i] - timeVector[i - 1] ) + velocity[i - 1] );
        // velocity.append( 9.80665 * acc * ( timeVector[i] - timeVector[i - 1] ) + velocity[i - 1] );
    }

    // calc position
    position.append( QVector3D( 0, 0, 0 ) );

    for ( int i = 1; i < dataCount; i++ ) {
        QVector3D v1( velocity[i - 1] );
        QVector3D v2( velocity[i] );
        double t1 = timeVector[i - 1];
        double t2 = timeVector[i];

        QVector3D p1 = position[i - 1];
        QVector3D p2;

        p2.setX( calcArea( QVector2D( t1, v1.x() ), QVector2D( t2, v2.x() ) ) );
        p2.setY( calcArea( QVector2D( t1, v1.y() ), QVector2D( t2, v2.y() ) ) );
        p2.setZ( calcArea( QVector2D( t1, v1.z() ), QVector2D( t2, v2.z() ) ) );

        p2 = p1 + p2;

        position.append( p2 );

        // position.append( vel * ( timeVector[i] - timeVector[i - 1] ) + position[i - 1] );
    }

    /*
    velocity.append( QVector3D( 0, 0, 0 ) );

    for ( int i = 1; i < dataCount; i++ ) {
        QVector3D acc( modifiedAcc[i - 1] );
        // QVector3D acc( modifiedAcc[i] );
        // QVector3D rotatedAvrAcc( 0, 0, avrZeroAcc.length() );
        QVector3D rotatedAvrAcc( rotateAxis[0] * avrZeroAcc.x() + rotateAxis[1] * avrZeroAcc.y() + rotateAxis[2] * avrZeroAcc.z() );

        // acc = ( unitX[i] + unitY[i] + unitZ[i] );

        velocity.append( 9.80665 * acc * ( timeVector[i] - timeVector[i - 1] ) + velocity[i - 1] );
    }

    // calc position
    position.append( QVector3D( 0, 0, 0 ) );

    QVector3D gv;

    for ( int i = 1; i < dataCount; i++ ) {
        QVector3D vel( velocity[i - 1] );
        gv = gv + QVector3D( 0, 0, 9.80665 * avrZeroAcc.length() * ( timeVector[i] - timeVector[i - 1] ) );

        vel = unitX[i - 1] * vel.x() + unitY[i - 1] * vel.y() + unitZ[i - 1] * vel.z();

        position.append( ( vel - gv ) * ( timeVector[i] - timeVector[i - 1] ) + position[i - 1] );
    }
    */

    // Output all data

    // Create header
    stream << tr( "time[sec]" ) << "," <<
              tr( "posX[m]" ) << "," <<
              tr( "posY[m]" ) << "," <<
              tr( "posZ[m]" ) << "," <<
              tr( "posLen[m]" ) << "," <<
              tr( "VelX[m/s]" ) << "," <<
              tr( "VelY[m/s]" ) << "," <<
              tr( "VelZ[m/s]" ) << "," <<
              tr( "VLen[m/s]" ) <<  "\n";

    // data outpu loop
    for ( int i = 0; i < dataCount; i++ ) {
        stream << timeVector[i]   << "," <<
                  position[i].x() << "," <<
                  position[i].y() << "," <<
                  position[i].z() << "," <<
                  position[i].length() << "," <<
                  velocity[i].x() << "," <<
                  velocity[i].y() << "," <<
                  velocity[i].z() << "," <<
                  velocity[i].length() << "\n";
    }

    // Close
    analyzedFile.close();

    return true;
}


QVector3D Widget::rowToVec3D(QVector<double> row)
{
    QVector3D ret;

    ret.setX( row[1] );
    ret.setY( row[2] );
    ret.setZ( row[3] );

    return ret;
}

QVector<double> Widget::getColumnVector(const QList<QVector3D> &list, int column, int startRow, int count)
{
    // Get column vector
    QVector<double> ret( count );

    for ( int i = 0; i < count; i++ ) {
        int row = i + startRow;
        if ( row < 0 || row >= list.count() ) {
            ret[i] = 0;
        } else {
            ret[i] = list[row][column];
        }
    }

    return ret;
}

QVector<double> Widget::getColumnVector(const QVector<double> &vector, int startRow, int count)
{
    // Get ranged column vector from vector
    QVector<double> ret( count );

    for ( int i = 0; i < count; i++ ) {
        int row = i + startRow;
        if ( row < 0 || row >= vector.count() ) {
            ret[i] = 0;
        } else {
            ret[i] = vector[row];
        }
    }

    return ret;
}

double Widget::vectorLength(const QVector<double> &vector)
{
    double len = 0;

    for ( auto && elem : vector ) {
        len += elem * elem;
    }

    return qSqrt( len );
}

QVector<QVector3D> Widget::groundAxisFromG(QVector3D g)
{
    // 初期のGベクトルからセンサの地面からみた初期姿勢を取得する
    QVector3D x, y, z;
    QMatrix4x4 trans;
    double theta;
    double cosPhi, cosPsi, cosTheta;

    // 角度
    cosPhi   = QVector3D::dotProduct( g, QVector3D( 0, g.y(), 0 ) ) / ( g.length() * qAbs( g.y() ) );
    cosPsi   = QVector3D::dotProduct( g, QVector3D( 0, 0, g.z() ) ) / ( g.length() * qAbs( g.z() ) );
    cosTheta = QVector3D::dotProduct( g, QVector3D( g.x(), 0, 0 ) ) / ( g.length() * qAbs( g.x() ) );

    theta = qAcos( cosTheta );

    // 最初にxを作る( axとgの貼る面と平行にEx軸をとり，Ex⊥g )
    x = QVector3D( qAbs( g.x() ), 0, 0 );

    trans.setToIdentity();
    trans.rotate( -( 90 - qRadiansToDegrees( theta ) ), 0, 1, 0 );

    x = trans * x;

    // yz, zzを計算
    y.setZ( cosPhi * qAbs( g.y() ) );
    z.setZ( cosPsi * qAbs( g.z() ) );

    // Zx
    z.setX( -1 * x.z() * z.z() / x.x() );

    // Zyは条件によって符号が変わる
    if ( g.x() * g.y() * g.z() > 0 ) {
        z.setY( -qSqrt( z.x() * ( -x.x() - z.x() ) + y.z() * z.z() ) );
    } else {
        z.setY( qSqrt( z.x() * ( -x.x() - z.x() ) + y.z() * z.z() ) );
    }

    // y
    y.setX( -x.x() - z.x() );
    y.setY( -z.y() );

    // それぞれの加速度軸方向に向きを変える
    x = x * g.x() / qAbs( g.x() );
    z = z * g.z() / qAbs( g.z() );
    y = y * g.y() / qAbs( g.y() );

    // 単位ベクトルにして返
    x.normalize();
    y.normalize();
    z.normalize();

    return QVector<QVector3D>( { x, y, z } );
}

double Widget::calcArea(QVector2D p1, QVector2D p2)
{
    // 二つの点の間の積分
    // 三角形で近似する
    // xが時間，yが値としてる

    return ( p2.y() - p1.y() ) * ( p2.x() - p1.x() ) / 2 + ( p1.y() * ( p2.x() - p1.x() ) );
}

