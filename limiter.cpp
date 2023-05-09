#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

int main()
{
    ofstream outputFile1("C:\\Users\\UZAIR\\Downloads\\ibm01.are");
    ofstream outputFile2("C:\\Users\\UZAIR\\Downloads\\ibm01.net");
    string fil = "ibm01.are";
    string text;
    string area;
    ifstream infil(fil);
    vector <string> areas;
    vector <string> pins;
    vector <string> rareas;
    vector <string> rpins;
    string arg1, arg2, arg3;
    string file = "ibm01.net";
    ifstream infile(file);
    vector<vector<string>> nets;
    vector<vector<string>> remove;
    vector<string> tnet;
    bool flag = false;
    int count = 0;
    int plimit = 0, alimit = 0;

    int pinlimit = 51;
    int arealimit = 1000;


    if (!infil.is_open())
    {
        cout << "Error opening file";
        return 0;
    }
    else
    {
        while (!infil.eof())
        {
            infil >> text;
            infil >> area;
            if (area == "0")
            {
                if (plimit < pinlimit)
                    pins.push_back(text);
                else
                    rpins.push_back(text);
                plimit++;
            }
            else
            {
                if (alimit < arealimit)
                    areas.push_back(area);
                else
                    rareas.push_back(text);
                alimit++;
            }
        }
    }
    infil.close();


   
    if (!infile.is_open())
    {
        cout << "Error opening file";
        return 0;
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            infile >> arg1;
        }
        while (!infile.eof())
        {
            infile >> arg1;
            infile >> arg2;
            string first(1, arg1[0]);
            string str_num = arg1.substr(1);
            int num = stoi(str_num);

            if (arg2.compare("s") == 0)	// Tests to see if start of net
            {
                infile >> arg3;
                if (count > 0)
                {
                    if (flag == true)
                    {
                        remove.push_back(tnet);
                        tnet.clear();
                        flag = false;
                    }
                    else
                    {
                        nets.push_back(tnet);
                        tnet.clear();
                    }
                }
                tnet.push_back(arg1);
                count++;
            }
            else
            {
                tnet.push_back(arg1);
            }
            if (num > arealimit)
            {
                flag = true;
            }
            if (num > pinlimit && first.compare("p") == 0)
            {
                flag = true;
            }
        }
    }
    infile.close();


    for (int i = 0; i < areas.size(); i++)
    {
        outputFile1 << "a" << i << " " << areas[i] << endl;
    }

    for (int i = 0; i < pins.size(); i++)
    {
        outputFile1 << pins[i] << " 0" << endl;
    }


    for (int i = 0; i < 5; i++)
    {
        outputFile2 << "0" << endl;
    }

    for (int i = 0; i < nets.size(); i++)
    {
        if (nets[i].size() > 1)
        {
            outputFile2 << nets[i][0] << " s 1" << endl;
            for (int j = 1; j < nets[i].size(); j++)
            {
                if (nets[i].size() > 1)
                {
                    outputFile2 << nets[i][j] << " l" << endl;
                }
            }
        }
    }
    cout << "Completed";
    return 0;
}
