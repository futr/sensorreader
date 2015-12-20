#include "csvwritefileworker.h"

CSVWriteFileWorker::CSVWriteFileWorker(QObject *parent) :
    QObject(parent),
    m_running( false )
{
}

bool CSVWriteFileWorker::error()
{
    // Error is occurred
    return m_error;
}

void CSVWriteFileWorker::setup()
{
    // 初期化など
    m_running = true;
}

qint64 CSVWriteFileWorker::getProgress()
{
    return m_progress;
}

bool CSVWriteFileWorker::isRunning()
{
    // 走っているか
    return m_running;
}

void CSVWriteFileWorker::setParameter(QFile *logFile, QMap<int, QList<AbstractDataFilter *> > filterMap )
{
    // Set the parameter
    m_logFile = logFile;
    m_filterMap = filterMap;
}

void CSVWriteFileWorker::doSaveFile( void )
{
    // Save readFile to writeFile
    QElapsedTimer elapsed;
    qint64 beforePos;

    // init
    m_progress = 0;
    stopFlag   = false;
    m_error    = false;
    m_running  = true;
    beforePos  = 0;

    elapsed.start();

    // Init fp
    m_logFile->seek( 2 );

    // Save
    while ( !m_logFile->atEnd() ) {
        QByteArray header;
        QByteArray data;
        QByteArray signatureBuf;
        int signature;
        int devID;
        int dataSize;
        quint32 step;

        // 停止フラグ確認
        if ( stopFlag ) {
            break;
        }

        // シグネチャ読み込み
        signatureBuf = m_logFile->read( 1 );

        // サイズチェック
        if ( signatureBuf.size() < 1 ) {
            qWarning( "シグネチャ読み込み失敗" );

            m_error = true;

            break;
        }

        signature = (unsigned char)signatureBuf[0];

        // シグネチャチェック
        if ( signature == 0 || signature == LOG_END_SIGNATURE ) {
            // シグネチャ == 0,LOG_END_SIGNATUREなので終了
            break;
        } else if ( signature != LOG_SIGNATURE && signature != LOG_SIGNATURE_OLD ) {
            // qWarning( "シグネチャが一致しない : sig(%d) id(%d) size(%d) pos(%d)", signature, devID, dataSize, (int)m_logFile->pos() );

            m_error = true;

            break;
        }

        // Read and parse header
        header = m_logFile->read( 6 );

        // サイズチェック
        if ( header.size() != 6 ) {
            m_error = true;

            break;
        }

        step      = qFromLittleEndian<quint32>( (uchar *)header.data() );
        devID     = (unsigned char)header[4];
        dataSize  = (unsigned char)header[5];

        // データー部分読み込み
        data = m_logFile->read( dataSize );

        // もしGPSデーターだった場合は，サイズが適切ではない可能性があるので次のシグネチャまで読む
        if ( devID == ID_GPS ) {
            while ( 1 ) {
                QByteArray buf = m_logFile->read( 1 );

                if ( buf.size() < 1 ) {
                    break;
                }

                // ファイル終端かシグネチャ発見
                if ( (unsigned char)buf[0] == LOG_SIGNATURE || (unsigned char)buf[0] == 0 ||
                     (unsigned char)buf[0] == LOG_SIGNATURE_OLD || (unsigned char)buf[0] == LOG_END_SIGNATURE ) {
                    m_logFile->seek( m_logFile->pos() - 1 );

                    break;
                }

                data += buf;
            };

            // GPSのデーターが指定サイズよりも多かったためサイズ修正
            if ( dataSize != data.size() ) {
                // qWarning( "GPSデーターオーバーフロー : %d", data.size() );

                dataSize = data.size();
            }
        }

        // 読み込めなければ失敗
        if ( data.size() != dataSize ) {
            qWarning( "データーが足りない" );

            m_error = true;

            break;
        }

        // フィルター適用
        QList<AbstractDataFilter *> filterList;

        filterList = m_filterMap[devID];

        // 対応するフィルターがなかった
        if ( 0 == filterList.count() ) {
            // qWarning( "There was no match filter : %d", devID );

            continue;
        }

        // フィルター処理
        foreach ( AbstractDataFilter *filter,  filterList ) {
            filter->parseAndSave( step, data );
        }

        // 進捗設定
        m_progress = m_logFile->pos() / 1024;

        // 進捗シグナル
        if ( elapsed.hasExpired( 100 ) ) {
            emit progress( m_progress, m_logFile->size() / 1024, ( m_progress - beforePos ) * 1024 * 10, m_logFile->fileName() );

            beforePos = m_progress;

            elapsed.start();
        }
    }

    // emit error
    if ( m_error ) {
        emit errorOccurred();
    }

    emit finished();

    m_running = false;
}

void CSVWriteFileWorker::stopSave()
{
    // Stop saving
    stopFlag = true;
}
