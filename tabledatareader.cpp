#include "tabledatareader.h"

TableDataReader::TableDataReader()
{

}

TableDataReader::~TableDataReader()
{

}

bool TableDataReader::readFile(QString fileName, TableDataReader::DelimiterType delimiter, TableDataReader::HeaderMode headerMode)
{
    // read a file
    QFile file;
    QTextStream stream;
    int index;

    // Open the file
    file.setFileName( fileName );

    if ( !file.open( QFile::ReadOnly | QIODevice::Text ) ) {
        return false;
    }

    // Init stream
    stream.setDevice( &file );

    // Clear
    tableData.clear();
    m_header.clear();

    // Read all lines
    index = 0;

    while ( !stream.atEnd() ) {
        QString line;
        QVector<double> rowValue;
        QStringList row;

        line = stream.readLine();

        // Sprit line
        switch ( delimiter ) {
        case Comma:
            row = line.split( "," );
            break;

        case Space:
            row = line.split( QRegExp("\\s+") );
            break;

        default:
            row = line.split( "," );
            break;
        }

        // First row
        if ( index == 0 ) {
            if ( headerMode == Title ) {
                // Header is title
                m_header = row;
            }
        } else {
            // Create the row
            rowValue.resize( row.count() );

            for ( int i = 0; i < row.count(); i++ ) {
                rowValue[i] = row[i].toDouble();
            }

            // Add row
            tableData.append( rowValue );
        }

        // Increment index
        index++;
    }

    // Close file
    file.close();

    // Reset cursor and check count
    resetCursor();

    if ( tableData.count() == 0 || columnCount() == 0 ) {
        return false;
    }

    return true;
}

bool TableDataReader::readList(QList<QVector<double> > &list, QStringList header)
{
    // ベクタのリストを読み込む
    tableData = list;
    m_header  = header;

    return true;
}

bool TableDataReader::output(QIODevice *device, bool includeHeader)
{
    // 任意のデバイスにテーブルを出力する ( 今は疑似csvのみ )
    QTextStream stream;

    // Error check
    if ( !( device->openMode() & QIODevice::WriteOnly ) ) {
        return false;
    }

    if ( !device->isOpen() ) {
        return false;
    }

    if ( columnCount() == 0 ) {
        return false;
    }

    // Create delimiter output lambda
    auto outputDelimiter = [&]( int i ) {
        if ( i < columnCount() - 1 ) {
            stream << ",";
        } else {
            stream << "\n";
        }
    };

    // Init stream
    stream.setDevice( device );

    // Output header
    if ( includeHeader ) {
        for ( int i = 0; i < columnCount(); i++ ) {
            if ( i < m_header.count() ) {
                stream << m_header[i];
            }

            outputDelimiter( i );
        }
    }

    // Output table
    for ( auto vec : tableData ) {
        for ( int i = 0; i < vec.count(); i++ ) {
            stream << QString::number( vec[i] );

            outputDelimiter( i );
        }
    }

    return true;
}

QVector<double> TableDataReader::getRow(int row)
{
    // Get a row
    return tableData[row];
}

QVector<double> TableDataReader::getRowFromColumnValue(int column, double value, TableDataReader::EstimateMode estimateMode, bool *exist, int *index)
{
    // Get a row from column value
    int dir;
    int posRow;
    double rate;
    bool hit;
    QVector<double> ret( columnCount() );

    // Check before cursor
    if ( column == currentColumn ) {
        posRow = currentRow;

        // Set search direction
        if ( currentValue < value ) {
            dir = 1;
        } else {
            dir = -1;
        }
    } else {
        posRow = 0;
        dir = 1;
    }

    // Search
    hit = false;

    while ( posRow >= 0 && posRow < rowCount() ) {
        if ( dir == 1 ) {
            if ( tableData[posRow][column] == value ) {
                // Found
                hit = true;

                break;
            } else if ( tableData[posRow][column] > value ) {
                // Over
                hit = false;

                posRow--;

                break;
            }
        } else {
            if ( tableData[posRow][column] == value ) {
                // Found
                hit = true;

                break;
            } else if ( tableData[posRow][column] < value ) {
                // Under
                hit = false;

                break;
            }
        }

        // next
        posRow += dir;
    }

    // reset pos
    if ( posRow < 0 ) {
        posRow = 0;
    } else if ( posRow >= rowCount() ) {
        posRow = rowCount() - 1;
    }

    // Set current cursor
    currentColumn = column;
    currentRow    = posRow;
    currentValue  = tableData[posRow][column];

    if ( index != nullptr ) {
        *index = posRow;
    }

    // Hit
    if ( hit ) {
        if ( exist != nullptr ) {
            *exist = true;
        }

        return tableData[posRow];
    }

    if ( exist != nullptr ) {
        *exist = false;
    }

    // Estimate
    switch ( estimateMode ) {
    case PreviousValue:
        return tableData[posRow];
        break;

    case ProportionalValue:
        // Top or bottom
        if ( posRow == 0 || posRow == rowCount() - 1 ) {
            return tableData[posRow];
        }

        // Proportional
        for ( int i = 0; i < columnCount(); i++ ) {
            rate = ( tableData[posRow + 1][i] - tableData[posRow][i] ) / ( tableData[posRow + 1][column] - tableData[posRow][column] );
            ret[i] = rate * ( value - tableData[posRow][column] ) + tableData[posRow][i];
        }

        return ret;
        break;

    default:
        return ret;
        break;
    }
}

QStringList TableDataReader::getHeader()
{
    // Return header string list
    return m_header;
}

int TableDataReader::columnIndexFromName(QString name)
{
    // Get column index by column name
    return m_header.indexOf( name );
}

QVector<double> TableDataReader::getColumnVector(int column, int startRow, int count)
{
    // Get column vector
    QVector<double> ret( count );

    for ( int i = 0; i < count; i++ ) {
        int row = i + startRow;
        if ( row < 0 || row >= rowCount() ) {
            ret[i] = 0;
        } else {
            ret[i] = tableData[row][column];
        }
    }

    return ret;
}

int TableDataReader::rowCount()
{
    // Return number of row
    return tableData.count();
}

int TableDataReader::columnCount()
{
    // Return number of column
    if ( tableData.count() > 0 ) {
        return tableData[0].count();
    } else {
        return 0;
    }
}

void TableDataReader::resetCursor()
{
    // カーソルを初期位置に
    currentColumn = 0;
    currentRow = 0;

    if ( tableData.count() > 0 && columnCount() > 0 ) {
        currentValue = tableData[0][0];
    } else {
        currentValue = 0;
    }
}

