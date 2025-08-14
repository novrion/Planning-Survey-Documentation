#ifndef FIRM_H
#define FIRM_H

#include "macro.h"
#include "plandata.h"
#include "utility.h"
#include <stdexcept>
#include <string>
#include <vector>

namespace plan_database {

static const std::vector<int> REQUIRED_VAR_IDXS = {1, 7, 10, 16, 19, 22, 25};

class Firm {
public:
  struct Observation {
    int year;
    double employees, sales, import_cost, wage_sum, wage;
  };

  int id;
  int mkt_id;
  bool is_synthetic = false;
  std::vector<Observation> obs;

  // the "years" variable filters to only convert observations from certain
  // years. Any empty vector implies NO filter (all observations are converted)

  Firm(int _id, int _mkt_id, std::vector<Observation> &_obs);

  Firm(const Division &div, const std::vector<int> &years = std::vector<int>(),
       const std::vector<int> &required_var_idxs =
           REQUIRED_VAR_IDXS); // Initialise from division

  Firm(const std::vector<Firm> &firms, const MacroData &macrodata,
       const int _mkt_id,
       const std::vector<int> &years =
           std::vector<int>()); // Create synthetic residual firms per industry

  Firm(const std::vector<Firm> &firms, const MacroData &macrodata,
       const std::vector<int> &years =
           std::vector<int>()); // Create synthetic residual firm

  static double aggregate(const std::vector<Firm> &firms, const int year,
                          const std::string var_name,
                          const int mkt_id = NO_MKT);

  std::vector<Firm> static to_real_firms(
      const PlanData &plandata,
      const std::vector<int> &years = std::vector<int>(),
      const std::vector<int> &required_var_idxs = REQUIRED_VAR_IDXS);

  static std::vector<Firm>
  to_synthetic_firms(const PlanData &plandata, const MacroData &macrodata,
                     const std::vector<Firm> &real_firms,
                     const bool divide_synthetic = true,
                     const std::vector<int> &years = std::vector<int>());

  static std::vector<Firm>
  to_firms(const PlanData &plandata, const MacroData &macrodata,
           const bool divide_synthetic = true,
           const std::vector<int> &years = std::vector<int>());

  static std::vector<Firm> filter_markets(const std::vector<Firm> &firms,
                                          const std::vector<int> mkt_ids);
  static std::vector<Firm> filter_years(const std::vector<Firm> &firms,
                                        const std::vector<int> &years);
  bool has_year(int year) const;
};

} // namespace plan_database

#endif // FIRM_H
