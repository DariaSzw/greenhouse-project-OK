#ifndef ANTCOLONY_H
#define ANTCOLONY_H

#include <vector>
#include "plant.h"

using namespace std;

vector<int> ant_colony(const int &numbers_beds, vector<Plant> plants, const int* layout, const vector<vector<int>> &costs);

#endif