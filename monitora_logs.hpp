// Copyright 2026 UnB - Tecnicas de Programacao 2

/**
 * @file monitora_logs.hpp
 * @brief Biblioteca de monitoramento e merge de arquivos de log.
 *
 * Este modulo implementa o sistema de monitoramento de logs via TDD.
 */

#ifndef MONITORA_LOGS_HPP_
#define MONITORA_LOGS_HPP_

#include <string>
#include <vector>

/**
 * @brief Representa um registro individual de log.
 */
struct LogEntry {
  int day;             /**< Dia do mes, 1-31. */
  int month;           /**< Mes, 1-12. */
  int year;            /**< Ano com 4 digitos. */
  int hour;            /**< Hora, 0-23. */
  int minute;          /**< Minuto, 0-59. */
  int second;          /**< Segundo, 0-59. */
  std::string message; /**< Mensagem do log (1 a 100 caracteres). */
  bool valid;          /**< Indica se o registro foi parseado com sucesso. */
};

/**
 * @brief Converte uma string de log em uma estrutura LogEntry.
 * @param line A string contendo a linha do log a ser parseada.
 * @return Um LogEntry com os dados extraidos e o campo valid preenchido.
 */
LogEntry parse_log_line(const std::string& line);

/**
 * @brief Compara dois LogEntry cronologicamente.
 * @param a O primeiro LogEntry.
 * @param b O segundo LogEntry.
 * @return Menor que 0 se a < b, 0 se iguais, maior que 0 se a > b.
 */
int compare_log_entries(const LogEntry& a, const LogEntry& b);

/**
 * @brief Le um arquivo de log e extrai suas linhas validas.
 * @param path Caminho para o arquivo a ser lido no disco.
 * @param entries Ponteiro para o vetor onde os logs serao armazenados.
 * @return true se o arquivo foi aberto e lido com sucesso.
 */
bool read_log_file(const std::string& path, std::vector<LogEntry>* entries);

/**
 * @brief Escreve um vetor de logs validos em um arquivo de texto.
 * @param path Caminho do arquivo de destino.
 * @param entries Vetor contendo os registros de log a serem escritos.
 * @return true se a escrita for bem sucedida, false se falhar ao abrir.
 */
bool write_log_file(const std::string& path,
                    const std::vector<LogEntry>& entries);

/**
 * @brief Realiza o merge de duas listas de log mantendo a ordem cronologica.
 * @param a Primeira lista de logs.
 * @param b Segunda lista de logs.
 * @return Um novo vetor contendo a mescla ordenada de a e b.
 */
std::vector<LogEntry> merge_entries(const std::vector<LogEntry>& a,
                                    const std::vector<LogEntry>& b);

/**
 * @brief Cria o nome do arquivo total adicionando o prefixo "total_".
 * @param source_path Caminho original do arquivo de log.
 * @return O nome do arquivo modificado com o prefixo (sem os diretorios).
 */
std::string make_total_filename(const std::string& source_path);

/**
 * @brief Processa uma lista de arquivos de log.
 * @param logs_txt_path Caminho para o arquivo de configuracao principal.
 * @return Numero de arquivos processados, ou -1 em caso de falha.
 */
int process_log_list(const std::string& logs_txt_path);

#endif  // MONITORA_LOGS_HPP_
