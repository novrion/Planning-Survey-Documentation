#include "print_industries.h"

namespace plan_database {

void print_names_per_industry(Database &db, const int year) {
  std::cout << "R: Råvaror" << std::endl;
  std::cout << "K: Konsumtionsvaror" << std::endl;
  std::cout << "S: Insatsvaror" << std::endl;
  std::cout << "V: Investeringsvaror" << std::endl;
  std::cout << "B: Bygg" << std::endl;
  std::cout << std::endl;

  std::vector<Division::Observation> obs[5];

  // Extract cross-section
  for (int mkt_id = 0; mkt_id < N_MKT_PLAN; mkt_id++) {
    for (auto &div : db.plandata.divs) {
      for (auto &ob : div.obs) {
        if (ob.year == year && get_mkt_id(ob.industry) == mkt_id)
          obs[mkt_id].push_back(ob);
      }
    }
  }

  // Sort by labour
  for (auto &v : obs) {
    std::sort(
        v.begin(), v.end(),
        [](const Division::Observation &a, const Division::Observation &b) {
          return a.vars[1] > b.vars[1];
        });
  }

  std::cout << "========== CROSS SECTION " << year
            << " ==========" << std::endl;
  std::cout << std::endl;
  std::cout << "INDUSTRY\tLABOUR\tDIVISION" << std::endl;

  for (int mkt_id = 0; mkt_id < N_MKT_PLAN; mkt_id++) {
    std::cout << std::endl;
    for (const auto &ob : obs[mkt_id]) {
      if (ob.year == year) {
        std::cout << ob.industry << "\t\t" << ob.vars[1] << "\t" << ob.name
                  << std::endl;
      }
    }
  }
}

void print_names_per_industry_interval(Database &db, const int low,
                                       const int high, const bool hard) {
  std::cout << "R: Råvaror" << std::endl;
  std::cout << "K: Konsumtionsvaror" << std::endl;
  std::cout << "S: Insatsvaror" << std::endl;
  std::cout << "V: Investeringsvaror" << std::endl;
  std::cout << "B: Bygg" << std::endl;
  std::cout << std::endl;

  std::vector<Division::Observation> obs[5];

  // Extract divisions in interval
  for (int mkt_id = 0; mkt_id < N_MKT_PLAN; mkt_id++) {
    for (auto &div : db.plandata.divs) {
      Division::Observation ob = div.obs[div.obs.size() - 1];
      if (div.in_interval(low, high, hard) &&
          get_mkt_id(ob.industry) == mkt_id) {
        obs[mkt_id].push_back(ob);
      }
    }
  }

  // Sort by labour
  for (auto &v : obs) {
    std::sort(
        v.begin(), v.end(),
        [](const Division::Observation &a, const Division::Observation &b) {
          return a.vars[1] > b.vars[1];
        });
  }

  // Count number of firms in the interval
  int count = 0;
  for (int mkt_id = 0; mkt_id < N_MKT_PLAN; mkt_id++) {
    count += (int)obs[mkt_id].size();
  }

  std::cout << "==================== INTERVAL " << LOW << "-" << HIGH
            << " ====================" << std::endl;
  std::cout << "TOTAL NUMBER OF DIVISIONS IN INTERVAL: " << count << std::endl;
  std::cout << std::endl;
  std::cout << "INDUSTRY\tLABOUR\tDIVISION" << std::endl;

  for (int mkt_id = 0; mkt_id < N_MKT_PLAN; mkt_id++) {
    std::cout << std::endl;
    for (const auto &ob : obs[mkt_id]) {
      std::cout << ob.industry << "\t\t" << ob.vars[1] << "\t" << ob.name
                << std::endl;
    }
  }
}

} // namespace plan_database

int main() {
  plan_database::Database db;
  db.plandata.parse_csv("../data/plan1975-2000-full.csv", ',', true);

  // print_names_per_industry(db, YEAR);
  plan_database::print_names_per_industry_interval(db, LOW, HIGH, HARD);

  return 0;
}
