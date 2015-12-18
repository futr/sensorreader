#include "abstractdatafilter.h"

AbstractDataFilter::AbstractDataFilter(QObject *parent) :
    QObject(parent),
    m_fileName("default.csv"),
    m_filterName("DefaultFilter")
{
}

AbstractDataFilter::~AbstractDataFilter()
{
    // デストラクタ－起動時にファイルが開かれていれば閉じる
    if ( m_file.isOpen() ) {
        m_file.close();
    }
}

QString AbstractDataFilter::getFileName()
{
    // ファイル名を返す
    return m_fileName;
}

void AbstractDataFilter::setFileName(QString name)
{
    // ファイル名を設定
    m_fileName = name;
}

QVector<double> AbstractDataFilter::parseToVector(unsigned int step, QByteArray data)
{
    // Parse data to double vector
    Q_UNUSED( step )
    Q_UNUSED( data )

    return QVector<double>();
}

void AbstractDataFilter::makeHeader()
{
    // ヘッダを作る
}

bool AbstractDataFilter::openFile(QString path)
{
    // ファイルを開く
    bool opened;

    m_file.setFileName( path + "/" + m_fileName );

    opened = m_file.open( QFile::WriteOnly );

    if ( !opened ) {
        return false;
    }

    // ヘッダ作成
    makeHeader();

    return true;
}

void AbstractDataFilter::closeFile()
{
    // ファイルを閉じる
    m_file.close();
}

int AbstractDataFilter::getFilterID()
{
    // フィルターIDを返す
    return m_filterID;
}
