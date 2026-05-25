// Copyright 2026 UnB - Tecnicas de Programacao 2

/**
 * @file monitora_logs.cpp
 * @brief Implementacao do sistema de monitoramento de logs.
 */

#include "monitora_logs.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr char kDateSep = '/';
constexpr char kTimeSep = ':';
constexpr char kPathSeps[] = "/\\";
constexpr int kMinDay = 1;
constexpr int kMaxDay = 31;
constexpr int kMinMonth = 1;
constexpr int kMaxMonth = 12;
constexpr int kMinYear = 1;
constexpr int kMinHour = 0;
constexpr int kMaxHour = 23;
constexpr int kMinMinSec = 0;
constexpr int kMaxMinSec = 59;
constexpr size_t kMinMsgLen = 1;
constexpr size_t kMaxMsgLen = 100;

bool is_valid_date(int day, int month, int year) {
  if (day < kMinDay || day > kMaxDay) return false;
  if (month < kMinMonth || month > kMaxMonth) return false;
  if (year < kMinYear) return false;
  return true;
}

bool is_valid_time(int hour, int minute, int second) {
  if (hour < kMinHour || hour > kMaxHour) return false;
  if (minute < kMinMinSec || minute > kMaxMinSec) return false;
  if (second < kMinMinSec || second > kMaxMinSec) return false;
  return true;
}

bool is_valid_message(const std::string& msg) {
  return msg.size() >= kMinMsgLen && msg.size() <= kMaxMsgLen;
}

bool read_date_and_time(std::istringstream* iss, LogEntry* entry) {
  char sep1 = 0, sep2 = 0, sep3 = 0, sep4 = 0;
  (*iss) >> entry->day >> sep1 >> entry->month >> sep2 >> entry->year >>
      entry->hour >> sep3 >> entry->minute >> sep4 >> entry->second;
  if (iss->fail()) return false;
  if (sep1 != kDateSep || sep2 != kDateSep || sep3 != kTimeSep ||
      sep4 != kTimeSep)
    return false;
  if (!is_valid_date(entry->day, entry->month, entry->year)) return false;
  if (!is_valid_time(entry->hour, entry->minute, entry->second)) return false;
  return true;
}

}  // namespace

/**
 * @brief Converte uma string de log em uma estrutura LogEntry.
 */
LogEntry parse_log_line(const std::string& line) {
  LogEntry entry;
  entry.valid = false;
  std::istringstream iss(line);
  if (!read_date_and_time(&iss, &entry)) return entry;
  std::string rest;
  std::getline(iss, rest);
  if (!rest.empty() && rest[0] == ' ') rest = rest.substr(1);
  if (!is_valid_message(rest)) return entry;
  entry.message = rest;
  entry.valid = true;
  return entry;
}

/**
 * @brief Compara dois LogEntry cronologicamente.
 */
int compare_log_entries(const LogEntry& a, const LogEntry& b) {
  if (a.year != b.year) return a.year - b.year;
  if (a.month != b.month) return a.month - b.month;
  if (a.day != b.day) return a.day - b.day;
  if (a.hour != b.hour) return a.hour - b.hour;
  if (a.minute != b.minute) return a.minute - b.minute;
  return a.second - b.second;
}

/**
 * @brief Le um arquivo de log e extrai suas linhas validas.
 */
bool read_log_file(const std::string& path, std::vector<LogEntry>* entries) {
  if (entries != nullptr) {
    entries->clear();
  }

  std::ifstream file(path);
  if (!file.is_open()) {
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty()) continue;
    LogEntry entry = parse_log_line(line);
    if (entry.valid && entries != nullptr) {
      entries->push_back(entry);
    }
  }

  return true;
}

/**
 * @brief Escreve um vetor de logs validos em um arquivo de texto.
 */
bool write_log_file(const std::string& path,
                    const std::vector<LogEntry>& entries) {
  std::ofstream file(path);
  if (!file.is_open()) {
    return false;
  }

  for (const auto& entry : entries) {
    if (!entry.valid) continue;
    file << entry.day << kDateSep << entry.month << kDateSep << entry.year
         << " " << entry.hour << kTimeSep << entry.minute << kTimeSep
         << entry.second << " " << entry.message << "\n";
  }

  return true;
}

/**
 * @brief Realiza o merge de duas listas de log mantendo a ordem cronologica.
 */
std::vector<LogEntry> merge_entries(const std::vector<LogEntry>& a,
                                    const std::vector<LogEntry>& b) {
  if (a.empty()) return b;
  if (b.empty()) return a;

  std::vector<LogEntry> result;
  result.reserve(a.size() + b.size());

  size_t i = 0, j = 0;
  while (i < a.size() && j < b.size()) {
    if (compare_log_entries(a[i], b[j]) <= 0) {
      result.push_back(a[i]);
      i++;
    } else {
      result.push_back(b[j]);
      j++;
    }
  }

  while (i < a.size()) {
    result.push_back(a[i]);
    i++;
  }
  while (j < b.size()) {
    result.push_back(b[j]);
    j++;
  }

  return result;
}

/**
 * @brief Cria o nome do arquivo total adicionando o prefixo "total_".
 */
std::string make_total_filename(const std::string& source_path) {
  size_t last_slash = source_path.find_last_of(kPathSeps);
  if (last_slash == std::string::npos) {
    return "total_" + source_path;
  }
  return "total_" + source_path.substr(last_slash + 1);
}

/**
 * @brief Processa uma lista de arquivos de log descrita em um arquivo texto.
 */
int process_log_list(const std::string& logs_txt_path) {
  std::ifstream file(logs_txt_path);
  if (!file.is_open()) {
    return -1;
  }

  int processed_count = 0;
  std::string log_file_path;
  while (std::getline(file, log_file_path)) {
    if (log_file_path.empty()) continue;

    std::vector<LogEntry> new_entries;
    if (!read_log_file(log_file_path, &new_entries)) {
      continue;
    }

    std::string total_filename = make_total_filename(log_file_path);
    std::vector<LogEntry> current_total_entries;

    std::ifstream check_file(total_filename);
    bool total_existed = check_file.is_open();
    check_file.close();

    if (total_existed) {
      read_log_file(total_filename, &current_total_entries);
    }

    if (new_entries.empty() && !total_existed) {
      processed_count++;
      continue;
    }

    std::sort(new_entries.begin(), new_entries.end(),
              [](const LogEntry& x, const LogEntry& y) {
                return compare_log_entries(x, y) < 0;
              });

    std::vector<LogEntry> merged =
        merge_entries(current_total_entries, new_entries);
    write_log_file(total_filename, merged);

    processed_count++;
  }

  return processed_count;
}
