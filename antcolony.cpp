#include "antcolony.h"
#include "plant.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <climits>
#include <algorithm>

using namespace std;

/*
    Algorytm mrówkowy, zwracający wektor z najbardziej optymalnym rozwiązaniem
*/
vector<int> ant_colony(const int &numbers_beds, vector<Plant> plants, const int* layout, const vector<vector<int>> &costs)
{
    int first_watering_time, second_watering_time;
 
    vector<vector<double>> heuristic(numbers_beds, vector<double>(numbers_beds));
 
    /*
        tworzenie dwuwymiarowego wektora, którego zawartość jest odwrotnością kosztów (heurystyka)
    */
    for (int i = 0; i<numbers_beds; ++i) 
    {
        for (int j = 0; j<numbers_beds; ++j) 
        {
            if (i != j) 
                heuristic[i][j] = 1.0/costs[i][j];
            else 
                heuristic[i][j] = 0.0;
        }
    }
 
    //początkowo ustawiona wartość feromonów dla każdego możliwego połączenia na 1
    vector<vector<double>> pheromones(numbers_beds, vector<double>(numbers_beds, 1.0));
 
    vector<vector<int>> all_paths;
    vector<int> path_cost;
 
    int tmp;
 
    for (int a=0; a<numbers_beds*numbers_beds; a++) //każde wykonanie pętli to jedna 'mrówka'
    {
        vector<bool> visited_plants(numbers_beds, false); //ustawienie wszystkich roślin jako nieodwiedzone
        int visited_fields=0, all_cost=0, all_time=0;
 
        /*
             Pierwszą roślinę, wybieramy na podstawie najkrótszego czasu podlewania, 
             Drugą na podstawie najdłuższego czasu podlewania
        */
 
        int first_index=-1, second_index=-1;
 
        vector<int> choice_first, choice_second; //wektory, które zapisują rośliny o jednakowym najkrótszym czasie podlewania i najdłuższym
        vector<int> path; //aktualnie tworzona ścieżka wybieranych roślin
 
        for (int i=0; i<numbers_beds; i++)
            plants[i].reset_watering_time();
 
        while (visited_fields<numbers_beds) //pętla wykonuje się dopóki wszystkie rośliny nie zostaną odwiedzone
        {
            /*
            wartość -1 oznacza że program musi wybrać roślinę o najdłuższym czasie podlewania, która nie została jeszcze podlana
            */
            if (second_index==-1)
            {
                //pętla sprawdzająca od końca, czy któraś z roślin wcześniej dodanych została już wybrana do podlewania
                for (int i=choice_second.size()-1; i>=0; i--)
                {
                    if (visited_plants[choice_second[i]])
                        choice_second.erase(choice_second.begin() + i);
                }
 
                if (choice_second.size()==0) //jeśli nie ma wybranych roślin o jednakowym najdłuższym czasie podlewania
                {
                    second_watering_time=0; //na początku wartość zero, w pętli wybiera największy dostępny czas
 
                    for (int i=0; i<numbers_beds; i++)
                    {
                        if (!visited_plants[i] && plants[layout[i]].get_watering_time()>second_watering_time)
                            second_watering_time=plants[layout[i]].get_watering_time();
                    }
 
                    //dla znalezionego najdłuższego czasu, wszystkie które mają taki sam dodaje do wektora, z którego mrówka będzie mogła wybierać rozwiązanie
                    for (int i=0; i<numbers_beds; i++)
                    {
                        if (!visited_plants[i] && plants[layout[i]].get_watering_time()==second_watering_time)
                        {
                            choice_second.push_back(i);
                        }
 
                    }
                }
 
                if (first_index==-1) //jeśli pierwsza nie jest wybrana to roślina jest losowana z dostępnych
                {
                    if (choice_second.size() > 1)
                    {
                        int temp=rand()%choice_second.size();
                        second_index=choice_second[temp];
                    }
                    else 
                        second_index=choice_second[0];
                }
                else //pierwsza roślina jest wybrana
                {
                    /*
                        tworzenie wektora prawdopodobieństw wybrania poszczególnych roślin na podstawie feromonów i heurystyki
                    */
                    vector<double> probabilities(choice_second.size(), 0.0);
                    double sum_probabilities = 0.0;
 
                    tmp=choice_second.size();
                    for (int i = 0; i<tmp; i++) 
                    {
                        if (!visited_plants[choice_second[i]]) 
                        {
                            probabilities[i] = pheromones[first_index][choice_second[i]]*heuristic[first_index][choice_second[i]]*heuristic[first_index][choice_second[i]];
                            sum_probabilities += probabilities[i];
                        }
                    }
 
                    double rand_prob = ((double)rand() / RAND_MAX) * sum_probabilities; //losowana wartość prawdopodobieństwa
                    double cumulative_prob = 0.0;
 
                    tmp=choice_second.size();
                    for (int i = 0; i<tmp; i++) 
                    {
                        if (!visited_plants[choice_second[i]]) 
                        {
                            cumulative_prob += probabilities[i];
                            if (cumulative_prob >= rand_prob) //gdy suma poszczególnych prawdopodobieństw przekroczy wylosowaną to wybiera daną roślinę (dzięki temu mrówki mają szansę odwiedzić różne ścieżki)
                            {
                                second_index=choice_second[i];
                                break;
                            }
                        }
                    }
                }
                //oznaczenie rośliny jako odwiedzonej
                choice_second.erase(find(choice_second.begin(), choice_second.end(), second_index), choice_second.end());
                visited_plants[second_index]=true;
            }
 
            /*
                wartość -1 oznacza że program musi wybrać roślinę o najkrótszym czasie podlewania, która nie została jeszcze podlana
            */
            if (first_index==-1)
            {
                //pętla sprawdzająca od końca, czy któraś z roślin wcześniej dodanych została już wybrana do podlewania
                for (int i=choice_first.size()-1; i>=0; i--)
                {
                    if (visited_plants[choice_first[i]])
                        choice_first.erase(choice_first.begin() + i);
                }
 
                if (choice_first.size()==0) //jeśli nie ma wybranych roślin o jednakowym najdłuższym czasie podlewania
                {
                    first_watering_time=INT_MAX; //na początku wartość maksymalna, w pętli wybiera najkrótszy dostępny czas
 
                    for (int i=0; i<numbers_beds; i++)
                    {
                        if (!visited_plants[i] && plants[layout[i]].get_watering_time()<first_watering_time)
                            first_watering_time=plants[layout[i]].get_watering_time();
                    }
 
                    //dla znalezionego najkrótszego czasu, wszystkie które mają taki sam dodaje do wektora, z którego mrówka będzie mogła wybierać rozwiązanie
                    for (int i=0; i<numbers_beds; i++)
                    {
                        if (!visited_plants[i] && plants[layout[i]].get_watering_time()==first_watering_time)
                            choice_first.push_back(i);
                    }
                }
 
                /*
                    tworzenie wektora prawdopodobieństw wybrania poszczególnych roślin na podstawie feromonów i heurystyki
                */
                vector<double> probabilities(choice_first.size(), 0.0);
                double sum_probabilities = 0.0;
 
                tmp=choice_first.size();
                for (int i = 0; i<tmp; i++) 
                {
                    if (!visited_plants[choice_first[i]]) 
                    {
                        probabilities[i] = pheromones[choice_first[i]][second_index]*heuristic[choice_first[i]][second_index]*heuristic[choice_first[i]][second_index];
                        sum_probabilities += probabilities[i];
                    }
                }
 
                double rand_prob = ((double)rand() / RAND_MAX) * sum_probabilities; //losowana wartość prawdopodobieństwa
                double cumulative_prob = 0.0;
 
                tmp=choice_first.size();
                for (int i = 0; i<tmp; i++) 
                {
                    if (!visited_plants[choice_first[i]]) 
                    {
                        cumulative_prob += probabilities[i];
                        if (cumulative_prob >= rand_prob) //gdy suma poszczególnych prawdopodobieństw przekroczy wylosowaną to wybiera daną roślinę (dzięki temu mrówki mają szansę odwiedzić różne ścieżki)
                        {
                            first_index=choice_first[i];
                            choice_first.erase(find(choice_first.begin(), choice_first.end(), first_index), choice_first.end());
                            break;
                        }
                    }
                }
                //oznaczenie rośliny jako odwiedzonej
                visited_plants[first_index]=true;
            }
 
            //zapisanie poszczególnych czasów podlewania
            first_watering_time=plants[layout[first_index]].get_watering_time();
            second_watering_time=plants[layout[second_index]].get_watering_time();
 
            //dodanie wybranych roślin do wektora, aby na końcu móc uzyskać najlepszą opcję
            path.push_back(layout[first_index]); path.push_back(first_watering_time);
            path.push_back(layout[second_index]); path.push_back(second_watering_time);
            path.push_back(all_cost); path.push_back(all_time);
 
            //powiększenie ogólnego kosztu o koszt obecnego podlewania
            all_cost+=costs[first_index][second_index];
 
            if (first_watering_time==second_watering_time) //przypadek, gdy czas obu roślin pozostał taki sam
            {
                all_time+=first_watering_time;
 
                plants[layout[first_index]].change_watering_time(first_watering_time);
                plants[layout[second_index]].change_watering_time(first_watering_time);
                visited_fields+=2;
 
                //obu roślinom, czas się zeruje, więc obie trzeba wybrać na nowo
                first_index=-1;
                second_index=-1;
            }
            else if (first_watering_time<second_watering_time) //przypadek, gdy pierwsza roślina kończy podlewanie
            {
                all_time+=first_watering_time;
 
                plants[layout[first_index]].change_watering_time(first_watering_time);
                plants[layout[second_index]].change_watering_time(first_watering_time);
                visited_fields++;
 
                //tylko pierwszej roślinie czas się zeruje, więc druga wciąż pozostaje wybrana
                first_index=-1;
            }
            else //the plant with the longest initial time finishes watering //przypadek, gdy druga roślina kończy podlewanie
            {
                all_time+=second_watering_time;
 
                plants[layout[first_index]].change_watering_time(second_watering_time);
                plants[layout[second_index]].change_watering_time(second_watering_time);
                visited_fields++;
 
                //tylko drugiej roślinie czas się zeruje, więc pierwsza wciąż pozostaje wybrana
                second_index=-1;
            }
 
            //dodatkowe sprawdzenie czy przypadkiem wszystkie rośliny nie zostały odwiedzone
            int temp=0;
            for (int i=0; i<numbers_beds; i++)
            {
                if (visited_plants[i])
                    temp++;
            }
            if (temp == numbers_beds)
                break;
 
        }
 
        //po wykonaniu pętli, należy sprawdzić, czy jakaś roślina nie wymaga podlania
        if (first_index!=-1 && second_index==-1) //przypadek, gdy do podlania pozostała pierwsza roślina
        {
            first_watering_time=plants[layout[first_index]].get_watering_time();
 
            path.push_back(layout[first_index]); path.push_back(first_watering_time);    
 
            plants[layout[first_index]].change_watering_time(first_watering_time);
 
            all_time+=first_watering_time;
        }
        else if (first_index==-1 && second_index!=-1) //przypadek, gdy do podlania pozostała druga roślina
        {
            second_watering_time=plants[layout[second_index]].get_watering_time();
 
            path.push_back(layout[second_index]); path.push_back(second_watering_time);
 
            plants[layout[second_index]].change_watering_time(second_watering_time);  
 
            all_time+=second_watering_time;
        }
 
        path.push_back(all_cost); path.push_back(all_time);
 
        all_paths.push_back(path);
        path_cost.push_back(all_cost);
 
        /*
             sprawdzenie, jeśli ponad 5 mrówek znalazło rozwiązanie oraz 5 ostatnich uzyskanych kosztów jest takich samych to algorytm się kończy, zwraca ścieżkę z najmniejszym kosztem
        */
        if (a>5) 
        {
            int temp=path_cost.size();
            if (path_cost[temp-1]==path_cost[temp-2] && path_cost[temp-2]==path_cost[temp-3] && path_cost[temp-3]==path_cost[temp-4] && path_cost[temp-4]==path_cost[temp-5])
            {
                int min=INT_MAX, min_index;
                for (int i=0; i<temp; i++)
                {
                    if (path_cost[i]<min)
                    {
                        min=path_cost[i];
                        min_index=i;
                    }
                }
                return all_paths[min_index];
            }
 
        }
    }
 
    //jeśli algorytm zakończy się wykonywać bez zapętlenia wyników kosztów, algorytm zwraca ścieżkę z najmniejszym uzyskanym kosztem
    int min=INT_MAX, min_index;
    tmp=path_cost.size();
    for (int i=0; i<tmp; i++)
    {
        if (path_cost[i]<min)
        {
            min=path_cost[i];
            min_index=i;
        }
    }
    return all_paths[min_index];
}