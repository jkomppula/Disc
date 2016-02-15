#include "element.h"
#include <QString>
#include <QStringList>
#include <sstream>
#include <iostream>

Element::Element()
{
}

bool Element::SeparateLine(string line, string& variable, string& value)
{
    variable.clear();
    value.clear();

    QStringList splits=QString::fromStdString(line).split("=",QString::SkipEmptyParts);

    if(splits.size()==2)
    {
        variable=splits[0].toStdString();
        value=splits[1].toStdString();
        return(0);
    }
    return(1);
}

bool Element::LoadFromFile(string filename)
{
    ifstream file;
    file.open(filename.c_str());
    if(file.is_open())
    {
        string tempString;

        while (getline(file,tempString)) {
            if(tempString.find("=")!=string::npos)
            {
                string variable;
                string value;
                if(!SeparateLine(tempString,variable,value))
                {
                    if(variable.compare("isotopes")==0)
                    {
                        ReadIsotopes(file);
                    }
                    else
                    {
                        SetVariable(variable,value);
                    }


                }
            }
        }
    }
    else
    {
        return(0);
    }


    return(1);

}

bool Element::SetVariable(string& variable, string& valueString)
{
    stringstream value;
    value << valueString;

    if(!variable.compare("name"))
    {
        value >> name;
    }
    else if(!variable.compare("avgCharge"))
    {
        value >> avgCharge;
    }
    else if(!variable.compare("chargeFWHM"))
    {
        value >> chargeFWHM;
    }
    else if(!variable.compare("protons"))
    {
        value >> protons;
    }
    else
    {
        return(1);
    }
    return(0);
}

bool Element::ReadIsotopes(ifstream& file)
{
    string tempString;
    isotopeNames.clear();
    isotopes.clear();
    abundance.clear();

    while (getline(file,tempString) && tempString.find("}")==string::npos) {

        QStringList splits=QString::fromStdString(tempString).split(" ",QString::SkipEmptyParts);

        if(splits.size()>2)
        {
            isotopeNames.push_back(splits[0].toStdString());
            isotopes.push_back(splits[1].toInt());
            abundance.push_back(splits[2].toDouble());
        }



    }


}
double Element::AvgMass(void)
{
    double weightedMass=0;
    double totalAbundance=0;
    for(unsigned int i=0;i<isotopes.size();i++)
    {
        weightedMass=weightedMass+(double) abundance[i]*(double) isotopes[i];
        totalAbundance=totalAbundance+(double) abundance[i];
    }
    return(weightedMass/totalAbundance);
}

double Element::MostProbableMass(void)
{
    double largestAbundance=0;
    int isotope=0;
    for(unsigned int i=0;i<isotopes.size();i++)
    {
        if(abundance[i]>largestAbundance)
        {
            largestAbundance=abundance[i];
            isotope=i;
        }
    }
    return((double) isotopes[isotope]);
}
