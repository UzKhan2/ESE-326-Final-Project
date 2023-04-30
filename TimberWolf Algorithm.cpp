#include <iostream>
#include <fstream>
#include <vector>
#include <ctime> 
#include <math.h>
#include <string>

using namespace std;

int main()
{
	srand((unsigned int)time(NULL));
	string fileare = "ibm01.are";
	string text;
	int area;
	int asum = 0;
	int maxl = 0;
	int minl = 99999;
	int range = 0;
	int maxh = 0;
	double ph = 0;
	double pl = 0;
	int npins = 0;
	int nlength = 0, nwidth = 0;
	ifstream infil(fileare);
	vector <int> areas;
	vector <string> pins;
	vector <string> names;
	vector <vector<string>> coord;
	vector <string> tcoord;
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
			if (area == 0)
			{
				pins.push_back(text);
			}
			else
			{
				if (area > maxl)
				{
					maxl = area;
				}
				if (area <= minl)
				{
					minl = area;
				}
				asum += area;
				areas.push_back(area);
			}
			names.push_back(text);
		}
		range = maxl - minl;
	}

	//***********************************************************************************************************//

	string filenet = "ibm01.net";
	string arg1, arg2, arg3;
	ifstream infile(filenet);
	vector<vector<string>> nets;
	vector<string> tnet;
	int count = 0, numpads = 0;

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
			numpads = stoi(arg1);
		}

		while (!infile.eof())
		{
			infile >> arg1;
			infile >> arg2;
			if (arg2.compare("s") == 0)
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
		}
	}
	for (int i = 0; i < nets.size(); i++)
	{
		//cout << "Net" << i << " ";
		for (int j = 0; j < nets[0].size(); j++)
		{
			//cout << nets[i][j] << " ";
		}
		//cout << endl;
	}

	for (int i = 0; i < areas.size(); i++)
	{
		//cout << areas[i] << endl;
	}
	asum /= minl;
	maxl /= minl;

	if (range < 9999)
	{
		maxh = ceil(sqrt(maxl));
		maxl = maxh;
		//maxh /= minl;
		//maxl /= minl;
	}
	else
	{
		maxh = ceil((double)asum / maxl);
		//maxh /= minl;
		//maxl /= minl;
	}
	ph = maxh / (maxl + maxh);
	pl = 1 - ph;
	npins = pins.size() - 1;
	nlength = npins * pl / 2;
	nwidth = npins * ph / 2;


	//cout << ph << endl;
	cout << maxh << endl;
	//cout << maxh << endl;
	//cout << ceil(sqrt(asum)) << endl;
	//cout << asum << endl;
	//cout << numpads << endl;
	cout << npins;	// Pretend it makes sense

	int randx = 0, randy = 0;
	for (int i = 0; i < areas.size(); i++) //initial placement of gates only, done randomly
	{
		tcoord.push_back(names[i]);
		randx = rand() % maxl;
		randy = rand() % maxh;
		tcoord.push_back(to_string(randx));
		tcoord.push_back(to_string(randy));
		coord.push_back(tcoord);
		tcoord.clear();
	}

	for (int h = 0; h < coord.size(); h++) {
		for (int r = 0; r < coord[0].size(); r++) {
			cout << coord[h][r] << " ";
		}
		cout << endl;
	}

	infil.close();
	return 0;
}
