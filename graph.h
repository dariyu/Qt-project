#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>
#include <QVector>
#include <QPointF>

namespace Ui {
class Graph;
}

class Graph : public QWidget
{
    Q_OBJECT

public:
    explicit Graph(QWidget *parent = 0);
    ~Graph();

    void setData(QVector <double> newData);

    QVector <double>* getDataPointer();

signals:
    void areaSelected(int, int);


protected:
    virtual void paintEvent(QPaintEvent*);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);

private slots:
    void slide(int);
    void clearSeleced();
    void ins();

private:
//UI items
    Ui::Graph* ui;

private:
    double dx, koef, pointNumber;

//dataItem
    QVector <double> data, buff;
    double Beg, BaseEnd, End, Max, occuracity;
    bool mousePressed, mouseMoved;
    int ChoosedStart, ChoosedEnd, navigator;
    void clear();
};

#endif // GRAPH_H
