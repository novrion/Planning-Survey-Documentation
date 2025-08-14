#include "db.h"
using namespace plan_database;

int main() {
  Database db;
  db.plandata.parse_csv("plandata.csv", ';', true);
  db.plandata.write_csv("out.csv", ',', true);

  return 0;
}
