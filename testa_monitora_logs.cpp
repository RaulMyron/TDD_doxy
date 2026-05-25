// Copyright 2026 UnB - Tecnicas de Programacao 2

/**
 * @file testa_monitora_logs.cpp
 * @brief Modulo de testes da biblioteca monitora_logs.
 *
 * Os testes seguem a metodologia TDD com o framework Catch2 v2.13.10.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "monitora_logs.hpp"

// ----------------------------------------------------------------------------
// T1 - ParseLogLine: aceita uma linha valida completa.
// Tabela de decisao (caixa fechada): coluna R4 da sub-tabela ParseLogLine
// (todas as condicoes verdadeiras -> retorna LogEntry valido).
// ----------------------------------------------------------------------------
TEST_CASE("ParseLogLine_AceitaLinhaCompleta", "[parse][black-box]") {
  LogEntry e = parse_log_line("16/1/2026 13:27:46 Este e um exemplo de log");

  REQUIRE(e.valid == true);
  REQUIRE(e.day == 16);
  REQUIRE(e.month == 1);
  REQUIRE(e.year == 2026);
  REQUIRE(e.hour == 13);
  REQUIRE(e.minute == 27);
  REQUIRE(e.second == 46);
  REQUIRE(e.message == "Este e um exemplo de log");
}
