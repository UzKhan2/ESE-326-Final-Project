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
				if (plimit < 50)
					pins.push_back(text);
				else
					rpins.push_back(text);
				plimit++;
			}
			else
			{
				if (alimit < 1000)
					areas.push_back(area);
				else
					rareas.push_back(text);
				alimit++;
			}
		}
	}
	for (int i = 0; i < rareas.size(); i++)
	{
		//cout << rareas[i] << endl;
	}
	infil.close();

	file = "ibm01.net";
	string arg1, arg2, arg3;
	ifstream infile(file);
	vector<vector<string>> nets;
	vector <string> test;
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
			string str_num = arg1.substr(1); 
			int num = stoi(str_num);
			if (num > 1000) {
				//test.push_back(arg1);
				flag = true;
			}
			
		
				if (arg2.compare("s") == 0)
				{
					infile >> arg3;
					if (count > 0)
					{
						if (flag != true)
						{
							nets.push_back(tnet);
							tnet.clear();
							
						}
						else
						{
							//remove.push_back(tnet);
							tnet.clear();
							flag = false;
						}
						tnet.clear();
					}

					
					if (num < 1000) {
						tnet.push_back(arg1);
						//test.push_back(arg1);
						//flag = true;
					}
					
						count++;
				
				}
				else
				{
					if (num < 1000) {
						tnet.push_back(arg1);
						//test.push_back(arg1);
						//flag = true;
					}
					//tnet.push_back(arg1);
					
				}
		}
	}
	//cout << nets.size() << endl << remove.size();
	for (int i = 0; i < test.size(); i++)
	{
		//cout << "Net" << i << " ";
		{
			cout << test[i] << " ";
		}
		cout << endl;
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
