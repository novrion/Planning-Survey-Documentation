#!/bin/bash
g++ -O2 -std=c++17 -o run connect_ids.cpp ../lib/db.cpp ../lib/plandata.cpp ../lib/division.cpp ../lib/csv.cpp ../lib/macro.cpp ../lib/utility.cpp
./run
rm run
