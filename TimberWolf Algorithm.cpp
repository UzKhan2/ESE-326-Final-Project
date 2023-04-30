#include <iostream>
#include <fstream>
#include <vector>
#include <ctime> 
#include <math.h>
#include <string>
using namespace std;

int main()
{
	srand((unsigned int)time(NULL));	// Seed random generator
	string fileare = "ibm01.are";
	string text;
	int area;	// Individual area of part
	int asum = 0;	// Sum of all areas in a file
	int maxl = 0;	// Largest length of a part
	int minl = 99999;	// Smallest length of a part
	int range = 0;	// Diffrence between largest and smallest part
	int maxh = 0;	// Max height
	double ph = 0;	// Total height pins
	double pl = 0;	// Total length pins
	int npins = 0;	// Number of pins
	int nlength = 0, nwidth = 0;	// Number of pins in height and length
	int pinplc = 0;	// Pin placement location

	ifstream infil(fileare);
	vector <int> areas;
	vector <string> pins;
	vector <string> names;
	vector <vector<string>> coord;
	vector <string> tcoord;
	vector <int> scldwnarea;
	vector <string> pins2;

	string filenet = "ibm01.net";
	string arg1, arg2, arg3;
	ifstream infile(filenet);
	vector<vector<string>> nets;
	vector<string> tnet;

	int pperh =  0; // Pins divided by height
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
	if (minl > 0)
	{
		asum /= minl;
		maxl /= minl;
	}

	if (range < 9999)
	{
		maxh = (int) ceil(sqrt(maxl));
		maxl = maxh;
		//maxh /= minl;
		//maxl /= minl;
	}
	else
	{
		maxh = (int) ceil((double) asum / maxl);
		//maxh /= minl;
		//maxl /= minl;
	}
	ph = maxh / (maxl + maxh);
	pl = 1 - ph;
	npins = (int) pins.size() - 1;
	nlength = npins *  (int) pl / 2;
	nwidth = npins * (int) ph / 2;

	cout << maxh << endl;
	cout << npins;

	int randx = 0, randy = 0;

	for (int i = 0; i < areas.size(); i++)
	{
		scldwnarea.push_back(areas[i] / minl);
		//cout << scldwnarea[i] << endl;
	}
	for (int i = 0; i < areas.size(); i++) //initial placement of gates only, done randomly
	{
		tcoord.push_back(names[i]);
		if (maxl - scldwnarea[i] == 0)
		{
			randx = 0;
		}
		else
		{
			randx = rand() % (maxl - scldwnarea[i]);
		}
		//cout << randx << endl;
		randy = rand() % maxh;
		tcoord.push_back(to_string(randx));
		tcoord.push_back(to_string(randy));
		coord.push_back(tcoord);
		tcoord.clear();
	}

	for (int i = areas.size(); i < names.size(); i++)
	{
		pins2.push_back(names[i]);
	}
	if (nwidth != 0) {pperh = maxh / nwidth;}

	for (int i = 0; i < nwidth; i++)	// Left wall
	{
		pinplc = rand() % pins2.size();	// Find random part
		tcoord.push_back(pins2[pinplc]);	// Takes in pin
		tcoord.push_back(to_string(0));
		tcoord.push_back(to_string(i* pperh));
		coord.push_back(tcoord);
		tcoord.clear();
		pins2.erase(pins2.begin()+pinplc);
	}

	for (int i = 0; i < nwidth; i++)	// Right wall
	{
		pinplc = rand() % pins2.size();	// Find random part
		tcoord.push_back(pins2[pinplc]);	// Takes in pin
		tcoord.push_back(to_string(maxl));
		tcoord.push_back(to_string(i * pperh));
		coord.push_back(tcoord);
		tcoord.clear();
		pins2.erase(pins2.begin() + pinplc);
	}

	/*for (int i = areas.size(); i < names.size(); i++)
	{
		tcoord.push_back(names[i]);
		
		tcoord.clear();
	}*/

	for (int h = 0; h < coord.size(); h++)
	{
		for (int r = 0; r < coord[0].size(); r++)
		{
			cout << coord[h][r] << " ";
		}
		cout << endl;
	}

	infil.close();
	return 0;
}
