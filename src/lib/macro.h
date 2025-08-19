#ifndef MACRO_H
#define MACRO_H

#include "csv.h"
#include "utility.h"
#include <vector>

namespace plan_database {

class MacroData {
public:
  struct Observation {
    int year, mkt_id;
    double sales, input_cost, wage, wage_sum, value_added, employees, manhours,
        gross_investments;

    static Observation parse_tokens(const std::vector<std::string> &tokens);
  };

  std::vector<Observation> obs; // Observations

  void sort_obs();
  void filter_interval(const int low, const int high);
  void filter_markets(const std::vector<int> &mkt_ids);
  void filter_years(const std::vector<int> &years);
  bool has_market(const int mkt_id) const;

  void parse_csv(const std::string &path, const char separator = ',');
  void write_csv(const std::string &path, const char separator = ',');
};

} // namespace plan_database

#endif // MACRO_H
