#ifndef CSV_CPP_H
#define CSV_CPP_H

#include <codecvt>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define EMPTY "NA"
#define EMPTY_NUM -1e9

namespace csv {

const std::string EMPTY_STRINGS[] = {" ", "", "na", "NA", "Na", "nA"};

std::vector<std::vector<std::string>> parse(const std::string &path,
                                            const char separator = ',');
void write(const std::string &path,
           const std::vector<std::vector<std::string>> &rows,
           const char separator = ',',
           const std::vector<std::string> &header = {});

std::vector<std::string> tokenise(const std::string &line,
                                  const char separator);

std::string ftostr(const float f);
std::string dtostr(const double d);
bool empty(const std::string &str);
std::string trim(const std::string &str);
std::string to_uppercase_utf8(const std::string &str);
std::string to_uppercase_simple(const std::string &str);

} // namespace csv

#endif // CSV_CPP_H
