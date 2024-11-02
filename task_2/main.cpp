#include <chrono>
#include <fstream>
#include <sys/wait.h>

#include "classes.cpp"

std::vector<int> jobs_generator(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);   // times

    std::vector<int> jobs(n);
    for (int i = 0; i < n; ++i) {
        jobs[i] = dis(gen);
    }
    return jobs;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << "Please use the next scheme: file tasks_num processes_num processors_num" << std::endl;
        return 0;
    }

    int n = std::stoi(argv[1]);
    int m = std::stoi(argv[2]);
    int processors = std::stoi(argv[3]);
    std::vector<int> jobs = jobs_generator(n);
    /*for (int i = 0; i < n; ++i) {
        std::cout << i << ": " << jobs[i] << std::endl;
    }*/
    attr.mq_maxmsg = processors;
    attr.mq_msgsize = 8192;
    attr.mq_flags = 1;
    attr.mq_curmsgs = 0;

    Cooler* bolz = new Bolzman_cooler(1000.0);
    Mutation* mut = new Schedule_mutation();
    Schedule_solution *best_sol = new Schedule_solution(n, m, jobs);
    int best_cost = best_sol->calculate();

    std::cout << best_cost << std::endl;

    Annealing anneal = Annealing();
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<pid_t> pids(processors);
    for (int i = 0; i < processors; ++i) {
        pid_t tmp = fork();
        if (tmp == 0) {   // son is here
            anneal.run(n, m);
        } else {
            pids[i] = tmp;
        }
    }
    // open messages queues
    mqd_t from = mq_open(from_main, O_CREAT|O_WRONLY, 0777, &attr);
    mqd_t to = mq_open(to_main, O_CREAT|O_RDONLY, 0777, &attr);
    //std::cout << to << " " << from << std::endl;

    // here the main process will answer other processes
    int no_improvement_counter = 0;
    int iters = 0;
    while (no_improvement_counter < 10) {
        char buffer[8192];
        bool improve = false;
        double temp = bolz->get_temp();
        for (int i = 0; i < processors; ++i) {
            while (mq_receive(to, buffer, 8192, &prio) == -1) {}

            //std::cout << "serv received 1" << std::endl;

            std::pair<int, int> rcv = receiving_parser(buffer);
            Schedule_solution *sol = best_sol->clone();
            mut->mutate(*sol, rcv.first, rcv.second);
            /*sol->print();
            std::cout << "----------------------" << std::endl;*/
            int sol_cost = sol->calculate();

            //std::cout << i <<"th processor" << std::endl;

    	    double accept_prob = exp((best_cost - sol_cost) / temp);
            double x;
    	    if (sol_cost < best_cost || accept_prob > (x = ((double)rand() / RAND_MAX))) {

                //std::cout << sol_cost << " " << best_cost << " " << accept_prob << " " << x << std::endl;

    	        delete best_sol;
    	        best_sol = sol->clone();
    	        best_cost = sol_cost;
                improve = true;
    	    } else {
    	        delete sol;
    	    }
        }
        bolz->cool(iters + 1);
        if (!improve) {
            ++no_improvement_counter;
        }
        for (int i = 0; i < processors; ++i) {
            if (no_improvement_counter == 10) {
                mq_send(from, sending_parser(0, 0, 2, buffer), 8192, prio);
                
                //std::cout << "serv send 2" << std::endl;

            } else {
                mq_send(from, sending_parser(0, 0, 1, buffer), 8192, prio);
                
                //std::cout << "serv send 1" << std::endl;

            }
        }
        std::destroy_at(buffer);
        ++iters;
    }
    std::chrono::duration<double> dur = std::chrono::high_resolution_clock::now() - start;
    sleep(1);
    for (int i = 0; i < processors; ++i) {
        kill(pids[i], SIGTERM);
        wait(NULL);
    }

    mq_close(to);
    mq_close(from);

    std::cout << "Time at all: " << dur.count() << std::endl;
    std::cout << "Minimal result: " << best_sol->calculate() << std::endl;
    
    //std::cout << iters << std::endl;

    std::ofstream results("results_parallel.csv", std::ios::app);
    if (results.is_open()) {
        results << n << ", " << m << ", " << dur.count() << ", " << best_sol->calculate() << std::endl;
        results.close();
    }
    delete bolz;
    delete mut;
    
    //best_sol->print();

    delete best_sol;

    return 0;
}