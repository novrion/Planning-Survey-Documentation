#include "../lib/db.h"
#include "../lib/utility.h"

// Parameters
#define YEAR 1982
#define LOW 1982
#define HIGH 1997
#define HARD false

namespace plan_database {

void print_names_per_industry(Database &db, const int year);
void print_names_per_industry_interval(Database &db, const int low,
                                       const int high, const bool hard = false);

} // namespace plan_database
