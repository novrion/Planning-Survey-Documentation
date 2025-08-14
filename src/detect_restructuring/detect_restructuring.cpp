#include "detect_restructuring.h"

namespace plan_database {

void detect_restructuring(PlanData &plandata) {
  std::cout << "=========================== Organizational Restructures "
               "==========================="
            << std::endl
            << std::endl;
  std::cout << "ID\tYEAR\tVARIABLE\t\t\t\tNAME" << std::endl;
  std::cout << "---------------------------------------------------------------"
               "--------------------"
            << std::endl;

  for (const auto &div : plandata.divs) {

    double prev_val = div.obs[0].vars[VAR_IDX];
    if (prev_val == EMPTY_NUM)
      continue;

    for (const auto &ob : div.obs) {

      double val = ob.vars[VAR_IDX];
      if (val == EMPTY_NUM)
        continue;

      double change = val / prev_val;

      if (std::abs(change) > THRESHOLD) {
        std::cout << div.id << "\t" << ob.year << "\tX" << VAR_IDX + 1 << " ("
                  << change * 100 << "%)\t" << prev_val << " -> " << val
                  << "\t\t" << ob.name << std::endl;
      }

      prev_val = val;
    }
  }

  std::cout << std::endl
            << "==============================================================="
               "=========="
               "=========="
            << std::endl;
}

} // namespace plan_database

int main() {
  plan_database::Database db;
  db.plandata.parse_csv("../data/plan1975-2000-full.csv", ',', true);

  plan_database::detect_restructuring(db.plandata);

  return 0;
}
