#include "print_key.h"

namespace plan_database {

void print_key_csv(Database &db) {
  std::cout << "id;name" << std::endl;
  for (const Division &div : db.plandata.divs) {
    for (int i = div.obs.size() - 1; i >= 0; i--) {
      if (!csv::empty(div.obs[i].name)) {
        std::cout << div.id << ";" << div.obs[i].name << std::endl;
        break;
      }
    }
  }
}

void print_key_beautiful(Database &db) {
  std::cout << "ID\tNAME" << std::endl;
  for (const Division &div : db.plandata.divs) {
    for (int i = div.obs.size() - 1; i >= 0; i--) {
      if (!csv::empty(div.obs[i].name)) {
        std::cout << div.id << "\t" << div.obs[i].name << std::endl;
        break;
      }
    }
  }
}

} // namespace plan_database

int main() {
  plan_database::Database db;
  db.plandata.parse_csv("../data/plan1975-2000.csv", ';', true);

  print_key_csv(db);
  // print_key_beautiful(db);

  return 0;
}
