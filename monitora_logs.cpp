// Copyright 2026 UnB - Tecnicas de Programacao 2

/**
 * @file monitora_logs.cpp
 * @brief Implementacao do sistema de monitoramento de logs.
 */

#include "monitora_logs.hpp"

#include <sstream>
#include <string>
#include <vector>

LogEntry parse_log_line(const std::string& line) {
  LogEntry entry;
  entry.valid = false;

  std::istringstream iss(line);
  char sep1 = 0;
  char sep2 = 0;
  char sep3 = 0;
  char sep4 = 0;

  iss >> entry.day >> sep1 >> entry.month >> sep2 >> entry.year >> entry.hour >>
      sep3 >> entry.minute >> sep4 >> entry.second;

  if (iss.fail()) {
    return entry;
  }
  if (sep1 != '/' || sep2 != '/' || sep3 != ':' || sep4 != ':') {
    return entry;
  }

  // Le o resto da linha (mensagem). getline consome o espaco separador.
  std::string rest;
  std::getline(iss, rest);
  if (!rest.empty() && rest[0] == ' ') {
    rest = rest.substr(1);
  }
  entry.message = rest;
  entry.valid = true;

  return entry;
}

int compare_log_entries(const LogEntry& /*a*/, const LogEntry& /*b*/) {
  return 0;
}

bool read_log_file(const std::string& /*path*/,
                   std::vector<LogEntry>* entries) {
  if (entries != nullptr) {
    entries->clear();
  }
  return false;
}

bool write_log_file(const std::string& /*path*/,
                    const std::vector<LogEntry>& /*entries*/) {
  return false;
}

std::vector<LogEntry> merge_entries(const std::vector<LogEntry>& /*a*/,
                                    const std::vector<LogEntry>& /*b*/) {
  return std::vector<LogEntry>();
}

std::string make_total_filename(const std::string& /*source_path*/) {
  return std::string();
}

int process_log_list(const std::string& /*logs_txt_path*/) { return -1; }