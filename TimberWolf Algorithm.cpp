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
double lastCost2; //the last cost2 calculated
double prevLastCost2; //a restore for lastCost2. Since it changes every time the newCost2 function runs, but the newP won't always be accepted, this restores lastCost2 in that case
vector<int> cell1Coords, prevCell1Coords; //holds the coordinates of the gate whose coordinates are changed during perturb. The coordinates before and after the change
vector<vector<int>> cor; //a list of every unique coordinate and how many times they appear in the list of all coordinates
vector<vector<int>> restoreCor; //saves cor before changes from newCost2 so if the newP isn't accepted, cor can be reverted
vector<int> netLengths; //a list of every edge between gates/pins in nets

vector<vector<string>> perturb(vector<vector<string>> chip, int height, int length, vector<int> sda, int numGates, vector<int> areas, int lcd, double T) { //move, swap, or mirror a random gate
	int choice = rand() % 2; //0 or 1 to decide on move or swap
	int cell1 = rand() % numGates; //choose random gate
	int cell2 = 999999; //a large value the random function wouldn't generate
	while ((cell2 == 999999) || (stoi(chip[cell2][1]) > (stoi(chip[cell1][1]) + (lt / 2))) || (stoi(chip[cell2][1]) < (stoi(chip[cell1][1]) - (lt / 2))) || (stoi(chip[cell2][2]) > (stoi(chip[cell1][2]) + (ht / 2))) || (stoi(chip[cell2][2]) < (stoi(chip[cell1][2]) - (ht / 2)))) {
		//hasn't changed	goes past right range wall						goes past left range wall						goes past top wall				goes under bottom wall
		cell2 = rand() % numGates; //keep looking for a cell until it is within the scope
	}
	int row = 0;
	string placeholderX1, placeholderY1, placeholderX2, placeholderY2; //used for swapping coordinates
	int len1 = areas[cell1] / lcd; //length of gate1
	int len2 = areas[cell2] / lcd; //length of gate2
	int dFromEdge = 0; //distance from closest edge. either top of the chip or bottom
	double tcurr = T; //current temperature
	double tnext = T - .95; //next temperature
	prevCell1Coords.push_back(stoi(chip[cell1][1]));//save the old coordinates of the chosen gate to be moved
	prevCell1Coords.push_back(stoi(chip[cell1][2]));


	if (choice == 0) {//Move
		row = rand() % height; //move to random row
		chip[cell1][2] = to_string(row);
		cell1Coords.push_back(stoi(chip[cell1][1]));
		cell1Coords.push_back(row);
		option = 0;
	}
	else {//Swap or mirror
		placeholderX1 = chip[cell1][1];//save the coordinates of the 2 cells to be swapped in their placeholders
		placeholderY1 = chip[cell1][2];
		placeholderX2 = chip[cell2][1];
		placeholderY2 = chip[cell2][2];

		if (((len2 + stoi(chip[cell1][1])) > length) || ((len1 + stoi(chip[cell2][1])) > length)) { //mirror. if the new x coordinate makes either of the gates go over the right wall of the chip, then can't swap and must mirror
			if (stoi(chip[cell1][2]) > (height / 2)) {//depending on which vertical half of the chip the coordinate is, finds the distance from the closer of the top and bottom of the chip and uses that to find the mirrored coordinate on the other half of the chip and sets the y coordinate to that
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
		else {//swaps the x and y coordinates of the 2 selected gates
			chip[cell1][1] = placeholderX2;
			chip[cell1][2] = placeholderY2;
			chip[cell2][1] = placeholderX1;
			chip[cell2][2] = placeholderY1;
			cell1Coords.push_back(stoi(chip[cell1][1]));
			cell1Coords.push_back(stoi(chip[cell1][2]));
			option = 1;
		}
	}
	if (tcurr > 1 && tnext > 1) { //reduces the scope of the window for selecting a cell. Must stay above 1 as 1 and below produces 0 and negative values
		lt = lt * (log(tnext) / log(tcurr)); 
		ht = ht * (log(tnext) / log(tcurr));
	}
	return chip;
}

double schedule(double T)
{
	if ((T >= 0 && T < 1000000 / 3) || (T > (2/3)*1000000 && T <= 1000000)) {//for low and high temperatures, the temperature is reduced by .8
		return T - .8;
	}
	else {// for middling temperatures, the temperature is reduced by .95
		return T - .95;
	}
}

int max(vector<int> temp)//returns the largest value in a vector
{
	int max = temp[0];
	for (int i = 1; i < temp.size(); i++)
	{
		if (temp[i] > max) { max = temp[i]; }
	}
	return max;
}

int min(vector <int> temp)//returns the smallest value in a vector
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
	double maxNLength = 0, avgNetLength = 0;//largest net and average net length
	vector <int> xcoord;//stores all x coordinates of gates/pins in a net
	vector <int> ycoord;//stores all y coordinates of gates/pins in a net
	vector <int> net_length;//stores all net lengths

	for (int i = 0; i < nets.size(); i++)
	{
		for (int j = 0; j < nets[i].size(); j++)//for every pin/gate in every net
		{
			if (nets[i][j].substr(0).compare("p") == 0) { //if pin
				ind = stoi(nets[i][j].substr(1, nets[i][j].size() - 1)) + (areas.size()-1);//index to find coordinate of the pin
			}
			else {// if gate
				ind = stoi(nets[i][j].substr(1, nets[i][j].size() - 1));//index to find coordinate of the gate
			}	
			xcoord.push_back(stoi(coord[ind][1]));//store x and y coordinates 
			ycoord.push_back(stoi(coord[ind][2]));
		}
		rangey = max(ycoord) - min(ycoord);//these ranges represent the height and length of the net bounding box. The net length is half the perimeter, or just the length+height
		rangex = max(xcoord) - min(xcoord);
		net_length.push_back(rangey + rangex);//save net length
		sum += rangey + rangex;//summing net length to later find the average
		xcoord.clear();
		ycoord.clear();
	}
	netLengths = net_length;//to show the net lengths at the end of the algorithm after the final iteration
	maxNLength = max(net_length);//the critical net
	
	if (c == 1)//these are selects. since this function can easily do costs 1 and 5, the select determines which value to return. 1 is average net length, so cost 1
	{
		avgNetLength = sum / nets.size();
		avgNetLength /= maxNLength;
		cout << "avgnetlength: " << avgNetLength << endl;
		return avgNetLength;
	}
	else if (c == 5)//this is cost 5, the critical net
	{
		cout << "maxNLength: " << maxNLength << endl;
		return maxNLength;
	}
	else {
		cout << "Invalid select chosen" << endl;
		return 0;
	}
}

double cost2(vector<vector<string>> coord, vector<int> areas, int minl, int length, int height) {//overlap cost, but only used once, when calculating the initial placement cost
	double overlap = 0;
	int numCoords = length * height, x = 0, y = 0;
	vector<int> row;
	bool found = false;//represents if a coordinate is already in the lsit of coordinates, and so it overlaps that coordinate

	for (int i = 0; i < areas.size(); i++) {//for every gate
		x = stoi(coord[i][1]);//store their coordinates
		y = stoi(coord[i][2]);
		for (int j = 0; j < cor.size(); j++) {//for every row in the list of coordinates, cor
			if (cor[j][0] == x && cor[j][1] == y) {//if the coordinate is found in the list
				if (cor[j][2] == 1) {//and it's only there once
					overlap++;//increase the overlap count
				}
				cor[j][2]++;//increase the count of how many gates are on that coordinate
				found = true;//indicate the coordinate was already in the list
				break;//now that it's been found, no need to check the rest of the list
			}
		}
		if (found == false) {//if not found, add it to the list with an appearnace count of 1
			row.push_back(x);
			row.push_back(y);
			row.push_back(1);
			cor.push_back(row);
			row.clear();
		}
		found = false;//if found is true, it skips the adding to the list part, but now found must be reset for the next iteration
	}
	lastCost2 = overlap / numCoords; //the number of overlapped coordinates/the number of possible coordinates
	prevLastCost2 = lastCost2;//these 2 lines before return are giving these variables their initial value of the overlap of the initial placement
	return overlap / numCoords;
}

double newCost2(int length, int height) {//overlap cost, only for calculating newCost. Only adjusts cor for the change from perturb so the entire overlap of the chip doesn't have to be calculated every time, just what changed
	int numCoords = length * height, x = cell1Coords[0], y = cell1Coords[1], px = prevCell1Coords[0], py = prevCell1Coords[1];//max number of coordinates, then the xy coordinates for the gate changed by perturb, the coordinates before the change and after
	restoreCor = cor;//save cor before changing it for possible restoration later
	vector<int> tempCor;
	bool foundNew = false;//tracks if the coordinate thatthe gate was changed to is already in cor or not
	if (option == 1) {//if a swap occurs, overlap doesn't change
		cout << "lastCost2: " << lastCost2 << endl;
		return lastCost2;
	}
	lastCost2 = lastCost2 * numCoords;//remove the divided out number of coordinates so lastCost2 is the number of overlapped coordinates
	for (int j = 0; j < cor.size(); j++) {//for every coordinate in cor
		if (cor[j][0] == px && cor[j][1] == py && px == x && py == y) {//if the coordinate the gate was mvoed to is the same as where it was, the overlap didn't change
			return lastCost2;
			cout << "a0" << endl;
		}
		else if (cor[j][0] == px && cor[j][1] == py) {// if the previous location of the gate is in cor
			cout << "a1" << endl;
			if (cor[j][2] > 2) {//if the appearance count is over 2, removing 1 will still make the count greater than 1 so overlap is still occuring there so the overlap count doesn't have to be decreased, just the appearance count
				cor[j][2]--;
				
			}
			else if (cor[j][2] == 2) {//if the appearance count is 2, then now that the gate moved, the count becomes 1, which means there's no longer overlap there and so the overlap count and appearance account are decremeneted by 1
				cor[j][2]--;
				lastCost2--;
			}
			else {//if the attendance count is 1 (can't be any value less than 1) then the gate's previous coordinate is the only instance of that coordinate and so by moving the gate, that coordinate is gone and so it has to be removed from cor
				cor.erase(cor.begin() + j);
			}
		}
		else if (cor[j][0] == x && cor[j][1] == y) {//if the coordinate that the gate mvoed to is found
			cout << "a2" << endl;
			if (cor[j][2] == 1) {//and there's already another gate there, but only 1, increase overlap count by 1 and the appearance count
				lastCost2++;
				cor[j][2]++;
			}
			else {//otherwise the coordinate is already considered in the overlap count and so no need to increase overlap, just the appearance count
				cor[j][2]++;
			}
			foundNew = true;//if found, then no need to add it
		}
		else if (j == (cor.size() - 1) && foundNew == false) {//if the coordinate of the new location is not found by the end of cor, it is not in cor and must be added
			cout << "a3" << endl;
			tempCor.push_back(x);
			tempCor.push_back(y);
			tempCor.push_back(1);
			cor.push_back(tempCor);
			tempCor.clear();
		}
	}
	lastCost2 /= numCoords;
	cout << "lastCost2: " << lastCost2 << endl;
	return lastCost2;
}

double cost3(vector<vector<string>> coord, vector<int> areas, int minl, int maxh, int maxl) {//average row length
	vector<int> xsofrow, rowLens;
	int minx = 0, maxx = 0, range = 0;

	for (int j = 0; j < maxh; j++) {//for every row on the chip
		for (int i = 0; i < areas.size(); i++) {//for every gate
			if (stoi(coord[i][2]) == j) {// if a gate exists on that row of the chip, it's s value is added to a vector that tracks them all
				xsofrow.push_back(stoi(coord[i][1]));
			}
		}
		maxx = max(xsofrow);//finds the largest x value in the row
		minx = min(xsofrow);//finds the smallest x value of the row
		range = maxx - minx;//the range of the x values is the length of the row
		rowLens.push_back(range);//save the length of the row
		xsofrow.clear();
	}

	double avgRowLen = 0;
	for (int k = 0; k < rowLens.size(); k++) {//sum up all the row lengths
		avgRowLen += rowLens[k];
	}
	avgRowLen /= rowLens.size();//find the average row length
	avgRowLen /= maxl;//normalize with the largest possible row length
	cout << "avgRowLen: " << avgRowLen << endl;
	return avgRowLen;
}

double cost4(vector<vector<string>> nets, int length, int height, vector<vector<string>> coords, int numGates) {//congestion
	double slope = 0;
	double yint = 0; //y intercept
	vector<vector<int>> edgeCoords; //holds the coordinates of every point used by net edges
	vector<int> edgePoint;
	string point1 = "", point2 = "";
	int ind1 = 0, ind2 = 0;
	int boundx1 = length / 4, boundx2 = length / 2, boundx3 = 3 * length / 4, boundy1 = height / 4, boundy2 = height / 2, boundy3 = 3 * height / 4;//boundary lines for the regions of congestion
	int r1C = 0, r2C = 0, r3C = 0, r4C = 0, r5C = 0, r6C = 0, r7C = 0, r8C = 0, r9C = 0, r10C = 0, r11C = 0, r12C = 0, r13C = 0, r14C = 0, r15C = 0, r16C = 0; //region counters of congestion
	double avgCongestion = 0;
	int x = 0, y = 0;
	bool found = false;//tracks if a common coordinate between 2 edges occurs

	for (int i = 0; i < nets.size(); i++) {
		for (int j = 0; j < nets[i].size() - 1; j++) {//for every pair of pins/gates in each net
			point1 = nets[i][j];//get the name of the pin/gate
			point2 = nets[i][j + 1];
			if (point1.substr(0).compare("p") == 0) {//remove the first value of the name and if it's p, then displace the value previously attached to p byy the number of gates to get the right index for the pin in the coordinate list
				ind1 = stoi(point1.substr(1, point1.length())) + (numGates - 1);
			}
			else {//otherwise it's a gate so the index is the number next to a
				ind1 = stoi(point1.substr(1, point1.length()));
			}

			if (point2.substr(0).compare("p") == 0) {//repeat for the second gates/pins in the pair
				ind2 = stoi(point2.substr(1, point2.length())) + (numGates - 1);
			}
			else {
				ind2 = stoi(point2.substr(1, point2.length()));
			}

			if (stoi(coords[ind1][1]) == stoi(coords[ind2][1])) {//if the x coordinates of the pair are the same, the edge is just a vertical line so the slope is 1
				slope = 1;
			}
			else {//otherwise calculates the slope with rise over run
				slope = abs(stoi(coords[ind1][2]) - stoi(coords[ind2][2])) / abs(stoi(coords[ind1][1]) - stoi(coords[ind2][1]));
			}
			yint = stoi(coords[ind1][2]) - (slope * stoi(coords[ind1][1]));//using y=mx+b, calculates the y intercept


			if (stoi(coords[ind1][2]) < stoi(coords[ind2][2])) {// depending on which gate/pin is above the other on the chip. This is if the second gate/pin of the pair is above the first
				for (int k = stoi(coords[ind1][2]); k < stoi(coords[ind2][2]); k++) {//for every y value between the gates/pins
					x = (k - yint) / slope;//these values must be in the net edge
					y = k;
					for (int q = 0; q < edgeCoords.size(); q++) {//for every net edge
						if (edgeCoords[q][0] == x && edgeCoords[q][1] == y) {//if the coordinate is already in the list of net edge coordinates
							if (edgeCoords[q][2] == 1) {//if there isn't an intersection there yet
								if ((x >= 0) && (x < boundx1)) {//find the region of the intersection and increment
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
							edgeCoords[q][2]++;//increase appearance count
							found = true;//indicate the point was found at least once
							//keep going since a net edge can have mroe than one intersection
							//break;
						}
					}
					if (found == false) {//if not in the list of net edge coordinates, must be added with an appearance count of 1
						edgePoint.push_back(x);
						edgePoint.push_back(y);
						edgePoint.push_back(1);
						edgeCoords.push_back(edgePoint);
						edgePoint.clear();
					}
					found = false;//reset the found tracker
				}
			}
			else {//the same thing as above but instead if gate/pin 1 is above gate/pin 2
				for (int k = stoi(coords[ind2][2]); k < stoi(coords[ind1][2]); k++) {
					x = (k - yint) / slope;
					y = k;
					for (int q = 0; q < edgeCoords.size(); q++) {
						if (edgeCoords[q][0] == x && edgeCoords[q][1] == y) {
							if (edgeCoords[q][2] == 1) {
								if ((x >= 0) && (x < boundx1)) {
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
	avgCongestion /= 16; //find congestion by taking the average of the congestions of the 16 regions
	cout << "avgCongestion: " << avgCongestion << endl;
	return avgCongestion;
}



int main()
{
	srand((unsigned int)time(NULL));	// Seed random generator
	option = 0; //whcih action perturb did. move, swap, mirror are respectively 0, 1, 2
	lastCost2 = 0, prevLastCost2 = 0; //the overlap cost of the new placement after perturb and a restore variable in case the changes aren't accepted
	string fileare = "ibm01.are";//area file
	string text; //stores arguments from the text file
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
	bool changed = false;

	ifstream infil(fileare); //open the area file
	vector <int> areas; //holds all the gate areas. pins aren't incldued since their area is always 0
	vector <string> pins; //holds all pin names
	vector <string> names; //gate names
	vector <string> tcoord; // holds each set of coordinates to be stored in coord
	vector <int> scldwnarea; //holds the scaled down areas
	vector <string> pins2;//a copy of pins

	string filenet = "ibm01.net";//the net file
	string arg1, arg2, arg3; //stores the arguments from the net file
	ifstream infile(filenet);//open net file
	vector<vector<string>> nets; //stores all the nets, eadch row holds every gate/pin in the net in their proper order
	vector<string> tnet;//holds each row of nets and pushes it into nets
	vector<vector<string>> coord; //holds all coordinates for gates and pins
	vector<vector<string>> newP;  //holds the perturbed version of coord

	int pperh = 0; // Pins divided by height
	if (!infil.is_open())//check if file is succesfully opened
	{
		cout << "Error opening file";
		return 0;
	}
	else
	{
		while (!infil.eof())//take in arguments from text file. Store names of gates in names and areas of gates in areas
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
		range = maxl - minl; //find variance in length of gates
	}

	//***********************************************************************************************************//

	int count = 0, numpads = 0;

	if (!infile.is_open())//make sure file opens ok
	{
		cout << "Error opening file";
		return 0;
	}
	else
	{
		for (int i = 0; i < 5; i++)//remove first 5 lines, not needed
		{
			infile >> arg1;
			numpads = stoi(arg1);
		}

		while (!infile.eof())//store nets in the nets list. Based on s. It stores every pin/gate name from the first s to just before the next s in a row. Each row is a net
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
	if (minl > 0)//calculate the longest length and total length
	{
		asum /= minl;
		maxl /= minl;
	}

	if (range < 9999)//if the variance between the smallest and largest lengths is less than 9999, then the chip can be roughly square shaped
	{
		maxh = (int)ceil(sqrt(maxl));
		maxl = maxh;
	}
	else // otherwise there's at least one massive gate and to avoid too much empty space, the length must revolve around that massive length
	{
		maxh = (int)ceil((double)asum / maxl);
	}
	ph = (double)maxh / (maxl + maxh);//what % that height makes up the chip's perimeter
	pl = (double)1 - ph;//what % length makes up of the chip's perimeter
	npins = (int)pins.size() - 1;//number of pins
	nwidth = (int)ceil(npins * ph);// number of pins allocated to be on the sides of the chip
	if (nwidth % 2 != 0) {//if not an even value, give it one more
		nwidth++;
	}
	nlength = npins - nwidth;//number of pins allocated for the top and bottom of the chip

	nlength /= 2;//divide by 2 so they're how many pins per indivual length and height rather than both sides
	nwidth /= 2;
	lt = maxl;//the range limiting factor used in perturb. They initially start at the chip's entire length and height
	ht = maxh;

	int randx = 0, randy = 0;

	for (int i = 0; i < areas.size(); i++)
	{
		scldwnarea.push_back(areas[i] / minl);//the lengths of each gate. minl is the height of each gate
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
		pins2.push_back(names[i]);//copy names to pins2
	}
	int pcount = 0;

	if (nwidth != 0) {//places the pins at equal intervals along the border of the chip. The number of pins on each side of the chip was determiend by the ratio of length and height
		if (nwidth <= 2) {//if the number of pins for the height are 2, the pins have to be placed at the middle of both sides
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
		else {//otherwise they are placed at equal intervals
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
	if (nlength != 0) {//same is done for the length
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
	//initial placement done
	//********************************************************************************************************************************************************************************************************************************************************************//
	
	
	/*
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
	*/
	double T = 100000; //initial temperature
	
	tChecker = T; //used to check temperature while running
	int counter = 0;//loop counter for inner while loop
	int rnd = 0;//random value either 1 or 0 for one of the conditions that allows a changed placement to be approved even if the change in cost is worse
	cost = cost1_5(nets, coord, areas, 1) + cost2(coord, areas, minl, maxl, maxh) + cost3(coord, areas, minl, maxh, maxl) + cost4(nets, maxl, maxh, coord, areas.size()) + cost1_5(nets, coord, areas, 5);//calculate the cost of the initial placement
	cout << "cost: " << cost << endl;
	while (T > 1) {//until temperature goes under 1, repeat
		counter = 0;
		while (counter < (areas.size() / 500)) {//trials per temperature
			newP = perturb(coord, maxh, maxl, scldwnarea, areas.size(), areas, minl, T);//create a slightly changed placement
			if (changed == true) {//only if the previous changed placement was approved
				cost = newCost;//cost is now the previous newCost. Since newCost hasn't been recalculated yet, its value is saved and so I never have to recalculate cost again. It either doesn't change, so it stays what it is, or it changed to the newCost, whose value is already saved
			}
			newCost = cost1_5(nets, newP, areas, 1) + newCost2(maxl, maxh) + cost3(newP, areas, minl, maxh, maxl) + cost4(nets, maxl, maxh, newP, areas.size()) + cost1_5(nets, newP, areas, 5);//calculate the cost of the new placement
			cout << "cost: " << cost << endl;
			cout << "newCost: " << newCost << endl;
			deltaCost = newCost - cost;//calculate the difference in cost
			rnd = rand() % 2; //0 or 1
			if (deltaCost < 0) {//if the new cost is lower, the new placement becomes the saved one
				coord = newP;//new placement is now the standard
				changed = true;//tracks that the new placement was approved
				prevLastCost2 = lastCost2;//the restore variable is updated to the overlap of the accepted placement
			}
			else if (rnd > exp(deltaCost / T)) {//if the cost is worse but these conditions are met, the new placement is approved anyways
				coord = newP;//new placement is now the standard
				changed = true;//tracks that the new placement was approved
				prevLastCost2 = lastCost2;//the restore variable is updated to the overlap of the accepted placement
			}
			else {
				lastCost2 = prevLastCost2; //restore lastCost2 since cost didn't change so neither should lastCost2
				cor = restoreCor; //restore cor as the change wasn't accepted
				changed = false;//new placement was rejected so cost won't be altered in the next iteration
			}
			counter++;
		}
		//T = schedule(T);//decrease the temperature based on if it's a high, medium, or low temperature
		T -= .95;
		tChecker = T;//update tChecker for viewing in the watch feature during running
	}

	for (int i = 0; i < coord.size(); i++) {//
		
		if (i > areas.size()-1) {
			coord[i].push_back("0");
		}
		else {
			coord[i].push_back(to_string(scldwnarea[i]));//attach the length of all the gates to their coordinates. Useful for graphing later
		}
	}
	ofstream after;
	after.open("FinalOutput.txt");
	after << minl*maxl << endl;
	after << minl*maxh << endl;
	for (int h = 0; h < coord.size(); h++)//store the final placement in an output file
	{
		for (int r = 0; r < coord[h].size(); r++)
		{
			after << coord[h][r] << " ";
		}
		after << endl;
	}
	after.close();

	cost1_5(nets, coord, areas, 1);//recalculate net length of final placement and display it

	cout << "Net Lengths:" << endl;
	for (int i = 0; i < netLengths.size(); i++) {
		cout << netLengths[i] << endl;
	}
	cout << endl;
	cout << "Overlap:" << endl;
	cout << newCost2(maxl, maxh) << endl;//show overlap of final placement
	
	/*
	// HERE FOR TESTING PURPOSES
	for (int i = 0; i < 10; i++) {
		newP = perturb(coord, maxh, maxl, scldwnarea, areas.size(), areas, minl, T);
		newCost = cost1_5(nets, newP, areas, 1) + newCost2(maxl, maxh) + cost3(newP, areas, minl, maxh, maxl) + cost4(nets, maxl, maxh, newP, areas.size()) + cost1_5(nets, newP, areas, 5);
		cout << "newCost: " << newCost << endl;
		cout << "option: " << option << endl;
	}
	/*
	for (int i = 0; i < coord.size(); i++) {
		if (stoi(coord[i][2]) != stoi(newP[i][2])) {
			cout << "difference found" << endl;
		}
	}
	*/

	infil.close();
	return 0;
}
