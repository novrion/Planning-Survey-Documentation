#include "plandata.h"
#include "division.h"

namespace plan_database {

void PlanData::sort_divs() {
  std::sort(divs.begin(), divs.end(),
            [](const auto &a, const auto &b) { return a.id < b.id; });
}

void PlanData::filter_markets(const std::vector<int> &mkt_ids) {
  std::vector<Division> filtered_divs;
  for (const Division &div : divs) {
    if (std::find(mkt_ids.begin(), mkt_ids.end(),
                  get_mkt_id(div.obs[0].industry)) != mkt_ids.end())
      filtered_divs.push_back(div);
  }

  divs = std::move(filtered_divs);
}

void PlanData::filter_interval(const int low, const int high, const bool hard) {
  std::vector<Division> filtered_divs;
  for (Division &div : divs) {
    if (div.in_interval(low, high, hard))
      filtered_divs.push_back(div);
  }

  divs = std::move(filtered_divs);
}

void PlanData::filter_years(const std::vector<int> &years) {
  std::vector<Division> filtered_divs;

  // Keep divisions with at least one observation from years
  for (Division &div : divs) {

    bool flag = false;
    for (const Division::Observation &ob : div.obs) {
      if (std::find(years.begin(), years.end(), ob.year) != years.end()) {
        flag = true;
        break;
      }
    }

    if (flag)
      filtered_divs.push_back(div);
  }

  divs = std::move(filtered_divs);

  // Filter remaining divisions' observations by year
  for (Division &div : divs) {
    div.filter_years(years);
  }
}

void PlanData::parse_csv(const std::string &path, const char separator,
                         const bool full_data) {
  std::vector<std::vector<std::string>> rows = csv::parse(path, separator);

  for (const std::vector<std::string> &tokens : rows) {
    Division::Observation ob =
        Division::Observation::parse_tokens(tokens, full_data);

    // Parse division ID
    if (csv::empty(tokens[0]))
      throw std::runtime_error("ERROR: Observation has no ID");
    int id = std::stoi(tokens[0]);

    // Add division if ID does not exist
    auto it = std::find_if(divs.begin(), divs.end(),
                           [id](const Division &div) { return div.id == id; });
    if (it == divs.end()) {
      divs.push_back(Division(id));
      it = divs.end() - 1;
    }

    // Insert observation in division
    it->obs.push_back(std::move(ob));
  }

  // Sort by division ID and year (ascending)
  this->sort_divs();
  for (Division &div : divs) {
    div.sort_obs();
  }
}

void PlanData::write_csv(const std::string &path, const char separator,
                         const bool full_data, const int filter_year) {
  std::vector<std::string> header;
  std::vector<std::vector<std::string>> rows;

  // Header
  if (full_data)
    header = {"id", "code", "industry", "sni", "year", "name"};
  else
    header = {"id", "industry", "year"};

  for (int i = 1; i <= 65; i++) {
    header.push_back("X" + std::to_string(i));
  }

  // Rows
  for (const Division &div : divs) {
    std::vector<std::vector<std::string>> div_rows = div.tokenise(full_data);

    if (filter_year) { // Only include observations in one cross-section
      for (const std::vector<std::string> tokens : div_rows) {
        if ((full_data && tokens[4] == std::to_string(filter_year)) ||
            (!full_data && tokens[2] == std::to_string(filter_year)))
          rows.push_back(tokens);
      }
    } else // Append all observations from the division
      rows.insert(rows.end(), div_rows.begin(), div_rows.end());
  }

  csv::write(path, rows, separator, header);
}

} // namespace plan_database
