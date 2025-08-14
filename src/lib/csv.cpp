#include "csv.h"

namespace csv {

std::string ftostr(const float f) {
  std::stringstream ss;
  ss << f;
  return ss.str();
}

std::string dtostr(const double d) {
  std::stringstream ss;
  ss << d;
  return ss.str();
}

bool empty(const std::string &str) {
  for (const std::string &empty_str : EMPTY_STRINGS) {
    if (str == empty_str)
      return true;
  }
  return false;
}

std::string trim(const std::string &str) {
  size_t first = str.find_first_not_of(" \t\r\n");
  if (first == std::string::npos)
    return "";
  size_t last = str.find_last_not_of(" \t\r\n");
  return str.substr(first, (last - first + 1));
}

std::string to_uppercase_utf8(const std::string &str) {
  try {
    // Convert UTF-8 to wide string
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wide_str = converter.from_bytes(str);

    // Convert to uppercase using locale's ctype facet
    std::locale loc("");
    const std::ctype<wchar_t> &ct = std::use_facet<std::ctype<wchar_t>>(loc);
    ct.toupper(&wide_str[0], &wide_str[0] + wide_str.size());

    // Convert back to UTF-8
    return converter.to_bytes(wide_str);
  } catch (const std::exception &e) {
    std::cerr << "Warning: UTF-8 conversion failed for '" << str
              << "', using original" << std::endl;
    // Fallback to simple ASCII uppercase
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
  }
}

std::string to_uppercase_simple(const std::string &str) {
  std::string result = str;
  std::locale loc("");
  for (size_t i = 0; i < result.length(); ++i) {
    result[i] = std::toupper(result[i], loc);
  }
  return result;
}

std::vector<std::string> tokenise(const std::string &line,
                                  const char separator) {
  std::vector<std::string> tokens;
  std::istringstream ss(line);
  std::string token;

  while (std::getline(ss, token, separator)) {
    tokens.push_back(token);
  }

  // If the line ends with a delimiter, add an empty token
  if (!line.empty() && line.back() == separator) {
    tokens.push_back("");
  }

  return tokens;
}

std::vector<std::vector<std::string>> parse(const std::string &path,
                                            const char separator) {
  std::vector<std::vector<std::string>> parsed;

  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("ERROR: Could not open file: " + path);
  }

  // Remove header
  std::string line;
  std::getline(file, line);

  // Get lines
  std::vector<std::string> lines;
  while (std::getline(file, line)) {
    if (!line.empty()) {
      lines.push_back(line);
    }
  }

  // Add each tokenised line to parsed
  for (const auto &line : lines) {
    std::vector<std::string> tokens = tokenise(line, separator);
    parsed.push_back(tokens);
  }

  file.close();
  if (file.is_open()) {
    throw std::runtime_error("ERROR: Could not close file: " + path);
  }

  return parsed;
}

void write(const std::string &path,
           const std::vector<std::vector<std::string>> &rows,
           const char separator, const std::vector<std::string> &header) {
  std::ofstream file(path);
  if (!file.is_open())
    throw std::runtime_error(
        "ERROR: Could not open file to write csv content to: " + path);

  // Header
  if (!header.empty()) {
    file << header[0];
    if (header.size() > 1) {
      for (int i = 1; i < header.size(); i++) {
        file << separator << header[i];
      }
    }
  }
  file << std::endl;

  // Rows
  for (const std::vector<std::string> &row : rows) {
    file << row[0];
    if (row.size() > 1) {
      for (int i = 1; i < row.size(); i++) {
        file << separator << row[i];
      }
    }
    file << std::endl;
  }

  file.close();
  if (file.is_open())
    throw std::runtime_error(
        "ERROR: Could not close file to write csv content to: " + path);
}

} // namespace csv
