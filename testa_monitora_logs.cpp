// Copyright 2026 UnB - Tecnicas de Programacao 2

/**
 * @file testa_monitora_logs.cpp
 * @brief Testes unitarios para o sistema de monitoramento de logs.
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "monitora_logs.hpp"

TEST_CASE("ParseLogLine_AceitaLinhaCompleta", "[parse_log_line]") {
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

TEST_CASE("ParseLogLine_RejeitaDataMalFormatada", "[parse_log_line]") {
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

TEST_CASE("ParseLogLine_RejeitaHoraMalFormatada", "[parse_log_line]") {
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

TEST_CASE("ParseLogLine_RejeitaMensagemVaziaOuLonga", "[parse_log_line]") {
  std::string longa(101, 'a');
  std::string ok(100, 'a');

  SECTION("mensagem ausente") {
    REQUIRE(parse_log_line("1/1/2026 12:00:00").valid == false);
  }
  SECTION("mensagem vazia apos espaco") {
    REQUIRE(parse_log_line("1/1/2026 12:00:00 ").valid == false);
  }
  SECTION("mensagem com 101 caracteres") {
    REQUIRE(parse_log_line("1/1/2026 12:00:00 " + longa).valid == false);
  }
  SECTION("boundary: mensagem com 100 caracteres aceita") {
    REQUIRE(parse_log_line("1/1/2026 12:00:00 " + ok).valid == true);
  }
  SECTION("boundary: mensagem com 1 caractere aceita") {
    REQUIRE(parse_log_line("1/1/2026 12:00:00 a").valid == true);
  }
}

TEST_CASE("CompareLogEntries_OrdenaCronologicamente", "[compare_log_entries]") {
  LogEntry a{16, 1, 2026, 13, 27, 46, "msg", true};
  LogEntry b{20, 1, 2026, 17, 45, 38, "msg", true};
  LogEntry c{16, 1, 2026, 13, 27, 46, "outra msg", true};

  REQUIRE(compare_log_entries(a, b) < 0);
  REQUIRE(compare_log_entries(b, a) > 0);
  REQUIRE(compare_log_entries(a, c) == 0);
}

TEST_CASE("MakeTotalFilename_ExtraiNomeBaseEPrefixa", "[make_total_filename]") {
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
    REQUIRE(make_total_filename("f:\\backup\\2026\\logs\\log_backup.txt") ==
            "total_log_backup.txt");
  }
}

TEST_CASE("T7 RED: MergeEntries_ListasVaziasRetornaVazio", "[merge_entries]") {
  std::vector<LogEntry> lista_a;
  std::vector<LogEntry> lista_b;

  std::vector<LogEntry> resultado = merge_entries(lista_a, lista_b);

  REQUIRE(resultado.empty() == true);
}

TEST_CASE("T8 RED: MergeEntries_UmaListaVaziaRetornaOutraOrdenada", "[merge_entries]") {
  std::vector<LogEntry> lista_vazia;
  std::vector<LogEntry> lista_com_elementos = {
    {16, 1, 2026, 13, 27, 46, "Log A", true},
    {20, 1, 2026, 17, 45, 38, "Log B", true}
  };

  SECTION("Lista A vazia e Lista B populada") {
    std::vector<LogEntry> resultado = merge_entries(lista_vazia, lista_com_elementos);
    REQUIRE(resultado.size() == 2);
    REQUIRE(resultado[0].message == "Log A");
  }

  SECTION("Lista A populada e Lista B vazia") {
    std::vector<LogEntry> resultado = merge_entries(lista_com_elementos, lista_vazia);
    REQUIRE(resultado.size() == 2);
    REQUIRE(resultado[1].message == "Log B");
  }
}
