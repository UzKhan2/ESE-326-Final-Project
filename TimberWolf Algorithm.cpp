#include <iostream>
#include <fstream>
#include <vector>
#include <ctime> 
#include <math.h>
#include <string>
using namespace std;

double lt; //length perturb range limit
double ht; //height perturb range limit
double tChecker; //just allows me to check on T while running. If I pause, I can see T's value even when not in the main functions scope
int option; //tracks which 
double lastCost2;
double prevLastCost2;
vector<int> cell1Coords, prevCell1Coords;
vector<vector<int>> cor;
vector<int> netLengths;

vector<vector<string>> perturb(vector<vector<string>> chip, int height, int length, vector<int> sda, int numGates, vector<int> areas, int lcd, double T) { //coords, maxh, maxl, scldwnarea, areas.size(), areas, minl, temperature
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
	prevCell1Coords.push_back(stoi(chip[cell1][1]));
	prevCell1Coords.push_back(stoi(chip[cell1][2]));


	if (choice == 0) {//Move
		row = rand() % height; //move to random row
		chip[cell1][2] = to_string(row);
		cell1Coords.push_back(stoi(chip[cell1][1]));
		cell1Coords.push_back(row);
		option = 0;
	}
	else {//Swap
		placeholderX1 = chip[cell1][1];
		placeholderY1 = chip[cell1][2];
		placeholderX2 = chip[cell2][1];
		placeholderY2 = chip[cell2][2];

		if (((len2 + stoi(chip[cell1][1])) > length) || ((len1 + stoi(chip[cell2][1])) > length)) { //mirror. if the new x coordinate makes either of the gates go over the right wall of the chip, then can't swap and must mirror
			if (stoi(chip[cell1][2]) > (height / 2)) {
				dFromEdge = height - stoi(chip[cell1][2]);
				chip[cell1][2] = to_string(dFromEdge);
			}
			else {
				dFromEdge = stoi(chip[cell1][2]);
				chip[cell1][2] = to_string(height - dFromEdge);
			}
			cell1Coords.push_back(stoi(chip[cell1][1]));
			cell1Coords.push_back(stoi(chip[cell1][2]));
			option = 2;
		}
		else {
			chip[cell1][1] = placeholderX2;
			chip[cell1][2] = placeholderY2;
			chip[cell2][1] = placeholderX1;
			chip[cell2][2] = placeholderY1;
			cell1Coords.push_back(stoi(chip[cell1][1]));
			cell1Coords.push_back(stoi(chip[cell1][2]));
			option = 1;
		}
	}
	if (tcurr > 1 && tnext > 1) {
		lt = lt * (log(tnext) / log(tcurr)); //reduce scope
		ht = ht * (log(tnext) / log(tcurr));
	}
	return chip;
}

double schedule(double T)
{
	if ((T >= 0 && T < 1000000 / 3) || (T > (2/3)*1000000 && T <= 1000000)) {
		return T - .8;
	}
	else {
		return T - .95;
	}
}

int max(vector<int> temp)
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
		for (int j = 0; j < nets[i].size(); j++)
		{
			if (nets[i][j].substr(0).compare("p") == 0) { //if pin
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
	netLengths = net_length;
	maxNLength = max(net_length);
	if (c == 1)
	{
		avgNetLength = sum / nets.size();
		avgNetLength /= maxNLength;
		return avgNetLength;
	}
	else if (c == 5)
	{
		return maxNLength;
	}
	else {
		cout << "Invalid select chosen" << endl;
		return 0;
	}
}

double cost2(vector<vector<string>> coord, vector<int> areas, int minl, int length, int height) {
	double overlap = 0;
	int numCoords = length * height, x = 0, y = 0;
	vector<int> row;
	bool found = false;

	for (int i = 0; i < areas.size(); i++) {
		x = stoi(coord[i][1]);
		y = stoi(coord[i][2]);
		for (int j = 0; j < cor.size(); j++) {
			if (cor[j][0] == x && cor[j][1] == y) {
				if (cor[j][2] == 1) {
					overlap++;
				}
				cor[j][2]++;
				found = true;
				break;
			}
		}
		if (found == false) {
			row.push_back(x);
			row.push_back(y);
			row.push_back(1);
			cor.push_back(row);
			row.clear();
		}
		found = false;
	}
	lastCost2 = overlap / numCoords;
	prevLastCost2 = lastCost2;
	return overlap / numCoords;
}

double newCost2(int length, int height) {
	int numCoords = length * height, x = cell1Coords[0], y = cell1Coords[1], px = prevCell1Coords[0], py = prevCell1Coords[1];
	vector<int> tempCor;
	bool foundNew = false;
	if (option == 1) {
		return lastCost2;
	}
	lastCost2 = lastCost2 * numCoords;
	for (int j = 0; j < cor.size(); j++) {
		if (cor[j][0] == px && cor[j][1] == py && px == x && py == y) {
			return lastCost2;
		}
		else if (cor[j][0] == px && cor[j][1] == py) {
			if (cor[j][2] > 2) {
				cor[j][2]--;
			}
			else if (cor[j][2] == 2) {
				cor[j][2]--;
				lastCost2--;
			}
			else {
				//cor[j].clear();
				cor.erase(cor.begin() + j);
			}
		}
		else if (cor[j][0] == x && cor[j][1] == y) {
			if (cor[j][2] == 1) {
				lastCost2++;
				cor[j][2]++;
			}
			else {
				cor[j][2]++;
			}
			foundNew = true;
		}
		else if (j == (cor.size() - 1) && foundNew == false) {
			tempCor.push_back(x);
			tempCor.push_back(y);
			tempCor.push_back(1);
			cor.push_back(tempCor);
			tempCor.clear();
		}
	}
	lastCost2 /= numCoords;
	return lastCost2;
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

double cost4(vector<vector<string>> nets, int length, int height, vector<vector<string>> coords, int numGates) {
	double slope = 0;
	double yint = 0; //y intercept
	lastCost2 = 0, prevLastCost2 = 0;
	vector<vector<int>> edgeCoords;
	vector<int> edgePoint;
	string point1 = "", point2 = "";
	int ind1 = 0, ind2 = 0;
	int boundx1 = length / 4, boundx2 = length / 2, boundx3 = 3 * length / 4, boundy1 = height / 4, boundy2 = height / 2, boundy3 = 3 * height / 4;
	int r1C = 0, r2C = 0, r3C = 0, r4C = 0, r5C = 0, r6C = 0, r7C = 0, r8C = 0, r9C = 0, r10C = 0, r11C = 0, r12C = 0, r13C = 0, r14C = 0, r15C = 0, r16C = 0; //region counts of congestion
	double avgCongestion = 0;
	int x = 0, y = 0;
	bool found = false;

	for (int i = 0; i < nets.size(); i++) {
		for (int j = 0; j < nets[i].size() - 1; j++) {
			point1 = nets[i][j];
			point2 = nets[i][j + 1];
			if (point1.substr(0).compare("p") == 0) {
				ind1 = stoi(point1.substr(1, point1.length())) + (numGates - 1);
			}
			else {
				ind1 = stoi(point1.substr(1, point1.length()));
			}

			if (point2.substr(0).compare("p") == 0) {
				ind2 = stoi(point2.substr(1, point2.length())) + (numGates - 1);
			}
			else {
				ind2 = stoi(point2.substr(1, point2.length()));
			}

			if (stoi(coords[ind1][1]) == stoi(coords[ind2][1])) {
				slope = 1;
			}
			else {
				slope = abs(stoi(coords[ind1][2]) - stoi(coords[ind2][2])) / abs(stoi(coords[ind1][1]) - stoi(coords[ind2][1]));
			}
			yint = stoi(coords[ind1][2]) - (slope * stoi(coords[ind1][1]));


			if (stoi(coords[ind1][2]) < stoi(coords[ind2][2])) {
				for (int k = stoi(coords[ind1][2]); k < stoi(coords[ind2][2]); k++) {
					x = (k - yint) / slope;
					y = k;
					for (int q = 0; q < edgeCoords.size(); q++) {
						if (edgeCoords[q][0] == x && edgeCoords[q][1] == y) {
							if (edgeCoords[q][2] == 1) {
								if ((x >= 0) && (x < boundx1)) {//find and count intersections in each region
									if ((y >= 0) && (y < boundy1)) {
										r1C++;
									}
									else if ((y >= boundy1) && (y < boundy2)) {
										r5C++;
									}
									else if ((y >= boundy2) && (y < boundy3)) {
										r9C++;
									}
									else {
										r13C++;
									}
								}
								else if ((x >= boundx1) && (x < boundx2)) {
									if ((y >= 0) && (y < boundy1)) {
										r2C++;
									}
									else if ((y >= boundy1) && (y < boundy2)) {
										r6C++;
									}
									else if ((y >= boundy2) && (y < boundy3)) {
										r10C++;
									}
									else {
										r14C++;
									}
								}
								else if ((x >= boundx2) && (x < boundx3)) {
									if ((y >= 0) && (y < boundy1)) {
										r3C++;
									}
									else if ((y >= boundy1) && (y < boundy2)) {
										r7C++;
									}
									else if ((y >= boundy2) && (y < boundy3)) {
										r11C++;
									}
									else {
										r15C++;
									}
								}
								else {
									if ((y >= 0) && (y < boundy1)) {
										r4C++;
									}
									else if ((y >= boundy1) && (y < boundy2)) {
										r8C++;
									}
									else if ((y >= boundy2) && (y < boundy3)) {
										r12C++;
									}
									else {
										r16C++;
									}
								}
							}
							edgeCoords[q][2]++;
							found = true;
							break;
						}
					}
					if (found == false) {
						edgePoint.push_back(x);
						edgePoint.push_back(y);
						edgePoint.push_back(1);
						edgeCoords.push_back(edgePoint);
						edgePoint.clear();
					}
					found = false;
				}
			}
			else {
				for (int k = stoi(coords[ind2][2]); k < stoi(coords[ind1][2]); k++) {
					x = (k - yint) / slope;
					y = k;
					for (int q = 0; q < edgeCoords.size(); q++) {
						if (edgeCoords[q][0] == x && edgeCoords[q][1] == y) {
							if (edgeCoords[q][2] == 1) {
								if ((x >= 0) && (x < boundx1)) {//find and count intersections in each region
									if ((y >= 0) && (y < boundy1)) {
										r1C++;
									}
									else if ((y >= boundy1) && (y < boundy2)) {
										r5C++;
									}
									else if ((y >= boundy2) && (y < boundy3)) {
										r9C++;
									}
									else {
										r13C++;
									}
								}
								else if ((x >= boundx1) && (x < boundx2)) {
									if ((y >= 0) && (y < boundy1)) {
										r2C++;
									}
									else if ((y >= boundy1) && (y < boundy2)) {
										r6C++;
									}
									else if ((y >= boundy2) && (y < boundy3)) {
										r10C++;
									}
									else {
										r14C++;
									}
								}
								else if ((x >= boundx2) && (x < boundx3)) {
									if ((y >= 0) && (y < boundy1)) {
										r3C++;
									}
									else if ((y >= boundy1) && (y < boundy2)) {
										r7C++;
									}
									else if ((y >= boundy2) && (y < boundy3)) {
										r11C++;
									}
									else {
										r15C++;
									}
								}
								else {
									if ((y >= 0) && (y < boundy1)) {
										r4C++;
									}
									else if ((y >= boundy1) && (y < boundy2)) {
										r8C++;
									}
									else if ((y >= boundy2) && (y < boundy3)) {
										r12C++;
									}
									else {
										r16C++;
									}
								}
							}
							edgeCoords[q][2]++;
							found = true;
							break;
						}
					}
					if (found == false) {
						edgePoint.push_back(x);
						edgePoint.push_back(y);
						edgePoint.push_back(1);
						edgeCoords.push_back(edgePoint);
						edgePoint.clear();
					}
					found = false;
				}
			}
		}
	}
	avgCongestion = (double)r1C + (double)r2C + (double)r3C + (double)r4C + (double)r5C + (double)r6C + (double)r7C + (double)r8C + (double)r9C + (double)r10C + (double)r11C + (double)r12C + (double)r13C + (double)r14C + (double)r15C + (double)r16C;
	avgCongestion /= 16;
	return avgCongestion;
}



int main()
{
	srand((unsigned int)time(NULL));	// Seed random generator
	option = 0;
	lastCost2 = 0;
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
	double cost = 0, newCost = 0, deltaCost = 0; 
	bool changed = true;

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
	vector<vector<string>> coord; //holds all coordinates for gates and pins
	vector<vector<string>> newP; 

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
	if (minl > 0)
	{
		asum /= minl;
		maxl /= minl;
	}

	if (range < 9999)
	{
		maxh = (int)ceil(sqrt(maxl));
		maxl = maxh;
	}
	else
	{
		maxh = (int)ceil((double)asum / maxl);
	}
	ph = (double)maxh / (maxl + maxh);
	pl = (double)1 - ph;
	npins = (int)pins.size() - 1;
	nwidth = (int)ceil(npins * ph);
	if (nwidth % 2 != 0) {
		nwidth++;
	}
	nlength = npins - nwidth;

	nlength /= 2;
	nwidth /= 2;
	lt = maxl;
	ht = maxh;


	int randx = 0, randy = 0;

	for (int i = 0; i < areas.size(); i++)
	{
		scldwnarea.push_back(areas[i] / minl);
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

	//********************************************************************************************************************************************************************************************************************************************************************//
	
	ofstream before;
	before.open("InitialPlacementOutput.txt");
	before << "Initial Placement: " << endl;
	for (int h = 0; h < coord.size(); h++)
	{
		for (int r = 0; r < coord[h].size(); r++)
		{
			before << coord[h][r] << " ";
		}
		before << endl;
	}
	before.close();

	double T = 100000; //initial temperature
	tChecker = T;
	int counter = 0;
	int rnd = 0;
	cost = cost1_5(nets, coord, areas, 1) + cost2(coord, areas, minl, maxl, maxh) + cost3(coord, areas, minl, maxh) + cost4(nets, maxl, maxh, coord, areas.size()) + cost1_5(nets, coord, areas, 5);
	while (T > 1) {
		counter = 0;
		while (counter < (areas.size() / 500)) {//coords, maxh, maxl, scldwnarea, areas.size(), areas, minl, temperature
			newP = perturb(coord, maxh, maxl, scldwnarea, areas.size(), areas, minl, T);
			if (changed == true) {
				cost = newCost;
			}
			newCost = cost1_5(nets, newP, areas, 1) + newCost2(maxl, maxh) + cost3(newP, areas, minl, maxh) + cost4(nets, maxl, maxh, newP, areas.size()) + cost1_5(nets, newP, areas, 5);
			deltaCost = newCost - cost;
			rnd = rand() % 2; //0 or 1
			if (deltaCost < 0) {
				coord = newP;
				changed = true;
				prevLastCost2 = lastCost2;
			}
			else if (rnd > exp(deltaCost / T)) {
				coord = newP;
				changed = true;
				prevLastCost2 = lastCost2;
			}
			else {
				lastCost2 = prevLastCost2; //restore lastCost2 since cost didn't change so neither should lastCost2
				changed = false;
			}
			counter++;
		}
		//T = schedule(T);
		T -= .95;
		tChecker = T;
	}

	for (int i = 0; i < coord.size(); i++) {
		
		if (i > areas.size()-1) {
			coord[i].push_back("0");
		}
		else {
			coord[i].push_back(to_string(areas[i]));
		}
	}
	ofstream after;
	after.open("FinalOutput.txt");
	after << "Final Placement: " << endl;
	for (int h = 0; h < coord.size(); h++)
	{
		for (int r = 0; r < coord[h].size(); r++)
		{
			after << coord[h][r] << " ";
		}
		after << endl;
	}
	after.close();

	cost1_5(nets, coord, areas, 1);

	cout << "Net Lengths:" << endl;
	for (int i = 0; i < netLengths.size(); i++) {
		cout << netLengths[i] << endl;
	}
	cout << endl;
	cout << "Overlap:" << endl;
	cout << newCost2(maxl, maxh) << endl;
	
	infil.close();
	return 0;
}
