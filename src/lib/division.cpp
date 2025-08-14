#include "division.h"

namespace plan_database {

Division::Observation::Observation(int _year, std::string &_industry,
                                   std::vector<double> &_vars,
                                   const std::string &_name, const int _sni,
                                   const std::string &_code)
    : year(_year), industry(_industry), vars(std::move(_vars)), name(_name),
      sni(_sni), code(_code) {}

std::vector<std::string>
Division::Observation::tokenise(const bool full_data) const {
  std::vector<std::string> tokens;

  if (full_data) {
    tokens.push_back(code);
    tokens.push_back(industry);
    tokens.push_back(sni == EMPTY_NUM ? EMPTY : std::to_string(sni));
    tokens.push_back(year == EMPTY_NUM ? EMPTY : std::to_string(year));

    std::string temp = name;
    temp.erase(std::remove(temp.begin(), temp.end(), ','),
               temp.end()); // Remove commas
    temp = csv::to_uppercase_utf8(temp);
    tokens.push_back(temp);
  } else {
    tokens.push_back(industry);
    tokens.push_back(year == EMPTY_NUM ? EMPTY : std::to_string(year));
  }

  for (const double var : vars) {
    tokens.push_back(var == EMPTY_NUM ? EMPTY : csv::dtostr(var));
  }

  return tokens;
}

std::vector<std::vector<std::string>>
Division::tokenise(const bool full_data) const {
  std::vector<std::vector<std::string>> rows;

  for (const Observation &ob : obs) {
    std::vector<std::string> ob_tokens = ob.tokenise(full_data);
    std::vector<std::string> tokens = {std::to_string(id)};
    tokens.insert(tokens.end(), ob_tokens.begin(), ob_tokens.end());
    rows.push_back(tokens);
  }

  return rows;
}

Division::Division(int _id) : id(_id) {}
Division::Division(int _id, std::vector<Observation> &_obs)
    : id(_id), obs(std::move(_obs)) {}

void Division::sort_obs() {
  std::sort(obs.begin(), obs.end(),
            [](const auto &a, const auto &b) { return a.year < b.year; });
}

bool Division::in_interval(const int low, const int high, const bool hard) {
  // Determines if a division has observations that span the interval low-high.
  // EASY: needs to have an observation that is before or at low and one after
  // or at high. HARD: needs to have an observation for each year low-high.

  this->sort_obs();

  if (hard) {
    std::map<int, bool> years;
    for (int year = low; year <= high; year++) {
      years.insert({year, false});
    }

    for (const auto &ob : obs) {
      years[ob.year] = true;
    }

    for (const auto &p : years) {
      if (p.second == false)
        return false;
    }
  } else {
    if (obs[0].year > low || obs[obs.size() - 1].year < high)
      return false;
  }

  return true;
}

bool Division::assert_no_na(const std::vector<int> &var_idxs,
                            const std::vector<int> &years) const {
  for (const Observation &ob : obs) {
    if (!years.empty() &&
        std::find(years.begin(), years.end(), ob.year) == years.end())
      continue;

    for (const int var_idx : var_idxs) {
      if (ob.vars[var_idx] == EMPTY_NUM)
        return false;
    }
  }

  return true;
}

bool Division::has_year(const int year) const {
  for (const Observation &ob : obs) {
    if (ob.year == year)
      return true;
  }

  return false;
}

void Division::filter_years(const std::vector<int> &years) {
  std::vector<Observation> filtered_obs;
  for (Observation &ob : obs) {
    if (std::find(years.begin(), years.end(), ob.year) != years.end())
      filtered_obs.push_back(ob);
  }

  obs = std::move(filtered_obs);
}

Division::Observation
Division::Observation::parse_tokens(const std::vector<std::string> &tokens,
                                    const bool full_data) {
  int year = EMPTY_NUM, sni = EMPTY_NUM;
  std::string industry = EMPTY, code = EMPTY, name = EMPTY;
  std::vector<double> vars;

  for (int i = 0; i < tokens.size(); i++) {
    std::string token = tokens[i];
    if (token.empty())
      token = EMPTY;

    if (full_data) {
      if (i == 1) {
        code = token;
      } else if (i == 2) {
        industry = token;
      } else if (i == 3) {
        if (token != EMPTY)
          sni = std::stoi(token);
        else
          sni = EMPTY_NUM;
      } else if (i == 4) {
        if (token != EMPTY)
          year = std::stoi(token);
        else
          year = EMPTY_NUM;
      } else if (i == 5) {
        name = token;
      } else if (i >= 6) {
        if (token != EMPTY)
          vars.push_back(std::stod(token));
        else
          vars.push_back(EMPTY_NUM);
      }
    } else {
      // full_data = false

      if (i == 1) {
        industry = token;
      } else if (i == 2) {
        if (token != EMPTY)
          year = std::stoi(token);
        else
          year = EMPTY_NUM;
      } else if (i >= 3) {
        if (token != EMPTY)
          vars.push_back(std::stod(token));
        else
          vars.push_back(EMPTY_NUM);
      }
    }
  }

  if (vars.size() != 65)
    throw std::runtime_error("Error: malformed row, unexpected length (len: " +
                             std::to_string((int)vars.size()) + ", id: " +
                             tokens[0] + ", year: " + std::to_string(year));

  if (full_data)
    return Observation(year, industry, vars, name, sni, code);
  else
    return Observation(year, industry, vars);
}

} // namespace plan_database
