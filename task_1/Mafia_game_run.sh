#! /bin/bash
rm -rf ./logs
g++ -std=c++20 -O2 main.cpp -o main -fcoroutines
./main