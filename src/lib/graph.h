#ifndef GRAPH_H
#define GRAPH_H

#include "firm.h"
#include "utility.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QString>
#include <QtGui/QBrush>
#include <QtGui/QFont>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include <QtPrintSupport/QPrinter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <cstddef>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#define N_STYLES 5

namespace plan_database {

class Graph {
public:
  struct Point {
    double x, y;
    bool is_marked = false;

    Point(double _x, double _y, bool _is_marked = false);
  };

  struct Serie {
    int style_id = NA;
    std::vector<Point> points;
    std::string name = "";

    bool beam_mark = false;

    Serie(std::vector<Point> &_points, const std::string _name = "",
          const int _style_id = NA);

    void sort_points();
    QString get_serie_name(const int style_idx) const;

    std::vector<QLineSeries *>
    create_salter_series_segments(const int style_idx) const;
  };

  std::vector<Serie> series;
  QString title, x_label, y_label;

  // Configuration
  //
  //
  constexpr static QColor line_color = QColor(0, 0, 0);
  constexpr static Qt::PenStyle line_styles[N_STYLES] = {
      Qt::SolidLine,      // Solid line
      Qt::DashLine,       // Dashed line
      Qt::DashDotLine,    // Dash-dot pattern
      Qt::DashDotDotLine, // Dash-dot-dot pattern
      Qt::DotLine,        // Dotted line

  };
  static inline std::string line_style_legends[N_STYLES] = {
      "─────",   // SolidLine
      "─ ─ ─",   // DashLine
      "─·─·─",   // DashDotLine
      "─··─··─", // DashDotDotLine
      "· · ·",   // DotLine
  };

  //
  //
  // END Configuration

  Graph(const std::string &_title, const std::string &_x_label,
        const std::string &_y_label, const std::vector<Serie> &_series);

  QChart *create_chart(const std::pair<double, double> x_range = {NA, NA},
                       const std::pair<double, double> y_range = {NA, NA},
                       const std::pair<int, int> n_ticks = {NA, NA});
  QChart *create_salter_chart(const std::pair<double, double> y_range = {NA,
                                                                         NA},
                              const int n_y_ticks = NA);

  static void export_chart(const std::string &path, QChart *chart,
                           const int width = 2000, const int length = 2000);
}; // namespace plan_database

} // namespace plan_database

#endif // GRAPH_H
