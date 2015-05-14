#include "graph.h"
#include <QApplication>
#include <math.h>
#include <QPushButton>
#include <QObject>
#include <FFT.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Graph w;
    std::vector <double> data;
    std::vector <cplx> spec;
    int pointCount = 44100 * 60 * 5;
    for (int i = 0; i < pointCount; i++)
    {
        data.push_back(-cos((double)i * 2 * 3.14 / pointCount * 3));
    }

    w.setData(QVector <double> ::fromStdVector(data));

    w.show();

    return a.exec();
}
