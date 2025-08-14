#include "../lib/db.h"
using namespace plan_database;

int main() {
  Database db;
  db.plandata.parse_csv("../data/plan1975-2000.csv", ';', true);

  for (int year = 1975; year <= 2000; year++) {
    db.plandata.write_csv("cross_sections/plan" + std::to_string(year) + ".csv",
                          ',', true, year);
  }

  return 0;
}
