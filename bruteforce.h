#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

#include <vector>
#include "plant.h"

using namespace std;

extern vector<int> brute_result;

void brute_force_algorithm(const int &numbers_beds, vector<Plant> plants, const int* layout, const vector<vector<int>> &costs, vector<bool> used_plants, int current_plant, int all_cost, int all_time, vector<int> result);

#endif