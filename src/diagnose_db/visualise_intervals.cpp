#include "visualise_intervals.h"

namespace plan_database {

void visualise_intervals(PlanData &plandata) {
  std::map<std::pair<int, int>, int> hash;
  for (int base_year = MIN_YEAR; base_year <= MAX_YEAR; base_year++) {
    for (int upper_year = MIN_YEAR; upper_year <= MAX_YEAR; upper_year++) {
      if (base_year > upper_year)
        continue;

      std::pair<int, int> key = {base_year, upper_year};
      hash[key] = 0;

      for (Division &div : plandata.divs) {
        if (div.in_interval(base_year, upper_year, HARD))
          hash[key]++;
      }
    }
  }

  // Put the hash map (of all ranges) into a vector
  struct Range {
    int base_year, upper_year, count;
    Range(int _base_year, int _upper_year, int _count)
        : base_year(_base_year), upper_year(_upper_year), count(_count) {}
  };
  std::vector<Range> ranges;
  for (auto const &x : hash) {
    ranges.push_back(Range(x.first.first, x.first.second, x.second));
  }

  // sort by range length
  std::sort(ranges.begin(), ranges.end(), [](const auto &a, const auto &b) {
    return (a.upper_year - a.base_year) > (b.upper_year - b.base_year);
  });

  std::cout << "INTERVAL\tLENGTH\tCOUNT\tCOUNT VISUALISATION" << std::endl;
  for (const auto &range : ranges) {
    std::cout << range.base_year << "-" << range.upper_year << "\t"
              << range.upper_year - range.base_year << "\t" << range.count
              << "\t";
    for (int i = 0; i < range.count * 0.4; i++) {
      std::cout << "=";
    }
    std::cout << std::endl;
  }
}

} // namespace plan_database

int main() {
  plan_database::Database db;
  db.plandata.parse_csv("../data/plan1975-2000.csv", ';', true);

  plan_database::visualise_intervals(db.plandata);

  return 0;
}
