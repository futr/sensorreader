#include "graph.h"

Graph::Graph(QWidget *parent) : QWidget(parent),
    bar( new QScrollBar( this ) ),
    layout( new QVBoxLayout( this ) ),
    label( new QLabel( this ) ),
    wave( new WaveGraphWidget( this ) )
{
    // Setup
    bar->setOrientation( Qt::Horizontal );
    label->setText( "text" );

    wave->setMinimumSize( 0, 110 );
    wave->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding );

    // Layout
    setLayout( layout );
    layout->addWidget( label );
    layout->addWidget( wave );
    layout->addWidget( bar );

    // Connect
    connect( wave, SIGNAL(rangeChanged(int,int)), bar, SLOT(setRange(int,int)) );
    connect( bar, SIGNAL(valueChanged(int)), wave, SLOT(setHeadIndex(int)) );
    connect( wave, SIGNAL(headChanged(int)), bar, SLOT(setValue(int)) );
}

void Graph::setLabel(const QString &text)
{
    label->setText( text );
}

