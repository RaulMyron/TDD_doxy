// Copyright 2026 UnB - Tecnicas de Programacao 2

/**
 * @file monitora_logs.hpp
 * @brief Biblioteca de monitoramento e merge de arquivos de log.
 *
 * Este modulo implementa o sistema de monitoramento de logs descrito no
 * Trabalho 2 da disciplina CIC0198 (Tecnicas de Programacao 2). Le um
 * arquivo "logs.txt" contendo uma lista de caminhos para arquivos de log,
 * verifica a existencia de cada um, e, para cada arquivo encontrado, faz
 * o merge ordenado por data/hora dos registros em um arquivo
 * "total_<nome>.txt" correspondente.
 *
 * O desenvolvimento segue a metodologia TDD (Test-Driven Development)
 * usando o framework Catch2.
 */

#ifndef MONITORA_LOGS_HPP_
#define MONITORA_LOGS_HPP_

#include <string>
#include <vector>

/**
 * @brief Representa um registro individual de log.
 *
 * Cada registro contem data (dia/mes/ano), hora (hora:minuto:segundo) e
 * uma mensagem com no maximo 100 caracteres. O campo @c valid indica se
 * o registro foi parseado com sucesso.
 */
struct LogEntry {
  int day;              ///< Dia do mes, 1-31.
  int month;            ///< Mes, 1-12.
  int year;             ///< Ano com 4 digitos.
  int hour;             ///< Hora, 0-23.
  int minute;           ///< Minuto, 0-59.
  int second;           ///< Segundo, 0-59.
  std::string message;  ///< Mensagem do log (1-100 caracteres).
  bool valid;           ///< true se o registro foi parseado com sucesso.
};

/***************************************************************************
 * Funcao: parse_log_line
 * Descricao
 *   Parseia uma linha textual de log no formato "D/M/AAAA H:MM:SS msg"
 *   em uma estrutura LogEntry. Linhas mal formatadas (data invalida,
 *   hora invalida, mensagem ausente ou mensagem com mais de 100
 *   caracteres) retornam uma LogEntry com campo @c valid igual a false.
 * Parametros
 *   line  - linha de texto a ser parseada.
 * Valor retornado
 *   Estrutura LogEntry. O campo @c valid indica sucesso.
 * Assertiva de entrada
 *   line: string qualquer (pode ser vazia).
 * Assertiva de saida
 *   Se retorno.valid == true:
 *     1 <= retorno.day <= 31
 *     1 <= retorno.month <= 12
 *     0 <= retorno.hour <= 23
 *     0 <= retorno.minute <= 59
 *     0 <= retorno.second <= 59
 *     1 <= retorno.message.size() <= 100
 *   Se retorno.valid == false:
 *     campos numericos indefinidos; message indefinida.
 ***************************************************************************/
LogEntry parse_log_line(const std::string& line);

/***************************************************************************
 * Funcao: compare_log_entries
 * Descricao
 *   Compara duas LogEntries pela data/hora (ano, mes, dia, hora, minuto,
 *   segundo nessa ordem). Utilizada para ordenacao crescente do log.
 * Parametros
 *   a - primeira entrada.
 *   b - segunda entrada.
 * Valor retornado
 *   Inteiro negativo se a < b, zero se a == b, positivo se a > b.
 * Assertiva de entrada
 *   a.valid == true e b.valid == true.
 * Assertiva de saida
 *   retorno < 0  <=> a e cronologicamente anterior a b.
 *   retorno == 0 <=> a e b possuem mesma data/hora.
 *   retorno > 0  <=> a e cronologicamente posterior a b.
 ***************************************************************************/
int compare_log_entries(const LogEntry& a, const LogEntry& b);

/***************************************************************************
 * Funcao: read_log_file
 * Descricao
 *   Le um arquivo de log do disco, parseando cada linha em uma LogEntry.
 *   Linhas mal formatadas sao descartadas silenciosamente. Se o arquivo
 *   nao existir, retorna um vetor vazio e false.
 * Parametros
 *   path     - caminho do arquivo a ser lido.
 *   entries  - ponteiro para vetor de saida (sera limpo antes de
 *              preenchido).
 * Valor retornado
 *   true se o arquivo existe e pode ser lido; false caso contrario.
 * Assertiva de entrada
 *   path != "".
 *   entries != nullptr.
 * Assertiva de saida
 *   Para todo e em *entries: e.valid == true.
 *   Se retorno == false: entries->empty() == true.
 ***************************************************************************/
bool read_log_file(const std::string& path, std::vector<LogEntry>* entries);

/***************************************************************************
 * Funcao: write_log_file
 * Descricao
 *   Escreve um vetor de LogEntries em um arquivo, uma entrada por linha,
 *   no formato "D/M/AAAA H:MM:SS msg". Sobrescreve o arquivo se ele ja
 *   existir.
 * Parametros
 *   path     - caminho do arquivo de saida.
 *   entries  - vetor de entradas a serem escritas.
 * Valor retornado
 *   true se a escrita foi bem sucedida; false em caso de erro de IO.
 * Assertiva de entrada
 *   path != "".
 *   Para toda e em entries: e.valid == true.
 * Assertiva de saida
 *   Se retorno == true: arquivo em path contem entries.size() linhas.
 ***************************************************************************/
bool write_log_file(const std::string& path,
                    const std::vector<LogEntry>& entries);

/***************************************************************************
 * Funcao: merge_entries
 * Descricao
 *   Une duas listas de LogEntries em uma unica lista ordenada
 *   cronologicamente (do mais antigo para o mais recente). Nao remove
 *   duplicatas: entradas com mesma data/hora aparecem ambas no
 *   resultado.
 * Parametros
 *   a - primeira lista.
 *   b - segunda lista.
 * Valor retornado
 *   Vetor ordenado contendo todas as entradas de a e b.
 * Assertiva de entrada
 *   Para toda e em a e em b: e.valid == true.
 * Assertiva de saida
 *   retorno.size() == a.size() + b.size().
 *   Para todo i em [0, retorno.size()-2]:
 *     compare_log_entries(retorno[i], retorno[i+1]) <= 0.
 ***************************************************************************/
std::vector<LogEntry> merge_entries(const std::vector<LogEntry>& a,
                                    const std::vector<LogEntry>& b);

/***************************************************************************
 * Funcao: make_total_filename
 * Descricao
 *   Dado o caminho de um arquivo de log (ex. "c:\\logs\\log1.txt" ou
 *   "/home/user/log1.txt"), retorna o nome do arquivo total
 *   correspondente prefixado por "total_" (ex. "total_log1.txt").
 *   O caminho do diretorio e descartado: o arquivo total e sempre
 *   gravado no diretorio corrente.
 * Parametros
 *   source_path - caminho original do arquivo de log.
 * Valor retornado
 *   String com o nome do arquivo total.
 * Assertiva de entrada
 *   source_path != "".
 * Assertiva de saida
 *   retorno comeca com "total_".
 *   retorno nao contem '/' nem '\\'.
 ***************************************************************************/
std::string make_total_filename(const std::string& source_path);

/***************************************************************************
 * Funcao: process_log_list
 * Descricao
 *   Funcao principal do modulo. Le o arquivo logs.txt indicado, e para
 *   cada caminho listado:
 *     1) verifica se o arquivo existe;
 *     2) le suas entradas validas;
 *     3) le total_<nome>.txt se existir;
 *     4) faz o merge ordenado;
 *     5) escreve o resultado em total_<nome>.txt.
 *   Arquivos listados mas inexistentes sao pulados silenciosamente.
 * Parametros
 *   logs_txt_path - caminho para o arquivo de lista (logs.txt).
 * Valor retornado
 *    0 em caso de sucesso (mesmo que nenhum arquivo tenha sido
 *      processado por nao existirem);
 *   -1 se logs_txt_path nao puder ser aberto.
 * Assertiva de entrada
 *   logs_txt_path != "".
 * Assertiva de saida
 *   Para cada arquivo X listado em logs.txt que existe e contem
 *   entradas validas, o arquivo total_<nome(X)>.txt existira no
 *   diretorio corrente e contera as entradas previas (se havia) somadas
 *   as novas, ordenadas cronologicamente.
 ***************************************************************************/
int process_log_list(const std::string& logs_txt_path);

#endif  // MONITORA_LOGS_HPP_
