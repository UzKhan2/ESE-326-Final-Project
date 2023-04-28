#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main()
{
	string file = "ibm01.net";
	string arg1,arg2,arg3;
	ifstream infile(file);
	vector<vector<string>> nets;
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
		//int limit = 0;
		//while (limit < 50)
		{
			infile >> arg1;
			infile >> arg2;
			if (arg2.compare("s") == 0)	// Tests to see if start of net
			{
				infile >> arg3;
				if (count > 0)
				{
					nets.push_back(tnet);
					tnet.clear();
				}
				tnet.push_back(arg1);
				count++;
			}
			else
			{
				tnet.push_back(arg1);
			}
			//limit++;
		}
	}
	for (int i = 0; i < nets.size(); i++)
	{
		cout << "Net" << i << " ";
		for (int j = 0; j < nets[0].size(); j++)
		{
			cout << nets[i][j] << " ";
		}
		cout << endl;
	}
	infile.close();
	return 0;
}
