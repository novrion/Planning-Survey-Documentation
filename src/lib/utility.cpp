#include "utility.h"

namespace plan_database {

int get_mkt_id(const std::string &industry) {
  if (industry == "B") {
    return CONSTR;
  } else if (industry == "S") {
    return IMED;
  } else if (industry == "R") {
    return RAW;
  } else if (industry == "K") {
    return NDUR;
  } else if (industry == "V") {
    return DUR;
  } else {
    throw std::runtime_error("ERROR: Invalid industry");
  }
}

std::string get_industry_name(const int mkt_id) {
  if (mkt_id == DUR)
    return "Investment Goods";
  else if (mkt_id == NDUR)
    return "Consumption Goods";
  else if (mkt_id == RAW)
    return "Raw Material Processing";
  else if (mkt_id == IMED)
    return "Intermediate Goods";
  else if (mkt_id == NO_MKT)
    return "Synthetic Firm";

  return "NA";
}

} // namespace plan_database
