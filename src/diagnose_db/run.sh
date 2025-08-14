#!/bin/bash
g++ -O2 -std=c++17 -o visualise_intervals visualise_intervals.cpp ../lib/db.cpp ../lib/plandata.cpp ../lib/division.cpp ../lib/csv.cpp ../lib/macro.cpp ../lib/utility.cpp
g++ -O2 -std=c++17 -o print_division_occurences print_division_occurences.cpp ../lib/db.cpp ../lib/plandata.cpp ../lib/division.cpp ../lib/csv.cpp ../lib/macro.cpp ../lib/utility.cpp
./visualise_intervals >intervals.txt
./print_division_occurences >occurences.txt
rm visualise_intervals
rm print_division_occurences

#
# Draw Coverage
#

# macOS Qt6 framework build script
QT_PATH="/opt/homebrew/opt/qt6"

# Check if Qt6 exists
if [ ! -d "$QT_PATH" ]; then
  echo "Qt6 not found at $QT_PATH"
  echo "Try: brew install qt6"
  exit 1
fi

echo "Building with Qt frameworks at: $QT_PATH"

# Compile using frameworks (macOS approach)
g++ -std=c++17 -O2 \
  -I$QT_PATH/include \
  -I$QT_PATH/include/QtCore \
  -I$QT_PATH/include/QtWidgets \
  -I$QT_PATH/include/QtCharts \
  -I$QT_PATH/include/QtGui \
  -I$QT_PATH/include/QtPrintSupport \
  draw_coverage.cpp ../lib/db.cpp ../lib/plandata.cpp ../lib/macro.cpp ../lib/division.cpp ../lib/csv.cpp ../lib/firm.cpp ../lib/graph.cpp ../lib/utility.cpp \
  -F$QT_PATH/lib \
  -framework QtCore \
  -framework QtWidgets \
  -framework QtCharts \
  -framework QtGui \
  -framework QtPrintSupport \
  -rpath $QT_PATH/lib \
  -o draw_coverage

if [ $? -eq 0 ]; then
  echo "Build successful!"
  echo "Running draw_coverage..."
  ./draw_coverage
  rm draw_coverage
else
  echo "Build failed!"
fi

#
# END Draw Coverage
#
