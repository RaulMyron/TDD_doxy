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
// Sub-tabela de decisao ParseLogLine, coluna R4
// (data ok, hora ok, mensagem ok -> retorna LogEntry valido).
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

// ----------------------------------------------------------------------------
// T2 - ParseLogLine: rejeita linhas com data invalida.
// Sub-tabela de decisao ParseLogLine, coluna R1
// (data invalida -> retorna LogEntry invalido).
// ----------------------------------------------------------------------------
TEST_CASE("ParseLogLine_RejeitaDataMalFormatada", "[parse][black-box]") {
  SECTION("dia zero") {
    REQUIRE(parse_log_line("0/1/2026 13:27:46 msg").valid == false);
  }
  SECTION("dia maior que 31") {
    REQUIRE(parse_log_line("32/1/2026 13:27:46 msg").valid == false);
  }
  SECTION("mes zero") {
    REQUIRE(parse_log_line("1/0/2026 13:27:46 msg").valid == false);
  }
  SECTION("mes maior que 12") {
    REQUIRE(parse_log_line("1/13/2026 13:27:46 msg").valid == false);
  }
  SECTION("ano zero") {
    REQUIRE(parse_log_line("1/1/0 13:27:46 msg").valid == false);
  }
  SECTION("data nao numerica") {
    REQUIRE(parse_log_line("abc/1/2026 13:27:46 msg").valid == false);
  }
}

// ----------------------------------------------------------------------------
// T3 - ParseLogLine: rejeita linhas com hora invalida.
// Sub-tabela de decisao ParseLogLine, coluna R2.
// ----------------------------------------------------------------------------
TEST_CASE("ParseLogLine_RejeitaHoraMalFormatada", "[parse][black-box]") {
  SECTION("hora 24") {
    REQUIRE(parse_log_line("1/1/2026 24:00:00 msg").valid == false);
  }
  SECTION("hora negativa") {
    REQUIRE(parse_log_line("1/1/2026 -1:00:00 msg").valid == false);
  }
  SECTION("minuto 60") {
    REQUIRE(parse_log_line("1/1/2026 12:60:00 msg").valid == false);
  }
  SECTION("segundo 60") {
    REQUIRE(parse_log_line("1/1/2026 12:00:60 msg").valid == false);
  }
}

// ----------------------------------------------------------------------------
// T4 - ParseLogLine: rejeita mensagem vazia ou maior que 100 caracteres.
// Sub-tabela de decisao ParseLogLine, coluna R3.
// ----------------------------------------------------------------------------
TEST_CASE("ParseLogLine_RejeitaMensagemVaziaOuLonga", "[parse][black-box]") {
  SECTION("mensagem ausente") {
    REQUIRE(parse_log_line("1/1/2026 12:00:00").valid == false);
  }
  SECTION("mensagem vazia apos espaco") {
    REQUIRE(parse_log_line("1/1/2026 12:00:00 ").valid == false);
  }
  SECTION("mensagem com 101 caracteres") {
    std::string longa(101, 'x');
    REQUIRE(parse_log_line("1/1/2026 12:00:00 " + longa).valid == false);
  }
  SECTION("boundary: mensagem com 100 caracteres aceita") {
    std::string ok(100, 'x');
    REQUIRE(parse_log_line("1/1/2026 12:00:00 " + ok).valid == true);
  }
  SECTION("boundary: mensagem com 1 caractere aceita") {
    REQUIRE(parse_log_line("1/1/2026 12:00:00 a").valid == true);
  }
}

// ----------------------------------------------------------------------------
// T5 - compare_log_entries: ordena cronologicamente.
// ----------------------------------------------------------------------------
TEST_CASE("CompareLogEntries_OrdenaCronologicamente", "[compare][black-box]") {
  LogEntry a = parse_log_line("16/1/2026 13:27:46 msg a");
  LogEntry b = parse_log_line("20/1/2026 17:45:38 msg b");
  LogEntry c = parse_log_line("16/1/2026 13:27:46 msg c");

  REQUIRE(compare_log_entries(a, b) < 0);
  REQUIRE(compare_log_entries(b, a) > 0);
  REQUIRE(compare_log_entries(a, c) == 0);
}

TEST_CASE("T6 RED: MakeTotalFilename_ExtraiNomeBaseEPrefixa", "[make_total_filename]") {
    // Caminhos do tipo Windows e Unix para extração do nome base
    SECTION("Caminho absoluto com barras invertidas (Windows)") {
        REQUIRE(make_total_filename("c:\\logs\\log1.txt") == "total_log1.txt");
    }
    SECTION("Caminho absoluto com barras normais (Unix)") {
        REQUIRE(make_total_filename("/var/log/syslog.log") == "total_syslog.log");
    }
    SECTION("Apenas o nome do arquivo sem diretorio") {
        REQUIRE(make_total_filename("meulog.txt") == "total_meulog.txt");
    }
    SECTION("Caminho com multiplos niveis de diretorio") {
        REQUIRE(make_total_filename("f:\\backup\\2026\\logs\\log_backup.txt") == "total_log_backup.txt");
    }
}

TEST_CASE("T7 RED: MergeEntries_ListasVaziasRetornaVazio", "[merge_entries]") {
    std::vector<LogEntry> lista_a;
    std::vector<LogEntry> lista_b;
    
    std::vector<LogEntry> resultado = merge_entries(lista_a, lista_b);
    
    // Forçamos a falha no RED alterando temporariamente o stub se necessário, 
    // ou validando que o comportamento esperado ainda não está devidamente mapeado.
    REQUIRE(resultado.empty() == true);
}
