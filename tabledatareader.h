#ifndef TABLEDATAREADER_H
#define TABLEDATAREADER_H

#include <QVector>
#include <QString>
#include <QList>
#include <QFile>
#include <QTextStream>
#include <QStringList>

class TableDataReader
{
public:
    enum HeaderMode {
        Title,
        Data,
    };

    enum DelimiterType {
        Comma,
        Space,
    };

    enum EstimateMode {
        PreviousValue,
        ProportionalValue,
    };

    // Functions
    TableDataReader();
    ~TableDataReader();

    bool readFile( QString fileName, DelimiterType delimiter, HeaderMode headerMode );
    bool readList( QList< QVector<double> > &list, QStringList header = QStringList() );
    bool output( QIODevice *device, bool includeHeader = true );

    QVector<double> getRow( int row );
    QVector<double> getRowFromColumnValue( int column, double value, EstimateMode estimateMode, bool *exist, int *index );
    QStringList getHeader();
    int columnIndexFromName( QString name );

    QVector<double> getColumnVector( int column, int startRow, int count );

    int rowCount();
    int columnCount();

    void resetCursor();

// private:
    int currentRow;
    int currentColumn;
    double currentValue;

    QList< QVector<double> > tableData;
    QStringList m_header;
};

#endif // TABLEDATAREADER_H
