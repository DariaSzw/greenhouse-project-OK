#include "bruteforce.h"
#include "plant.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <climits>
#include <algorithm>

using namespace std;

//zdefiniowanie zmiennych globalnych
int brute_cost=INT_MAX;
vector<int> brute_result;
 
/*
    Algorytm siłowy, wywoływany rekurencyjnie w celu przeszukania wszystkich możliwych opcji i zapisania do zmiennych globalnych najbardziej optymalnego wyniku
*/
void brute_force_algorithm(const int &numbers_beds, vector<Plant> plants, const int* layout, const vector<vector<int>> &costs, vector<bool> used_plants, int current_plant, int all_cost, int all_time, vector<int> result)
{
    //sprawdzenie ile roślin pozostało nie odwiedzonych
    int used_number=0;
    for (int i=0; i<numbers_beds; i++)
    {
        if (used_plants[i]==false)
            used_number++;
    }
 
    /*
        jeśli wszystkie są odwiedzone i koszt uzyskanego wyniku jest mniejszy od zapisanego, to nadpisuje dane
    */
    if (used_number==0)
    {
        if (all_cost<brute_cost)
        {
            brute_cost=all_cost;
            brute_result=result;
        }
        return;
    }
    /*
        jeśli pozostała ostatnia roślina do podlania, to zostaje podlana i zapisana do ścieżki wynikowej, oraz występuje ostatnie wywołanie rekurencyjne
    */
    else if (used_number==1) 
    {
        int tmp=plants[layout[current_plant]].get_watering_time();
        all_time+=tmp;
 
        result.push_back(layout[current_plant]); result.push_back(plants[layout[current_plant]].get_watering_time());
        result.push_back(all_cost); result.push_back(all_time);
 
        plants[layout[current_plant]].change_watering_time(tmp);
        used_plants[current_plant]=true;
 
        brute_force_algorithm(numbers_beds, plants, layout, costs, used_plants, current_plant, all_cost, all_time, result);
        return;
    }
 
    /*
        właściwa część wyszukiwania roślin do podlania
    */
 
    vector<vector<int>> plants_sort;
 
    int min;
 
    vector<bool> used=used_plants;
 
    while (true)
    {
        /*
            na początku wartość maksymalna, w pętli wybiera najkrótszy dostępny czas z pozostałych niewykorzystanych jeszcze roślin (z każdym wywołaniem pętli najkrótszy czas podlewania się zwiększa, aż wszystkie rośliny zostaną posortowane)
        */
        min=INT_MAX; 
        for (int i=0; i<numbers_beds; i++)
        {
            if (plants[layout[i]].get_Watering_Time()<min && !used[i])
                min=plants[layout[i]].get_Watering_Time();
        } 
 
        vector<int> plants_min; //zapisanie roślin o wybranym czasie podlewania
        for (int i=0; i<numbers_beds; i++)
        {
            if (plants[layout[i]].get_Watering_Time()==min && !used[i])
            {
                plants_min.push_back(i);
                used[i]=true;
            }  
        }
 
        plants_sort.push_back(plants_min);
 
        /*
            sprawdzenie czy wszystkie rośliny zostały wykorzystane, jeśli tak pętla się kończy
        */
        bool temp=true; 
        for (int i=0; i<numbers_beds; i++)
        {
            if (used[i]==false)
            {
                temp=false;
                break;
            }    
        }
 
        if (temp==true)
            break;
    }
 
    int len_pl_sort=plants_sort.size()-1;
 
    /*
        jeśli obecnie nie ma żadnej wybranej rośliny
    */
    if (current_plant==-1)
    {
        /*
            przechodzenie po wszystkich dostępnych roślinach z posortowanych roślin
            pierwsza pętla wybiera roślinę z najdłuższym czasem podlewania
            druga pętla wybiera roślinę z najkrótszym czasem podlewania
        */
        int temp1=plants_sort[len_pl_sort].size();
        for (int i=0; i<temp1; i++)
        {
            int temp2=plants_sort[0].size();
            for (int j=0; j<temp2; j++)
            {
                //jeśli wybrana jest ta sama roślina to program pomija taką możliwość (nie można podlewać jednej tej samej rośliny jako dwóch różnych
                if (plants_sort[0][j]==plants_sort[len_pl_sort][i])
                    continue;
 
                //jeśli roślina wybrana jako ta o krótszym czasie podlewania ma krótszy lub równy czas podlewania to zapisujemy dany czas podlewania
                if (plants[layout[plants_sort[0][j]]].get_watering_time()<=plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time())
                    min=plants[layout[plants_sort[0][j]]].get_watering_time();
                //jeśli roślina wybrana jako ta o dłuższym czasie podlewania ma krótszy czas podlewania to zapisujemy dany czas podlewaniaelse
                    min=plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time();
 
                //wybrane rośliny zapisywane są to ścieżki wynikowej
                result.push_back(layout[plants_sort[0][j]]); result.push_back(plants[layout[plants_sort[0][j]]].get_watering_time());
                result.push_back(layout[plants_sort[len_pl_sort][i]]); result.push_back(plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time());
                result.push_back(all_cost); result.push_back(all_time);
 
                plants[layout[plants_sort[0][j]]].change_watering_time(min);
                plants[layout[plants_sort[len_pl_sort][i]]].change_watering_time(min);
 
                //powiększenie ogólnego czasu i kosztu o ten z pojedynczej operacji
                all_time+=min;
                all_cost+=costs[plants_sort[0][j]][plants_sort[len_pl_sort][i]];
 
                //jeśli roślina wybrana jako ta z krótszym czasem kończy podlewanie, to jako aktualnie wybrana roślina pozostaje ta z dłuższym czasem podlewania
                if (plants[layout[plants_sort[0][j]]].get_watering_time()==0 && plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time()!=0)
                {
                    used_plants[plants_sort[0][j]]=true;
                    current_plant=plants_sort[len_pl_sort][i];
                }
                //jeśli roślina wybrana jako ta z dłuższym czasem kończy podlewanie, to jako aktualnie wybrana roślina pozostaje ta z krótszym czasem podlewania
                else if (plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time()==0 && plants[layout[plants_sort[0][j]]].get_watering_time()!=0)
                {
                    used_plants[plants_sort[len_pl_sort][i]]=true;
                    current_plant=plants_sort[0][j];
                }
                //jeśli obie rośliny kończą podlewanie, to nie ma aktualnie wybranej rośliny
                else
                {
                    used_plants[plants_sort[0][j]]=true;
                    used_plants[plants_sort[len_pl_sort][i]]=true;
                    current_plant=-1;
                }
 
                //wywołanie rekurencyjne, względem aktualnie wybranej rośliny
                brute_force_algorithm(numbers_beds, plants, layout, costs, used_plants, current_plant, all_cost, all_time, result);
            }
        }
    }
    /*
        jeśli obecnie jest wybrana jedna roślina, wybrana jako ta z krótszym czasem podlewania
    */
    else if (plants[layout[plants_sort[0][0]]].get_watering_time()==plants[layout[current_plant]].get_watering_time())
    {
        //wybieranie drugiej rośliny względem najdłuższego czasu podlewania
        int temp1=plants_sort[len_pl_sort].size();
        for (int i=0; i<temp1; i++)
        {
            //jeśli znalazło tą samą roślinę to pomija
            if (current_plant==plants_sort[len_pl_sort][i])
                continue;
 
            //jeśli roślina aktualnie wybrana roślina ma krótszy lub równy czas podlewania w porównaniu do tej wybranej jako z najdłuższym czasem to zapisujemy dany czas podlewania
            if (plants[layout[current_plant]].get_watering_time()<=plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time())
                min=plants[layout[current_plant]].get_watering_time();
            //jeśli roślina aktualnie wybrana roślina ma dłuższy czas podlewania w porównaniu do tej wybranej jako z najdłuższym czasem to zapisujemy dany czas podlewania
            else
                min=plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time();
 
            //zapisanie wybranych roslin to wynikowego wektoru
            result.push_back(layout[current_plant]); result.push_back(plants[layout[current_plant]].get_watering_time());
            result.push_back(layout[plants_sort[len_pl_sort][i]]); result.push_back(plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time());
            result.push_back(all_cost); result.push_back(all_time);
 
            plants[layout[current_plant]].change_watering_time(min);
            plants[layout[plants_sort[len_pl_sort][i]]].change_watering_time(min);
 
            all_time+=min;
            all_cost+=costs[current_plant][plants_sort[len_pl_sort][i]];
 
            //jeśli obecna roślina kończy podlewanie, to jako aktualnie wybrana roślina zostaje wybrana ta z dłuższym czasem podlewania
            if (plants[layout[current_plant]].get_watering_time()==0 && plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time()!=0)
            {
                used_plants[current_plant]=true;
                current_plant=plants_sort[len_pl_sort][i];
            }
            //jeśli obecna roślina nie kończy podlewania, to pozostaje jako aktualnie wybrana
            else if (plants[layout[plants_sort[len_pl_sort][i]]].get_watering_time()==0 && plants[layout[current_plant]].get_watering_time()!=0)
            {
                used_plants[plants_sort[len_pl_sort][i]]=true;
                //current_plant=current_plant;
            }
            //jeśli obie rośliny kończą podlewanie, to nie ma aktualnie wybranej rośliny
            else
            {
                used_plants[current_plant]=true;
                used_plants[plants_sort[len_pl_sort][i]]=true;
                current_plant=-1;
            }
 
            //rekurencyjne wywołanie fukncji dla obecnej rośliny
            brute_force_algorithm(numbers_beds, plants, layout, costs, used_plants, current_plant, all_cost, all_time, result);   
        }
    }
    /*
        jeśli obecnie jest wybrana jedna roślina, wybrana jako ta z dłuższym czasem podlewania
    */
    else
    {
        //wybieranie drugiej rośliny względem najkrótszego czasu podlewania
        int temp1=plants_sort[0].size();
        for (int j=0; j<temp1; j++)
        {
            //jeśli znalazło tą samą roślinę to pomija
            if (plants_sort[0][j]==current_plant)
                continue;
 
            //jeśli roślina aktualnie wybrana roślina ma krótszy lub równy czas podlewania w porównaniu do tej wybranej jako z najkrótszym czasem to zapisujemy dany czas podlewania
            if (plants[layout[plants_sort[0][j]]].get_watering_time()<=plants[layout[current_plant]].get_watering_time())
                min=plants[layout[plants_sort[0][j]]].get_watering_time();
            //jeśli roślina aktualnie wybrana roślina ma dłuższy czas podlewania w porównaniu do tej wybranej jako z najkrótszym czasem to zapisujemy dany czas podlewania
            else
                min=plants[layout[current_plant]].get_watering_time();
 
            //zapisanie wybranych roslin to wynikowego wektoru
            result.push_back(layout[plants_sort[0][j]]); result.push_back(plants[layout[plants_sort[0][j]]].get_watering_time());
            result.push_back(layout[current_plant]); result.push_back(plants[layout[current_plant]].get_watering_time());
            result.push_back(all_cost); result.push_back(all_time);
 
            plants[layout[plants_sort[0][j]]].change_watering_time(min);
            plants[layout[current_plant]].change_watering_time(min);
 
            all_time+=min;
            all_cost+=costs[plants_sort[0][j]][current_plant];
 
            //jeśli obecna roślina nie kończy podlewania, to pozostaje jako aktualnie wybrana
            if (plants[layout[plants_sort[0][j]]].get_watering_time()==0 && plants[layout[current_plant]].get_watering_time()!=0)
            {
                used_plants[plants_sort[0][j]]=true;
                //current_plant=current_plant;
            }
            //jeśli obecna roślina kończy podlewanie, to jako aktualnie wybrana roślina zostaje wybrana ta z krótszym czasem podlewania
            else if (plants[layout[current_plant]].get_watering_time()==0 && plants[layout[plants_sort[0][j]]].get_watering_time()!=0)
            {
                used_plants[current_plant]=true;
                current_plant=plants_sort[0][j];
            }
            //jeśli obie rośliny kończą podlewanie, to nie ma aktualnie wybranej rośliny
            else
            {
                used_plants[plants_sort[0][j]]=true;
                used_plants[current_plant]=true;
                current_plant=-1;
            }
 
            //rekurencyjne wywołanie fukncji dla obecnej rośliny
            brute_force_algorithm(numbers_beds, plants, layout, costs, used_plants, current_plant, all_cost, all_time, result);
        }
    }
}