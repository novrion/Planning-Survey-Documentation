#!/bin/bash

# Prepare interpolation input csv
g++ -O2 -std=c++17 -o prepare_interpolation_input prepare_interpolation_input.cpp ../lib/db.cpp ../lib/plandata.cpp ../lib/division.cpp ../lib/csv.cpp ../lib/macro.cpp ../lib/utility.cpp
./prepare_interpolation_input

# Interpolate (R)
Rscript interpolate.R

# Clean up interpolation output (and only overwrite selected variables)
g++ -O2 -std=c++17 -o clean_interpolation_output clean_interpolation_output.cpp ../lib/db.cpp ../lib/plandata.cpp ../lib/division.cpp ../lib/csv.cpp ../lib/macro.cpp ../lib/utility.cpp
./clean_interpolation_output

# Delete intermediary csv files
rm interpolation_input.csv
rm interpolation_output.csv

# Delete binaries
rm prepare_interpolation_input
rm clean_interpolation_output
