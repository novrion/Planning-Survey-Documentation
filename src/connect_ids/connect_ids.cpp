#include "connect_ids.h"

namespace plan_database {

void print_observation(const Division::Observation &ob, const int id) {
  std::cout << id << "\t" << ob.industry << "\t\t" << ob.year << "\t" << ob.name
            << "\t";
  for (int i = 0; i <= 8; i++) {
    std::cout << color_alternator[i % 3];
    if (i % 3 == 0)
      std::cout << "  ";

    if (ob.vars[i] == EMPTY_NUM)
      std::cout << EMPTY << ";";
    else
      std::cout << ob.vars[i] << ";";
  }

  std::cout << RESET;
  std::cout << std::endl;
}

int prompt(Division &div, std::vector<Division> &potential_matches,
           int progress, int total) {
  std::system("clear");
  std::cout << "[" << progress << "/" << total << "]" << std::endl << std::endl;
  std::cout << "SID\tID\tINDUSTRY\tYEAR\tNAME\t\tX1;X2;X3; X4;X5;X6; X7;X8;X9;"
            << std::endl;

  int selection_id = 0;
  if (potential_matches.empty()) {
    std::cout << std::endl
              << std::endl
              << "\t\t ===== NO MATCHES FOUND =====" << std::endl
              << std::endl;
  } else {
    for (Division &match : potential_matches) {
      for (Division::Observation &ob : match.obs) {
        std::cout << selection_id << "  |\t";
        print_observation(ob, match.id);
      }
      std::cout << std::endl;
      selection_id++;
    }
  }

  std::cout << std::endl
            << "|===== Should the following division connect with any of the "
               "above divisions? ====="
            << std::endl;
  for (Division::Observation &ob : div.obs) {
    print_observation(ob, div.id);
  }

  while (true) {
    std::cout << std::endl << "NO [N/n], YES [<SID>], EXIT & WRITE [X/x] : ";

    std::string response;
    std::cin >> response;

    if (response == "N" || response == "n")
      return 0;
    if (response == "x" || response == "X")
      return 1;

    int SID = -1;
    try {
      SID = std::stoi(response);
    } catch (const std::exception &e) {
      std::cout << "Invalid response:" << std::endl;
      continue;
    }

    if (SID < 0 || SID > potential_matches.size() - 1) {
      std::cout << "Invalid response:" << std::endl;
      continue;
    }

    std::cout << "Changed division's ID from " << div.id
              << " to: " << potential_matches[SID].id << std::endl;
    div.id = potential_matches[SID].id;

    printf("Enter any character to continue... ");
    std::cin >> response;

    break;
  }

  return 0;
}

double calculate_proximity_score(double base_val, double ob_val) {
  if (base_val <= 0 || ob_val <= 0)
    return 0.0; // Avoid division by zero

  // Calculate ratio (always <= 1.0)
  double ratio = std::min(base_val / ob_val, ob_val / base_val);

  // Minimum acceptable ratio (e.g., 1/1.1 = 0.909 for 10% tolerance)
  double min_ratio = 1.0 / FUZZY_WEIGHT;

  if (ratio < min_ratio)
    return 0.0; // Outside tolerance

  // Scale from min_ratio->0.2 to 1.0->0.9
  double score = 0.2 + (ratio - min_ratio) / (1.0 - min_ratio) * 0.7;
  return score;
}

std::vector<Division>
find_potential_matches(Division &div,
                       std::vector<Division> &potential_base_divs) {
  Division::Observation first_ob =
      div.obs[0]; // earliest observation of the division that is trying to
                  // connect
  std::vector<std::pair<int, Division>> ranking;

  for (const Division &base : potential_base_divs) {
    // Find base observation (could be within list of observations, i.e. no
    // observations from 1994-1997)
    int base_ob_idx = -1;
    for (int i = 0; i < base.obs.size(); i++) {
      if (base.obs[i].year < first_ob.year) {
        base_ob_idx = i;
      }
    }

    // Did not find a base observation
    if (base_ob_idx < 0)
      continue;

    // No overlap of years
    bool overlap = false;
    for (int i = base_ob_idx + 1; i < base.obs.size(); i++) {
      for (int j = 0; j < div.obs.size(); j++) {
        if (base.obs[i].year == div.obs[j].year) {
          overlap = true;
          goto done_checking;
        }
      }
    }
  done_checking:

    // Found overlap
    if (overlap)
      continue;

    Division::Observation base_ob = base.obs[base_ob_idx];
    int value = 0;

    // Perfect match
    // Check if number of employees or number of manhours or sales abroad is
    // perfectly consistent
    if (base_ob.year == first_ob.year - 1 &&
        (base_ob.vars[7] == first_ob.vars[6] ||
         base_ob.vars[4] == first_ob.vars[3] ||
         base_ob.vars[1] == first_ob.vars[0])) {
      ranking.push_back(std::make_pair(1000, base));
      continue;
    }

    // Fuzzy match
    // Check if number of employed or number of worked hours or third field is
    // decently consistent
    double score1 =
        calculate_proximity_score(base_ob.vars[7], first_ob.vars[6]);
    double score2 =
        calculate_proximity_score(base_ob.vars[4], first_ob.vars[3]);
    double score3 =
        calculate_proximity_score(base_ob.vars[1], first_ob.vars[0]);
    value += score1 + score2 + score3;

    // Fuzzy match entry names

    // Tokenise base names across all years
    std::set<std::string> base_name_tokens;
    for (const Division::Observation &ob : base.obs) {
      std::vector<std::string> tokens = csv::tokenise(ob.name, ' ');
      for (const std::string &token : tokens) {
        base_name_tokens.insert(token);
      }
    }

    // Tokenise the division's names across all years
    std::set<std::string> cmp_name_tokens;
    for (const Division::Observation &ob : div.obs) {
      std::vector<std::string> tokens = csv::tokenise(ob.name, ' ');
      for (const std::string token : tokens) {
        cmp_name_tokens.insert(token);
      }
    }

    // Add points if the base names' tokens are similar to the division's names'
    // tokens
    std::vector<std::string> save_tokens;
    for (const std::string &token : base_name_tokens) {
      if (cmp_name_tokens.find(token) != cmp_name_tokens.end() &&
          token != "AB" && token != "BRUK" && token != "ABB" && token != "CO" &&
          token != "&" && token != "STORA" && token != "SVENSKA") {
        save_tokens.push_back(token);
        value += 5;
      }
    }

    // Attempt ID conversion
    if (div.id * 10 - 1000000 == base.id)
      value += 25;

    if (value >= VALUE_THRESHOLD)
      ranking.push_back(std::make_pair(value, base));
  }

  std::sort(ranking.begin(), ranking.end(),
            [](const auto &a, const auto &b) { return a.first < b.first; });

  std::vector<Division> potential_matches;
  for (const auto &p : ranking) {
    potential_matches.push_back(p.second);
  }

  return potential_matches;
}

void start_prompter(Database &db) {
  std::vector<Division> to_connect;
  std::vector<Division> potential_base_divs;

  for (Division &div : db.plandata.divs) {
    div.sort_obs();
    if (div.obs[0].year >= CUT_OFF_YEAR)
      to_connect.push_back(div);
    else
      potential_base_divs.push_back(div);
  }

  int i = 0;
  for (Division &div : to_connect) {
    std::vector<Division> potential_matches =
        find_potential_matches(div, potential_base_divs);
    if (prompt(div, potential_matches, i++ + 1, (int)to_connect.size()))
      break;
  }

  db.plandata.divs.clear();
  db.plandata.divs.insert(db.plandata.divs.end(), to_connect.begin(),
                          to_connect.end());
  db.plandata.divs.insert(db.plandata.divs.end(), potential_base_divs.begin(),
                          potential_base_divs.end());
}

} // namespace plan_database

int main() {
  plan_database::Database db;
  db.plandata.parse_csv("../data/plan1975-2000-full.csv", ',', true);
  start_prompter(db);
  db.plandata.write_csv("out.csv", ';', true);

  return 0;
}
