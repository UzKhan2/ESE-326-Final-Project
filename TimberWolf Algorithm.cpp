#include <iostream>
#include <fstream>

using namespace std;

int main()
{
	string file = "ibm01.are";
	string text;
	ifstream infil(file);
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
			cout << "Name " << text;
			infil >> text;
			cout << " , Size  =  " << text << endl;
		}
	}
	infil.close();
	return 0;
}
