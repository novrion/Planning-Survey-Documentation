#include "../lib/db.h"
#include <set>

// Parameters
#define FUZZY_WEIGHT                                                           \
  0.1 // Weight (%) tolerance to consider values to be close to each other
      // (calculate_proximity_score())
#define CUT_OFF_YEAR                                                           \
  1997 // Year to search for observations to match with previous years
#define VALUE_THRESHOLD                                                        \
  1.5 // Minimum matching value for divisions to be considered a potential match

// ANSI color codes
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"

namespace plan_database {

const std::vector<std::string> color_alternator = {MAGENTA, RED, BLUE};

void print_observation(const Division::Observation &ob, const int id);
int prompt(Division &div, std::vector<Division> &potential_matches,
           int progress, int total);

double calculate_proximity_score(double base_val, double ob_val);
std::vector<Division>
find_potential_matches(Division &div,
                       std::vector<Division> &potential_base_divs);

void start_prompter(Database &db);

} // namespace plan_database
