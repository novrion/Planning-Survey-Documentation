#ifndef DB_H
#define DB_H

#include "macro.h"
#include "plandata.h"
#include "utility.h"

namespace plan_database {

class Database {
public:
  PlanData plandata;
  MacroData macrodata;
};

} // namespace plan_database

#endif
