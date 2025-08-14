#ifndef PLANDATA_H
#define PLANDATA_H

#include "csv.h"
#include "division.h"
#include "utility.h"
#include <stdexcept>

namespace plan_database {

class PlanData {
public:
  std::vector<Division> divs; // Divisions

  void sort_divs();
  void filter_markets(const std::vector<int> &mkt_ids);
  void filter_interval(const int low, const int high, const bool hard);
  void filter_years(const std::vector<int> &years);

  void parse_csv(const std::string &path, const char separator = ',',
                 const bool full_data = false);
  void write_csv(const std::string &path, const char separator = ',',
                 const bool full_data = false, const int filter_year = 0);
};

} // namespace plan_database

#endif // PLANDATA_H
