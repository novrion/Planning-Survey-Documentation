#include "../lib/db.h"

// Parameters
#define LOW 1982  // Lower bound of interval to interpolate
#define HIGH 1997 // Upper bound of interval to interpolate
#define ONLY_FILL_BETWEEN                                                      \
  false // Only fill interpolate between data points (do not interpolate outside
        // of "easy" interval (see lib/division.cpp, in_interval())
const std::vector<int> selected_vars = {
    // 0-indexed
    1,          // anställda
    4,          // arbetade timmar
    25,         // lönekostnader
    16, 19, 22, // inköpskostnader
    7,  10,     // faktureringar till utlandet och hemmamarknaden
}; // Selected variables to interpolate

namespace plan_database {

void clean_interpolation_output(PlanData &base_data, PlanData &interp_data);

} // namespace plan_database
