#include "macro.h"

namespace plan_database {

MacroData::Observation
MacroData::Observation::parse_tokens(const std::vector<std::string> &tokens) {
  Observation ob;

  for (int i = 0; i < tokens.size(); i++) {
    std::string token = csv::trim(tokens[i]);
    if (token.empty() || token.size() < 1)
      token = EMPTY;

    switch (i) {
    case 0:
      ob.mkt_id = get_mkt_id(token);
      break;
    case 1:
      ob.year = std::stoi(token);
      break;
    case 2:
      ob.sales = (token == EMPTY ? EMPTY_NUM : 1e6 * std::stod(token));
      break;
    case 3:
      ob.input_cost = (token == EMPTY ? EMPTY_NUM : 1e6 * std::stod(token));
      break;
    case 4:
      ob.wage_sum = (token == EMPTY ? EMPTY_NUM : 1e6 * std::stod(token));
      break;
    case 5:
      ob.value_added = (token == EMPTY ? EMPTY_NUM : 1e6 * std::stod(token));
      break;
    case 6:
      ob.employees = (token == EMPTY ? EMPTY_NUM : 1e3 * std::stod(token));
      break;
    case 7:
      ob.manhours = (token == EMPTY ? EMPTY_NUM : 1e6 * std::stod(token));
      break;
    case 8:
      ob.gross_investments =
          (token == EMPTY ? EMPTY_NUM : 1e6 * std::stod(token));
      break;
    default:
      throw std::runtime_error("ERROR: Malformed row");
    }
  }

  return ob;
}

void MacroData::sort_obs() {
  std::sort(obs.begin(), obs.end(),
            [](const auto &a, const auto &b) { return a.year < b.year; });
}

void MacroData::filter_interval(const int low, const int high) {
  std::vector<Observation> filtered_obs;
  for (Observation &ob : obs) {
    if (ob.year <= high && ob.year >= low)
      filtered_obs.push_back(ob);
  }

  obs = std::move(filtered_obs);
}

void MacroData::filter_markets(const std::vector<int> &mkt_ids) {
  std::vector<Observation> filtered_obs;
  for (Observation &ob : obs) {
    if (std::find(mkt_ids.begin(), mkt_ids.end(), ob.mkt_id) != mkt_ids.end())
      filtered_obs.push_back(ob);
  }

  obs = std::move(filtered_obs);
}

void MacroData::filter_years(const std::vector<int> &years) {
  std::vector<Observation> filtered_obs;
  for (Observation &ob : obs) {
    if (std::find(years.begin(), years.end(), ob.year) != years.end())
      filtered_obs.push_back(ob);
  }

  obs = std::move(filtered_obs);
}

bool MacroData::has_market(const int mkt_id) const {
  for (const Observation &ob : obs) {
    if (ob.mkt_id == mkt_id)
      return true;
  }
  return false;
}

void MacroData::parse_csv(const std::string &path, const char separator) {
  std::vector<std::vector<std::string>> rows = csv::parse(path, separator);

  obs = std::vector<Observation>();
  for (const std::vector<std::string> &tokens : rows) {
    MacroData::Observation ob = MacroData::Observation::parse_tokens(tokens);
    obs.push_back(ob);
  }

  sort_obs();
}

void MacroData::write_csv(const std::string &path, const char separator) {}

} // namespace plan_database
