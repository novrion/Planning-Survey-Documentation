#ifndef DIVISION_H
#define DIVISION_H

#include "csv.h"
#include <map>
#include <string>
#include <vector>

namespace plan_database {

class Division {
public:
  struct Observation {
    int year, sni;                    // sni: full_data
    std::string industry, code, name; // code, name: full_data
    std::vector<double> vars;         // variables X1-X65

    Observation(int _year, std::string &_industry, std::vector<double> &_vars,
                const std::string &_name = EMPTY, const int _sni = EMPTY_NUM,
                const std::string &_code = EMPTY);

    std::vector<std::string> tokenise(const bool full_data = false) const;
    static Observation parse_tokens(const std::vector<std::string> &tokens,
                                    const bool full_data = false);
  };

  int id;                       // division ID
  std::vector<Observation> obs; // observations

  Division(int _id);
  Division(int _id, std::vector<Observation> &_obs);

  void sort_obs();
  bool in_interval(const int low, const int high, const bool hard = false);
  bool assert_no_na(const std::vector<int> &var_idxs,
                    const std::vector<int> &years = std::vector<int>()) const;
  bool has_year(const int year) const;
  void filter_years(const std::vector<int> &years);
  std::vector<std::vector<std::string>>
  tokenise(const bool full_data = false) const;
};

} // namespace plan_database

#endif // DIVISION_H
