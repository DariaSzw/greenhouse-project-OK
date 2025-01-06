#ifndef PLANT_H
#define PLANT_H

#include <string>
#include <cmath>

using namespace std;

class Plant
{
    private:
        string name;
        int water_demand; //w litrach na metr kwadratowy
        int number_field; 
        bool visit_field; 
        int watering_time, Watering_Time;
    
    public:
        Plant (string n, int wd)
         : name(n), water_demand(wd) {}

        int get_watering_time()
        {
            return watering_time;
        }

        int get_Watering_Time()
        {
            return Watering_Time;
        }

        void change_watering_time(int num)
        {
            watering_time-=num;
        }

        void reset_watering_time()
        {
            watering_time=Watering_Time;
        }

        int get_visit_field()
        {
            return visit_field;
        }

        void change_visit_field()
        {
            visit_field=true;
        }

        void planting(int num_field)
        {
            number_field=num_field;
            visit_field=false;
        }

        double single_watering_value()
        {
            double temp=(water_demand*0.25);
            return temp; //rozmiar grządki 0.5mx0.5m
        }

        void counting_watering_time(double water_flow) //wynik w sekundach
        {
            double temp=single_watering_value()/water_flow;
            watering_time=static_cast<int>(ceil(temp));
            Watering_Time=watering_time;
        }

        string get_name()
        {
            return name;
        }
};

#endif