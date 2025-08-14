//
//
// NOTE:
// The firm conversion catch for if employment is less than 0 was disabled when
// running this program (labor irrelevant for the coverage_va graph)
//
//

#include "draw_coverage.h"
#include <stdexcept>

namespace plan_database {

void draw_coverage(Database &db, int var) {
  std::vector<int> required_var_idxs;
  std::string y_axis_name;
  std::string path;
  if (var == LABOR) {
    required_var_idxs = {1};
    y_axis_name = "Labor (% of Swedish manufacturing)";
    path = "coverage_l";
  } else if (var == VALUEADDED) {
    required_var_idxs = {7, 10, 16, 19, 22};
    y_axis_name = "Value-added (% of Swedish manufacturing)";
    path = "coverage_va";
  } else
    throw std::runtime_error("Invalid 'var' argument.");

  std::vector<Graph::Point> points;
  for (int year = LOW; year <= MAX_YEAR; year++) {
    std::vector<Firm> real_firms =
        Firm::to_real_firms(db.plandata, {year}, required_var_idxs);
    std::vector<Firm> synthetic_firms = Firm::to_synthetic_firms(
        db.plandata, db.macrodata, real_firms, false, {year});
    Firm residual_firm = synthetic_firms[0];

    double sum = 0.0;
    for (const Firm &firm : real_firms) {
      for (const Firm::Observation &ob : firm.obs) {
        if (ob.year != year)
          continue;

        if (var == LABOR)
          sum += ob.employees;
        else if (var == VALUEADDED)
          sum += ob.sales - ob.import_cost;
      }
    }

    double macro = 0.0;
    for (const Firm::Observation &ob : residual_firm.obs) {
      if (ob.year == year) {

        if (var == LABOR)
          macro = ob.employees;
        else if (var == VALUEADDED)
          macro = ob.sales - ob.import_cost;

        break;
      }
    }

    double total = sum + macro;
    double coverage = 100 * sum / total;
    points.push_back(Graph::Point(year, coverage));
  }

  Graph::Serie serie = Graph::Serie(points);

  Graph graph = Graph("", "Year", y_axis_name, {serie});
  QChart *chart = graph.create_chart({LOW, MAX_YEAR}, {0, 100}, {NA, 11});
  Graph::export_chart(path, chart, 1000, 2000);
}

} // namespace plan_database

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  plan_database::Database db;
  db.plandata.parse_csv("../data/plan1975-2000.csv", ';', true);
  db.macrodata.parse_csv("../data/macrodatabase.csv");

  plan_database::draw_coverage(db, LABOR);
  plan_database::draw_coverage(db, VALUEADDED);

  return 0;
}
