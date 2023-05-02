#include <iostream>
#include <fstream>
#include <vector>
#include <ctime> 
#include <math.h>
#include <string>
using namespace std;

int lt; //length perturb range limit
int ht; //height perturb range limit

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

	int pperh = 0; // Pins divided by height
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
		maxh = (int)ceil(sqrt(maxl));
		maxl = maxh;
		//maxh /= minl;
		//maxl /= minl;
	}
	else
	{
		maxh = (int)ceil((double)asum / maxl);
		//maxh /= minl;
		//maxl /= minl;
	}
	ph = (double)maxh / (maxl + maxh);
	pl = (double)1 - ph;
	npins = (int)pins.size() - 1;
	nwidth = ceil(npins * ph);
	if (nwidth == 1) {
		nwidth = 2;
	}
	nlength = npins - nwidth;

	nlength /= 2;
	nwidth /= 2;

	cout << pl << endl;
	cout << ph << endl;
	cout << npins << endl;
	cout << nlength << endl;
	cout << nwidth << endl;

	cout << maxh << endl;
	cout << npins;

	lt = maxl;
	ht = maxh;

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
	int pcount = 0;

	if (nwidth != 0) {
		if (nwidth <= 2) {
			pperh = maxh / (nwidth + 1);

			for (int i = 1; i <= nwidth; i++)	// Left wall
			{
				tcoord.push_back(pins2[pcount]);	// Takes in pin
				tcoord.push_back("0"); //x
				tcoord.push_back(to_string(i * pperh)); //y
				coord.push_back(tcoord);
				tcoord.clear();
				pcount++;
			}

			for (int i = 1; i <= nwidth; i++)	// Right wall
			{
				tcoord.push_back(pins2[pcount]);	// Takes in pin
				tcoord.push_back(to_string(maxl)); //x
				tcoord.push_back(to_string(i * pperh)); //y
				coord.push_back(tcoord);
				tcoord.clear();
				pcount++;
			}
		}
		else {
			pperh = maxh / (nwidth - 1);

			for (int i = 0; i < nwidth; i++)	// Left wall
			{
				tcoord.push_back(pins2[pcount]);	// Takes in pin
				tcoord.push_back("0"); //x
				tcoord.push_back(to_string(i * pperh)); //y
				coord.push_back(tcoord);
				tcoord.clear();
				pcount++;
			}

			for (int i = 0; i < nwidth; i++)	// Right wall
			{
				tcoord.push_back(pins2[pcount]);	// Takes in pin
				tcoord.push_back(to_string(maxl)); //x
				tcoord.push_back(to_string(i * pperh)); //y
				coord.push_back(tcoord);
				tcoord.clear();
				pcount++;
			}
		}
	}



	int pperl = 0;
	if (nlength != 0) {
		if (nlength <= 2) {
			pperl = maxl / (nlength + 1);

			for (int i = 1; i <= nlength; i++)	// Top border
			{
				tcoord.push_back(pins2[pcount]);	// Takes in pin
				tcoord.push_back(to_string(i * pperl)); //x
				tcoord.push_back(to_string(maxh)); //y
				coord.push_back(tcoord);
				tcoord.clear();
				pcount++;
			}

			for (int i = 1; i <= nlength; i++)	// Bottom border
			{
				tcoord.push_back(pins2[pcount]);	// Takes in pin
				tcoord.push_back(to_string(i * pperl)); //x
				tcoord.push_back("0"); //y
				coord.push_back(tcoord);
				tcoord.clear();
				pcount++;
			}
		}
		else {
			pperl = maxl / (nlength - 1);

			for (int i = 0; i < nlength; i++)	// Top border
			{
				tcoord.push_back(pins2[pcount]);	// Takes in pin
				tcoord.push_back(to_string(i * pperl)); //x
				tcoord.push_back(to_string(maxh)); //y
				coord.push_back(tcoord);
				tcoord.clear();
				pcount++;
			}

			for (int i = 0; i < nlength; i++)	// Bottom border
			{
				tcoord.push_back(pins2[pcount]);	// Takes in pin
				tcoord.push_back(to_string(i * pperl)); //x
				tcoord.push_back("0"); //y
				coord.push_back(tcoord);
				tcoord.clear();
				pcount++;
			}
		}
	}


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

void perturb(vector<vector<string>> chip, int height, int length, vector<int> sda, int numGates, vector<int> areas, int lcd, double T) { //coords, maxh, maxl, scldwnarea, areas.size(), areas, minl, temperature
	int choice = rand() % 2; //0 or 1 to decide on move or swap
	int cell1 = rand() % numGates; //choose random gate
	int cell2 = 999999;
	while ((cell2 == 999999) || (stoi(chip[cell2][1]) > (stoi(chip[cell1][1]) + (lt / 2))) || (stoi(chip[cell2][1]) < (stoi(chip[cell1][1]) - (lt / 2))) || (stoi(chip[cell2][2]) > (stoi(chip[cell1][2]) + (ht / 2))) || (stoi(chip[cell2][2]) < (stoi(chip[cell1][2]) - (ht / 2)))) {
		   //hasn't changed				goes past right range wall								goes past left range wall										goes past top wall														goes under bottom wall
		cell2 = rand() % numGates; //keep looking for a cell until it is within the scope
	}
	int row = 0;
	string placeholderX1, placeholderY1, placeholderX2, placeholderY2;
	int len1 = areas[cell1] / lcd; //length of gate1
	int len2 = areas[cell2] / lcd; //length of gate2
	int dFromEdge = 0;
	double tcurr = T;
	double tnext = T - .95;


	if(choice == 0){//Move
		row = rand() % height; //move to random row
		chip[cell1][2] = row;
	}
	else{//Swap
		placeholderX1 = chip[cell1][1];
		placeholderY1 = chip[cell1][2];
		placeholderX2 = chip[cell2][1];
		placeholderY2 = chip[cell2][2];

		if (((len2 + stoi(chip[cell1][1])) > length) || ((len1 + stoi(chip[cell2][1])) > length)) { //if the new x coordinate makes either of the gates go over the right wall of the chip, then can't swap and must mirror
			if (stoi(chip[cell1][2]) > (height/2)) {
				dFromEdge = height - stoi(chip[cell1][2]);
				chip[cell1][2] = to_string(dFromEdge);
			}
			else {
				dFromEdge = stoi(chip[cell1][2]);
				chip[cell1][2] = to_string(height-dFromEdge);
			}
		}
		chip[cell1][1] = placeholderX2;
		chip[cell1][2] = placeholderY2;
		chip[cell2][1] = placeholderX1;
		chip[cell2][2] = placeholderY1;
	}
	lt = lt * (log(tnext) / log(tcurr)); //reduce scope
	ht = ht * (log(tnext) / log(tcurr));
}

double schedule(double T) {
	double newT = T - .95;
	if (newT < 0) {
		return T;
	}
	else {
		return newT;
	}
}
