#include "analyzeworker.h"

AnalyzeWorker::AnalyzeWorker(QObject *parent) : QObject(parent),
    m_error( false ),
    m_progress( 0 ),
    m_running( false )
{

}

bool AnalyzeWorker::error()
{
    return m_error;
}

int AnalyzeWorker::getProgress()
{
    return m_progress;
}

bool AnalyzeWorker::isRunning()
{
    return m_running;
}

void AnalyzeWorker::setParameter(TableDataReader *accReader, TableDataReader *gyroReader, TableDataReader *magReader, TableDataReader *pressureReader, TableDataReader *tempReader,
                                 QFile *analyzedFile,
                                 bool useMag, bool usePressure, bool useTemp,
                                 double xUnit,
                                 int zeroSampleCount, quint32 zeroStartTime, quint32 zeroEndTime, quint32 endTime, double gVal)
{
    this->accReader = accReader;
    this->gyroReader = gyroReader;
    this->magReader = magReader;
    this->pressureReader = pressureReader;
    this->tempReader = tempReader;

    this->analyzedFile = analyzedFile;

    this->zeroSampleCount = zeroSampleCount;
    this->zeroStartTime = zeroStartTime;
    this->zeroEndTime = zeroEndTime;
    this->endTime = endTime;
    this->gVal = gVal;
    this->xUnit = xUnit;

    this->useMag = useMag;
    this->usePressure = usePressure;
    this->useTemp = useTemp;
}

void AnalyzeWorker::setup()
{
}

void AnalyzeWorker::doWork()
{
    // 仕事を始める

    // init
    stopFlag   = false;
    m_error    = false;
    m_progress = 0;
    m_running  = true;

    // プログレスバーはうまく表現できないのでぐるぐぐるしてもらう
    emit progress( 0, 0, 0, tr( "Calculating" ), true );

    ////// 解析開始
    do {  // 一回だけ回せばいい while ( false )
        QTextStream stream( analyzedFile );

        // 処理方法クオータニオンを使う?
        bool useQuaternion = true;

        QVector3D avrZeroGyro;
        QVector3D avrZeroAcc;
        QVector<double> timeVector;
        QVector<double> offsetedAccLength;
        QVector<double> modifiedAccLength;
        QVector<QVector3D> rotateAxis( 3 );
        QVector<QVector3D> minusRotateAxis( 3 );
        QList<QVector3D> mag;
        QList<QVector<double> > pressure;
        QList<QVector<double> > temp;
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
        QList<QQuaternion> qList;
        QVector<double> zeroAccX( zeroSampleCount );
        QVector<double> zeroAccY( zeroSampleCount );
        QVector<double> zeroAccZ( zeroSampleCount );

        bool exist;
        int dataStartRow;
        int dataEndRowAcc, dataEndRowGyro, dataEndRow;
        int zeroStartRow;
        int zeroEndRow;
        int dataCount;

        // 磁場読み込み
        if ( useMag ) {
            magReader->getRowFromColumnValue( 0, zeroStartTime, TableDataReader::ProportionalValue, &exist, &zeroStartRow );
            magReader->getRowFromColumnValue( 0, zeroEndTime, TableDataReader::ProportionalValue, &exist, &zeroEndRow );

            for ( int i = zeroStartRow; i <= zeroEndRow; i++ ) {
                mag << rowToVec3D( magReader->getRow( i ) );
            }
        }

        // 気圧読み込み
        if ( usePressure ) {
            pressureReader->getRowFromColumnValue( 0, zeroStartTime, TableDataReader::ProportionalValue, &exist, &zeroStartRow );
            pressureReader->getRowFromColumnValue( 0, zeroEndTime, TableDataReader::ProportionalValue, &exist, &zeroEndRow );

            for ( int i = zeroStartRow; i <= zeroEndRow; i++ ) {
                pressure << pressureReader->getRow( i );
            }
        }

        // 温度読み込み
        if ( useTemp ) {
            tempReader->getRowFromColumnValue( 0, zeroStartTime, TableDataReader::ProportionalValue, &exist, &zeroStartRow );
            tempReader->getRowFromColumnValue( 0, zeroEndTime, TableDataReader::ProportionalValue, &exist, &zeroEndRow );

            for ( int i = zeroStartRow; i <= zeroEndRow; i++ ) {
                temp << tempReader->getRow( i );
            }
        }

        // Calc zeroGyro
        gyroReader->getRowFromColumnValue( 0, zeroStartTime, TableDataReader::ProportionalValue, &exist, &zeroStartRow );
        gyroReader->getRowFromColumnValue( 0, zeroEndTime, TableDataReader::ProportionalValue, &exist, &zeroEndRow );

        for ( int i = zeroStartRow; i <= zeroEndRow; i++ ) {
            avrZeroGyro += rowToVec3D( gyroReader->getRow( i ) );
        }

        avrZeroGyro /= ( zeroEndRow - zeroStartRow + 1 );

        // Calc zeroAcc
        accReader->getRowFromColumnValue( 0, zeroStartTime, TableDataReader::ProportionalValue, &exist, &zeroStartRow );
        accReader->getRowFromColumnValue( 0, zeroEndTime, TableDataReader::ProportionalValue, &exist, &zeroEndRow );

        for ( int i = zeroStartRow; i <= zeroEndRow; i++ ) {
            avrZeroAcc += rowToVec3D( accReader->getRow( i ) );
        }

        avrZeroAcc /= ( zeroEndRow - zeroStartRow + 1 );

        // make zeroAcc
        zeroAccX = accReader->getColumnVector( 1, zeroStartRow, zeroSampleCount );
        zeroAccY = accReader->getColumnVector( 2, zeroStartRow, zeroSampleCount );
        zeroAccZ = accReader->getColumnVector( 3, zeroStartRow, zeroSampleCount );

        // Start integral
        // DEBUG Change data start row to zeroStartRow
        dataStartRow = zeroStartRow;
        accReader->getRowFromColumnValue( 0, endTime, TableDataReader::ProportionalValue, &exist, &dataEndRowAcc );
        gyroReader->getRowFromColumnValue( 0, endTime, TableDataReader::ProportionalValue, &exist, &dataEndRowGyro );

        dataEndRow = qMin( dataEndRowAcc, dataEndRowGyro );
        dataCount    = dataEndRow - dataStartRow + 1;

        // Create time vector;
        timeVector.resize( dataCount );

        for ( int i = dataStartRow; i <= dataEndRow; i++ ) {
            timeVector[i - dataStartRow] = gyroReader->getRow( i )[0] * 0.0001;
        }

        // calc dgyro
        for ( int i = dataStartRow; i <= dataEndRow; i++ ) {
            if ( i == dataStartRow ) {
                dGyro.append( QVector3D( 0, 0, 0 ) );
            } else {
                double time;

                time = ( gyroReader->getRow( i )[0] - gyroReader->getRow( i - 1 )[0] ) * 0.0001;

                dGyro.append( ( QVector3D( rowToVec3D( gyroReader->getRow( i - 1 ) ) ) - avrZeroGyro ) * time );
            }
        }

        // クオータニオンをつかってみる
        if ( useQuaternion ) {
            for ( int i = dataStartRow; i <= dataEndRow; i++ ) {
                if ( i == dataStartRow ) {
                    qList.append( QQuaternion() );
                } else {
                    // クオータニオンの計算
                    // 角速度ベクトルを作る( 角度に変換済みのを角速度に戻してラジアンに )
                    // 本来4x3行列だけど，めんどくさいので4x4を使う
                    double time;
                    QVector4D omega( ( rowToVec3D( gyroReader->getRow( i - 1 ) ) - avrZeroGyro ) / 180.0 * M_PI );
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
                    time = ( gyroReader->getRow( i )[0] - gyroReader->getRow( i - 1 )[0] ) * 0.0001;

                    q = q + ( dq * time );
                    q.normalize();

                    qList.append( q );
                }
            }
        }

        // ひとまず終了フラグ確認
        if ( stopFlag ) {
            break;
        }

        // 回転する単位ベクトル軸を，初期重力加速度を元に地面を推定して回転
        rotateAxis = groundAxisFromG( avrZeroAcc );
        minusRotateAxis = rotateAxis;

        for ( int i = 0; i < dataCount; i++ ) {
            // Create translate matrix
            QMatrix4x4 trans, minusTrans;

            trans.setToIdentity();
            trans.rotate( dGyro[i].x(), 1, 0, 0 );
            trans.rotate( dGyro[i].y(), 0, 1, 0 );
            trans.rotate( dGyro[i].z(), 0, 0, 1 );
            dTranslate.append( trans );

            minusTrans.setToIdentity();
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

            if ( i == 0 ) {
                unitX.append( rotateAxis[0] );
                unitY.append( rotateAxis[1] );
                unitZ.append( rotateAxis[2] );
            } else {
                if ( useQuaternion ) {
                    QQuaternion q;

                    q = qList[i];

                    transRotateAxis.setToIdentity();
                    transRotateAxis.rotate( q );

                    QVector3D rx = transRotateAxis * QVector3D( 1, 0, 0 );
                    QVector3D ry = transRotateAxis * QVector3D( 0, 1, 0 );
                    QVector3D rz = transRotateAxis * QVector3D( 0, 0, 1 );

                    QVector3D ux = rotateAxis[0] * rx.x() + rotateAxis[1] * rx.y() + rotateAxis[2] * rx.z();
                    QVector3D uy = rotateAxis[0] * ry.x() + rotateAxis[1] * ry.y() + rotateAxis[2] * ry.z();
                    QVector3D uz = rotateAxis[0] * rz.x() + rotateAxis[1] * rz.y() + rotateAxis[2] * rz.z();

                    unitX.append( ux );
                    unitY.append( uy );
                    unitZ.append( uz );
                } else {
                    transRotateAxis.setToIdentity();
                    transRotateAxis.rotate( dGyro[i].x(), rotateAxis[0] );
                    transRotateAxis.rotate( dGyro[i].y(), rotateAxis[1] );
                    transRotateAxis.rotate( dGyro[i].z(), rotateAxis[2] );

                    rotateAxis[0] = transRotateAxis * rotateAxis[0];
                    rotateAxis[1] = transRotateAxis * rotateAxis[1];
                    rotateAxis[2] = transRotateAxis * rotateAxis[2];

                    unitX.append( rotateAxis[0] );
                    unitY.append( rotateAxis[1] );
                    unitZ.append( rotateAxis[2] );
                }
            }
        }

        // ひとまず終了フラグ確認
        if ( stopFlag ) {
            break;
        }

        // calc offseted acc
        for ( int i = dataStartRow; i <= dataEndRow; i++ ) {
            QVector3D acc( rowToVec3D( accReader->getRow( i ) ) );

            // DEBUG Do not offset
            offsetedAcc.append( acc );
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

        // ひとまず終了フラグ確認
        if ( stopFlag ) {
            break;
        }

        // DEBUG : Create velocity and position by rotated acc vector
        // calc velocity
        // 三角形部分の値も計算することで積分するか、しないかを以下で切り替えてる。
        // とりあえず今は三角形をつかう
        // もともとはクオータニオン版でのみ使ってた
        velocity.append( QVector3D( 0, 0, 0 ) );

        if ( useQuaternion ) {
            for ( int i = 1; i < dataCount; i++ ) {
                QVector3D acc1( modifiedAcc[i - 1] );
                QVector3D acc2( modifiedAcc[i] );
                QVector3D rotatedAvrAcc( rotateAxis[0] * avrZeroAcc.x() + rotateAxis[1] * avrZeroAcc.y() + rotateAxis[2] * avrZeroAcc.z() );

                acc1 = unitX[i - 1] * acc1.x() + unitY[i - 1] * acc1.y() + unitZ[i - 1] * acc1.z();
                acc2 = unitX[i] * acc2.x() + unitY[i] * acc2.y() + unitZ[i] * acc2.z();

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
            }
        } else {
            for ( int i = 1; i < dataCount; i++ ) {
                QVector3D acc( modifiedAcc[i - 1] );
                QVector3D rotatedAvrAcc( 0, 0, avrZeroAcc.length() );

                acc = unitX[i] * acc.x() + unitY[i] * acc.y() + unitZ[i] * acc.z();
                velocity.append( gVal * ( acc - rotatedAvrAcc ) * ( timeVector[i] - timeVector[i - 1] ) + velocity[i - 1] );
            }

            // calc position
            position.append( QVector3D( 0, 0, 0 ) );

            for ( int i = 1; i < dataCount; i++ ) {
                QVector3D vel( velocity[i - 1] );

                position.append( vel * ( timeVector[i] - timeVector[i - 1] ) + position[i - 1] );
            }
        }

        // ひとまず終了フラグ確認
        if ( stopFlag ) {
            break;
        }

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
                  tr( "VLen[m/s]" ) << "," <<
                  tr( "Magnetic fieldX[µT]" ) << "," <<
                  tr( "Magnetic fieldY[µT]" ) << "," <<
                  tr( "Magnetic fieldZ[µT]" ) << "," <<
                  tr( "Pressure[hPa]" ) << "," <<
                  tr( "Temperature[℃]" ) << "\n";

        // data outpu loop
        for ( int i = 0; i < dataCount; i++ ) {
            QVector3D magV;
            double pressV, tempV;

            if ( useMag && i < mag.count() ) {
                magV = mag[i];
            } else {
                magV = QVector3D( 0, 0, 0 );
            }

            if ( usePressure && i < pressure.count() ) {
                pressV = pressure[i][1];
            } else {
                pressV = 0;
            }

            if ( useTemp && i < temp.count() ) {
                tempV = temp[i][1];
            } else {
                tempV = 0;
            }

            stream << timeVector[i]   << "," <<
                      position[i].x() << "," <<
                      position[i].y() << "," <<
                      position[i].z() << "," <<
                      position[i].length() << "," <<
                      velocity[i].x() << "," <<
                      velocity[i].y() << "," <<
                      velocity[i].z() << "," <<
                      velocity[i].length() << "," <<
                      magV.x() << "," <<
                      magV.y() << "," <<
                      magV.z() << "," <<
                      pressV << "," <<
                      tempV << "\n";
        }
    } while ( false );

    // 終了フラグはエラーとする
    m_error = stopFlag;

    // 仕事終了処理開始
    if ( m_error ) {
        emit errorOccurred();
    }

    emit finished();

    m_running = false;
}

void AnalyzeWorker::stopWork()
{
    stopFlag = true;
}

QVector3D AnalyzeWorker::rowToVec3D(QVector<double> row)
{
    QVector3D ret;

    ret.setX( row[1] );
    ret.setY( row[2] );
    ret.setZ( row[3] );

    return ret;
}

QVector<double> AnalyzeWorker::getColumnVector(const QList<QVector3D> &list, int column, int startRow, int count)
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

QVector<double> AnalyzeWorker::getColumnVector(const QVector<double> &vector, int startRow, int count)
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

double AnalyzeWorker::vectorLength(const QVector<double> &vector)
{
    double len = 0;

    for ( auto && elem : vector ) {
        len += elem * elem;
    }

    return qSqrt( len );
}

QVector<QVector3D> AnalyzeWorker::groundAxisFromG(QVector3D g)
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

double AnalyzeWorker::calcArea(QVector2D p1, QVector2D p2)
{
    // 二つの点の間の積分
    // 三角形で近似する
    // xが時間，yが値としてる

    return ( p2.y() - p1.y() ) * ( p2.x() - p1.x() ) / 2 + ( p1.y() * ( p2.x() - p1.x() ) );
}
