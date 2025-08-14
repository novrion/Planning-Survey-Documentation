#!/bin/bash

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
  draw_salter.cpp ../lib/db.cpp ../lib/plandata.cpp ../lib/macro.cpp ../lib/division.cpp ../lib/csv.cpp ../lib/firm.cpp ../lib/graph.cpp ../lib/utility.cpp \
  -F$QT_PATH/lib \
  -framework QtCore \
  -framework QtWidgets \
  -framework QtCharts \
  -framework QtGui \
  -framework QtPrintSupport \
  -rpath $QT_PATH/lib \
  -o salter_curve

if [ $? -eq 0 ]; then
  echo "Build successful!"
  echo "Running salter_curve..."
  ./salter_curve
  rm salter_curve
else
  echo "Build failed!"
fi
