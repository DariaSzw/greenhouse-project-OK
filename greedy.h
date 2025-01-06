#ifndef GREEDY_H
#define GREEDY_H

#include <vector>
#include "plant.h"

using namespace std;

unsigned long long Get_time();

bool check_all_visit(vector<Plant> plants);

void greedy(const int &numbers_beds, vector<Plant> plants, const int* layout, const vector<vector<int>> &costs);

#endif