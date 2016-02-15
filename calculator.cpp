#include "calculator.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <QSettings>
#include <QTextStream>
#include <QFile>
#include <sstream>
#include <math.h>
#include <iostream>
#include <fstream>


calculator::calculator()
{
    maxCurrent=1;
    Ioffset=0;
    plotter=NULL;
    plotYlimits.resize(2);
    LoadBasicSettings();

}


int calculator::LoadElements(QString file)
{
    if(!QFile::exists(file))
    {
        qDebug() << "Settings file" << file << " does not exist!";
        return(1);
    }

    QFile elementFile(file);
    int counter=0;
    if(elementFile.open(QIODevice::ReadOnly))
    {
        elements.clear();
        QTextStream elementList(&elementFile);
        while(!elementList.atEnd())
        {
            QString tempString = elementList.readLine();

            if(tempString.length()>0)
            {
                counter++;

                string filename="./elements/";
                filename.append(tempString.toStdString());
                filename.append(".txt");
                Element tempElement;
                if(tempElement.LoadFromFile(filename))
                {
                    elements.push_back(tempElement);
                }
                else
                {
                    qDebug() << "calculator::LoadElements: Element file " << QString::fromStdString(filename) << "can not be loaded";
                }
            }
        }
    }
    else
    {
        qDebug() << "Settings file" << file << " can not be opened!";
    }

    return(0);

}

double calculator::Gaussian(double x, double FWHM)
{
    double sigma=FWHM/2.355;
    double A=1/(sigma*2.506);
    double B=-1/(2*sigma*sigma);

    return(A*exp(x*x*B));
}
double calculator::NormGaussian(double x, double FWHM)
{
    double sigma=FWHM/2.355;
    double A=1/(sigma*2.506);
    double B=-1/(2*sigma*sigma);

    return(exp(x*x*B));
}

double calculator::IterMQ(double I)
{
    //Iterate m/q from the current
    double limit=1e-6;
    int maxIterations=1000;

    double lower=1;
    double upper=1000;
    double counter=0;
    double tempI=0;

    while((upper-lower)>limit && counter<maxIterations)
    {
        counter++;
        tempI=dipoleI((upper+lower)/2.0);
        if(tempI>=I)
        {
            upper=(upper+lower)/2.0;
        }
        else
        {
            lower=(upper+lower)/2.0;
        }
    }
    //cout << counter << " " << lower << " "<< I << endl;
    return(lower);
}

double calculator::dipoleI(double MQ)
{
    //qDebug() << "dipoleA: " << dipoleA << ", dipoleB: " << dipoleB << ", dipoleC: " << dipoleC << ", dipoleD: " << dipoleD;
    //calibration function
    return(dipoleA+dipoleB*sqrt(Vacc*(MQ+dipoleC*pow(MQ,2)+dipoleD*pow(MQ,3)+dipoleE*pow(MQ,4))));
}

bool calculator::CheckIfSettingsExist(QSettings *file, QString parameter)
{
    if (file->contains(parameter))
    {
        return(1);
    }
    else
    {
        qDebug() << "File " << file->fileName() << " does not contain parameter " << parameter;
        return(0);
    }
}

void calculator::AddElements(int number, QComboBox* box)
{
    //add elements to user interface
    box->clear();

    for(int i=0;i<elements.size();i++)
    {
        box->addItem(QString::fromStdString(elements[i].name));
        if (elements[i].name.compare(defaultElements[number].toStdString())==0)
        {
            box->setCurrentIndex(i);
        }

    }

}

void calculator::AddIsotopes(int elementNumber,QComboBox* box,QDoubleSpinBox* avgCharge,QDoubleSpinBox* chargeFWHM,QDoubleSpinBox* current)
{
    //add isotopes to user interface

    Element& element=elements[elementNumber];
    avgCharge->setValue(element.avgCharge);
    if(current->value()<1)
    {
        current->setValue(maxCurrent);
    }

    chargeFWHM->setValue(element.chargeFWHM);

    box->addItem("All");

    for(int i=0;i<element.isotopes.size();i++)
    {
        stringstream stream;
        stream << element.isotopes[i] << " (" << element.abundance[i] << "%)";
        box->addItem(QString::fromStdString(stream.str()));
    }

}


void calculator::CalculatePointsMQ(vector <double> x, vector <double>& y, int elementNumber, int isotopeNumber, double avgCharge, double chargeFWHM, double intensity, double peakFWHM)
{

    //calculates data to plot lines
    Element& element=elements[elementNumber];
    y.clear();
    y.resize(x.size(),0.0);

    if (isotopeNumber==0)
    {
        //all isotopes, if All is selected
        double maxIsotope=0.0;
        for(int i=0;i<element.abundance.size();i++)
        {
            if(element.abundance[i]>maxIsotope)
            {
                maxIsotope=element.abundance[i];
            }
        }

        for(int i=0;i<element.isotopes.size();i++)
        {
            for(int j=0;j<element.protons;j++)
            {
                for(int k=0;k<x.size();k++)
                {
                    y[k]=y[k]+element.abundance[i]/maxIsotope*NormGaussian((((double) (j+1))-avgCharge),chargeFWHM)*NormGaussian((x[k]-(((double) element.isotopes[i])/((double) (j+1)))),peakFWHM)*intensity;

                }
            }
        }
    }
    else
    {
        //specific isotope
        for(int j=0;j<element.protons;j++)
        {
            for(int k=0;k<x.size();k++)
            {
                y[k]=y[k]+NormGaussian((((double) (j+1))-avgCharge),chargeFWHM)*NormGaussian((x[k]-(((double) element.isotopes[isotopeNumber-1])/((double) (j+1)))),peakFWHM)*intensity;

            }
        }
    }

}

void calculator::CalculateCurrentCharges(double I, vector<double> &charges, vector<int> plot, vector<int> elementNumbers, vector<int> isotopes)
{
    //Draw current dipole value line to plot
    if(plotter != NULL)
    {
        plotter->clearData(plotter->lines-1);
        vector<double> x;
        vector<double> y;
        x.push_back(I-0.0001*I);
        x.push_back(I);

        double min;
        double max;
        plotter->CurrentRange(min,max);
        y.push_back(min);
        y.push_back(max);
        plotter->AddData(plotter->lines-1,x,y);
        plotter->Replot(0);
    }

    charges.clear();
    charges.resize(plot.size(),0.0);
    double MQ=IterMQ(I-Ioffset);

    //Calculate charge states to Q column
    Element tempElement;
    for(int i=0;i<plot.size();i++)
    {
        if(plot[i]>0)
        {
            tempElement=elements[elementNumbers[i]];
            double mass=0;
            if(isotopes[i]==0)
            {
                for(int j=0;j<tempElement.isotopes.size();j++)
                {
                    mass=mass+((double) tempElement.isotopes[j])*tempElement.abundance[j]/100.0;
                }
            }
            else
            {
                mass=tempElement.isotopes[isotopes[i]-1];
            }

            charges[i]=mass/MQ;
        }
    }
}


void calculator::SetUpPlotter(QCustomPlot* plot, int lines)
{


    plotter=new QtPlotter(plot,lines+5,0);
    plotter->SetXlabel("Dipole current [A]");
    plotter->SetYlabel("Beam current [uA]");
    plotter->SetReversedXaxis(1);
    plotter->SetLegend(1);
    plotter->SetName(plotter->lines-1,"");
    plotter->SetName(plotter->lines-2,"");
    plotter->SetName(plotter->lines-3,"");
    plotter->SetName(plotter->lines-4,"");

    plot->setBackground(Qt::white);
}
void calculator::PlotGraphs(double fromI, double toI, vector <int> plot, vector <int> elementNumbers, vector <int> isotopes,vector <double> avgCharge,vector <double> chargeFWHMs, vector <double> intensities)
{

    if(plotter!=NULL)
    {
        if(plotLegend==1)
        {
            plotter->SetLegend(1);
        }
        else
        {
            plotter->SetLegend(0);
        }

    double yMax=0;
    int steps=2000;

    vector <double> x;
    vector <double> I;
    vector <double> y;
    vector <double> yAll;

    I.resize(steps,0.0);
    x.resize(steps,0.0);
    y.resize(steps,0.0);
    yAll.resize(steps,0.01);

    double step=(toI-fromI)/(steps-1);

    for(int i=0;i<steps;i++)
    {
        I[i]=fromI+((double) step)*i;
        x[i]=IterMQ(I[i]-Ioffset);
    }
    for(int i=0;i<plot.size();i++)
    {
        plotter->clearData(i+1);
        plotter->SetName(i+1,"");
        if(plot[i]>0)
        {
            if (isotopes[i]>0)
            {
                stringstream nameStream;
                nameStream << elements[elementNumbers[i]].name << "-" << elements[elementNumbers[i]].isotopes[isotopes[i]-1];
                plotter->SetName(i+1,nameStream.str());
            }
            else
            {
                //qDebug() << "Here i:" << i << " and elements " << elementNumbers.size() << " and " << elements.size() << "and" << elementNumbers[i];
                plotter->SetName(i+1,elements[elementNumbers[i]].name);
                //qDebug() << "really????";
            }
            CalculatePointsMQ(x, y, elementNumbers[i], isotopes[i], avgCharge[i], chargeFWHMs[i], intensities[i],peakFWHM);
            plotter->AddData(i+1,I,y);
            plotter->Replot();


            for(int j=0;j<yAll.size();j++)
            {
                yAll[j]=yAll[j]+y[j];
                if (plotYlimits[1]==0 && plotAll==1 && yAll[j]>yMax)
                {
                    yMax=yAll[j];

                }
                else if (plotYlimits[1]==0 && y[j]>yMax )
                {
                    yMax=yAll[j];
                }

            }
       }
       else
       {
            plotter->SetName(i+1,"");
       }
    }
    plotter->Yrange[0]=plotYlimits[0];
    if (plotYlimits[1]==0)
    {
        plotter->Yrange[1]=yMax;
    }
    else
    {
        plotter->Yrange[1]=plotYlimits[1];
    }
    plotter->Xrange[0]=fromI;
    plotter->Xrange[1]=toI;
    plotter->Replot();
    plotter->clearData(0);
    plotter->SetName(0,"");
    if(plotAll==1)
    {
        plotter->SetName(0,"Total signal");
        plotter->AddData(0,I,yAll);
        plotter->Replot();
    }
    }
}

void calculator::ChargeStateList(QString& list, vector <int> plot, vector <int> elementNumbers, vector <int> isotopes)
{
    list.clear();
    list.append("     ");
    int maxCharge=0;

    for(int i=0;i<plot.size();i++)
    {
        if(plot[i]>0)
        {
            if(elements[elementNumbers[i]].protons>maxCharge)
            {
                maxCharge=elements[elementNumbers[i]].protons;
            }
            QString name;
            name.append(QString::fromStdString(elements[elementNumbers[i]].name));
            name.append("-");
            if (isotopes[i]>0)
            {
                name.append(QString::number(elements[elementNumbers[i]].isotopes[isotopes[i]-1]));
            }
            else
            {
                name.append(QString::number((int) elements[elementNumbers[i]].MostProbableMass()));
            }
            list.append(name.rightJustified(7,' '));
        }
        else
        {
            list.append(QString(" ").rightJustified(7,' '));
        }
    }
    list.append("\r\n");

    for(int i=1;i<(maxCharge+1);i++)
    {
        QString row;
        row.append(QString::number(i).rightJustified(2,' '));
        row.append("+  ");
        for(int j=0;j<plot.size();j++)
        {
            if(plot[j]>0 && i < (elements[elementNumbers[j]].protons+1))
            {
                double mass;
                if(isotopes[j]==0)
                {
                    mass=elements[elementNumbers[j]].MostProbableMass();
                }
                else
                {
                    mass=(double)elements[elementNumbers[j]].isotopes[isotopes[j]-1];
                }


                row.append(QString::number(dipoleI(mass/((double) i)+Ioffset),'f',1).rightJustified(7,' '));
            }
            else
            {
                row.append(QString(" ").rightJustified(7,' '));
            }
        }
        row.append("\r\n");
        list.append(row);
    }
    recentChargeStateList=list;

}







int calculator::LoadSettings(QString file)
{
    if(!QFile::exists(file))
    {
        qDebug() << "Settings file" << file << " does not exist!";
        return(1);
    }
    QSettings *settings;
    settings=new QSettings(file,QSettings::IniFormat);

    if(!CheckIfSettingsExist(settings, "DipoleA"))
        return(1);
    dipoleA=settings->value("DipoleA").toDouble();

    if(!CheckIfSettingsExist(settings, "DipoleB"))
        return(1);
    dipoleB=settings->value("DipoleB").toDouble();

    if(!CheckIfSettingsExist(settings, "DipoleC"))
        return(1);
    dipoleC=settings->value("DipoleC").toDouble();

    if(!CheckIfSettingsExist(settings, "DipoleD"))
        return(1);
    dipoleD=settings->value("DipoleD").toDouble();

    if(!CheckIfSettingsExist(settings, "DipoleE"))
        return(1);
    dipoleE=settings->value("DipoleE").toDouble();

    if(!CheckIfSettingsExist(settings, "PeakWidth"))
        return(1);
    peakFWHM=settings->value("PeakWidth").toDouble();

    if(!CheckIfSettingsExist(settings, "DefaultDataPath"))
        return(1);
    defaultDataPath=settings->value("DefaultDataPath").toString();

    if(!CheckIfSettingsExist(settings, "DipoleCurrent"))
        return(1);
    defaultDipoleCurrent=settings->value("DipoleCurrent").toDouble();

    if(!CheckIfSettingsExist(settings, "DipoleCurrentStep"))
        return(1);
    defaultDipoleCurrentStep=settings->value("DipoleCurrentStep").toDouble();

    if(!CheckIfSettingsExist(settings, "DipoleOffset"))
        return(1);
    defaultDipoleOffset=settings->value("DipoleOffset").toDouble();

    if(!CheckIfSettingsExist(settings, "DipoleOffsetStep"))
        return(1);
    defaultDipoleOffsetStep=settings->value("DipoleOffsetStep").toDouble();

    if(!CheckIfSettingsExist(settings, "SourceBias"))
        return(1);
    defaultSourceBias=settings->value("SourceBias").toDouble();

    if(!CheckIfSettingsExist(settings, "SourceBiasStep"))
        return(1);
    defaultSourceBiasStep=settings->value("SourceBiasStep").toDouble();

    if(!CheckIfSettingsExist(settings, "PlotSum"))
        return(1);
    defaultPlotSum=settings->value("PlotSum").toBool();

    if(!CheckIfSettingsExist(settings, "ChargeStateDistribution"))
        return(1);
    defaultPlotDistribution=settings->value("ChargeStateDistribution").toBool();

    defaultSourceBiasUnit.clear();

    if(!CheckIfSettingsExist(settings, "SourceBiasUnit"))
        return(1);
    defaultSourceBiasUnit.append(settings->value("SourceBiasUnit").toString());

    if(!CheckIfSettingsExist(settings, "Name"))
        return(1);
    dipoleName.clear();
    dipoleName.append(settings->value("Name").toString());
    dipoleName.append(":");

    return(0);
}

int calculator::LoadBasicSettings()
{
    if(!QFile::exists("DiscSettings.ini"))
    {
        qDebug() << "Settinfg file DiscSettings.ini does not exist!";
        return(1);
    }

    QSettings *settings;
    settings = new QSettings("DiscSettings.ini",QSettings::IniFormat);


    if(!CheckIfSettingsExist(settings, "SettingsFile0"))
        return(1);
    QString settingsFile1=settings->value("SettingsFile0").toString();

    if(!CheckIfSettingsExist(settings, "SettingsFile1"))
        return(1);
    QString settingsFile2=settings->value("SettingsFile1").toString();

    if(!CheckIfSettingsExist(settings, "SettingsFile2"))
        return(1);
    QString settingsFile3=settings->value("SettingsFile2").toString();

    if(!CheckIfSettingsExist(settings, "SettingsName0"))
        return(1);
    QString settingsName1=settings->value("SettingsName0").toString();

    if(!CheckIfSettingsExist(settings, "SettingsName1"))
        return(1);
    QString settingsName2=settings->value("SettingsName1").toString();

    if(!CheckIfSettingsExist(settings, "SettingsName2"))
        return(1);
    QString settingsName3=settings->value("SettingsName2").toString();

    if(!CheckIfSettingsExist(settings, "DefaulSettings"))
        return(1);
    defaultSettingsSelected=settings->value("DefaulSettings").toInt();

    defaultSettingsFiles.clear();
    defaultSettingsFiles.push_back(settingsFile1);
    defaultSettingsFiles.push_back(settingsFile2);
    defaultSettingsFiles.push_back(settingsFile3);

    defaultSettingsNames.clear();
    defaultSettingsNames.push_back(settingsName1);
    defaultSettingsNames.push_back(settingsName2);
    defaultSettingsNames.push_back(settingsName3);



    if(!CheckIfSettingsExist(settings, "ElementsFile0"))
        return(1);
    QString elementsFile1=settings->value("ElementsFile0").toString();

    if(!CheckIfSettingsExist(settings, "ElementsFile1"))
        return(1);
    QString elementsFile2=settings->value("ElementsFile1").toString();

    if(!CheckIfSettingsExist(settings, "ElementsFile2"))
        return(1);
    QString elementsFile3=settings->value("ElementsFile2").toString();

    if(!CheckIfSettingsExist(settings, "ElementsName0"))
        return(1);
    QString elementsName1=settings->value("ElementsName0").toString();

    if(!CheckIfSettingsExist(settings, "ElementsName1"))
        return(1);
    QString elementsName2=settings->value("ElementsName1").toString();

    if(!CheckIfSettingsExist(settings, "ElementsName2"))
        return(1);
    QString elementsName3=settings->value("ElementsName2").toString();


    if(!CheckIfSettingsExist(settings, "DefaultElements"))
        return(1);
    defaultElementsSelected=settings->value("DefaultElements").toInt();

    defaultElements.clear();

    if(!CheckIfSettingsExist(settings, "DefaultElement0"))
        return(1);
    defaultElements.push_back(settings->value("DefaultElement0").toString());

    if(!CheckIfSettingsExist(settings, "DefaultElement1"))
        return(1);
    defaultElements.push_back(settings->value("DefaultElement1").toString());

    if(!CheckIfSettingsExist(settings, "DefaultElement2"))
        return(1);
    defaultElements.push_back(settings->value("DefaultElement2").toString());

    if(!CheckIfSettingsExist(settings, "DefaultElement3"))
        return(1);
    defaultElements.push_back(settings->value("DefaultElement3").toString());

    if(!CheckIfSettingsExist(settings, "DefaultElement4"))
        return(1);
    defaultElements.push_back(settings->value("DefaultElement4").toString());

    if(!CheckIfSettingsExist(settings, "DefaultElement5"))
        return(1);
    defaultElements.push_back(settings->value("DefaultElement5").toString());

    defaultElementsFiles.clear();
    defaultElementsFiles.push_back(elementsFile1);
    defaultElementsFiles.push_back(elementsFile2);
    defaultElementsFiles.push_back(elementsFile3);

    defaultElementsNames.clear();
    defaultElementsNames.push_back(elementsName1);
    defaultElementsNames.push_back(elementsName2);
    defaultElementsNames.push_back(elementsName3);


    if(!CheckIfSettingsExist(settings, "PlotXmin"))
        return(1);
    defaultPlotXmin=settings->value("PlotXmin").toDouble();

    if(!CheckIfSettingsExist(settings, "PlotXmax"))
        return(1);
    defaultPlotXmax=settings->value("PlotXmax").toDouble();

    if(!CheckIfSettingsExist(settings, "PlotYmin"))
        return(1);
    defaultPlotYmin=settings->value("PlotYmin").toDouble();

    if(!CheckIfSettingsExist(settings, "PlotYmax"))
        return(1);
    defaultPlotYmax=settings->value("PlotYmax").toDouble();


    if(!CheckIfSettingsExist(settings, "Intensity"))
        return(1);
    defaultIntensity=settings->value("Intensity").toDouble();

    if(LoadSettings(defaultSettingsFiles[defaultSettingsSelected]))
    {
        return(1);
    }

    return(LoadElements(defaultElementsFiles[defaultElementsSelected]));
}

void calculator::SaveToFile(QString pdfFileName,QString chargeListFilename)
{
    plotter->SavePlotToFile(pdfFileName,1189,841,1);

    QFile file(chargeListFilename);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {

        QTextStream out(&file);
        out << "Dipole currents for different charge states \r\n";
        out << recentChargeStateList;
        file.close();
    }
}
