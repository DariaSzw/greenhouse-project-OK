#include "greedy.h"
#include "plant.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>
#include <chrono>
#include <fstream>
#include <sstream>
#include <climits>
#include <algorithm>

using namespace std;

unsigned long long Get_time()
{
    // using namespace chrono;
    return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
}

/*
    Funkcja sprawdzająca czy wszystkie wierzchołki zostały odwiedzone, gdy znajdzie pierwszą nieodwiedzoną od razu zwraca wynik negatywny
*/
bool check_all_visit(vector<Plant> plants)
{
    int tmp=plants.size();
    for (int i=0; i<tmp; i++)
    {
        if (!plants[i].get_visit_field())
            return false;
    }
    return true;
}
 
/*
    Algorytm zachłanny
    Wyniki są bezpośrednio zapisywane do pliku .json
*/
void greedy(const int &numbers_beds, vector<Plant> plants, const int* layout, const vector<vector<int>> &costs)
{
    unsigned long long time1, time2;
 
    ofstream greedy_algorithm("output/greedy_algorithm.json");
    greedy_algorithm << "[\n";
 
    /*
        Pierwszą roślinę, wybieramy na podstawie najkrótszego czasu podlewania, 
        Drugą na podstawie najdłuższego czasu podlewania
    */
 
    int first_index=-1, second_index=-1;
    int all_costs=0, all_time=0;
    int cost; //koszt pojedynczego podlewania
    int first_watering_time, second_watering_time;
 
    time1=Get_time();
 
    while (check_all_visit(plants)==false) //pętla kończy się gdy wszystkie rośliny zostaną podlane
    {
        /*
            wartość -1 oznacza że program musi wybrać roślinę o najdłuższym czasie podlewania, która nie została jeszcze podlana
        */
        if (second_index==-1)
        {
            second_watering_time=0; //na początku wartość zero, w pętli wybiera największy dostępny czas
            for (int i=0; i<numbers_beds; i++)
            {
                if (!plants[layout[i]].get_visit_field() && plants[layout[i]].get_watering_time()>second_watering_time)
                    second_watering_time=plants[layout[i]].get_watering_time(); 
            }
 
            cost=INT_MAX; //na początku koszt jest największy, ze wszystkich roślin o jednakowym, największym czasie podlewania, wybiera tą co ma najmniejszy koszt
            for (int i=0; i<numbers_beds; i++)
            {
                if (!plants[layout[i]].get_visit_field() && plants[layout[i]].get_watering_time()==second_watering_time)
                {
                    if (first_index==-1) //jeśli pierwsza roślina nie jest wybrana to przypisuje obecną bez patrzenia na koszty
                    {
                        second_index=i;
                    }
                    else if (costs[first_index][i]<cost && i!=first_index) //jeśli pierwsza roślina jest wybrana to szuka drugiej względem najmniejszego kosztu 
                    {
                        second_index=i;
                        cost=costs[first_index][i];  
                    }
                }
            }
            plants[layout[second_index]].change_visit_field(); //zmiana oznaczenia rośliny na odwiedzoną, czyli wybraną do podlania
        }
        /*
            wartość -1 oznacza że program musi wybrać roślinę o najkrótszym czasie podlewania, która nie została jeszcze podlana
        */
        if (first_index==-1) 
        {
            first_watering_time=INT_MAX; //na początku wartość maksymalna, w pętli wybiera najkrótszy dostępny czas
            for (int i=0; i<numbers_beds; i++)
            {
                if (!plants[layout[i]].get_visit_field() && plants[layout[i]].get_watering_time()<first_watering_time)
                    first_watering_time=plants[layout[i]].get_watering_time();           
            }
 
            cost=INT_MAX; //na początku koszt jest największy, ze wszystkich roślin o jednakowym, największym czasie podlewania, wybiera tą co ma najmniejszy koszt
            for (int i=0; i<numbers_beds; i++)
            {
                if ((!plants[layout[i]].get_visit_field()) && (plants[layout[i]].get_watering_time()==first_watering_time))
                {
                    /*
                        przy wyborze pierwszej rośliny, druga zawsze jest już wybrana (wcześniejsze sprawdzenie), więc należy tylko szukać rośliny, która względem drugiej ma najmniejszy koszt
                    */
                    if ((costs[i][second_index]<cost) && (i!=second_index))
                    {
                        first_index=i;
                        cost=costs[i][second_index];
                    }
                }    
            }
            plants[layout[first_index]].change_visit_field(); //zmiana oznaczenia rośliny na odwiedzoną, czyli wybraną do podlania
        }
 
        /*
            zapis wybranych roślin bezpośrednio do pliku
        */
 
        greedy_algorithm << "\t{\n";
        greedy_algorithm << "\t\t\"pierwsza_roślina\": \"" << plants[layout[first_index]].get_name() << "\",  \"pozostały_czas_podlewania_pierwszej_rośliny\": \"" << plants[layout[first_index]].get_watering_time() << "\",\n";
        greedy_algorithm << "\t\t\"druga_roślina\": \"" << plants[layout[second_index]].get_name() << "\",  \"pozostały_czas_podlewania_drugiej_rośliny\": \"" << plants[layout[second_index]].get_watering_time() << "\",\n";
        greedy_algorithm << "\t\t\"obecny_koszt\": \"" << all_costs << "\",  \"obecny_czas\": \"" << all_time << "\"\n";
        greedy_algorithm << "\t},\n";
 
        all_costs+=cost; //powiększenie całego kosztu o koszt obecnie podlanych roślin
 
        if (first_watering_time==second_watering_time) //przypadek, gdy czas obu roślin pozostał taki sam
        {
            all_time+=first_watering_time;
 
            plants[layout[first_index]].change_watering_time(first_watering_time);
            plants[layout[second_index]].change_watering_time(first_watering_time);
 
            //obu roślinom, czas się zeruje, więc obie trzeba wybrać na nowo
            first_index=-1;
            second_index=-1;
        }
        else if (first_watering_time<second_watering_time) //przypadek, gdy pierwsza roślina kończy podlewanie
        {
            all_time+=first_watering_time;
 
            plants[layout[first_index]].change_watering_time(first_watering_time);
            plants[layout[second_index]].change_watering_time(first_watering_time);
 
            //tylko pierwszej roślinie czas się zeruje, więc druga wciąż pozostaje wybrana
            first_index=-1;
            second_watering_time=plants[layout[second_index]].get_watering_time();
        }
        else //przypadek, gdy druga roślina kończy podlewanie
        {
            all_time+=second_watering_time;
 
            plants[layout[first_index]].change_watering_time(second_watering_time);
            plants[layout[second_index]].change_watering_time(second_watering_time);
 
            //tylko drugiej roślinie czas się zeruje, więc pierwsza wciąż pozostaje wybrana
            second_index=-1;
            first_watering_time=plants[layout[first_index]].get_watering_time();
        }
    }
 
    greedy_algorithm << "\t{\n";
 
    //po wykonaniu pętli, należy sprawdzić, czy jakaś roślina nie wymaga podlania
    if (first_index!=-1 && second_index==-1) //przypadek, gdy do podlania pozostała pierwsza roślina
    {
        greedy_algorithm << "\t\t\"ostatnia_roślina\": \"" << plants[layout[first_index]].get_name() << "\",  \"ostatni_czas_podlewania\": \"" << plants[layout[first_index]].get_watering_time() << "\",\n";
 
        all_time+=first_watering_time;
 
        plants[layout[first_index]].change_watering_time(first_watering_time);
    }
    else if (first_index==-1 && second_index!=-1) //przypadek, gdy do podlania pozostała druga roślina
    {
        greedy_algorithm << "\t\t\"ostatnia_roślina\": \"" << plants[layout[second_index]].get_name() << "\",  \"ostatni_czas_podlewania\": \"" << plants[layout[second_index]].get_watering_time() << "\",\n";
 
        all_time+=second_watering_time;
 
        plants[layout[second_index]].change_watering_time(second_watering_time);  
    }
 
    greedy_algorithm << "\t\t\"całkowity_koszt\": \"" << all_costs << "\",  \"całkowity_czas_podlewania\": \"" << all_time << "\",\n";
 
    time2=Get_time();
 
    greedy_algorithm << "\t\t\"czas_trwania_algorytmu\": \"" << (time2-time1)/1000.0 << "\"\n"; //algorithm duration
    greedy_algorithm << "\t}\n";
 
    greedy_algorithm << "]\n";
    greedy_algorithm.close();
}