#include "firm.h"
#include "macro.h"
#include <stdexcept>

namespace plan_database {

Firm::Firm(int _id, int _mkt_id, std::vector<Observation> &_obs)
    : id(_id), mkt_id(_mkt_id), obs(std::move(_obs)) {}

Firm::Firm(const Division &div, const std::vector<int> &years,
           const std::vector<int> &required_var_idxs) {
  id = div.id;
  mkt_id = get_mkt_id(div.obs[0].industry);
  obs = std::vector<Observation>();

  for (const Division::Observation &div_ob : div.obs) {
    if (!years.empty() &&
        std::find(years.begin(), years.end(), div_ob.year) == years.end())
      continue;

    // check for na values before initialisation from division
    for (const int var : required_var_idxs) {
      if (div_ob.vars[var] == EMPTY_NUM)
        throw std::runtime_error(
            "ERROR: NA value in initialisation "
            "of firm from division (id: " +
            std::to_string(div.id) + " year: " + std::to_string(div_ob.year) +
            " variable: X" + std::to_string(var + 1) + ")");
    }

    int year = div_ob.year;

    double employees = div_ob.vars[1];
    double sales = 1e6 * (div_ob.vars[7] + div_ob.vars[10]);
    double input_cost =
        1e6 * (div_ob.vars[16] + div_ob.vars[19] + div_ob.vars[22]);
    double wage_sum = 1e6 * div_ob.vars[25];
    double wage = wage_sum / employees;

    if (employees <= 0) {
      throw std::runtime_error("ERROR: employees <= 0 in initialisation of "
                               "firm from division (id: " +
                               std::to_string(div.id) +
                               " year: " + std::to_string(div_ob.year) + ")");
    }

    obs.push_back({year, employees, sales, input_cost, wage_sum, wage});
  }
}

Firm::Firm(const std::vector<Firm> &firms, const MacroData &macrodata,
           const std::vector<int> &years) {
  id = -1000;
  mkt_id = NO_MKT;
  obs = std::vector<Observation>();
  is_synthetic = true;

  // Aggregate macro data by year
  std::map<int, MacroData::Observation> macro_by_year;
  for (const MacroData::Observation &mac_ob : macrodata.obs) {
    if (!years.empty() &&
        std::find(years.begin(), years.end(), mac_ob.year) == years.end())
      continue;

    auto it = macro_by_year.find(mac_ob.year);
    if (it == macro_by_year.end()) {
      macro_by_year[mac_ob.year] = mac_ob;
    } else {
      it->second.employees += mac_ob.employees;
      it->second.sales += mac_ob.sales;
      it->second.input_cost += mac_ob.input_cost;
      it->second.wage_sum += mac_ob.wage_sum;
    }
  }

  // Subtract all real firms from macro totals for each year
  for (const auto &[year, mac_ob] : macro_by_year) {
    double employees = mac_ob.employees - aggregate(firms, year, "employees");
    double sales = mac_ob.sales - aggregate(firms, year, "sales");
    double input_cost =
        mac_ob.input_cost - aggregate(firms, year, "input_cost");
    double wage_sum = mac_ob.wage_sum - aggregate(firms, year, "wage_sum");
    double wage = wage_sum / employees;

    if (employees <= 0 || sales <= 0 || input_cost <= 0 || wage_sum <= 0)
      throw std::runtime_error(
          "ERROR: Invalid value in synthetic residual firm for year " +
          std::to_string(year));

    obs.push_back({year, employees, sales, input_cost, wage_sum, wage});
  }

  if (obs.empty())
    throw std::runtime_error("ERROR: Tried to initialise synthetic residual "
                             "firm without macro data");
}

Firm::Firm(const std::vector<Firm> &firms, const MacroData &macrodata,
           const int _mkt_id, const std::vector<int> &years) {
  id = -1000 - _mkt_id;
  mkt_id = _mkt_id;
  obs = std::vector<Observation>();
  is_synthetic = true;

  for (const MacroData::Observation &mac_ob : macrodata.obs) {
    if (!years.empty() &&
        std::find(years.begin(), years.end(), mac_ob.year) == years.end())
      continue;
    if (mac_ob.mkt_id != _mkt_id)
      continue;

    int year = mac_ob.year;

    double employees =
        mac_ob.employees - aggregate(firms, year, "employees", mkt_id);
    double sales = mac_ob.sales - aggregate(firms, year, "sales", mkt_id);
    double input_cost =
        mac_ob.input_cost - aggregate(firms, year, "input_cost", mkt_id);
    double wage_sum =
        mac_ob.wage_sum - aggregate(firms, year, "wage_sum", mkt_id);
    double wage = wage_sum / employees;

    if (employees <= 0 || sales <= 0 || input_cost <= 0 || wage_sum <= 0)
      throw std::runtime_error("ERROR: Invalid value in initialisation of "
                               "synthetic residual firm (mkt_id: " +
                               std::to_string(_mkt_id) +
                               " year: " + std::to_string(mac_ob.year) + ")");

    obs.push_back({year, employees, sales, input_cost, wage_sum, wage});
  }

  if (obs.empty())
    throw std::runtime_error(
        "ERROR: Tried to initialise synthetic residual "
        "firm without macro data from that sector (mkt_id: " +
        std::to_string(_mkt_id));
}

double Firm::aggregate(const std::vector<Firm> &firms, const int year,
                       const std::string var_name, const int mkt_id) {
  double ret = 0;
  for (const Firm &firm : firms) {

    if (mkt_id != NO_MKT && mkt_id != firm.mkt_id)
      continue;

    if (firm.is_synthetic)
      continue;

    for (const Observation &ob : firm.obs) {

      if (ob.year != year)
        continue;

      if (var_name == "employees")
        ret += ob.employees;
      else if (var_name == "sales")
        ret += ob.sales;
      else if (var_name == "input_cost")
        ret += ob.input_cost;
      else if (var_name == "wage_sum")
        ret += ob.wage_sum;
      else
        throw std::runtime_error(
            "ERROR: Invalid var_name in Firm::aggregate()");
    }
  }

  return ret;
}

std::vector<Firm>
Firm::to_real_firms(const PlanData &plandata, const std::vector<int> &years,
                    const std::vector<int> &required_var_idxs) {
  std::vector<Firm> real_firms;
  for (const Division &div : plandata.divs) {

    // Only include firms with observations from all desired years
    // (and with no NA in those observations)
    if (!years.empty()) {
      bool flag = false;
      if (!div.assert_no_na(required_var_idxs, years))
        flag = true;
      for (int year : years) {
        if (!div.has_year(year))
          flag = true;
      }

      if (flag) {
        std::cerr << "WARNING:: Skipping real firm conversion due to NA or "
                     "missing value (id: "
                  << div.id << ")" << std::endl;
        continue;
      }
    }

    real_firms.push_back(Firm(div, years, required_var_idxs));
  }

  return real_firms;
}

std::vector<Firm> Firm::to_synthetic_firms(const PlanData &plandata,
                                           const MacroData &macrodata,
                                           const std::vector<Firm> &real_firms,
                                           const bool divide_synthetic,
                                           const std::vector<int> &years) {
  std::vector<Firm> synthetic_firms;
  if (divide_synthetic) {

    // Divide synthetic firms per industry
    for (int mkt_id = 0; mkt_id < N_MKT_PLAN; mkt_id++) {
      if (macrodata.has_market(mkt_id))
        synthetic_firms.push_back(Firm(real_firms, macrodata, mkt_id, years));
    }
  } else {

    // Only one synthetic aggregate
    synthetic_firms.push_back(Firm(real_firms, macrodata, years));
  }

  return synthetic_firms;
}

std::vector<Firm> Firm::to_firms(const PlanData &plandata,
                                 const MacroData &macrodata,
                                 const bool divide_synthetic,
                                 const std::vector<int> &years) {
  std::vector<Firm> real_firms = to_real_firms(plandata, years);
  std::vector<Firm> synthetic_firms = to_synthetic_firms(
      plandata, macrodata, real_firms, divide_synthetic, years);

  std::vector<Firm> ret = real_firms;
  ret.insert(ret.end(), synthetic_firms.begin(), synthetic_firms.end());
  return ret;
}

std::vector<Firm> Firm::filter_markets(const std::vector<Firm> &firms,
                                       const std::vector<int> mkt_ids) {
  std::vector<Firm> ret;
  for (const Firm &firm : firms) {
    if (std::find(mkt_ids.begin(), mkt_ids.end(), firm.mkt_id) !=
        mkt_ids.end()) {
      ret.push_back(firm);
    }
  }

  return ret;
}

bool Firm::has_year(int year) const {
  for (const Observation &ob : obs) {
    if (ob.year == year)
      return true;
  }

  return false;
}

std::vector<Firm> Firm::filter_years(const std::vector<Firm> &firms,
                                     const std::vector<int> &years) {
  std::vector<Firm> ret;
  for (const Firm &firm : firms) {
    bool flag = true;
    for (int year : years) {
      if (!firm.has_year(year)) {
        flag = false;
        break;
      }
    }

    if (flag)
      ret.push_back(firm);
  }

  return ret;
}

} // namespace plan_database
