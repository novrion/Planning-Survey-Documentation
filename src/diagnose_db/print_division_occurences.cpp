#include "print_division_occurences.h"

namespace plan_database {

void print_division_occurences(PlanData &plandata) {
  struct Rank {
    int n_obs, n_missing_obs, n_gaps;
    int id;
    std::string name;

    Rank(int _id, std::string _name, int _n_obs, int _n_missing_obs,
         int _n_gaps)
        : id(_id), name(_name), n_obs(_n_obs), n_missing_obs(_n_missing_obs),
          n_gaps(_n_gaps) {}
  };

  std::vector<Rank> rankings;
  for (const Division &div : plandata.divs) {
    int n_obs = div.obs.size(); // number of observations
    int n_missing_obs =
        MAX_YEAR - MIN_YEAR - n_obs + 1; // number of missing observations

    int n_gaps = 0; // number of gaps of missing observations
    if (n_obs > 1) {
      for (int i = 1; i < n_obs; i++) {
        if (div.obs[i - 1].year != div.obs[i].year - 1)
          n_gaps++;
      }
    }

    std::string div_name = div.obs[n_obs - 1].name;
    rankings.push_back(Rank(div.id, div_name, n_obs, n_missing_obs, n_gaps));
  }

  // Sort by descending number of observations
  std::sort(rankings.begin(), rankings.end(),
            [](const auto &a, const auto &b) { return a.n_obs > b.n_obs; });

  std::cout << "ID\tOBSERVATIONS\tGAPS\tNAME" << std::endl;
  std::cout << "  \t(MISSING) (x/" << MAX_YEAR - MIN_YEAR + 1 << ")"
            << std::endl;
  std::cout << std::endl;

  for (const Rank &rank : rankings) {
    std::cout << rank.id << "\t" << rank.n_obs << " (" << rank.n_missing_obs
              << ")\t\t" << rank.n_gaps << "\t" << rank.name << std::endl;
  }
}

} // namespace plan_database

int main() {
  plan_database::Database db;
  db.plandata.parse_csv("../data/plan1975-2000-full.csv", ',', true);

  plan_database::print_division_occurences(db.plandata);

  return 0;
}
