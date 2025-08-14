#!/bin/bash
g++ -O2 -std=c++17 -o run print_key.cpp ../lib/db.cpp ../lib/plandata.cpp ../lib/division.cpp ../lib/csv.cpp ../lib/macro.cpp ../lib/utility.cpp
./run
rm run
