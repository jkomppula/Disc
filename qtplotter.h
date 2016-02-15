#include "qcustomplot.h"
#include <QColor>
#include <vector>
using namespace std;
#ifndef QTPLOTTER_H
#define QTPLOTTER_H

class QtPlotter
{
    vector <QColor> colors;
    vector <double> tempData;
    vector <int> counter;

    QCustomPlot* plot;


    int marker;
public:
    unsigned int lines;
    bool latestPointMarker;
    double maxlength;
    vector <double> Xrange;
    vector <double> Yrange;


    QtPlotter(QCustomPlot *plotPointer, int numberOfLines=1,int latestPointMarker=0);
    void Replot(int changeRange=1);

    void AddData(int lineNumber,double x, double y, double autoRange=0);
    void AddData(int lineNumber,vector<double> x, vector<double> y, double autoRange=0);
    void AddDataToAllLines(vector <double> data, double autoRange=0);
    void AddDataToAllLines(vector<vector<double> > data, double autoRange=0);
    void CurrentRange(double& min, double& max);


    void clearData(int lineNumber);

    void SetXlabel(string label);
    void SetYlabel(string label);
    void SetLegend(bool visible);
    void SetColor(int lineNumber,QColor color);
    void SetColor(int lineNumber,QColor color,QColor markerColor);
    void SetName(int lineNumber, string name);

    void SetReversedXaxis(bool value);

    void SavePlotToFile(QString filename,int width, int height, bool legend=1);

};

#endif // QTPLOTTER_H
