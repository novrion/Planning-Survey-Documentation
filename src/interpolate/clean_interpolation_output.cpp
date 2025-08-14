#include "clean_interpolation_output.h"

namespace plan_database {

void clean_interpolation_output(PlanData &base_data, PlanData &interp_data) {
  for (const int idx : selected_vars) {
    for (int div_idx = 0; div_idx < base_data.divs.size(); div_idx++) {

      // 1. Find interval to only overwrite between end values that are not NA
      // ("easy" interval) if ONLY_FILL_BETWEEN
      int ob_idx_low = -1, ob_idx_high = -1;
      if (ONLY_FILL_BETWEEN) {

        // Find ob_idx_low
        for (int ob_idx = 0; ob_idx < base_data.divs[div_idx].obs.size();
             ob_idx++) {
          if (base_data.divs[div_idx].obs[ob_idx].vars[idx] != EMPTY_NUM) {
            ob_idx_low = ob_idx;
            break;
          }
        }

        // Find ob_idx_high
        for (int ob_idx = base_data.divs[div_idx].obs.size() - 1; ob_idx >= 0;
             ob_idx--) {
          if (base_data.divs[div_idx].obs[ob_idx].vars[idx] != EMPTY_NUM) {
            ob_idx_high = ob_idx;
            break;
          }
        }

      } else {
        ob_idx_low = 0;
        ob_idx_high = base_data.divs[div_idx].obs.size();
      }

      if (ob_idx_low == -1 || ob_idx_high == -1) {
        std::cerr
            << "ERROR: Could not find ob_idx_low or ob_idx_high (all vars "
               "are NA): "
            << "{ id: " << base_data.divs[div_idx].id << ", var: X" << idx + 1
            << " }" << std::endl;

        // If error in trying to interpolate a division that is in the desired
        // interval, throw an exception
        if (base_data.divs[div_idx].in_interval(LOW, HIGH)) {
          throw std::runtime_error(
              std::string("ERROR: Could not find ob_idx_low or ob_idx_high for "
                          "division IN "
                          "THE SELECTED INTERVAL (all vars are NA): ") +
              " {id : " + std::to_string(base_data.divs[div_idx].id) +
              ", var: X" + std::to_string(idx + 1) + " }\n");
        }
      }

      // 2. Overwrite NA-values with interpolated values in interval
      for (int ob_idx = 0; ob_idx < base_data.divs[div_idx].obs.size();
           ob_idx++) {
        if (base_data.divs[div_idx].obs[ob_idx].vars[idx] == EMPTY_NUM &&
            interp_data.divs[div_idx].obs[ob_idx].vars[idx] != EMPTY_NUM) {

          double interpolated_value =
              interp_data.divs[div_idx].obs[ob_idx].vars[idx];
          if (interpolated_value < 0) {
            std::cerr << "WARNING: Negative interpolated value: " +
                             std::to_string(interpolated_value);
          }

          base_data.divs[div_idx].obs[ob_idx].vars[idx] = interpolated_value;
        }
      }
    }
  }

  // Make number of employee values integers
  for (auto &div : base_data.divs) {
    for (auto &ob : div.obs) {
      ob.vars[0] = (int)ob.vars[0];
      ob.vars[1] = (int)ob.vars[1];
      ob.vars[2] = (int)ob.vars[2];
    }
  }
}

} // namespace plan_database

int main() {

  // Base database to overwrite with interpolated values
  plan_database::Database db_base;
  db_base.plandata.parse_csv("interpolation_input.csv", ',', true);

  // Database with interpolated values
  plan_database::Database db_interp;
  db_interp.plandata.parse_csv("interpolation_output.csv", ',', true);

  // Overwrite NA-values in the base database with interpolated values
  plan_database::clean_interpolation_output(db_base.plandata,
                                            db_interp.plandata);

  // Write updated base database
  db_base.plandata.write_csv("interpolated.csv", ',', true);

  return 0;
}
