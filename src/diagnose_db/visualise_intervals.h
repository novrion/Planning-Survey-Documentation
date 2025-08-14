#include "../lib/db.h"
#include "../lib/utility.h"

// Parameters
#define HARD                                                                   \
  false // Count in interval if division has an observation before (or at base
        // year) and after (or at upper year) (EASY) or only count if division
        // has an observation for all years in the interval (HARD)

namespace plan_database {

void visualise_intervals(PlanData &plandata);

} // namespace plan_database
