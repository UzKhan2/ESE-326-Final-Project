#include <iostream>
#include <fstream>
#include <vector>
#include <ctime> 
#include <math.h>
#include <string>
using namespace std;

int lt; //length perturb range limit
int ht; //height perturb range limit
vector <vector<string>> coord; //holds all coordinates for gates and pins

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


	if (choice == 0) {//Move
		row = rand() % height; //move to random row
		chip[cell1][2] = row;
	}
	else {//Swap
		placeholderX1 = chip[cell1][1];
		placeholderY1 = chip[cell1][2];
		placeholderX2 = chip[cell2][1];
		placeholderY2 = chip[cell2][2];

		if (((len2 + stoi(chip[cell1][1])) > length) || ((len1 + stoi(chip[cell2][1])) > length)) { //if the new x coordinate makes either of the gates go over the right wall of the chip, then can't swap and must mirror
			if (stoi(chip[cell1][2]) > (height / 2)) {
				dFromEdge = height - stoi(chip[cell1][2]);
				chip[cell1][2] = to_string(dFromEdge);
			}
			else {
				dFromEdge = stoi(chip[cell1][2]);
				chip[cell1][2] = to_string(height - dFromEdge);
			}
		}
		chip[cell1][1] = placeholderX2;
		chip[cell1][2] = placeholderY2;
		chip[cell2][1] = placeholderX1;
		chip[cell2][2] = placeholderY1;
	}
	lt = lt * (log(tnext) / log(tcurr)); //reduce scope
	ht = ht * (log(tnext) / log(tcurr));
	coord = chip;
}

double schedule(double T)
{
	double newT = T - .95;
	if (newT < 0) {
		return T;
	}
	else {
		return newT;
	}
}

int max(vector <int> temp)
{
	int max = temp[0];
	for (int i = 1; i < temp.size(); i++)
	{
		if (temp[i] > max) { max = temp[i]; }
	}
	return max;
}

int min(vector <int> temp)
{
	int min = temp[0];
	for (int i = 1; i < temp.size(); i++)
	{
		if (temp[i] < min) { min = temp[i]; }
	}
	return min;
}

double cost1_5(vector<vector<string>> nets, vector<vector<string>> coord, vector<int> areas, int c)	// Length Cost and Critical Net cost. Determined by c, the select
{
	int rangey = 0, rangex = 0, ind = 0, sum = 0;
	double maxNLength = 0, avgNetLength = 0;
	vector <int> xcoord;
	vector <int> ycoord;
	vector <int> net_length;

	for (int i = 0; i < nets.size(); i++)
	{
		for (int j = 0; j < nets[0].size(); j++)
		{
			if (nets[i][j].substr(1, nets[i][j].size() - 1).compare("p") == 0) { //if pin
				ind = stoi(nets[i][j].substr(1, nets[i][j].size() - 1)) + (areas.size()-1);
			}
			else {// if gate
				ind = stoi(nets[i][j].substr(1, nets[i][j].size() - 1));
			}		
			xcoord.push_back(stoi(coord[ind][1]));
			ycoord.push_back(stoi(coord[ind][2]));
		}
		rangey = max(ycoord) - min(ycoord);
		rangex = max(xcoord) - min(xcoord);
		net_length.push_back(rangey + rangex);
		sum += rangey + rangex;
		xcoord.clear();
		ycoord.clear();
	}
	maxNLength = max(net_length);
	if (c == 1)
	{
		avgNetLength = sum / nets.size();
		return avgNetLength;
	}
	else if (c == 5)
	{
		return maxNLength;
	}
	else {
		cout << "Invalid select chosen" << endl;
	}
}


double cost2(vector<vector<string>> coord, vector<int> areas, int minl, int length, int height) {
	vector<vector<int>> coordsTakenUp; //if 0, free, if 1 then taken up, if 2 taken up by at least 2 cells
	vector<int> allZeroes;
	int overlap = 0; //number of coordinates with overlap
	int numCoords = length * height;
	int lenOfCell = 0;
	int y = 0;
	double percentOfOverlap;

	for (int i = 0; i < length; i++) {
		allZeroes.push_back(0);
	}
	for (int j = 0; j < height; j++) {
		coordsTakenUp.push_back(allZeroes);
	}

	for (int k = 0; k < areas.size(); k++) {
		lenOfCell = areas[k] / minl;
		y = stoi(coord[k][2]);
		for (int l = stoi(coord[k][1]); l < lenOfCell; l++) {
			if (coordsTakenUp[l][y] == 0) {
				coordsTakenUp[l][y] = 1; //marks as taken
			}
			else {
				coordsTakenUp[l][y] = 2; //marks as at least 2 cells took this coordinate
			}
		}
	}

	for (int i = 0; i < coordsTakenUp.size(); i++) { //counts all 2's. Must do this seperately because if say 3 cells use a coordinate, then an extra increment would occur if i did it in the previous loop. I only want overlap to count how many coordinates have at least 2 cells on it, not how many times it occurs
		for (int j = 0; j < coordsTakenUp[0].size(); j++) {
			if (coordsTakenUp[i][j] == 2) {
				overlap++;
			}
		}
	}

	percentOfOverlap = overlap/numCoords; //% of cells with overlap
	return percentOfOverlap;
}



double cost3(vector<vector<string>> coord, vector<int> areas, int minl, int maxh) {
	vector<int> xsofrow, rowLens;
	int minx = 0, maxx = 0, range = 0;

	for (int j = 0; j < maxh; j++) {
		for (int i = 0; i < areas.size(); i++) {
			if (stoi(coord[i][2]) == j) {
				xsofrow.push_back(stoi(coord[i][1]));
			}
		}
		maxx = max(xsofrow);
		minx = min(xsofrow);
		range = maxx - minx;
		rowLens.push_back(range);
		xsofrow.clear();
	}

	double avgRowLen = 0;
	for (int k = 0; k < rowLens.size(); k++) {
		avgRowLen += rowLens[k];
	}
	avgRowLen /= rowLens.size();

	return avgRowLen;
}


int cost4() {

}



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
