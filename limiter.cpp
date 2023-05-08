#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

int main()
{
	ofstream outputFile1("C:\\Users\\UZAIR\\Downloads\\ibm01.txt");
	ofstream outputFile2("C:\\Users\\UZAIR\\Downloads\\ibm02.txt");
	string file = "ibm01.are";
	string text;
	string area;
	ifstream infil(file);
	vector <string> areas;
	vector <string> pins;
	vector <string> rareas;
	vector <string> rpins;
	int plimit = 0, alimit = 0;
	bool flag = false;
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
				if (plimit < 20)
					pins.push_back(text);
				else
					rpins.push_back(text);
				plimit++;
			}
			else
			{
				if (alimit < 100)
					areas.push_back(area);
				else
					rareas.push_back(text);
				alimit++;
			}
		}
	}
	for (int i = 0; i < rpins.size(); i++)
	{
		//cout << rpins[i] << endl;
	}
	infil.close();

	file = "ibm01.net";
	string arg1, arg2, arg3;
	ifstream infile(file);
	vector<vector<string>> nets;
	vector<vector<string>> remove;
	vector<string> tnet;
	int count = 0;

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
			for (int i = 0; i < rareas.size(); i++)
			{
				if (arg1 == rareas[i]) 
				{
					flag = true;
					//cout << arg1 << " " << rareas[i] << endl;
				}
			}
			for (int i = 0; i < rpins.size(); i++)
			{
				if (arg1 == rpins[i])
				{
					flag = true;
					//cout << arg1 << " " << rpins[i] << endl;
				}
			}
			if (arg2.compare("s") == 0)	
			{
				infile >> arg3;
				if (count > 0)
				{
					if (flag == true)
					{
						remove.push_back(tnet);
						flag = false;
					}
					else
					{
						nets.push_back(tnet);
					}
					tnet.clear();
				}
				tnet.push_back(arg1);
				count++;
			}
			else
			{
				tnet.push_back(arg1);
			}
		}
	}
	//cout << nets.size() << endl << remove.size();
	for (int i = 0; i < nets.size(); i++)
	{
		//cout << "Net" << i << " ";
		for (int j = 0; j < nets[0].size(); j++)
		{
			//cout << nets[i][j] << " ";
		}
		//cout << endl;
	}
	infile.close();

	////////////////////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < areas.size(); i++)
	{
		outputFile1 << "a" << i << " " << areas[i] << endl;
	}

	for (int i = 0; i < pins.size(); i++)
	{
		outputFile1 << pins[i] << " 0" << endl;
	}

	////////////////////////////////////////////////////////////////

	for (int i = 0; i < 5; i++)
	{
		outputFile2 << "0" << endl;
	}

	for (int i = 0; i < nets.size(); i++)
	{
		outputFile2 << nets[i][1] << " s 1" << endl;
		for (int j = 1; j < nets[i].size(); j++)
		{
			outputFile2 << nets[i][j] << " l" << endl;
		}
	}
	cout << "Completed";
	return 0;
}
