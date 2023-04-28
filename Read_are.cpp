#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main()
{
	string file = "ibm01.are";
	string text;
	int area;
	ifstream infil(file);
	vector <int> areas;
	if (!infil.is_open())
	{
		cout << "Error opening file";
		return 0;
	}
	else
	{
		//while (!infil.eof())
		while (!infil.eof())
		{
			infil >> text;
			//cout << "Name " << text;
			infil >> area;
			if (area == 0)
			{
				break;
			}
			else
			{
				areas.push_back(area);
				//cout << " , Size  =  " << area << endl;
			}
		}
	}
	for (int i = 0; i < areas.size(); i++)
	{
		cout << areas[i] << endl;
	}
	infil.close();
	return 0;
}
