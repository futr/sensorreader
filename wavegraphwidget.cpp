#include "wavegraphwidget.h"

WaveGraphWidget::WaveGraphWidget(QWidget *parent) : QWidget(parent)
{
    // Set default colors
    setBgColor( Qt::white );
    setGridColor( Qt::gray );
    setZeroGridColor( Qt::black );
    setStrColor( Qt::black );
    setCursorColor( Qt::black );
    setRightCursorColor( Qt::black );

    // Setup default graph status
    setAutoUpdateYMax( false );
    setAutoUpdateYMin( false );

    setYMax( 1 );
    setYMin( -1 );
    setYZero( 0 );
    setXScale( 1 );
    setXGrid( 50 );
    setXName( "x" );

    setUpSize( 1, 10000 );

    resetIterator();

    headIndex = 0;

    validCursor = false;
    validRightCursor = false;

    autoUpdateYMax = false;
    autoUpdateYMin = false;
    autoZeroCenter = false;

    legendOffsetX = 10;
    legendOffsetY = 10;
    legendLineWidth = 2;
    legendLineLength = 30;
    legendFontSize = 20;

    defaultFontSize = 15;

    setCursorWidth( 2 );

    setShowCursor( true );
    setShowRightCursor( false );

    showCursorValue = true;
    showHeadValue   = true;
    showYGridValue  = true;
}

QColor WaveGraphWidget::getBgColor() const
{
    return bgColor;
}

void WaveGraphWidget::setBgColor(const QColor &value)
{
    bgColor = value;
}

QColor WaveGraphWidget::getGridColor() const
{
    return gridColor;
}

void WaveGraphWidget::setGridColor(const QColor &value)
{
    gridColor = value;
}

QColor WaveGraphWidget::getZeroGridColor() const
{
    return zeroGridColor;
}

void WaveGraphWidget::setZeroGridColor(const QColor &value)
{
    zeroGridColor = value;
}

QColor WaveGraphWidget::getStrColor() const
{
    return strColor;
}

void WaveGraphWidget::setStrColor(const QColor &value)
{
    strColor = value;
}

void WaveGraphWidget::setUpSize( int columnCount, int queueSize )
{
    // Setup
    this->columnCount = columnCount;
    this->queueSize = queueSize;

    for ( int i = 0; i < columnCount; i++ ) {
        colors << Qt::blue;
        names << "noname";
    }
}

void WaveGraphWidget::enqueueData( const QVector<double> &data )
{
    enqueueData( data, true );
}

void WaveGraphWidget::enqueueData( const QVector<double> &data, bool updateHead )
{
    // Add data to queue
    bool headmove = false;

    dataQueue.append( data );

    // update iterator
    if ( updateHead ) {
        head = dataQueue.end();
        head--;

        headIndex = dataQueue.size() - 1;

        headmove = true;
    } else if ( dataQueue.size() == 1 ) {
        // head is always valid iterator if dataQueue size is larger than 0
        head = dataQueue.begin();

        headIndex = 0;

        headmove = true;
    }

    // dequeue
    if ( dataQueue.size() > queueSize ) {
        // reset iterator
        if ( cursor == dataQueue.begin() )  cursor++;
        if ( rightCursor == dataQueue.begin() )  rightCursor++;

        headIndex--;

        if ( headIndex < 0 ) headIndex = 0;

        // dequeue
        dataQueue.removeFirst();

        headmove = true;
    }

    emit rangeChanged( 0, dataQueue.size() - 1 );

    if ( headmove ) {
        emit headChanged( headIndex );
    }

    update();
}

double WaveGraphWidget::getXScale() const
{
    return xScale;
}

void WaveGraphWidget::setXScale(double value)
{
    xScale = value;
}

bool WaveGraphWidget::getAutoUpdateYMax() const
{
    return autoUpdateYMax;
}

void WaveGraphWidget::setAutoUpdateYMax(bool value)
{
    autoUpdateYMax = value;
}

bool WaveGraphWidget::getAutoUpdateYMin() const
{
    return autoUpdateYMin;
}

void WaveGraphWidget::setAutoUpdateYMin(bool value)
{
    autoUpdateYMin = value;
}

double WaveGraphWidget::getXGrid() const
{
    return xGrid;
}

void WaveGraphWidget::setXGrid(double value)
{
    xGrid = value;
}

double WaveGraphWidget::getYMax() const
{
    return yMax;
}

void WaveGraphWidget::setYMax(double value)
{
    yMax = value;
}

double WaveGraphWidget::getYMin() const
{
    return yMin;
}

void WaveGraphWidget::setYMin(double value)
{
    yMin = value;
}

double WaveGraphWidget::getYZero() const
{
    return yZero;
}

void WaveGraphWidget::setYZero(double value)
{
    yZero = value;
}

bool WaveGraphWidget::getAutoZeroCenter() const
{
    return autoZeroCenter;
}

void WaveGraphWidget::setAutoZeroCenter(bool value)
{
    autoZeroCenter = value;
}

QList<QColor> WaveGraphWidget::getColors() const
{
    return colors;
}

void WaveGraphWidget::setColors(const QList<QColor> &value)
{
    colors = value;
}

QList<QString> WaveGraphWidget::getNames() const
{
    return names;
}

void WaveGraphWidget::setNames(const QList<QString> &value)
{
    names = value;
}

void WaveGraphWidget::addColorFilter(double left, double right, QColor color)
{
    // Add color filter
    ColorFilter filter( QVector<double>() << left << right, color );

    colorFilterList.append( filter );
}

void WaveGraphWidget::clearColorFilter()
{
    colorFilterList.clear();
}

WaveGraphWidget::DataQueue::iterator WaveGraphWidget::pixPosToIterator(int x)
{
    // Get iterator from widget pix pos X
    double ix;
    auto ret = dataQueue.end();

    // check available
    if ( dataQueue.size() == 0 ) {
        return ret;
    }

    ix = 0;

    for ( auto it = head; true; it-- ) {
        int dataX = width() - ix;

        if ( dataX < 0 ) {
            break;
        } else if ( dataX <= x ) {
            ret = it;
            break;
        }

        if ( it == dataQueue.begin() ) {
            // Exit
            break;
        } else {
            // calc next pix x
            ix += ( (*it)[0] - (*( it - 1 ))[0] ) * xScale;
        }
    }

    return ret;
}

void WaveGraphWidget::resetIterator()
{
    head = dataQueue.end();
    cursor = dataQueue.end();
    rightCursor = dataQueue.end();

    validCursor = false;
    validRightCursor = false;
}

void WaveGraphWidget::updateCursor(int x)
{
    // Update cursor iterator
    cursor = pixPosToIterator( x );

    if ( cursor != dataQueue.end() ) {
        validCursor = true;

        emit moveCursor( QPair<int, QVector<double> >( getCurrentPixXFromRawX( (*cursor)[0] ), *cursor ) );
    } else {
        validCursor = false;
    }
}

void WaveGraphWidget::updateRightCursor(int x)
{
    // Update right cursor iterator
    rightCursor = pixPosToIterator( x );

    if ( rightCursor != dataQueue.end() ) {
        validRightCursor = true;

        emit moveCursor( QPair<int, QVector<double> >( getCurrentPixXFromRawX( (*rightCursor)[0] ), *rightCursor ) );
    } else {
        validRightCursor = false;
    }
}

int WaveGraphWidget::getCurrentPixXFromRawX(double x)
{
    // Calculate display pix x from raw x
    if ( dataQueue.size() == 0 ) return 0;

    double headX = (*head)[0];

    return width() - ( headX - x ) * xScale;
}
int WaveGraphWidget::getDefaultFontSize() const
{
    return defaultFontSize;
}

void WaveGraphWidget::setDefaultFontSize(int value)
{
    defaultFontSize = value;
}


void WaveGraphWidget::clearQueue()
{
    // Clear queue data
    resetIterator();

    dataQueue.clear();

    update();
}
int WaveGraphWidget::getLegendFontSize() const
{
    return legendFontSize;
}

void WaveGraphWidget::setLegendFontSize(int value)
{
    legendFontSize = value;
}

bool WaveGraphWidget::getShowYGridValue() const
{
    return showYGridValue;
}

void WaveGraphWidget::setShowYGridValue(bool value)
{
    showYGridValue = value;
}

QPair<int, QVector<double> > WaveGraphWidget::getCursorValue()
{
    // Return cursor value
    if ( validCursor ) {
        return QPair<int, QVector<double> >( getCurrentPixXFromRawX( (*cursor)[0] ), *cursor );
    } else {
        return QPair<int, QVector<double> >( 0, QVector< double >() );
    }
}

QPair<int, QVector<double> > WaveGraphWidget::getRightCursorValue()
{
    // Return cursor value
    if ( validRightCursor ) {
        return QPair<int, QVector<double> >( getCurrentPixXFromRawX( (*rightCursor)[0] ), *rightCursor );
    } else {
        return QPair<int, QVector<double> >( 0, QVector< double >() );
    }
}

QString WaveGraphWidget::getXName() const
{
    return xName;
}

void WaveGraphWidget::setXName(const QString &value)
{
    xName = value;
}


bool WaveGraphWidget::getShowHeadValue() const
{
    return showHeadValue;
}

void WaveGraphWidget::setShowHeadValue(bool value)
{
    showHeadValue = value;
}

bool WaveGraphWidget::getShowCursorValue() const
{
    return showCursorValue;
}

void WaveGraphWidget::setShowCursorValue(bool value)
{
    showCursorValue = value;
}

int WaveGraphWidget::getHeadIndex() const
{
    return headIndex;
}

void WaveGraphWidget::setHeadIndex(int value)
{
    // Set head index and iterator
    if ( dataQueue.size() == 0 ) {
        headIndex = 0;

        return;
    }

    if ( value == headIndex ) {
        return;
    }

    if ( value < 0 ) {
        value = 0;
    } else if ( value >= dataQueue.size() ) {
        value = dataQueue.size() - 1;
    }

    int diff = value - headIndex;

    // move iterator
    head += diff;

    headIndex = value;

    emit headChanged( value );

    update();
}

void WaveGraphWidget::setHeadFromRawX(double x, const MoveMode mode)
{
    // Move head to real x nearest iterator
    if ( dataQueue.size() == 0 ) return;

    double headX = (*head)[0];

    if ( headX == x ) {
        return;
    }

    bool updated = false;

    // Search
    auto newHead = head;
    double beforeX = headX;
    auto beforeHead = head;
    int diff = 0;

    if ( headX < x ) {
        for ( newHead = head; newHead != dataQueue.end(); newHead++, diff++ ) {
            double newHeadX = (*newHead)[0];

            // Nearet values are found
            if ( newHeadX == x ) {
                // Update head
                head = newHead;
                headIndex += diff;

                updated = true;

                break;
            } else if ( newHeadX > x ) {
                // Check nearest and update head
                if ( mode == Nearest ) {
                    if ( qAbs( newHeadX - x ) <= qAbs( beforeX - x ) ) {
                        head = newHead;
                        headIndex += diff;
                    } else {
                        head = beforeHead;
                        headIndex += ( diff - 1 );
                    }
                } else if ( mode == SmallNearest ) {
                    head = beforeHead;
                    headIndex += ( diff - 1 );
                } else if ( mode == LargeNearest ) {
                    head = newHead;
                    headIndex += diff;
                }

                updated = true;

                break;
            }

            beforeX = newHeadX;
            beforeHead = newHead;
        }

        // reach the end of loop
        if ( newHead == dataQueue.end() ) {
            head = dataQueue.end() - 1;
            headIndex += ( diff - 1 );

            updated = true;
        }
    } else {
        for ( newHead = head; true; newHead--, diff++ ) {
            double newHeadX = (*newHead)[0];

            // Nearet values are found
            if ( newHeadX == x ) {
                // Update head
                head = newHead;
                headIndex -= diff;

                updated = true;

                break;
            } else if ( newHeadX < x ) {
                // Check nearest and update head
                if ( mode == Nearest ) {
                    if ( qAbs( newHeadX - x ) <= qAbs( beforeX - x ) ) {
                        head = newHead;
                        headIndex -= diff;
                    } else {
                        head = beforeHead;
                        headIndex -= ( diff - 1 );
                    }
                } else if ( mode == LargeNearest ) {
                    head = beforeHead;
                    headIndex -= ( diff - 1 );
                } else if ( mode == SmallNearest ) {
                    head = newHead;
                    headIndex -= diff;
                }

                updated = true;

                break;
            }

            // end check
            if ( newHead == dataQueue.begin() ) {
                break;
            }

            beforeX = newHeadX;
            beforeHead = newHead;
        }

        // reach the end of loop
        if ( newHead == dataQueue.begin() ) {
            head = dataQueue.begin();
            headIndex -= diff;

            updated = true;
        }
    }

    if ( updated ) {
        update();

        emit headChanged( headIndex );
    }
}

int WaveGraphWidget::getQueueSize()
{
    return dataQueue.size();
}

int WaveGraphWidget::getCursorWidth() const
{
    return cursorWidth;
}

void WaveGraphWidget::setCursorWidth(int value)
{
    cursorWidth = value;
}

void WaveGraphWidget::clearCursor()
{
    validCursor = false;
    cursor = dataQueue.end();
}

void WaveGraphWidget::clearRightCursor()
{
    validRightCursor = false;
    rightCursor = dataQueue.end();
}

QColor WaveGraphWidget::getRightCursorColor() const
{
    return rightCursorColor;
}

void WaveGraphWidget::setRightCursorColor(const QColor &value)
{
    rightCursorColor = value;
}

QColor WaveGraphWidget::getCursorColor() const
{
    return cursorColor;
}

void WaveGraphWidget::setCursorColor(const QColor &value)
{
    cursorColor = value;
}

bool WaveGraphWidget::getShowRightCursor() const
{
    return showRightCursor;
}

void WaveGraphWidget::setShowRightCursor(bool value)
{
    showRightCursor = value;
}

bool WaveGraphWidget::getShowCursor() const
{
    return showCursor;
}

void WaveGraphWidget::setShowCursor(bool value)
{
    showCursor = value;
}

void WaveGraphWidget::paintEvent(QPaintEvent *)
{
    // draw
    QPainter p( this );
    QPen pen;
    QFont font( p.font() );

    // Clear bg
    p.fillRect( rect(), bgColor );

    // Draw x grid
    p.setPen( gridColor );

    for ( double x = 0; x < width(); x += xScale * xGrid ) {
        p.drawLine( width() -  x, 0, width() - x, height() );
    }

    // Check available
    if ( dataQueue.size() < 2 ) {
        return;
    }

    // Create draw queue
    QQueue<QPair<int, QVector<double> > > drawQueue;
    QList<double> minRawY, maxRawY;
    double localMinY, localMaxY;
    double x = 0;

    for ( int i = 0; i < (*head).size(); i++ ) {
        minRawY << (*head)[i];
        maxRawY << (*head)[i];
    }

    for ( auto it = head; true; it-- ) {
        // enqueue
        drawQueue << QPair<int, QVector<double> >( x, *it );

        // min, max
        for ( int i = 0; i < (*it).size(); i++ ) {
            if ( minRawY[i] > (*it)[i] ) {
                minRawY[i] = (*it)[i];
            }

            if ( maxRawY[i] < (*it)[i] ) {
                maxRawY[i] = (*it)[i];
            }
        }

        // check pix x
        if ( x >= width() ) {
            break;
        }

        if ( it == dataQueue.begin() ) {
            // Exit
            break;
        } else {
            // calc next pix x
            x += ( (*it)[0] - (*( it - 1 ))[0] ) * xScale;
        }
    }

    // deceide local min max
    localMinY = minRawY[1];
    localMaxY = maxRawY[1];

    for ( int i = 1; i < minRawY.size(); i++ ) {
        if ( localMinY > minRawY[i] ) {
            localMinY = minRawY[i];
        }

        if ( localMaxY < maxRawY[i] ) {
            localMaxY = maxRawY[i];
        }
    }

    if ( autoZeroCenter ) {
        double localMax = qMax( qAbs( localMinY ), qAbs( localMaxY ) );

        localMinY = -localMax;
        localMaxY = localMax;
    }

    // Draw zero line
    if ( localMinY <= 0 && localMaxY >= 0 ) {
        double zeroY = height() - ( 0 - localMinY ) / ( localMaxY - localMinY ) * height();

        p.setPen( zeroGridColor );
        p.drawLine( 0, zeroY, width(), zeroY );

        // Draw zero value
        if ( showYGridValue ) {
            QString str = QString::number( 0 );
            QRect fr = p.fontMetrics().boundingRect( str );

            pen.setColor( zeroGridColor );
            pen.setWidth( 1 );
            p.setPen( pen );
            p.drawText( width() - fr.width() - 5, zeroY - 5, str );
        }
    }

    // Draw Y Grid value
    if ( showYGridValue ) {
        QString strTop = QString::number( localMaxY );
        QString strBottom = QString::number( localMinY );
        QRect frTop = p.fontMetrics().boundingRect( strTop );
        QRect frBottom = p.fontMetrics().boundingRect( strBottom );

        pen.setColor( gridColor );
        pen.setWidth( 1 );
        p.setPen( pen );
        p.drawText( width() - frTop.width() - 5, frTop.height(), strTop );
        p.drawText( width() - frBottom.width() - 5, height() - 2, strBottom );
    }

    // Draw wave
    p.setRenderHint( QPainter::Antialiasing );

    QList<QPolygon> polygonList;

    for ( int col = 0; col < columnCount; col++ ) {
        polygonList << QPolygon();
    }

    for ( int i = 0; i < drawQueue.size(); i++ ) {
        auto now = drawQueue[i];

        // all column
        for ( int col = 0; col < columnCount; col++ ) {
            double nowY = now.second[1 + col];
            double topY;
            double bottomY;

            if ( autoUpdateYMax ) {
                topY = localMaxY;
            } else {
                topY = yMax;
            }

            if ( autoUpdateYMin ) {
                bottomY = localMinY;
            } else {
                bottomY = yMin;
            }

            // calc pix pos
            double nowDrawY = height() - ( nowY - bottomY ) / ( topY - bottomY ) * height();
            int nowDrawX = width() - now.first;

            // Add point to polygon
            polygonList[col] << QPoint( nowDrawX, nowDrawY );
        }
    }

    for ( int col = 0; col < columnCount; col++ ) {
        pen.setColor( colors[col] );
        pen.setWidth( 1 );
        p.setPen( pen );
        p.drawPolyline( polygonList[col] );
    }

    p.setRenderHint( QPainter::Antialiasing, false );

    // Draw color filter
    for ( auto const &filter : colorFilterList ) {
        double headX = (*head)[0];
        double tailX = headX - ( width() / xScale );
        double fStart = filter.first[0];
        double fEnd = filter.first[1];

        // flip
        if ( fStart > fEnd ) {
            fStart = filter.first[1];
            fEnd   = filter.first[0];
        }

        // Check need drawing
        if ( fStart > headX || fEnd < tailX ) {
            continue;
        }

        int xStart = width() - ( headX - fStart ) * xScale;
        int xEnd   = width() - ( headX - fEnd ) * xScale;

        p.setPen( filter.second );
        p.fillRect( xStart, 0, xEnd - xStart, height(), filter.second );
    }

    // Draw legend
    font.setPixelSize( legendFontSize );
    p.setFont( font );

    for ( int col = 0; col < columnCount; col++ ) {
        int fh = p.fontMetrics().height();
        // Create str
        QString str = names[col];

        if ( showHeadValue ) {
            str = str + " : " + QString::number( (*head)[col + 1] );
        }

        pen.setColor( colors[col] );
        pen.setWidth( legendLineWidth );
        p.setPen( pen );
        p.drawLine( legendOffsetX, legendOffsetY + col * fh + fh / 2, legendOffsetX + legendLineLength, legendOffsetY + fh / 2 + col * fh );

        pen.setColor( Qt::black );
        pen.setWidth( 1 );
        p.setPen( pen );
        p.drawText( legendOffsetX + legendLineLength + 5, legendOffsetY + col * fh + fh / 2, str );
    }

    // Draw cursor
    if ( showCursor && ( validCursor ) ) {
        int cursorX = width() - ( (*head)[0] - (*cursor)[0] ) * xScale;

        if ( cursorX >= 0 && cursorX < width() ) {
            pen.setColor( cursorColor );
            pen.setWidth( cursorWidth );
            p.setPen( pen );
            p.drawLine( cursorX, 0, cursorX, height() );

            // Draw cursor value
            if ( showCursorValue ) {
                font.setPixelSize( defaultFontSize );
                p.setFont( font );

                for ( int col = 0; col < columnCount; col++ ) {
                    int fh = p.fontMetrics().height();

                    // Create str
                    QString str = names[col] + " : " + QString::number( (*cursor)[col + 1] );
                    QRect fr = p.fontMetrics().boundingRect( str );

                    pen.setColor( colors[col] );
                    pen.setWidth( 2 );
                    p.setPen( pen );
                    p.drawLine( cursorX - fr.width() - 10 - 10, height() - ( columnCount - 1 - col ) * fh - 5, cursorX - fr.width() - 10 - 5, height() - ( columnCount - 1 - col ) * fh - 5 );

                    pen.setColor( Qt::black );
                    pen.setWidth( 1 );
                    p.setPen( pen );
                    p.drawText( cursorX - fr.width() - 10, height() - ( columnCount - 1 - col ) * fh - 5, str );
                }

                // Draw x value
                QString str = xName + " : " + QString::number( (*cursor)[0], 'f' );
                QRect fr = p.fontMetrics().boundingRect( str );

                pen.setColor( Qt::black );
                pen.setWidth( 1 );
                p.setPen( pen );
                p.drawText( cursorX - fr.width() - 10, height() - columnCount * fr.height() - 5, str );
            }
        }
    }

    // Draw right cursor
    if ( showRightCursor && validRightCursor ) {
        int cursorX = width() - ( (*head)[0] - (*rightCursor)[0] ) * xScale;

        if ( cursorX >= 0 && cursorX < width() ) {
            pen.setColor( cursorColor );
            pen.setWidth( cursorWidth );
            p.setPen( pen );
            p.drawLine( cursorX, 0, cursorX, height() );
        }
    }
}

void WaveGraphWidget::mouseMoveEvent(QMouseEvent *event)
{
    // Mouse move handler
    if ( Qt::LeftButton & event->buttons() ) {
        updateCursor( event->pos().x() );

        update();
    } else if ( Qt::RightButton & event->buttons() ) {
        updateRightCursor( event->pos().x() );

        update();
    }
}


void WaveGraphWidget::mousePressEvent(QMouseEvent *event)
{
    // Mouse press handler
    if ( Qt::LeftButton & event->buttons() ) {
        updateCursor( event->pos().x() );

        update();
    } else if ( Qt::RightButton & event->buttons() ) {
        updateRightCursor( event->pos().x() );

        update();
    }
}
