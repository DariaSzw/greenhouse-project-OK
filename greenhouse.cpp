#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <climits>
#include "plant.h"
#include "greedy.h"
#include "antcolony.h"
#include "bruteforce.h"

using namespace std;

int main()
{   
    srand(time(nullptr));

    int numbers_rows=2; //liczba rzędów w szklarni
    int numbers_beds_in_row=3; //liczba grządek w rzędzie
    int numbers_beds=numbers_rows*numbers_beds_in_row; //liczba grządek w szklarni

    /*
        wczytywanie roślin z bazy
    */
    
    vector <Plant> plants;
    int lenght_vector_plants=plants.size();
    
    while (lenght_vector_plants!=numbers_beds)
    {
        ifstream file("output/plant_base.json");

        if (!file.is_open()) 
        {
            cerr << "Unable to read plant database from file!" << endl;
            return 1;
        }

        string line_base; 

        string name;
        int water_demand; //na metr kwadratowy w litrach

        while (getline(file, line_base)) 
        {
            line_base.erase(remove(line_base.begin(), line_base.end(), '"'), line_base.end());
            line_base.erase(remove(line_base.begin(), line_base.end(), ' '), line_base.end());
            
            stringstream ss1(line_base);  
            string cell1;

            if (line_base == "}," || line_base == "}")
            {
                plants.emplace_back(name, water_demand);
                lenght_vector_plants=plants.size();
            }
            else if (line_base != "{")
            {
                while (getline(ss1, cell1, ',')) 
                {
                    stringstream ss2(cell1);  
                    string key, value;

                    getline(ss2, key, ':');
                    getline(ss2, value, ':');
                    
                    if (key=="nazwa")
                        name=value;
                    else if (key=="zapotrzebowanie_(w_litrach)")
                        water_demand=stoi(value); 
                }
            }
            
            if (lenght_vector_plants==numbers_beds)
                break;
        }

        file.close();
    }

    /*
        parametry fizyczne potrzebne do obliczenia przepływu wody
    */

    int hose_diameter=5;
    double water_pressure=6;  //ciśnienie wody na minute
    
    int k=hose_diameter-4; //k to współczynnik potrzebny do obliczeń, hose_diameter to średnica węża podana w milimetrach (min 5mm)
    double water_flow=k*water_pressure/60; //przepływ wody podany w sekundach

    int layout[numbers_beds]; //tablica odpowiadająca za ułożenie roślin - która, gdzie została zasadzona
    int random_plant;

    int field=0; //zmienna oznaczająca ile roślin zostało posadzonych
    while (field<numbers_beds)
    {
        random_plant=rand()%numbers_beds; //losowanie rośliny
        
        int temp=0;

        /*
            tablica sprawdzająca czy roślina została już posadzona
        */
        for (int j=field-1; j>=0; j--)
        {
            if (random_plant==layout[j])
            {
                temp++;
                break;
            }
        }

        if (temp==0) //jeśli roślina jeszcze nie została posadzona
        {
            layout[field]=random_plant; //posadzenie rośliny w konkretnym miejscu
            plants[random_plant].planting(field);
            plants[random_plant].counting_watering_time(water_flow);
            field++;
        }
    }

    /*
        wektor z kosztami, między wszystkimi możliwymi roślinami - koszty odpowiadają odległościom 
    */
    vector<vector<int>> costs(numbers_beds,vector<int>(numbers_beds));

    for (int first=0; first<numbers_beds; first++)
    {
        for (int second=0; second<numbers_beds; second++)
        {
            if (first==second)
                costs[first][second]=0;
            else if (first<second)
                costs[first][second]=second-first;
            else
                costs[first][second]=numbers_beds+second-first;
        }
    }

    greedy(numbers_beds, plants, layout, costs);

    cout << "KONIEC ZACHŁANNEGO" << endl;

    unsigned long long time1=Get_time();
    vector<int> ant_colony_algorithm=ant_colony(numbers_beds, plants, layout, costs);
    unsigned long long time2=Get_time();
    int aco_len=ant_colony_algorithm.size();

    ofstream ant_colony_optimisation("output/ant_colony_optimisation.json");
    ant_colony_optimisation << "[\n";
    
    for (int i=0; i<aco_len-4; i+=6)
    {
        ant_colony_optimisation << "\t{\n";
        ant_colony_optimisation << "\t\t\"pierwsza_roślina\": \"" << plants[ant_colony_algorithm[i]].get_name() << "\",  \"pozostały_czas_podlewania_pierwszej_rośliny\": \"" << ant_colony_algorithm[i+1] << "\",\n";
        ant_colony_optimisation << "\t\t\"druga_roślina\": \"" << plants[ant_colony_algorithm[i+2]].get_name() << "\",  \"pozostały_czas_podlewania_drugiej_rośliny\": \"" << ant_colony_algorithm[i+3] << "\",\n";
        ant_colony_optimisation << "\t\t\"obecny_koszt\": \"" << ant_colony_algorithm[i+4] << "\",  \"obecny_czas\": \"" << ant_colony_algorithm[i+5] << "\"\n";
        ant_colony_optimisation << "\t},\n";
    }

    ant_colony_optimisation << "\t{\n";
    ant_colony_optimisation << "\t\t\"ostatnia_roślina\": \"" << plants[ant_colony_algorithm[aco_len-4]].get_name() << "\",  \"ostatni_czas_podlewania\": \"" << ant_colony_algorithm[aco_len-3] << "\",\n";
    ant_colony_optimisation << "\t\t\"całkowity_koszt\": \"" << ant_colony_algorithm[aco_len-2] << "\",  \"całkowity_czas_podlewania\": \"" << ant_colony_algorithm[aco_len-1] << "\",\n";
    ant_colony_optimisation << "\t\t\"czas_trwania_algorytmu\": \"" << (time2-time1)/1000.0 << "\"\n"; //algorithm duration
    ant_colony_optimisation << "\t}\n";

    ant_colony_optimisation << "]\n";
    ant_colony_optimisation.close();

    cout << "KONIEC MRÓWEK" << endl;

    vector<int> result;
    int all_time=0, all_cost=0, current_plant=-1;
    vector<bool> used_plants(numbers_beds, false);
    time1=Get_time();
    brute_force_algorithm(numbers_beds, plants, layout, costs, used_plants, current_plant, all_cost, all_time, result);
    time2=Get_time();

    ofstream brute_force_algorithm("output/brute_force_algorithm.json");
    brute_force_algorithm << "[\n";

    int brute_len=brute_result.size();
    
    for (int i=0; i<brute_len-4; i+=6)
    {
        brute_force_algorithm << "\t{\n";
        brute_force_algorithm << "\t\t\"pierwsza_roślina\": \"" << plants[brute_result[i]].get_name() << "\",  \"pozostały_czas_podlewania_pierwszej_rośliny\": \"" << brute_result[i+1] << "\",\n";
        brute_force_algorithm << "\t\t\"druga_roślina\": \"" << plants[brute_result[i+2]].get_name() << "\",  \"pozostały_czas_podlewania_drugiej_rośliny\": \"" << brute_result[i+3] << "\",\n";
        brute_force_algorithm << "\t\t\"obecny_koszt\": \"" << brute_result[i+4] << "\",  \"obecny_czas\": \"" << brute_result[i+5] << "\"\n";
        brute_force_algorithm << "\t},\n";
    }

    brute_force_algorithm << "\t{\n";
    brute_force_algorithm << "\t\t\"ostatnia_roślina\": \"" << plants[brute_result[brute_len-4]].get_name() << "\",  \"ostatni_czas_podlewania\": \"" << brute_result[brute_len-3] << "\",\n";
    brute_force_algorithm << "\t\t\"całkowity_koszt\": \"" << brute_result[brute_len-2] << "\",  \"całkowity_czas_podlewania\": \"" << brute_result[brute_len-1] << "\",\n";
    brute_force_algorithm << "\t\t\"czas_trwania_algorytmu\": \"" << (time2-time1)/1000.0 << "\"\n"; //algorithm duration
    brute_force_algorithm << "\t}\n";

    brute_force_algorithm << "]\n";
    brute_force_algorithm.close();

    cout << "KONIEC SIŁOWEGO" << endl;

    return 0;
}