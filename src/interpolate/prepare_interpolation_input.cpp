#include "prepare_interpolation_input.h"

namespace plan_database {

void prepare_interpolation_input(PlanData &plandata) {
  for (Division &div : plandata.divs) {

    div.sort_obs();

    // 1. Add empty years with no data
    std::map<int, bool> years;
    for (int year = div.obs[0].year; year < div.obs[div.obs.size() - 1].year;
         year++) {
      years.insert({year, false});
    }
    for (const Division::Observation &ob : div.obs) {
      years[ob.year] = true;
    }

    for (const auto &p : years) {
      if (!p.second) {
        std::vector<std::string> tokens(71, "");
        tokens[2] = div.obs[0].industry;
        tokens[4] = std::to_string(p.first);
        div.obs.push_back(Division::Observation::parse_tokens(tokens, true));
      }
    }

    div.sort_obs();

    // 2. Force forward fill industry
    const std::string industry = div.obs[0].industry;
    for (auto &ob : div.obs) {
      ob.industry = industry;
    }

    // 3. Variables with historic values
    std::vector<int> historic_vars_to_check = {1,  4,  7,  60, 10, 13, 16, 19,
                                               22, 25, 63, 28, 30, 33, 36};
    for (int &i : historic_vars_to_check) {
      i--; // 0-index vars
    }

    // copy to avoid interpolating from interpolated values
    Division div_temp = div;
    for (const int var_hist : historic_vars_to_check) {
      const int var_cur = var_hist + 1;
      // var_hist: historic value of variable
      // var_cur: current value of variable

      // 3.1 Fill historic value with last year's current value
      for (int i = 1; i < div.obs.size(); i++) {
        if (div.obs[i].vars[var_hist] == EMPTY_NUM &&
            div.obs[i - 1].vars[var_cur] != EMPTY_NUM) {
          div_temp.obs[i].vars[var_hist] = div.obs[i - 1].vars[var_cur];
        }
      }

      // 3.2 Fill current value with next year's historic value
      for (int i = 0; i < div.obs.size() - 1; i++) {
        if (div.obs[i].vars[var_cur] == EMPTY_NUM &&
            div.obs[i + 1].vars[var_hist] != EMPTY_NUM) {
          div_temp.obs[i].vars[var_cur] = div.obs[i + 1].vars[var_hist];
        }
      }
    }

    div = div_temp;

    // 4 Forward fill names
    for (int i = 1; i < div.obs.size(); i++) {
      if (div.obs[i].name == EMPTY && div.obs[i - 1].name != EMPTY)
        div.obs[i].name = div.obs[i - 1].name;
    }

    // 5. Values with totals (gross investments)
    for (auto &ob : div.obs) {
      for (int var = 29; var <= 31; var++) {
        int one_i = var;
        int two_i = var + 3;
        int tot_i = var + 6;

        // No total, but components exist
        if (ob.vars[one_i] != EMPTY_NUM && ob.vars[two_i] != EMPTY_NUM &&
            ob.vars[tot_i] == EMPTY_NUM)
          ob.vars[tot_i] = ob.vars[one_i] + ob.vars[two_i];

        // Total and first value exists, but second value does not exist
        if (ob.vars[one_i] != EMPTY_NUM && ob.vars[two_i] == EMPTY_NUM &&
            ob.vars[tot_i] != EMPTY_NUM)
          ob.vars[two_i] = std::max(0.0, ob.vars[tot_i] - ob.vars[one_i]);

        // Total and second value exists, but first value does not exist
        if (ob.vars[one_i] == EMPTY_NUM && ob.vars[two_i] != EMPTY_NUM &&
            ob.vars[tot_i] != EMPTY_NUM)
          ob.vars[one_i] = std::max(0.0, ob.vars[tot_i] - ob.vars[two_i]);
      }
    }
  }
}

} // namespace plan_database

int main() {
  plan_database::Database db;
  db.plandata.parse_csv("../data/plan1975-2000.csv", ';', true);

  plan_database::prepare_interpolation_input(db.plandata);

  db.plandata.write_csv("interpolation_input.csv", ',', true);

  return 0;
}
