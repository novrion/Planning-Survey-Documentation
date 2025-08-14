#include "../lib/db.h"

#define VAR_IDX 1 // Variable (index) to analyse for restructuring
#define THRESHOLD                                                              \
  2 // Threshold, as the variable's relative difference between two years to
    // identify as restructuring

namespace plan_database {

void detect_restructuring(PlanData &plandata);

} // namespace plan_database
