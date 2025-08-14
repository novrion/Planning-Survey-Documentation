#include "../lib/db.h"
#include "../lib/firm.h"
#include "../lib/graph.h"

#define LOW 1982
#define HIGH 1997
#define HARD false

#define YEAR 1982
#define CASE_ID 00000

const std::vector<int> years = {1982, 1990, 1997};
const std::vector<int> years_fewer = {1982, 1997};

namespace plan_database {

void draw_one_firm_develops(Database db);
void draw_productivity_distrs(Database db);
void draw_productivity_distrs_per_industry(Database db, const int year);
void draw_productivity_distrs_no_selection(Database db);
void draw_productivity_distrs_no_selection_interpolated(Database db);
void draw_wage_cost_distrs(Database db);
void draw_wage_cost_distrs_per_industry(Database db, const int year);
void draw_productivity_and_wage_cost_distrs(Database db);
void draw_productivity_and_wage_cost_distrs_per_industry(Database db,
                                                         const int year_idx);

std::pair<Graph::Serie, Graph::Serie>
to_aligned_prod_wage_series(const std::vector<Firm> &firms, const int year_idx);
std::pair<Graph::Serie, Graph::Serie>
to_aligned_prod_wage_series_per_industry(const std::vector<Firm> &firms,
                                         const int year_idx, const int mkt_id);

} // namespace plan_database
