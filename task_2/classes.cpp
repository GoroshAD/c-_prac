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
char *sending_parser(int job, int proc, int signal, char *buffer);
std::pair<int, int> receiving_parser(char *buffer);

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
        std::vector<int> numbers(proc_num, 0);
        for (int i = 0; i < n; ++i) {
            schedule.push_back(std::pair(i % m, numbers[i % m]));
            numbers[i % m] += 1;
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
            std::cout << "Job " << i << ": processor " << schedule[i].first << " number on it " << schedule[i].second << std::endl;
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
    virtual void mutate(Schedule_solution* s, int job_ind, int new_proc) = 0;
    virtual std::pair<int, int> choose(int n, int m) = 0;
};

class Schedule_mutation : public Mutation {
    std::mt19937 gen;
public:
    Schedule_mutation() : gen(std::random_device{}()) {};
    
    std::pair<int, int> choose(int n, int m) override {
        std::uniform_int_distribution<> dist_job(0, n - 1);
        int job_ind = dist_job(gen);
        std::uniform_int_distribution<> dist_proc(0, m - 1);
        int new_proc = dist_proc(gen);

        return std::pair(job_ind, new_proc);
    }
    
    void mutate(Schedule_solution* s, int job_ind, int new_proc) override {
        int n = s->schedule.size();
        int m = s->proc_num;
        std::vector<int> numbers(m, 0);
        for (int i = 0; i < n; ++i) {
            if (i == job_ind) {
                s->schedule[i].first = new_proc;
            }
            s->schedule[i].second = numbers[s->schedule[i].first];
            ++numbers[s->schedule[i].first];
        }
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
        temp = (temp_zero / std::log(1 + iter));
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

    void run(int n, int m) {
        mqd_t to, from;
        //std::cout << "where" << std::endl;
        while ((to = mq_open(to_main, O_WRONLY)) == -1) {};
        //std::cout << "how" << std::endl;
        while ((from = mq_open(from_main, O_RDONLY)) == -1) {};
        //std::cout << to << " " << from << std::endl;
        Schedule_mutation mutation = Schedule_mutation();
        //std::cout << "okay" << std::endl;

    	while (true) {
    	    std::pair<int, int> mut = mutation.choose(n, m);
            char *buffer = (char *)calloc(8192, sizeof(char));
            //std::cout <<"!" << mut.first << " " << mut.second << std::endl;

            mq_send(to, sending_parser(mut.first, mut.second, 1, buffer), 8192, prio);
            
            //std::cout << "client send 1" << std::endl;

            while (mq_receive(from, buffer, 8192, NULL) == -1) {}
            if (int(buffer[0]) == 2) {

                //std::cout << "client rcvd 2" << std::endl;

                free(buffer);
                break;
            }
            free(buffer);
    	}
        exit(0);
    }
};
//----------------------------------------------------------------------------



//------------------------------parsers---------------------------------------
char *sending_parser(int job, int proc, int signal, char *buffer) {
    if (signal == 1) {
        buffer[0] = char(1);
        int i = 1;
        while (job > 0) {
            buffer[i] = char(job % 2);
            job = job / 2;
            i += 1;
        }
        buffer[i] = char(2);
        i += 1;
        while (proc > 0) {
            buffer[i] = char(proc % 2);
            proc = proc / 2;
            i += 1;
        }
        buffer[i] = char(2);
        /*for (int j = 1; j < i + 1; ++j) {
            printf("%d", int(buffer[j]));
        }
        printf("s\n");*/
        return buffer;
    }
    buffer[0] = char(2);
    return buffer;
}

std::pair<int, int> receiving_parser(char *buffer) {
    std::pair<int, int> rcv(0, 0);
    int counter = 1;
    int i = 1;
    while (int(buffer[i]) != 2) {
        rcv.first += int(buffer[i]) * counter;
        counter *= 2;
        ++i;
    }
    ++i;
    counter = 1;
    while (int(buffer[i]) != 2) {
        rcv.second += int(buffer[i]) * counter;
        counter *= 2;
        ++i;
    }
    /*for (int j = 1; j < i; ++j) {
        printf("%d", int(buffer[j]));
    }
    printf("r\n");
    std::cout << rcv.first << " " << rcv.second << std::endl;*/
    return rcv;
}
//----------------------------------------------------------------------------