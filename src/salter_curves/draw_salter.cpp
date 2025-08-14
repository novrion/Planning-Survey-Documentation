#include "draw_salter.h"
using namespace plan_database;

namespace plan_database {

void draw_one_firm_develops(Database db) {
  db.plandata.filter_interval(LOW, HIGH, false);

  std::vector<Firm> firms = Firm::to_real_firms(db.plandata, years_fewer);
  firms = Firm::filter_years(firms, years_fewer);

  std::vector<Graph::Serie> series;
  for (int year : years_fewer) {

    std::vector<Graph::Point> points;
    for (const Firm &firm : firms) {
      for (const Firm::Observation &ob : firm.obs) {
        if (ob.year == year) {
          const double y = 1e-6 * (ob.sales - ob.import_cost) / ob.employees;
          const double x = ob.sales - ob.import_cost;
          points.push_back({x, y, firm.id == CASE_ID});
          break;
        }
      }
    }

    Graph::Serie serie(points, std::to_string(year));
    serie.sort_points();
    serie.beam_mark = true;
    series.push_back(serie);
  }

  Graph salter("", "% of production", "Value productivity per employee (MSEK)",
               series);
  QChart *chart = salter.create_salter_chart();
  Graph::export_chart("labor_productivity_distrs_one_case", chart, 1000, 2000);
}

void draw_productivity_distrs(Database db) {
  std::vector<Firm> firms =
      Firm::to_firms(db.plandata, db.macrodata, false, years);

  std::vector<Graph::Serie> series;
  for (int year : years) {

    std::vector<Graph::Point> points;
    for (const Firm &firm : firms) {
      for (const Firm::Observation &ob : firm.obs) {
        if (ob.year == year) {
          const double y = 1e-6 * (ob.sales - ob.import_cost) / ob.employees;
          const double x = ob.sales - ob.import_cost;
          points.push_back({x, y, firm.is_synthetic});
          break;
        }
      }
    }

    Graph::Serie serie(points, std::to_string(year));
    serie.sort_points();
    series.push_back(serie);

    std::cout << "productivity: year: " << year
              << " n_points: " << serie.points.size() << std::endl;
  }

  Graph salter("", "% of production", "Value productivity per employee (MSEK)",
               series);
  QChart *chart = salter.create_salter_chart({0, NA});
  Graph::export_chart("labor_productivity_distrs_1982-1997", chart, 1000, 2000);
}

void draw_productivity_distrs_per_industry(Database db, const int year) {
  std::vector<Firm> firms =
      Firm::to_firms(db.plandata, db.macrodata, true, years);

  std::vector<Graph::Serie> series;
  for (int mkt_id : {RAW, IMED, DUR, NDUR}) {
    std::vector<Graph::Point> points;
    for (const Firm &firm : firms) {
      if (firm.mkt_id != mkt_id)
        continue;

      for (const Firm::Observation &ob : firm.obs) {
        if (ob.year == year) {
          const double y = 1e-6 * (ob.sales - ob.import_cost) / ob.employees;
          const double x = ob.sales - ob.import_cost;
          points.push_back({x, y, firm.is_synthetic});
          break;
        }
      }
    }

    Graph::Serie serie(points, get_industry_name(mkt_id));
    serie.sort_points();
    series.push_back(serie);
  }

  Graph salter("", "% of production", "Value productivity per employee (MSEK)",
               series);
  QChart *chart = salter.create_salter_chart({0, NA});
  Graph::export_chart("labor_productivity_distrs_per_industry_" +
                          std::to_string(year),
                      chart, 1000, 2000);
}

void draw_productivity_distrs_no_selection(Database db) {
  std::vector<Graph::Serie> series;
  for (int year : years) {

    std::vector<Firm> firms =
        Firm::to_firms(db.plandata, db.macrodata, false, {year});

    std::vector<Graph::Point> points;
    for (const Firm &firm : firms) {
      for (const Firm::Observation &ob : firm.obs) {
        if (ob.year == year) {
          const double y = 1e-6 * (ob.sales - ob.import_cost) / ob.employees;
          const double x = ob.sales - ob.import_cost;
          points.push_back({x, y, firm.is_synthetic});
          break;
        }
      }
    }

    Graph::Serie serie(points, std::to_string(year));
    serie.sort_points();
    series.push_back(serie);

    std::cout << "productivity: year: " << year
              << " n_points: " << serie.points.size() << std::endl;
  }

  Graph salter("", "% of production", "Value productivity per employee (MSEK)",
               series);
  QChart *chart = salter.create_salter_chart({0, NA});
  Graph::export_chart("labor_productivity_distrs_no_selection_1982-1997", chart,
                      1000, 2000);
}

void draw_productivity_distrs_no_selection_interpolated(Database db) {
  std::vector<Graph::Serie> series;
  for (int year : years) {

    std::vector<Firm> firms =
        Firm::to_firms(db.plandata, db.macrodata, false, {year});

    std::vector<Graph::Point> points;
    for (const Firm &firm : firms) {
      for (const Firm::Observation &ob : firm.obs) {
        if (ob.year == year) {
          const double y = 1e-6 * (ob.sales - ob.import_cost) / ob.employees;
          const double x = ob.sales - ob.import_cost;
          points.push_back({x, y, firm.is_synthetic});
          break;
        }
      }
    }

    Graph::Serie serie(points, std::to_string(year));
    serie.sort_points();
    series.push_back(serie);

    std::cout << "productivity: year: " << year
              << " n_points: " << serie.points.size() << std::endl;
  }

  Graph salter("", "% of production", "Value productivity per employee (MSEK)",
               series);
  QChart *chart = salter.create_salter_chart({0, NA});
  Graph::export_chart(
      "labor_productivity_distrs_no_selection_interpolated_1982-1997", chart,
      1000, 2000);
}

void draw_wage_cost_distrs(Database db) {
  std::vector<Firm> firms =
      Firm::to_firms(db.plandata, db.macrodata, false, years);

  std::vector<Graph::Serie> series;
  for (int year : years) {

    std::vector<Graph::Point> points;
    for (const Firm &firm : firms) {
      for (const Firm::Observation &ob : firm.obs) {
        if (ob.year == year) {
          const double y = 1e-6 * (ob.wage_sum) / ob.employees;
          const double x = ob.sales - ob.import_cost;
          points.push_back({x, y, firm.is_synthetic});
          break;
        }
      }
    }

    Graph::Serie serie(points, std::to_string(year));
    serie.sort_points();
    series.push_back(serie);
  }

  Graph salter("", "% of production", "Wage cost per employee (MSEK)", series);
  QChart *chart = salter.create_salter_chart({0, NA});
  Graph::export_chart("wage_cost_distrs_1982-1997", chart, 1000, 2000);
}

void draw_wage_cost_distrs_per_industry(Database db, const int year) {
  std::vector<Firm> firms =
      Firm::to_firms(db.plandata, db.macrodata, true, years);

  std::vector<Graph::Serie> series;
  for (int mkt_id : {RAW, IMED, DUR, NDUR}) {
    std::vector<Graph::Point> points;
    for (const Firm &firm : firms) {
      if (firm.mkt_id != mkt_id)
        continue;

      for (const Firm::Observation &ob : firm.obs) {
        if (ob.year == year) {
          const double y = 1e-6 * (ob.wage_sum) / ob.employees;
          const double x = ob.sales - ob.import_cost;
          points.push_back({x, y, firm.is_synthetic});
          break;
        }
      }
    }

    Graph::Serie serie(points, get_industry_name(mkt_id));
    serie.sort_points();
    series.push_back(serie);
  }

  Graph salter("", "% of production", "Wage cost per employee (MSEK)", series);
  QChart *chart = salter.create_salter_chart({0, NA});
  Graph::export_chart("wage_cost_distrs_per_industry_" + std::to_string(year),
                      chart, 1000, 2000);
}

std::pair<Graph::Serie, Graph::Serie>
to_aligned_prod_wage_series(const std::vector<Firm> &firms,
                            const int year_idx) {

  std::vector<Graph::Point> prod_points, wage_points;

  for (const Firm &firm : firms) {
    for (const Firm::Observation &ob : firm.obs) {

      if (ob.year != years[year_idx])
        continue;

      const double prod_y = 1e-6 * (ob.sales - ob.import_cost) / ob.employees;
      const double wage_y = 1e-6 * ob.wage_sum / ob.employees;
      const double x = ob.sales - ob.import_cost;

      prod_points.push_back(Graph::Point(x, prod_y, firm.is_synthetic));
      wage_points.push_back(Graph::Point(x, wage_y, firm.is_synthetic));
    }
  }

  //
  //
  // Sorting Logic
  //
  //

  // Create indices and sort by productivity (descending)
  std::vector<size_t> indices(prod_points.size());
  std::iota(indices.begin(), indices.end(), 0);
  std::sort(indices.begin(), indices.end(), [&prod_points](size_t i, size_t j) {
    return prod_points[i].y >
           prod_points[j].y; // sort by productivity descending
  });

  // Rearrange both vectors using sorted indices
  std::vector<Graph::Point> sorted_prod_points, sorted_wage_points;
  for (size_t idx : indices) {
    sorted_prod_points.push_back(prod_points[idx]);
    sorted_wage_points.push_back(wage_points[idx]);
  }
  prod_points = sorted_prod_points;
  wage_points = sorted_wage_points;

  //
  //
  //

  Graph::Serie prod_serie =
      Graph::Serie(prod_points, std::to_string(years[year_idx]), year_idx);
  Graph::Serie wage_serie = Graph::Serie(wage_points, "", year_idx);

  return std::make_pair(prod_serie, wage_serie);
}

void draw_productivity_and_wage_cost_distrs(Database db) {
  std::vector<Firm> firms =
      Firm::to_firms(db.plandata, db.macrodata, false, years);

  std::vector<Graph::Serie> series;
  for (int i = 0; i < years.size(); i++) {
    auto p = to_aligned_prod_wage_series(firms, i);
    series.push_back(p.first);
    series.push_back(p.second);
  }

  Graph salter("", "% of production", "Million SEK per employee", series);
  QChart *chart = salter.create_salter_chart({0, NA});
  Graph::export_chart("productivity_and_wage_cost_distrs_1982-1997", chart,
                      1000, 2000);
}

std::pair<Graph::Serie, Graph::Serie>
to_aligned_prod_wage_series_per_industry(const std::vector<Firm> &firms,
                                         const int year_idx, const int mkt_id) {

  std::vector<Graph::Point> prod_points, wage_points;

  for (const Firm &firm : firms) {
    if (firm.mkt_id != mkt_id)
      continue;

    for (const Firm::Observation &ob : firm.obs) {

      if (ob.year != years[year_idx])
        continue;

      const double prod_y = 1e-6 * (ob.sales - ob.import_cost) / ob.employees;
      const double wage_y = 1e-6 * ob.wage_sum / ob.employees;
      const double x = ob.sales - ob.import_cost;

      prod_points.push_back(Graph::Point(x, prod_y, firm.is_synthetic));
      wage_points.push_back(Graph::Point(x, wage_y, firm.is_synthetic));
    }
  }

  //
  //
  // Sorting Logic
  //
  //

  // Create indices and sort by productivity (descending)
  std::vector<size_t> indices(prod_points.size());
  std::iota(indices.begin(), indices.end(), 0);
  std::sort(indices.begin(), indices.end(), [&prod_points](size_t i, size_t j) {
    return prod_points[i].y >
           prod_points[j].y; // sort by productivity descending
  });

  // Rearrange both vectors using sorted indices
  std::vector<Graph::Point> sorted_prod_points, sorted_wage_points;
  for (size_t idx : indices) {
    sorted_prod_points.push_back(prod_points[idx]);
    sorted_wage_points.push_back(wage_points[idx]);
  }
  prod_points = sorted_prod_points;
  wage_points = sorted_wage_points;

  //
  //
  //

  Graph::Serie prod_serie =
      Graph::Serie(prod_points, get_industry_name(mkt_id), mkt_id);
  Graph::Serie wage_serie = Graph::Serie(wage_points, "", mkt_id);

  return std::make_pair(prod_serie, wage_serie);
}

void draw_productivity_and_wage_cost_distrs_per_industry(Database db,
                                                         const int year_idx) {
  std::vector<Firm> firms =
      Firm::to_firms(db.plandata, db.macrodata, true, years);

  std::vector<Graph::Serie> series;
  for (int mkt_id : {RAW, IMED, DUR, NDUR}) {
    auto p = to_aligned_prod_wage_series_per_industry(firms, year_idx, mkt_id);
    series.push_back(p.first);
    series.push_back(p.second);
  }

  Graph salter("", "% of production", "Million SEK per employee", series);
  QChart *chart = salter.create_salter_chart({0, NA});
  Graph::export_chart("productivity_and_wage_cost_distrs_per_industry_" +
                          std::to_string(years[year_idx]),
                      chart, 1000, 2000);
}

} // namespace plan_database

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  Database db_interpolated;

  db_interpolated.plandata.parse_csv("../data/interpolated.csv", ',', true);
  db_interpolated.macrodata.parse_csv("../data/macrodatabase.csv");
  draw_one_firm_develops(db_interpolated);

  db_interpolated.plandata.filter_markets({DUR, NDUR, IMED, RAW});
  db_interpolated.macrodata.filter_markets({DUR, NDUR, IMED, RAW});
  db_interpolated.plandata.filter_years(years);
  db_interpolated.macrodata.filter_years(years);
  draw_productivity_distrs(db_interpolated);
  draw_wage_cost_distrs(db_interpolated);
  draw_productivity_and_wage_cost_distrs(db_interpolated);

  // Per Industry
  for (int year_idx = 0; year_idx < years.size(); year_idx++) {
    draw_productivity_distrs_per_industry(db_interpolated, years[year_idx]);
    draw_wage_cost_distrs_per_industry(db_interpolated, years[year_idx]);
    draw_productivity_and_wage_cost_distrs_per_industry(db_interpolated,
                                                        year_idx);
  }
  // END Per industry

  Database db;

  db.plandata.parse_csv("../data/plan1975-2000.csv", ';', true);
  db.macrodata.parse_csv("../data/macrodatabase.csv");
  db.plandata.filter_markets({DUR, NDUR, IMED, RAW});
  db.macrodata.filter_markets({DUR, NDUR, IMED, RAW});
  draw_productivity_distrs_no_selection(db);

  Database db_interpolated2;
  db_interpolated2.plandata.parse_csv("../data/interpolated.csv", ',', true);
  db_interpolated2.macrodata.parse_csv("../data/macrodatabase.csv");
  db_interpolated2.plandata.filter_markets({DUR, NDUR, IMED, RAW});
  db_interpolated2.macrodata.filter_markets({DUR, NDUR, IMED, RAW});
  draw_productivity_distrs_no_selection_interpolated(db_interpolated2);

  return 0;
}
