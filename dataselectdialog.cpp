#include "dataselectdialog.h"
#include "ui_dataselectdialog.h"

DataSelectDialog::DataSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataSelectDialog)
{
    ui->setupUi(this);

    zeroCount = ui->zeroCountSpinBox->value();
    G = ui->gEdit->text().toDouble();

    ui->acc->setLabel( tr( "Acceleration" ) );
    ui->acc->wave->setRightCursorForceBig( true );
    ui->acc->wave->setUpSize( 3, 10000 );
    ui->acc->wave->setXScale( ui->xScaleSpinBox->value() );
    ui->acc->wave->setAutoUpdateYMax( true );
    ui->acc->wave->setAutoUpdateYMin( true );
    ui->acc->wave->setAutoZeroCenter( true );
    ui->acc->wave->setXGrid( 5 );
    ui->acc->wave->setLegendFontSize( 12 );
    ui->acc->wave->setDefaultFontSize( 12 );
    ui->acc->wave->setShowCursor( true );
    ui->acc->wave->setShowRightCursor( true );
    ui->acc->wave->setXName( "time[sec]" );
    ui->acc->wave->setNames( QStringList() << "x[G]" << "y[G]" << "z[G]" );
    ui->acc->wave->setColors( QList<QColor>() << Qt::red << Qt::darkGreen << Qt::blue );

    connect( ui->buttonBox, SIGNAL(accepted()), this, SLOT(okClicked()) );
    connect( ui->xScaleSpinBox, SIGNAL(valueChanged(int)), ui->acc->wave, SLOT(setXScale(int)) );

    connect( ui->acc->wave, SIGNAL(moveCursor(QPair<int,QVector<double> >)), this, SLOT(cursorChanged(QPair<int,QVector<double> >)) );
    connect( ui->acc->wave, SIGNAL(moveRightCursor(QPair<int,QVector<double> >)), this, SLOT(rightCursorChanged(QPair<int,QVector<double> >)) );

    // カーソル位置初期化
    cursorValue = QPair<int, QVector<double > >( -1, QVector<double>() );
    rightCursorValue = QPair<int, QVector<double > >( -1, QVector<double>() );
}

DataSelectDialog::~DataSelectDialog()
{
    delete ui;
}

void DataSelectDialog::setTable(TableDataReader *accReader, double xUnit)
{
    this->accReader = accReader;
    this->xUnit = xUnit;

    // Read all data
    ui->acc->wave->setUpSize( 3, accReader->tableData.size() );

    ui->acc->wave->clearQueue();

    for ( auto elem : accReader->tableData ) {
        elem[0] = elem[0] * xUnit;

        ui->acc->wave->enqueueData( elem, false );
    }

    ui->acc->wave->moveHeadToHead( true, true );

    // 最後の時間軸
    if ( ui->acc->wave->getQueueSize() > 0 ) {
        endXValue = ui->acc->wave->getHeadValue().second[0];
    } else {
        endXValue = -1;
    }
}

quint32 DataSelectDialog::getZeroStartTime()
{
    if ( ui->acc->wave->getValidCursor() ) {
        return cursorValue.second[0] / xUnit;
    }

    return -1;
}

quint32 DataSelectDialog::getZeroEndTime()
{
    if ( ui->acc->wave->getValidCursor() ) {
        return ui->acc->wave->getShiftedCursorValue( zeroCount, true ).second[0] / xUnit;
    }

    return -1;
}

quint32 DataSelectDialog::getEndTime()
{
    if ( ui->acc->wave->getValidRightCursor() ) {
        return rightCursorValue.second[0] / xUnit;
    }

    return -1;
}

int DataSelectDialog::getZeroCount() const
{
    return zeroCount;
}

void DataSelectDialog::setZeroCount(int value)
{
    zeroCount = value;
    ui->zeroCountSpinBox->setValue( value );
}

double DataSelectDialog::getG() const
{
    return G;
}

void DataSelectDialog::setG(double value)
{
    G = value;
    ui->gEdit->setText( QString::number( value, 'f', 10 ) );
}

void DataSelectDialog::updateFilter()
{
    // グラフのフィルターを更新する
    ui->acc->wave->clearColorFilter();

    // 両カーソル選択されてたらフィルター追加
    if ( ui->acc->wave->getValidCursor() && ui->acc->wave->getValidRightCursor() ) {
        double right = cursorValue.second[0];
        double left  = rightCursorValue.second[0];

        ui->acc->wave->addColorFilter( left, right, QColor( 0, 0, 255, 30 ) );
    }

    // ゼロ範囲フィルター追加
    if ( ui->acc->wave->getValidCursor() ) {
        double right = cursorValue.second[0];
        auto shifted = ui->acc->wave->getShiftedCursorValue( zeroCount, true );

        ui->acc->wave->addColorFilter( right, shifted.second[0], QColor( 255, 0, 0, 70 ) );
    }

    /*
    // 左カーソルフィルター作成
    if ( ui->acc->wave->getValidCursor() ) {
        double right = cursorValue.second[0];
        double left  = 0;

        ui->acc->wave->addColorFilter( left, right, QColor( 0, 0, 0, 50 ) );

        // ゼロゾーン表示用フィルターも作る
        auto shifted = ui->acc->wave->getShiftedCursorValue( zeroCount, true );

        ui->acc->wave->addColorFilter( right, shifted.second[0], QColor( 255, 0, 0, 100 ) );
    }

    // 右カーソルフィルター
    if ( ui->acc->wave->getValidRightCursor() ) {
        double right = endXValue;
        double left  = rightCursorValue.second[0];

        ui->acc->wave->addColorFilter( left, right, QColor( 0, 0, 0, 50 ) );
    }
    */

    ui->acc->wave->update();
}

void DataSelectDialog::cursorChanged(QPair<int, QVector<double> > data)
{
    cursorValue = data;
    updateFilter();
}

void DataSelectDialog::rightCursorChanged(QPair<int, QVector<double> > data)
{
    rightCursorValue = data;
    updateFilter();
}

void DataSelectDialog::okClicked()
{
    // 範囲指定が行われているか確認
    if ( !ui->acc->wave->getValidCursor() || !ui->acc->wave->getValidRightCursor() ) {
        QMessageBox::information( this, tr( "Information" ), tr( "Please select data range" ) );

        return;
    }

    // 左右がクロスしてないか、重なってないか確認
    if ( getZeroStartTime() >= getEndTime() ) {
        QMessageBox::information( this, tr( "Information" ), tr( "Start time is greater than or equal to End time\nPlease select right data range " ) );

        return;
    }

    // ゼロ終了点がデータ終了点を追い越していないか
    if ( getZeroEndTime() >= getEndTime() ) {
        QMessageBox::information( this, tr( "Information" ), tr( "Zero end time is greater than or equal to Data end time\nPlease select right data range " ) );

        return;
    }

    accept();
}

void DataSelectDialog::on_zeroCountSpinBox_valueChanged(int arg1)
{
    setZeroCount( arg1 );
    updateFilter();
}

void DataSelectDialog::on_gEdit_textChanged(const QString &arg1)
{
    bool ok;

    double v = arg1.toDouble( &ok );

    if ( ok ) {
        G = v;
    }
}
