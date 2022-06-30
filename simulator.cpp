#include <iostream>
#include <cstdlib>
#include <set>
#include <queue>
#include <vector>
#include <fstream>
#include <ctime>

#define POPULATION 100
#define INFECTION 0
#define RECOVERY 1
#define WHITE 0
#define GRAY 1
#define BLACK 2

//adjacency matrix
bool adjacency_matrix[POPULATION][POPULATION];
//shortest distance from initial node list
int shortest_distance[POPULATION];

struct event_node_t
{
    int node_id;
    int time_stamp;
    int event_type;
};
typedef struct event_node_t event_node;

class comparator_class
{
public:
    // Comparator function
    bool operator()(event_node o1,event_node o2)
    {
        return (o1.time_stamp > o2.time_stamp);
    }
};

/**
 * @brief calculates shortest distance of every node from the root and puts
 * it in shortest_distance array 
 * 
 * @param root starting node.
 */
void calculate_shortest_dist(int root)
{
   int color_field[POPULATION];

   for(int & i : color_field)
   {
       i = WHITE;
   }
   color_field[root] = GRAY;


   std::queue<int> Q;

   Q.push(root);

   shortest_distance[root] = 0;

   while(!Q.empty())
   {
       int x = Q.front();
       Q.pop();

       for (int i = 0; i < POPULATION; i++)
           if (adjacency_matrix[x][i] == 1 && color_field[i] == WHITE) {
               color_field[i] = GRAY;
               shortest_distance[i] = shortest_distance[x] + 1;

               Q.push(i);
           }
       color_field[x] = BLACK;
   }

}



int main(int argc, char const *argv[])
{
    std::vector<int> recovery_count;
    std::vector<int> infection_count;
    recovery_count.reserve(5);
    infection_count.reserve(5);


    int infected_time[POPULATION];

    for (int & i : infected_time)
    {
        i = -1;
    }

    srand(time(0));
    //set population relations
    for(int i=0;i<POPULATION;i++)
    {
        for(int j=0;j<=i;j++)
        {
            adjacency_matrix[i][j] = rand() % 2 == 0 ? 1 : 0;
            if(i==j)
                adjacency_matrix[i][j] = false;
            else
            adjacency_matrix[j][i] = adjacency_matrix[i][j];
        }
    }

    std::set<int> S,I,R;

    //initially every one is susceptible
    for(int i=0; i<POPULATION;i++)
    {
        S.insert(i);
    }
    //first infected person
    int u = rand()%POPULATION;

    //min heap over time stamp
    std::priority_queue<event_node,std::vector<event_node>,comparator_class> Q;
    
    //insert initial infection event into Q
    event_node init_infection = {u,0,INFECTION};
    Q.push(init_infection);

    int max_time=-1;
    while(!Q.empty())
    {
        event_node e = Q.top();
        Q.pop();

        //if event is recovery
        if(e.event_type == RECOVERY)
        {
            //modify recovery count against time_stamp
            if (e.time_stamp > max_time){

                for(int i=0;i<e.time_stamp-max_time;i++){
                    recovery_count.push_back(0);
                    infection_count.push_back(0);
                }
                max_time = e.time_stamp;
            }


            recovery_count[e.time_stamp]+=1;
            R.insert(e.node_id);
            I.erase(e.node_id);
        }
        //if event is infection
        else if(e.event_type == INFECTION)
        {
            if(S.count(e.node_id)){

                //modify infection count against time_stamp
                if (e.time_stamp > max_time)
                {

                    for (int i = 0; i < e.time_stamp - max_time; i++)
                    {
                        recovery_count.push_back(0);
                        infection_count.push_back(0);
                    }
                    max_time = e.time_stamp;
                }

                infection_count[e.time_stamp]+=1;

                I.insert(e.node_id);
                S.erase(e.node_id);
                // generate and insert recovery event into Q
                event_node e_recovery = {e.node_id, e.time_stamp + 1 + rand() % 5, RECOVERY};
                Q.push(e_recovery);
                infected_time[e.node_id] = e.time_stamp;
            }
            int i = 100;
            while(i)
            {
                i=i-1;
                int count = S.count(i);
                if(!adjacency_matrix[i][e.node_id] || !count)
                    continue;
                //generate an infection time j
                int j = 1;

                while (j<=5)
                {
                    if(rand()%2 == 0)
                        break;
                    j++;
                }
                //if it is not infected continue
                if(j>5)
                    continue;
                //insert infection event into Q
                event_node u_infection = {i,e.time_stamp+j,INFECTION};
                Q.push(u_infection);

            }
        }
    }

    //fill shortest_distance with -1 to indicate they are not connected
    for(int & i : shortest_distance)
    {
        i = -1;
    }
    calculate_shortest_dist(u);


    // file pointer
    std::ofstream file_output;

    //file 
    file_output.open("infection_distance.csv");

    file_output << "person,infected_time,shortest_distance\n";
    for(int i = 0;i<POPULATION;i++)
    {
        if(infected_time[i] == -1 || shortest_distance[i] == -1)
            continue;
        file_output << i << ',' << infected_time[i] << ',' << shortest_distance[i] << '\n';
    }
    file_output.close();

    
    //get no.of susceptible
    int no_of_susceptible[max_time];
    for (int i = 0; i <= max_time; i++)
    {
        if(i==0)
            no_of_susceptible[i] = POPULATION - infection_count[i];
        else
            no_of_susceptible[i] = no_of_susceptible[i-1]-infection_count[i];
    }
    //get no.of recovery
    int no_of_recovery[max_time];
    for (int i = 0; i <= max_time; i++)
    {
        if (i == 0)
            no_of_recovery[i] = recovery_count[i];
        else
            no_of_recovery[i] = no_of_recovery[i-1]+recovery_count[i];
    }
    //get no.of infection
    int no_of_infection[max_time];
    for (int i = 0; i <= max_time; i++)
    {
        if (i == 0)
            no_of_infection[i] = infection_count[i]-recovery_count[i];
        else
            no_of_infection[i] = no_of_infection[i - 1] + infection_count[i]-recovery_count[i];
    }
        std::ofstream file_output1;

    //time stats
    file_output1.open("time_stats.csv");

    file_output1<<"time,no_of_recovery,no_of_infection,no_of_susceptible\n";
    for(int i = 0;i<=max_time;i++)
    {
        file_output1<<i<<","<<no_of_recovery[i]<<","<<no_of_infection[i]<<
        ","<<no_of_susceptible[i]<<"\n";
    }
    return 0;
}
