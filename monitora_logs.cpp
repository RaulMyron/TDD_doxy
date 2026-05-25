// Copyright 2026 UnB - Tecnicas de Programacao 2

/**
 * @file monitora_logs.cpp
 * @brief Implementacao do sistema de monitoramento de logs.
 */

#include "monitora_logs.hpp"

#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr char kDateSep = '/';
constexpr char kTimeSep = ':';
constexpr int kMinDay = 1;
constexpr int kMaxDay = 31;
constexpr int kMinMonth = 1;
constexpr int kMaxMonth = 12;
constexpr int kMinYear = 1;
constexpr size_t kMinMsgLen = 1;
constexpr size_t kMaxMsgLen = 100;

/**
 * @brief Verifica se a mensagem tem entre 1 e 100 caracteres (inclusive).
 */
bool is_valid_message(const std::string& msg) {
  return msg.size() >= kMinMsgLen && msg.size() <= kMaxMsgLen;
}

/**
 * @brief Verifica se uma data (dia, mes, ano) esta dentro dos intervalos
 *        validos da especificacao.
 */
bool is_valid_date(int day, int month, int year) {
  if (day < kMinDay || day > kMaxDay) {
    return false;
  }
  if (month < kMinMonth || month > kMaxMonth) {
    return false;
  }
  if (year < kMinYear) {
    return false;
  }
  return true;
}

constexpr int kMinHour = 0;
constexpr int kMaxHour = 23;
constexpr int kMinMinSec = 0;
constexpr int kMaxMinSec = 59;

/**
 * @brief Verifica se hora/minuto/segundo estao em intervalos validos.
 */
bool is_valid_time(int hour, int minute, int second) {
  if (hour < kMinHour || hour > kMaxHour) return false;
  if (minute < kMinMinSec || minute > kMaxMinSec) return false;
  if (second < kMinMinSec || second > kMaxMinSec) return false;
  return true;
}

/**
 * @brief Le os 6 campos numericos de data e hora e valida a data.
 *
 * Espera o formato "D/M/AAAA H:MM:SS". Retorna false se a leitura
 * falhar, os separadores nao corresponderem, ou a data for invalida.
 */
bool read_date_and_time(std::istringstream* iss, LogEntry* entry) {
  char sep1 = 0;
  char sep2 = 0;
  char sep3 = 0;
  char sep4 = 0;

  (*iss) >> entry->day >> sep1 >> entry->month >> sep2 >> entry->year >>
      entry->hour >> sep3 >> entry->minute >> sep4 >> entry->second;

  if (iss->fail()) {
    return false;
  }
  if (sep1 != kDateSep || sep2 != kDateSep || sep3 != kTimeSep ||
      sep4 != kTimeSep) {
    return false;
  }
  if (!is_valid_date(entry->day, entry->month, entry->year)) {
    return false;
  }
  // Validacao de intervalos de hora.
  if (!is_valid_time(entry->hour, entry->minute, entry->second)) {
    return false;
  }
  return true;
}

}  // namespace

LogEntry parse_log_line(const std::string& line) {
  LogEntry entry;
  entry.valid = false;

  std::istringstream iss(line);
  if (!read_date_and_time(&iss, &entry)) {
    return entry;
  }

  std::string rest;
  std::getline(iss, rest);
  if (!rest.empty() && rest[0] == ' ') {
    rest = rest.substr(1);
  }

  if (!is_valid_message(rest)) {
    return entry;
  }

  entry.message = rest;
  entry.valid = true;
  return entry;
}

int compare_log_entries(const LogEntry& a, const LogEntry& b) {
  // Comparacao lexicografica por campo: ano > mes > dia > hora > min > seg.
  if (a.year != b.year) return a.year - b.year;
  if (a.month != b.month) return a.month - b.month;
  if (a.day != b.day) return a.day - b.day;
  if (a.hour != b.hour) return a.hour - b.hour;
  if (a.minute != b.minute) return a.minute - b.minute;
  return a.second - b.second;
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
