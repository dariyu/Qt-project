#include "graph.h"
#include "ui_graph.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QSpacerItem>
#include <QPushButton>
#include <math.h>
#include <QWheelEvent>

double getMax(QVector <double> data)
{
    if (!data.isEmpty())
    {
        double max = fabs(data[0]);
        for (int i = 1; i < data.size(); i++)
        {
           if (fabs(data[i]) > max)
            {
                max = fabs(data[i]);
            }
        }
        return max;
    }
    else return 0;
}

Graph::Graph(QWidget *parent) :
    QWidget(parent), ui(new Ui::Graph)
{
    ui->setupUi(this);
    connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slide(int)));
    ui->horizontalScrollBar->hide();
    pointNumber = 1000;
    ChoosedStart = ChoosedEnd = -1;
    mousePressed = mouseMoved = false;
    navigator = -1;
}


Graph::~Graph()
{
    delete ui;
}

void Graph::clear()
{
    QPainter painter(this);
    painter.setBrush(QColor(0,0,0));
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, this->width(), this->height());
    painter.setPen(QColor(0, 0, 255));
    painter.drawLine(QPointF(0, this->height() / 2 -  ui->horizontalScrollBar->height()),
                     QPointF(this->width(), this->height() / 2 -  ui->horizontalScrollBar->height()));
    painter.setBrush(QColor(240,230,140));
    if (ChoosedStart != ChoosedEnd)
    {
        painter.setPen(Qt::NoPen);
        painter.drawRect((double)(ChoosedStart - Beg) * dx, 0, (ChoosedEnd - ChoosedStart) * dx, this->height());
        painter.setPen(QColor(255,0,0));
        painter.drawLine(QPointF((double)(ChoosedStart - Beg) * dx, this->height() / 2 -  ui->horizontalScrollBar->height()),
                         QPointF((ChoosedEnd - Beg) * dx, this->height() / 2 -  ui->horizontalScrollBar->height()));
    }
    if (navigator != -1)
    {
        painter.setPen(QColor(255, 0, 0));
        painter.drawLine(QPointF((navigator - Beg) * dx, 0), QPointF((navigator - Beg) * dx, this->height() -  ui->horizontalScrollBar->height()));
    }
}

void Graph::paintEvent(QPaintEvent*)
{
    dx = (double)this->width() / (End - Beg);
    clear();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if (!data.isEmpty())
    {
        koef = (double)(this->height() / 2 - ui->horizontalScrollBar->height()) / Max;
        for (int i = Beg; i < End - (int)occuracity; i+=occuracity)
        {
            if ((i >= ChoosedStart && i <= ChoosedEnd - 1) || (i <= ChoosedStart - 1 && i >= ChoosedEnd))
            {
                painter.setPen(QColor(0, 0, 0));
            }
            else
            {
                painter.setPen(QColor(255, 255, 255));
            }
            painter.drawLine(QPointF((i - Beg) * dx, data[i] * koef + this->height() / 2 -  ui->horizontalScrollBar->height()),
                             QPointF((i + (int)occuracity - Beg) * dx, data[i + (int)occuracity] * koef + this->height() / 2 -  ui->horizontalScrollBar->height()));
        }
    }
}

void Graph::setData(QVector <double> newData)
{
    data = newData;
    Beg = 0;
    BaseEnd = data.size();
    End = BaseEnd;
    ui->horizontalScrollBar->setMaximum(data.size() - BaseEnd);
    Max = getMax(data);
    occuracity = (End - Beg) / pointNumber;
    if (occuracity < 1)
    {
        occuracity = 1;
    }
}

void Graph::slide(int a)
{
    Beg  = a;
    End = BaseEnd + a;
    update();
}

void Graph::wheelEvent(QWheelEvent *event)
{
    double k = (double)event->delta() / 100;
    if (k > 0)
    {
        BaseEnd /= k;
        if (BaseEnd < 10)
        {
            BaseEnd *= k;
        }
        ui->horizontalScrollBar->setMaximum(data.size() - BaseEnd);
        if (ui->horizontalScrollBar->maximum() > 0)
        {
            ui->horizontalScrollBar->show();
        }
        int a = ui->horizontalScrollBar->value();
        Beg  = a;
        End = BaseEnd + a;
        occuracity = (End - Beg) / pointNumber;
        if (occuracity < 1)
        {
            occuracity = 1;
        }
        update();
    }
    else if (k < 0)
    {
        k *= -1;
        BaseEnd *= k;
        if (BaseEnd > data.size())
        {
            BaseEnd = data.size();
        }
        ui->horizontalScrollBar->setMaximum(data.size() - BaseEnd);
        if (ui->horizontalScrollBar->maximum() == 0)
        {
            ui->horizontalScrollBar->hide();
        }
        int a = ui->horizontalScrollBar->value();
        Beg  = a;
        End = BaseEnd + a;
        occuracity = (End - Beg) / pointNumber;
        if (occuracity < 1)
        {
            occuracity = 1;
        }
        update();
    }
}

void Graph::mousePressEvent(QMouseEvent *)
{
    mousePressed = true;
    mouseMoved = false;
}

void Graph::mouseReleaseEvent(QMouseEvent* event)
{
    mousePressed = false;
    if (!mouseMoved)
    {
        ChoosedStart = ChoosedEnd = -1;
        navigator = event->pos().x() / dx + Beg;
        if (navigator < 0)
        {
            navigator = 0;
        }
        if (navigator > data.size() - 1)
        {
            navigator = data.size() - 1;
        }
        update();
    }
    else
    {
        if (ChoosedEnd > ChoosedStart)
        {
            emit areaSelected(ChoosedStart, ChoosedEnd);
        }
        else if (ChoosedEnd < ChoosedStart)
        {
            emit areaSelected(ChoosedEnd, ChoosedStart);
        }
    }
}

void Graph::mouseMoveEvent(QMouseEvent *event)
{
    if (!mouseMoved)
    {
        ChoosedStart = event->pos().x() / dx + Beg;
        ChoosedEnd = ChoosedStart;
    }
    else
    {
        ChoosedEnd = event->pos().x() / dx + Beg;
    }
    mouseMoved = true;
    navigator = -1;
    if (ChoosedStart < 0)
    {
        ChoosedStart = 0;
    }
    if (ChoosedEnd < 0)
    {
        ChoosedEnd = 0;
    }
    if (ChoosedStart > data.size() - 1)
    {
        ChoosedStart = data.size() - 1;
    }
    if (ChoosedEnd > data.size() - 1)
    {
        ChoosedEnd = data.size() - 1;
    }
    update();
}

QVector <double>* Graph::getDataPointer()
{
    return &data;
}

void Graph::clearSeleced()
{
    if (ChoosedEnd != ChoosedStart)
    {
        buff.clear();
        int start, end;
        if (ChoosedEnd > ChoosedStart)
        {
            start = ChoosedStart;
            end = ChoosedEnd;
        }
        else
        {
            end = ChoosedStart;
            start = ChoosedEnd;
        }
        buff.resize(end - start);
        for (int i = 0; i < (end - start); i++)
        {
            if (start < data.size() - 1)
            {
                buff[i] = data[start + i];
            }
        }
        for (int i = 0; end + i < data.size(); i++)
        {
            data[i + start] = data[i + end];
        }
        data.resize(data.size() - (end - start));
        Beg = 0;
        BaseEnd = data.size();
        End = BaseEnd;
        ui->horizontalScrollBar->setMaximum(data.size() - BaseEnd);
        Max = getMax(data);
        occuracity = (End - Beg) / pointNumber;
        if (occuracity < 1)
        {
            occuracity = 1;
        }
        navigator = start;
        ChoosedStart = ChoosedEnd;
        update();
    }
}

void Graph::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        clearSeleced();
    }
    if (event->key() == Qt::Key_V)
    {
        if (event->modifiers() & Qt::ControlModifier)
            ins();
    }
}

void Graph::ins()
{
    if (navigator != -1 && buff.size() > 0)
    {
        data.resize(buff.size() + data.size());
        int count = buff.size();
        for (int i = data.size() - 1; i >= navigator + count; i--)
        {
            data[i] = data[i - count];
        }
        for (int i = 0; i < count; i++)
        {
            data[navigator + i] = buff[i];
        }
        Beg = 0;
        BaseEnd = data.size();
        End = BaseEnd;
        ui->horizontalScrollBar->setMaximum(data.size() - BaseEnd);
        Max = getMax(data);
        occuracity = (End - Beg) / pointNumber;
        if (occuracity < 1)
        {
            occuracity = 1;
        }
        update();
    }
}
