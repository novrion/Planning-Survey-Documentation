#include "graph.h"

namespace plan_database {

Graph::Point::Point(double _x, double _y, bool _is_marked)
    : x(_x), y(_y), is_marked(_is_marked) {}

Graph::Serie::Serie(std::vector<Point> &_points, const std::string _name,
                    const int _style_id)
    : points(_points), style_id(_style_id), name(_name) {}

void Graph::Serie::sort_points() {
  std::sort(points.begin(), points.end(),
            [](const Point &a, const Point &b) { return a.y > b.y; });
}

QString Graph::Serie::get_serie_name(const int style_idx) const {
  if (name == "")
    return QString::fromStdString("");
  else
    return QString::fromStdString(line_style_legends[style_idx] + "  " + name +
                                  "     ");
}

std::vector<QLineSeries *>
Graph::Serie::create_salter_series_segments(const int style_idx) const {
  std::vector<QLineSeries *> series_segments;

  double x_total =
      std::accumulate(points.begin(), points.end(), 0.0,
                      [](double sum, const Point &p) { return sum += p.x; });

  QLineSeries *current_series = nullptr;
  bool current_marked_status;
  double cum = 0.0;
  bool first_series = true;
  double previous_y = 0.0; // Track previous y-value to connect segments

  for (size_t i = 0; i < points.size(); i++) {
    const Point &point = points[i];

    if (i == 0 || point.is_marked != current_marked_status) {

      // Add connection to the upcoming new segment if previous not marked
      if (!current_marked_status && i > 0 && previous_y != point.y &&
          !first_series)
        current_series->append(cum, point.y);

      // Initialise new segment
      current_series = new QLineSeries();

      // Name
      if (first_series) {
        current_series->setName(get_serie_name(style_idx));
        first_series = false;
      } else {
        current_series->setName("");
      }

      // Styling
      QPen pen;
      if (point.is_marked)
        pen = QPen(QColor(220, 50, 50), 2, line_styles[style_idx]);
      else
        pen = QPen(line_color, 2, line_styles[style_idx]);
      pen.setJoinStyle(Qt::MiterJoin);
      current_series->setPen(pen);

      // Add connection to previous segment if previous was marked
      if (current_marked_status && i > 0 && previous_y != point.y &&
          !first_series)
        current_series->append(cum, previous_y);

      // Add vertical line at the beginning of marked section if beam_mark
      // options is enabled
      if (beam_mark && point.is_marked) {
        current_series->append(cum, point.y);
        current_series->append(cum, 0);
        current_series->append(cum, point.y);
      }

      // Move segment to list and update marked status
      series_segments.push_back(current_series);
      current_marked_status = point.is_marked;
    }

    // Add point to current series
    double share = 100 * point.x / x_total;
    current_series->append(cum, point.y);
    cum += share;
    current_series->append(cum, point.y);

    previous_y = point.y; // Remember this y-value for next iteration
  }

  return series_segments;
}

Graph::Graph(const std::string &_title, const std::string &_x_label,
             const std::string &_y_label, const std::vector<Serie> &_series)
    : series(_series) {
  title = QString::fromStdString(_title);
  x_label = QString::fromStdString(_x_label);
  y_label = QString::fromStdString(_y_label);
}

QChart *Graph::create_chart(const std::pair<double, double> x_range,
                            const std::pair<double, double> y_range,
                            const std::pair<int, int> n_ticks) {
  QChart *chart = new QChart();
  chart->setTitle(title);

  // Append series to chart
  int style_count = 0;
  for (const Serie &serie : series) {
    int style_idx = serie.style_id == NA ? style_count++ : serie.style_id;
    if (style_idx >= N_STYLES)
      throw std::runtime_error("ERROR: style_idx > N_STYLES. Too many salter "
                               "series without set styles.");

    // Initialisation & styling
    QLineSeries *line_series = new QLineSeries();
    line_series->setName(serie.get_serie_name(style_idx));
    QPen pen = QPen(line_color, 2, line_styles[style_idx]);
    line_series->setPen(pen);

    for (const Point &point : serie.points) {
      line_series->append(point.x, point.y);
    }

    chart->addSeries(line_series);
  }

  // X-axis
  QValueAxis *x_axis = new QValueAxis();
  x_axis->setTitleText(x_label);
  x_axis->setLabelFormat("%.0f");
  if (n_ticks.first != NA)
    x_axis->setTickCount(n_ticks.first);
  x_axis->setMinorTickCount(0);
  x_axis->setTickType(QValueAxis::TicksFixed);

  double x_min = x_range.first, x_max = x_range.second;
  if (x_min == NA || x_max == NA) {

    double min_val = 1e9, max_val = -1e9;
    for (const Serie &serie : series) {
      auto [min_it, max_it] = std::minmax_element(
          serie.points.begin(), serie.points.end(),
          [](const Point &a, const Point &b) { return a.x < b.x; });

      min_val = std::min(min_it->x, min_val);
      max_val = std::max(max_it->x, max_val);
    }

    double range = max_val - min_val;
    double magnitude = std::pow(10, std::floor(std::log10(max_val)));
    x_min = x_min == NA ? std::floor(min_val / magnitude) * magnitude : x_min;
    x_max = x_max == NA ? std::ceil(max_val / magnitude) * magnitude : x_max;
  }

  x_axis->setRange(x_min, x_max);

  // Y-axis
  QValueAxis *y_axis = new QValueAxis();
  y_axis->setTitleText(y_label);
  y_axis->setLabelFormat("%.2f");
  if (n_ticks.second != NA)
    y_axis->setTickCount(n_ticks.second);
  y_axis->setMinorTickCount(0);
  y_axis->setTickType(QValueAxis::TicksFixed);

  double y_min = y_range.first, y_max = y_range.second;
  if (y_min == NA || y_max == NA) {

    double min_val = 1e9, max_val = -1e9;
    for (const Serie &serie : series) {
      auto [min_it, max_it] = std::minmax_element(
          serie.points.begin(), serie.points.end(),
          [](const Point &a, const Point &b) { return a.y < b.y; });

      min_val = std::min(min_it->y, min_val);
      max_val = std::max(max_it->y, max_val);
    }

    double range = max_val - min_val;
    double magnitude = std::pow(10, std::floor(std::log10(max_val)));
    y_min = y_min == NA ? std::floor(min_val / magnitude) * magnitude : y_min;
    y_max = y_max == NA ? std::ceil(max_val / magnitude) * magnitude : y_max;
  }

  y_axis->setRange(y_min, y_max);

  // Font configuration
  QFont title_font("Courier New", 28, QFont::Bold);
  QFont axis_font("Courier New", 18, QFont::Medium);
  QFont label_font("Courier New", 14, QFont::Normal);
  QFont legend_font("Courier New", 12, QFont::Normal);

  chart->setTitleBrush(QBrush(QColor(0, 0, 0)));
  x_axis->setTitleBrush(QBrush(QColor(0, 0, 0)));
  y_axis->setTitleBrush(QBrush(QColor(0, 0, 0)));
  x_axis->setLabelsBrush(QBrush(QColor(0, 0, 0)));
  y_axis->setLabelsBrush(QBrush(QColor(0, 0, 0)));

  chart->setTitleFont(title_font);
  x_axis->setTitleFont(axis_font);
  y_axis->setTitleFont(axis_font);
  x_axis->setLabelsFont(label_font);
  y_axis->setLabelsFont(label_font);

  // Remove grid lines
  x_axis->setGridLineVisible(false);
  y_axis->setGridLineVisible(false);
  x_axis->setMinorGridLineVisible(false);
  x_axis->setMinorGridLineVisible(false);

  // Add axes to chart
  chart->addAxis(x_axis, Qt::AlignBottom);
  chart->addAxis(y_axis, Qt::AlignLeft);

  // Attach all series to both axes
  for (auto series : chart->series()) {
    series->attachAxis(x_axis);
    series->attachAxis(y_axis);
  }

  // Legend
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);
  chart->legend()->setFont(legend_font);
  chart->legend()->setBrush(QBrush(QColor(0, 0, 0)));
  chart->legend()->setBackgroundVisible(false); // No background box
  chart->legend()->setMarkerShape(QLegend::MarkerShapeRectangle);

  // Force legend to wrap by setting geometry
  chart->legend()->setGeometry(QRectF(0, 0, chart->size().width(), 100));

  // Markets
  for (auto marker : chart->legend()->markers()) {
    if (marker->label().isEmpty())
      marker->setVisible(false);
    else {
      marker->setBrush(QBrush(Qt::transparent));
      marker->setPen(QPen(Qt::transparent));
    }
  }

  // Chart styling
  chart->setBackgroundBrush(QBrush(Qt::white));
  // chart->setMargins(QMargins(30, 30, 30, 60)); // Extra bottom margin for
  // legend
  chart->setMargins(QMargins(0, 0, 0, 0)); // Extra bottom margin for legend
  chart->setBackgroundRoundness(0);

  return chart;
}

QChart *Graph::create_salter_chart(const std::pair<double, double> y_range,
                                   const int n_y_ticks) {
  QChart *chart = new QChart();
  chart->setTitle(title);

  // Append series to chart
  int style_count = 0;
  for (const Serie &serie : series) {
    int style_idx = serie.style_id == NA ? style_count++ : serie.style_id;
    if (style_idx >= N_STYLES)
      throw std::runtime_error("ERROR: style_idx > N_STYLES. Too many salter "
                               "series without set styles.");

    // Add series
    std::vector<QLineSeries *> series_segments =
        serie.create_salter_series_segments(style_idx);
    for (QLineSeries *series_segment : series_segments) {
      chart->addSeries(series_segment);
    }
  }

  // X-axis
  QValueAxis *x_axis = new QValueAxis();
  x_axis->setTitleText(x_label);
  x_axis->setRange(0, 100);
  x_axis->setTickCount(11);
  x_axis->setLabelFormat("%.0f");

  // Y-axis
  QValueAxis *y_axis = new QValueAxis();
  y_axis->setTitleText(y_label);
  y_axis->setLabelFormat("%.2f");
  if (n_y_ticks != NA)
    y_axis->setTickCount(n_y_ticks);
  y_axis->setMinorTickCount(0);
  y_axis->setTickType(QValueAxis::TicksFixed);

  double y_min = y_range.first, y_max = y_range.second;
  if (y_min == NA || y_max == NA) {

    double min_val = 1e9, max_val = -1e9;
    for (const Serie &serie : series) {
      auto [min_it, max_it] = std::minmax_element(
          serie.points.begin(), serie.points.end(),
          [](const Point &a, const Point &b) { return a.y < b.y; });

      min_val = std::min(min_it->y, min_val);
      max_val = std::max(max_it->y, max_val);
    }

    double range = max_val - min_val;
    double magnitude = std::pow(10, std::floor(std::log10(max_val)));
    y_min = y_min == NA ? std::floor(min_val / magnitude) * magnitude : y_min;
    y_max = y_max == NA ? std::ceil(max_val / magnitude) * magnitude : y_max;
  }

  y_axis->setRange(y_min, y_max);

  // Font configuration
  QFont title_font("Courier New", 28, QFont::Bold);
  QFont axis_font("Courier New", 18, QFont::Medium);
  QFont label_font("Courier New", 14, QFont::Normal);
  QFont legend_font("Courier New", 12, QFont::Normal);

  chart->setTitleBrush(QBrush(QColor(0, 0, 0)));
  x_axis->setTitleBrush(QBrush(QColor(0, 0, 0)));
  y_axis->setTitleBrush(QBrush(QColor(0, 0, 0)));
  x_axis->setLabelsBrush(QBrush(QColor(0, 0, 0)));
  y_axis->setLabelsBrush(QBrush(QColor(0, 0, 0)));

  chart->setTitleFont(title_font);
  x_axis->setTitleFont(axis_font);
  y_axis->setTitleFont(axis_font);
  x_axis->setLabelsFont(label_font);
  y_axis->setLabelsFont(label_font);

  // Remove grid lines
  x_axis->setGridLineVisible(false);
  y_axis->setGridLineVisible(false);
  x_axis->setMinorGridLineVisible(false);
  x_axis->setMinorGridLineVisible(false);

  // Add axes to chart
  chart->addAxis(x_axis, Qt::AlignBottom);
  chart->addAxis(y_axis, Qt::AlignLeft);

  // Attach all series to both axes
  for (auto series : chart->series()) {
    series->attachAxis(x_axis);
    series->attachAxis(y_axis);
  }

  // Legend
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);
  chart->legend()->setFont(legend_font);
  chart->legend()->setBrush(QBrush(QColor(0, 0, 0)));
  chart->legend()->setBackgroundVisible(false); // No background box
  chart->legend()->setMarkerShape(QLegend::MarkerShapeRectangle);

  // Force legend to wrap by setting geometry
  chart->legend()->setGeometry(QRectF(0, 0, chart->size().width(), 100));

  // Markets
  for (auto marker : chart->legend()->markers()) {
    if (marker->label().isEmpty())
      marker->setVisible(false);
    else {
      marker->setBrush(QBrush(Qt::transparent));
      marker->setPen(QPen(Qt::transparent));
    }
  }

  // Chart styling
  chart->setBackgroundBrush(QBrush(Qt::white));
  // chart->setMargins(QMargins(30, 30, 30, 60)); // Extra bottom margin for
  // legend
  chart->setMargins(QMargins(0, 0, 0, 0)); // Extra bottom margin for legend
  chart->setBackgroundRoundness(0);

  return chart;
}

void Graph::export_chart(const std::string &path, QChart *chart,
                         const int width, const int height) {
  QChartView *chart_view = new QChartView(chart);
  // chart_view->setRenderHint(QPainter::Antialiasing);
  chart_view->setFrameStyle(QFrame::NoFrame);
  chart_view->setContentsMargins(0, 0, 0, 0);
  chart_view->setBackgroundBrush(QBrush(Qt::white));

  QMainWindow window;
  window.setCentralWidget(chart_view);
  window.resize(width, height);
  window.setStyleSheet("background-color: white; border: none;");
  window.show();

  // Write chart view to PNG
  QPixmap pixmap = chart_view->grab();
  QString file_name = QString::fromStdString(path + ".PNG");
  pixmap.save(file_name, "PNG" /* JPEG */, 100 /* max quality */);
  std::cout << "Saved PNG: " << file_name.toStdString() << std::endl;
}

} // namespace plan_database
