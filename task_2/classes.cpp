#include <cmath>
#include <iostream>
#include <random>
#include <ctime>
#include <vector>
#include <utility>
#include <limits>
#include <sys/types.h>
#include <mqueue.h>
#include <string>
#include <sstream>

#include <thread>

class Schedule_solution;
char *sending_parser(Schedule_solution *sol, int n, int signal, char *buffer);
Schedule_solution *receiving_parser(char *buffer, int n, int m, std::vector<int> &jobs);

//----------------------------------constants---------------------------------
unsigned int prio = 1;
struct mq_attr attr;
char from_main[] = "/from_main";
char to_main[] = "/to_main";
//----------------------------------------------------------------------------



//----------------------------------schedule----------------------------------
class Solution {
public:
    virtual ~Solution() = default;
    virtual int calculate() = 0;  
    virtual void print() const = 0;   
	virtual Solution* clone() const = 0;
};

class Schedule_solution : public Solution {
public:
    std::vector<std::pair<int, int>> schedule;
    int proc_num = 0;
    std::vector<int> job_times;

    Schedule_solution(int n, int m, std::vector<int>& j) : job_times(j), proc_num(m) {
        std::vector<int> completion_times(proc_num, 0);
        for (int i = 0; i < n; ++i) {
            schedule.push_back(std::pair(i % m, completion_times[i % m]));
            completion_times[i % m] += job_times[i];
        }
    }

    int calculate() override {
        std::vector<int> completion_times(proc_num, 0);
        int Tsum = 0;
        for (int i = 0; i < job_times.size(); ++i) {
            int proc_ind = schedule[i].first;
            completion_times[proc_ind] += job_times[i];
            Tsum += completion_times[proc_ind];
        }
		return Tsum;
    }

    void print() const override {
        for (int i = 0; i < schedule.size(); ++i) {
            std::cout << "Job " << i << ": processor " << schedule[i].first << " finish time " << schedule[i].second + job_times[i] << std::endl;
        }
    }

    Schedule_solution* clone() const override {
        return new Schedule_solution(*this);
    }
};
//----------------------------------------------------------------------------
 


//----------------------------------mutations---------------------------------
class Mutation {
public:
    virtual ~Mutation() = default;
    virtual void mutate(Schedule_solution& s) = 0;
};

class Schedule_mutation : public Mutation {
    std::mt19937 gen;
public:
    Schedule_mutation() : gen(std::random_device{}()) {};

    void mutate(Schedule_solution& s) override {
        int n = s.schedule.size();
        int m = s.proc_num;
        std::uniform_int_distribution<> dist_job(0, n - 1);
        int job_ind = dist_job(gen);
        std::uniform_int_distribution<> dist_proc(0, m - 1);
        int new_proc = dist_proc(gen);
        std::vector<int> proc_times(m);

        //std::cout << job_ind << " " << new_proc << std::endl;

        int starting_time_job = 0;
        for (int i = 0; i < n; ++i) {
            if (s.schedule[i].first == s.schedule[job_ind].first && i > job_ind) {
                s.schedule[i].second -= s.job_times[job_ind];
            }
            if (s.schedule[i].first == new_proc) {
                if (i > job_ind) {
                    s.schedule[i].second += s.job_times[job_ind];
                } else if (i < job_ind) {
                    starting_time_job += s.job_times[i];
                } else {
                    s.schedule[i].second = starting_time_job;
                }
            }
        }
        s.schedule[job_ind].first = new_proc;
    }
};
//----------------------------------------------------------------------------



//------------------------temperature-coolers-classes-------------------------
class Cooler {
public:
    virtual ~Cooler() = default;
    virtual double get_temp() const = 0;
    virtual void cool(int iter) = 0;
};

class Bolzman_cooler : public Cooler {
    double temp_zero;
    double temp;
public:
    Bolzman_cooler(double t) : temp_zero(t), temp(t) {}

    double get_temp() const override {
        return temp;
    }

    void cool(int iter) override {
        temp = temp_zero / std::log(1 + iter);
    }
};

class Cauchy_cooler : public Cooler {
    double temp_zero;
    double temp;
public:
    Cauchy_cooler(double t) : temp_zero(t), temp(t) {}

    double get_temp() const override {
        return temp;
    }

    void cool(int iter) override {
        temp = temp_zero / (1 + iter);
    }
};

class Third_cooler : public Cooler {
    double temp_zero;
    double temp;
public:
    Third_cooler(double t) : temp_zero(t), temp(t) {}

    double get_temp() const override {
        return temp;
    }

    void cool(int iter) override {
        temp = temp_zero * (std::log(1 + iter) / (1 + iter));
    }
};
//----------------------------------------------------------------------------



//-----------------------class-for-annealing-simulation-----------------------
class Annealing {
public:
    Annealing() {};

    void run(Schedule_solution* init_solution, int n, int m, std::vector<int> &jobs) {
        char buffer[1];
        mqd_t to, from;
        while ((to = mq_open(to_main, O_WRONLY)) == -1) {};
        while ((from = mq_open(from_main, O_RDONLY)) == -1) {};

		Schedule_solution* current_solution = init_solution->clone();
        Schedule_mutation mutation = Schedule_mutation();
    	while (true) {
    	    Schedule_solution* new_solution = current_solution->clone();
    	    mutation.mutate(*new_solution);
    	    
            //new_solution->print();
            char buffer[8192];
            mq_send(to, sending_parser(new_solution, n, 1, buffer), 8192, prio);
            //std::cout << "client send 1" << std::endl;
            mq_receive(from, buffer, 8192, &prio);
            if (int(buffer[0]) == 1) {
                //std::cout << "client rcvd 1" << std::endl;
                current_solution = receiving_parser(buffer, n, m, jobs)->clone();
            } else {
                //std::cout << "client rcvd 2" << std::endl;
                std::destroy_at(buffer);
                break;
            }
            std::destroy_at(buffer);
    	}

    	delete current_solution;
        exit(0);
    }
};
//----------------------------------------------------------------------------



//------------------------------parsers---------------------------------------
char *sending_parser(Schedule_solution *sol, int n, int signal, char *buffer) {
    if (signal == 1) {
        buffer[0] = char(1);
        for (int i = 0; i < n; ++i) {
            buffer[i + 1] = char(sol->schedule[i].first);
        }
        return buffer;
    }
    buffer[0] = 2;
    return buffer;
}

Schedule_solution *receiving_parser(char *buffer, int n, int m, std::vector<int> &jobs) {
    Schedule_solution* sol = new Schedule_solution(n, m, jobs);
    std::vector<int> start_times(m);
    for (int i = 0; i < m; ++i) {
        start_times[i] = 0;
    }
    for (int i = 0; i < n; ++i) {
        int selected = int(buffer[i + 1]);
        sol->schedule[i] = {selected, start_times[selected]};
        start_times[selected] += jobs[i];
    }
    return sol;
}
//----------------------------------------------------------------------------