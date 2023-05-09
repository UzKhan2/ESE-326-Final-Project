#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main()
{
    ifstream file1("InitialPlacementOutput.txt");
    ifstream file2("FinalOutput.txt");
    ofstream outfile("C:\\Users\\UZAIR\\Downloads\\Compare.txt");
    string arg1;
    int arg2, arg3, arg4;

    if (!file1.is_open() || !file2.is_open() || !outfile.is_open())
    {
        cout << "Error opening files\n";
        return 1;
    }


    //file1 >> arg1 >> arg1;
    //file2 >> arg2 >> arg2;


    vector <int> f1;
    vector <int> f2;
    vector <int> f3;
    vector <int> f4;
    if (!file1.is_open())
    {
        cout << "Error opening file";
        return 0;
    }
    else
    {
        //while (!infil.eof())
        while (!file1.eof())
        {
            file1 >> arg1;
            file1 >> arg2;
            f1.push_back(arg2);
            file1 >> arg3;
            f2.push_back(arg3);
        }
    }
    file1.close();

    if (!file2.is_open())
    {
        cout << "Error opening file";
        return 0;
    }
    else
    {
        //while (!infil.eof())
        while (!file2.eof())
        {
            file2 >> arg1;
            file2 >> arg2;
            f3.push_back(arg2);
            file2 >> arg3;
            f4.push_back(arg3);
            file2 >> arg4;
            
        }
    }
    file2.close();

    for (int i = 0; i < f1.size(); i++)
    {
        if (f1[i] != f3[i] || f2[i] != f4[i])
        {
            outfile << "Line: " << (i+1) << "\t" << f1[i] << ", " << f2[i] << " - " << f3[i] << ", " << f4[i] << endl;
        }
    }
    cout << "Complete" << endl;
    return 0;
}
    
