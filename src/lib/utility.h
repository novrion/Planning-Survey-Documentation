#ifndef UTILITY_H
#define UTILITY_H

#include <string>

namespace plan_database {

#define NA -1e9

#define N_MKT_PLAN 5
#define N_MKT_MOSES 4

#define NO_MKT -1
#define RAW 0
#define IMED 1
#define NDUR 2
#define DUR 3
#define CONSTR 4

#define MIN_YEAR 1975
#define MAX_YEAR 2000

int get_mkt_id(const std::string &industry);
std::string get_industry_name(const int mkt_id);

} // namespace plan_database

#endif // UTILITY_H
