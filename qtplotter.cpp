#include <QVector>
#include "qtplotter.h"
#include <iostream>


QtPlotter::QtPlotter(QCustomPlot *plotPointer, int numberOfLines, int latestPointMarker)
{
    maxlength=0.0;
    Xrange.resize(2,0.0);
    Yrange.resize(2,0.0);

    plot=plotPointer;
    colors.push_back(Qt::black);
    colors.push_back(Qt::blue);
    colors.push_back(Qt::red);
    colors.push_back(Qt::green);
    colors.push_back(Qt::magenta);
    colors.push_back(Qt::cyan);
    colors.push_back(Qt::yellow);
    colors.push_back(Qt::darkBlue);
    colors.push_back(Qt::darkRed);
    colors.push_back(Qt::darkGreen);
    colors.push_back(Qt::darkGray);
    colors.push_back(Qt::darkMagenta);
    colors.push_back(Qt::darkCyan);
    colors.push_back(Qt::darkYellow);


    lines=numberOfLines;
    marker=latestPointMarker;

    for(unsigned int i=0;i<lines;i++)
    {
        if(marker>0)
        {
            plot->addGraph();
            plot->graph(2*i)->setPen(QPen(colors[i % colors.size()]));
            plot->addGraph();
            plot->graph(2*i+1)->setPen(QPen(colors[i % colors.size()]));
            plot->graph(2*i+1)->setLineStyle(QCPGraph::lsNone);
            plot->graph(2*i+1)->setScatterStyle(QCPScatterStyle::ssDisc);

        }
        else
        {
            plot->addGraph();
            plot->graph(i)->setPen(QPen(colors[i % colors.size()]));
        }
    }
    plot->xAxis->setTickLabelType(QCPAxis::ltNumber);

    //plot->xAxis2->setVisible(true);
    //plot->xAxis2->setTickLabels(false);
    plot->yAxis2->setVisible(true);
    plot->yAxis2->setTickLabels(false);
    plot->setBackground(QColor(215, 214, 213,0));

}

void QtPlotter::Replot(int changeRange)
{
    if (changeRange==1)
    {
        plot->xAxis->setRange(0,0.01);
        plot->yAxis->setRange(0,0.01);
        //plot->xAxis2->setRange(plot->xAxis->range());
        plot->yAxis2->setRange(plot->yAxis->range());
        plot->replot();
        plot->rescaleAxes(true);
        //plot->replot();
        if(Xrange[0]!=0 || Xrange[1]!=0)
        {
            plot->xAxis->setRange(Xrange[0],Xrange[1]);
        //cout << "X range: " << Xrange[0] << " " << Xrange[1] << endl;
        }
        if(Yrange[0]!=0 || Yrange[1]!=0)
        {


            plot->yAxis->setRange(Yrange[0],Yrange[1]);

            //cout << "Y range: " << Yrange[0] << " " << Yrange[1] << endl;
        }
        //plot->xAxis2->setRange(plot->xAxis->range());
        plot->yAxis2->setRange(plot->yAxis->range());
    }

        plot->replot();

}

void QtPlotter::SetXlabel(string label)
{
    plot->xAxis->setLabel(QString::fromStdString(label));
    plot->replot();
}

void QtPlotter::SetYlabel(string label)
{
    plot->yAxis->setLabel(QString::fromStdString(label));
    plot->replot();
}

void QtPlotter::SetLegend(bool visible)
{
    plot->legend->setVisible(visible);
}

void QtPlotter::SetColor(int lineNumber, QColor color)
{
    if(marker>0)
    {
        plot->graph(2*lineNumber)->setPen(QPen(color));
        plot->graph(2*lineNumber+1)->setPen(QPen(color));
    }
    else
    {
        plot->graph(lineNumber)->setPen(QPen(color));
    }
}

void QtPlotter::SetColor(int lineNumber, QColor color, QColor markerColor)
{
    if(marker>0)
    {
        plot->graph(2*lineNumber)->setPen(QPen(color));
        plot->graph(2*lineNumber+1)->setPen(QPen(markerColor));
    }
    else
    {
        plot->graph(lineNumber)->setPen(QPen(color));
    }
}

void QtPlotter::SetName(int lineNumber, string name)
{
    if(marker>0)
    {

        if(name.length()>0)
        {
            plot->graph(2*lineNumber)->addToLegend();
            plot->graph(2*lineNumber)->setName(QString::fromStdString(name));

        }
        else
        {
            plot->graph(2*lineNumber)->removeFromLegend();
        }
    }
    else
    {
        if(name.length()>0)
        {
            plot->graph(lineNumber)->addToLegend();
            plot->graph(lineNumber)->setName(QString::fromStdString(name));
        }
        else
        {
            plot->graph(lineNumber)->removeFromLegend();

        }
    }
}


void QtPlotter::AddData(int lineNumber, double x, double y, double autoRange)
{
    if(marker>0)
    {
        plot->graph(2*lineNumber)->addData(x,y);
        plot->graph(2*lineNumber+1)->clearData();
        plot->graph(2*lineNumber+1)->addData(x,y);

        if(maxlength>0.0)
        {
            plot->graph(2*lineNumber)->removeDataBefore(x-maxlength);
            plot->graph(2*lineNumber)->removeDataAfter(x+maxlength);
        }
    }
    else
    {
        plot->graph(lineNumber)->addData(x,y);

        if(maxlength>0.0)
        {
            plot->graph(lineNumber)->removeDataBefore(x-maxlength);
            plot->graph(lineNumber)->removeDataAfter(x+maxlength);
        }
    }
    if(autoRange>0)
    {
        Xrange[0]=x-maxlength;
        Xrange[1]=autoRange*maxlength+x;
    }
}

void QtPlotter::AddData(int lineNumber, vector<double> x, vector<double> y, double autoRange)
{
    if(marker>0)
    {
        plot->graph(2*lineNumber)->addData(QVector<double>::fromStdVector(x),QVector<double>::fromStdVector(y));
        plot->graph(2*lineNumber+1)->clearData();
        plot->graph(2*lineNumber+1)->addData(x.back(),y.back());

        if(maxlength>0.0)
        {
            plot->graph(2*lineNumber)->removeDataBefore(x.back()-maxlength);
            plot->graph(2*lineNumber)->removeDataAfter(x.back()+maxlength);
        }
    }
    else
    {
        plot->graph(lineNumber)->addData(QVector<double>::fromStdVector(x),QVector<double>::fromStdVector(y));

        if(maxlength>0.0)
        {
            plot->graph(lineNumber)->removeDataBefore(x.back()-maxlength);
            plot->graph(lineNumber)->removeDataAfter(x.back()+maxlength);
        }
    }
    if(autoRange>0)
    {
        Xrange[0]=x.back()-maxlength;
        Xrange[1]=autoRange*maxlength+x.back();
    }

}

void QtPlotter::AddDataToAllLines(vector<double> data, double autoRange)
{
    if((data.size()+1)==lines)
    {
        for(unsigned int i=1;i<data.size();i++)
        {
            if(marker>0)
            {
                plot->graph(2*(i-1))->addData(data[0],data[i]);
                plot->graph(2*(i-1)+1)->clearData();
                plot->graph(2*(i-1)+1)->addData(data[0],data[i]);

                if(maxlength>0.0)
                {
                    plot->graph(2*(i-1))->removeDataBefore(data[0]-maxlength);
                    plot->graph(2*(i-1))->removeDataAfter(data[0]+maxlength);
                }
            }
            else
            {
                plot->graph(i-1)->addData(data[0],data[i]);

                if(maxlength>0.0)
                {
                    plot->graph(i-1)->removeDataBefore(data[0]-maxlength);
                    plot->graph(i-1)->removeDataAfter(data[0]+maxlength);
                }
            }
        }
        if(autoRange>0)
        {
            Xrange[0]=data[0]-maxlength;
            Xrange[1]=autoRange*maxlength+data[0];
        }
    }

}

void QtPlotter::AddDataToAllLines(vector<vector<double> > data, double autoRange)
{
    if((data.size()+1)==lines)
    {
        for(unsigned int i=1;i<data.size();i++)
        {
            if(marker>0)
            {
                plot->graph(2*(i-1))->addData(QVector<double>::fromStdVector(data[0]),QVector<double>::fromStdVector(data[i]));
                plot->graph(2*(i-1)+1)->clearData();
                plot->graph(2*(i-1)+1)->addData(data[0].back(),data[i].back());

                if(maxlength>0.0)
                {
                    plot->graph(2*(i-1))->removeDataBefore(data[0].back()-maxlength);
                    plot->graph(2*(i-1))->removeDataAfter(data[0].back()+maxlength);
                }
            }
            else
            {
                plot->graph(i-1)->addData(QVector<double>::fromStdVector(data[0]),QVector<double>::fromStdVector(data[i]));

                if(maxlength>0.0)
                {
                    plot->graph(i-1)->removeDataBefore(data[0].back()-maxlength);
                    plot->graph(i-1)->removeDataAfter(data[0].back()+maxlength);
                }
            }
        }
        if(autoRange>0)
        {
            Xrange[0]=data[0].back()-maxlength;
            Xrange[1]=autoRange*maxlength+data[0].back();
        }
    }

}

void QtPlotter::clearData(int lineNumber)
{
    if(marker>0)
    {
        plot->graph(2*lineNumber)->clearData();
        plot->graph(2*lineNumber+1)->clearData();
    }
    else
    {
        plot->graph(lineNumber)->clearData();
    }
}

void QtPlotter::SetReversedXaxis(bool value)
{
    plot->xAxis->setRangeReversed(value);
}

void QtPlotter::SavePlotToFile(QString filename,int width, int height, bool legend)
{
    bool beforeVisibility=plot->legend->visible();

    plot->legend->setVisible(legend);

    plot->savePdf(filename,false,width,height);

    plot->legend->setVisible(beforeVisibility);

    plot->replot();

}

void QtPlotter::CurrentRange(double& min, double& max)
{
    QCPRange range;
    range=plot->yAxis->range();
    min=range.lower;
    max=range.upper;

}
