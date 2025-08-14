//
//
// NOTE:
// This program was run with the var REQUIRED_VAR_IDXS define in firm.h modified
// for respective va or l charts. Also, sometimes the firm conversion catch for
// if employment is less than 0 was disabled when employment was unnecessary.
//
//
//

#include "draw_series.h"

namespace plan_database {

void draw_series(Database db) {
  double base_s = 0.0, base_va = 0.0, base_l = 0.0;
  double macro_base_s = 0.0, macro_base_va = 0.0, macro_base_l = 0.0;

  std::vector<Graph::Point> points, macro_points;
  for (int year = LOW; year <= MAX_YEAR; year++) {
    std::vector<Firm> real_firms = Firm::to_real_firms(db.plandata, {year});
    std::vector<Firm> synthetic_firms = Firm::to_synthetic_firms(
        db.plandata, db.macrodata, real_firms, false, {year});
    Firm residual_firm = synthetic_firms[0];

    double sum_s = 0.0, sum_va = 0.0, sum_l = 0.0;
    for (const Firm &firm : real_firms) {
      for (const Firm::Observation &ob : firm.obs) {
        if (ob.year != year)
          continue;

        sum_s += ob.sales;
        sum_va += ob.sales - ob.import_cost;
        sum_l += ob.employees;
      }
    }

    double res_s = 0.0, res_va = 0.0, res_l = 0.0;
    for (const Firm::Observation &ob : residual_firm.obs) {
      if (ob.year == year) {
        res_s += ob.sales;
        res_va = ob.sales - ob.import_cost;
        res_l = ob.employees;
        break;
      }
    }

    if (year == LOW) {
      base_s = sum_s / sum_l;
      base_va = sum_va / sum_l;
      macro_base_s = (res_s + sum_s) / (res_l + sum_l);
      macro_base_va = (res_va + sum_va) / (res_l + sum_l);
    }

    double relative_s = 100 * (sum_s / sum_l) / base_s;
    double relative_va = 100 * (sum_va / sum_l) / base_va;
    double relative_macro_s =
        100 * (sum_s + res_s) / (sum_l + res_l) / macro_base_s;
    double relative_macro_va =
        100 * (sum_va + res_va) / (sum_l + res_l) / macro_base_va;

    double s = 1e-6 * (sum_s / sum_l);
    double va = 1e-6 * (sum_va / sum_l);
    double macro_s = 1e-6 * (sum_s + res_s) / (sum_l + res_l);
    double macro_va = 1e-6 * (sum_va + res_va) / (sum_l + res_l);

    points.push_back(Graph::Point(year, relative_va));
    macro_points.push_back(Graph::Point(year, relative_macro_va));
  }

  Graph::Serie serie = Graph::Serie(points, "Planning Survey firms");
  Graph::Serie macro_serie =
      Graph::Serie(macro_points, "Swedish Manufacturing");

  Graph graph = Graph(
      "Productivity Value - Planning Survey and Swedish Manufacturing", "Year",
      "Productivity value per employee (index = 1980)", {serie, macro_serie});

  QChart *chart = graph.create_chart({LOW, MAX_YEAR});
  Graph::export_chart("series_va_index", chart);
}

} // namespace plan_database

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  plan_database::Database db;
  db.plandata.parse_csv("../data/plan1975-2000.csv", ';', true);
  db.macrodata.parse_csv("../data/macrodatabase.csv");

  plan_database::draw_series(db);

  return 0;
}
