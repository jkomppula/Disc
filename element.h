#include <vector>
#include <string>
#include <fstream>
using namespace std;

#ifndef ELEMENT_H
#define ELEMENT_H

class Element
{
    bool SeparateLine(string line, string& variable, string& value);
    bool ReadIsotopes(ifstream& file);
    bool SetVariable(string& variable,string& value);

public:
    double avgCharge;
    double chargeFWHM;
    string name;
    int protons;
    vector <int> isotopes;
    vector <double> abundance;
    vector <string> isotopeNames;

    Element();
    bool LoadFromFile(string filename);
    double AvgMass(void);
    double MostProbableMass(void);

};

#endif // ELEMENT_H
