// Copyright 2026 UnB - Tecnicas de Programacao 2

/**
 * @file testa_monitora_logs.cpp
 * @brief Testes unitarios para o sistema de monitoramento de logs.
 */

#define CATCH_CONFIG_MAIN
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

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

TEST_CASE("T7: MergeEntries_ListasVaziasRetornaVazio", "[merge_entries]") {
  std::vector<LogEntry> lista_a;
  std::vector<LogEntry> lista_b;
  std::vector<LogEntry> resultado = merge_entries(lista_a, lista_b);
  REQUIRE(resultado.empty() == true);
}

TEST_CASE("T8: MergeEntries_UmaListaVaziaRetornaOutraOrdenada",
          "[merge_entries]") {
  std::vector<LogEntry> lista_vazia;
  std::vector<LogEntry> lista_com_elementos = {
      {16, 1, 2026, 13, 27, 46, "Log A", true},
      {20, 1, 2026, 17, 45, 38, "Log B", true}};

  SECTION("Lista A vazia e Lista B populada") {
    std::vector<LogEntry> resultado =
        merge_entries(lista_vazia, lista_com_elementos);
    REQUIRE(resultado.size() == 2);
    REQUIRE(resultado[0].message == "Log A");
  }

  SECTION("Lista A populada e Lista B vazia") {
    std::vector<LogEntry> resultado =
        merge_entries(lista_com_elementos, lista_vazia);
    REQUIRE(resultado.size() == 2);
    REQUIRE(resultado[1].message == "Log B");
  }
}

TEST_CASE("T9: MergeEntries_DuasListasMesclaEOrdena", "[merge_entries]") {
  std::vector<LogEntry> lista_a = {{16, 1, 2026, 13, 27, 46, "Log A1", true},
                                   {20, 1, 2026, 17, 45, 38, "Log A2", true}};
  std::vector<LogEntry> lista_b = {{17, 1, 2026, 14, 17, 46, "Log B1", true},
                                   {21, 1, 2026, 18, 55, 38, "Log B2", true}};

  std::vector<LogEntry> resultado = merge_entries(lista_a, lista_b);

  REQUIRE(resultado.size() == 4);
  REQUIRE(resultado[0].message == "Log A1");
  REQUIRE(resultado[1].message == "Log B1");
  REQUIRE(resultado[2].message == "Log A2");
  REQUIRE(resultado[3].message == "Log B2");
}

TEST_CASE("T10: ReadLogFile_RetornaFalseSeArquivoNaoExiste",
          "[read_log_file]") {
  std::vector<LogEntry> entradas;
  entradas.push_back({1, 1, 2026, 0, 0, 0, "Lixo", true});

  bool resultado = read_log_file("arquivo_inexistente_123456.txt", &entradas);

  REQUIRE(resultado == false);
  REQUIRE(entradas.empty() == true);
}

TEST_CASE("T11: ReadLogFile_LeArquivoComLinhasValidas", "[read_log_file]") {
  std::ofstream criador("teste_linhas_validas.txt");
  criador << "16/1/2026 13:27:46 Log Valido 1\n";
  criador << "17/1/2026 14:17:46 Log Valido 2\n";
  criador.close();

  std::vector<LogEntry> entradas;
  bool resultado = read_log_file("teste_linhas_validas.txt", &entradas);

  REQUIRE(resultado == true);
  REQUIRE(entradas.size() == 2);
  REQUIRE(entradas[0].message == "Log Valido 1");
  REQUIRE(entradas[1].message == "Log Valido 2");

  std::remove("teste_linhas_validas.txt");
}

TEST_CASE("T12: WriteLogFile_EscreveEntradasNoFormatoCorreto",
          "[write_log_file]") {
  std::vector<LogEntry> entradas = {
      {16, 1, 2026, 13, 27, 46, "Texto do log A", true},
      {17, 1, 2026, 14, 17, 46, "Texto do log B", true}};

  bool resultado = write_log_file("teste_escrita.txt", entradas);
  REQUIRE(resultado == true);

  std::ifstream leitor("teste_escrita.txt");
  std::string linha1, linha2;
  std::getline(leitor, linha1);
  std::getline(leitor, linha2);
  leitor.close();

  REQUIRE(linha1 == "16/1/2026 13:27:46 Texto do log A");
  REQUIRE(linha2 == "17/1/2026 14:17:46 Texto do log B");

  std::remove("teste_escrita.txt");
}

TEST_CASE("T13: ProcessLogList_RetornaErroQuandoLogsTxtInexistente",
          "[process_log_list]") {
  int resultado = process_log_list("lista_de_logs_totalmente_inexistente.txt");
  REQUIRE(resultado == -1);
}

TEST_CASE("T14: ProcessLogList_RetornaZeroQuandoLogsTxtVazio",
          "[process_log_list]") {
  std::ofstream criador("lista_vazia.txt");
  criador.close();

  int resultado = process_log_list("lista_vazia.txt");
  REQUIRE(resultado == 0);

  std::remove("lista_vazia.txt");
}

TEST_CASE("T15: ProcessLogList_PulaArquivoInexistenteEContinua",
          "[process_log_list]") {
  std::ofstream f1("log_existente1.txt");
  f1 << "16/1/2026 13:27:46 Log Existente 1\n";
  f1.close();

  std::ofstream f2("log_existente2.txt");
  f2 << "17/1/2026 14:17:46 Log Existente 2\n";
  f2.close();

  std::ofstream lista("lista_mista.txt");
  lista << "log_existente1.txt\n";
  lista << "arquivo_fantasma_nao_existe.txt\n";
  lista << "log_existente2.txt\n";
  lista.close();

  int resultado = process_log_list("lista_mista.txt");
  REQUIRE(resultado == 2);

  std::remove("log_existente1.txt");
  std::remove("log_existente2.txt");
  std::remove("lista_mista.txt");
  std::remove("total_log_existente1.txt");
  std::remove("total_log_existente2.txt");
}

TEST_CASE("T16: ProcessLogList_NaoCriaTotalParaArquivoSemLinhasValidas",
          "[process_log_list]") {
  std::ofstream f_invalid("log_invalido.txt");
  f_invalid << "DATA_ERRADA msg_errada\n";
  f_invalid.close();

  std::ofstream lista("lista_invalida.txt");
  lista << "log_invalido.txt\n";
  lista.close();

  int resultado = process_log_list("lista_invalida.txt");
  REQUIRE(resultado == 1);

  std::ifstream checa_total("total_log_invalido.txt");
  REQUIRE(checa_total.is_open() == false);

  std::remove("log_invalido.txt");
  std::remove("lista_invalida.txt");
}

TEST_CASE("T17: ProcessLogList_CriaTotalOrdenadoQuandoNaoExiste",
          "[process_log_list]") {
  std::ofstream f_new("log_novo.txt");
  f_new << "20/1/2026 17:45:38 Log Novo 2\n";
  f_new << "16/1/2026 13:27:46 Log Novo 1\n";
  f_new.close();

  std::ofstream lista("lista_novo.txt");
  lista << "log_novo.txt\n";
  lista.close();

  int resultado = process_log_list("lista_novo.txt");
  REQUIRE(resultado == 1);

  std::ifstream leitor("total_log_novo.txt");
  std::string l1, l2;
  std::getline(leitor, l1);
  std::getline(leitor, l2);
  leitor.close();

  REQUIRE(l1 == "16/1/2026 13:27:46 Log Novo 1");
  REQUIRE(l2 == "20/1/2026 17:45:38 Log Novo 2");

  std::remove("log_novo.txt");
  std::remove("lista_novo.txt");
  std::remove("total_log_novo.txt");
}

TEST_CASE("T18: ProcessLogList_FazMergeOrdenadoComTotalExistente",
          "[process_log_list]") {
  std::ofstream f_total("total_log_merge.txt");
  f_total << "17/1/2026 14:17:46 Log Antigo de Base\n";
  f_total.close();

  std::ofstream f_log("log_merge.txt");
  f_log << "16/1/2026 13:27:46 Log Novo Intercalado\n";
  f_log.close();

  std::ofstream lista("lista_merge.txt");
  lista << "log_merge.txt\n";
  lista.close();

  int resultado = process_log_list("lista_merge.txt");
  REQUIRE(resultado == 1);

  std::ifstream leitor("total_log_merge.txt");
  std::string l1, l2;
  std::getline(leitor, l1);
  std::getline(leitor, l2);
  leitor.close();

  REQUIRE(l1 == "16/1/2026 13:27:46 Log Novo Intercalado");
  REQUIRE(l2 == "17/1/2026 14:17:46 Log Antigo de Base");

  std::remove("log_merge.txt");
  std::remove("lista_merge.txt");
  std::remove("total_log_merge.txt");
}

TEST_CASE("T19: ProcessLogList_ProcessaListaMistaExistentesEInexistentes",
          "[process_log_list]") {
  std::ofstream f_ok("log_real.txt");
  f_ok << "16/1/2026 13:27:46 Log Real\n";
  f_ok.close();

  std::ofstream lista("lista_mista_completa.txt");
  lista << "arquivo_que_nao_existe1.txt\n";
  lista << "log_real.txt\n";
  lista << "arquivo_que_nao_existe2.txt\n";
  lista.close();

  int resultado = process_log_list("lista_mista_completa.txt");
  REQUIRE(resultado == 1);

  std::remove("log_real.txt");
  std::remove("lista_mista_completa.txt");
  std::remove("total_log_real.txt");
}

TEST_CASE("T20: ProcessLogList_DescartaLinhasInvalidasEProcessaValidas",
          "[process_log_list]") {
  std::ofstream f_misto("log_linhas_mistas.txt");
  f_misto << "LINHA_TOTALMENTE_CORRUPTA_E_INVALIDA\n";
  f_misto << "16/1/2026 13:27:46 Linha Valida Inserida\n";
  f_misto.close();

  std::ofstream lista("lista_linhas_mistas.txt");
  lista << "log_linhas_mistas.txt\n";
  lista.close();

  int resultado = process_log_list("lista_linhas_mistas.txt");
  REQUIRE(resultado == 1);

  std::ifstream leitor("total_log_linhas_mistas.txt");
  std::string linha;
  std::getline(leitor, linha);
  REQUIRE(linha == "16/1/2026 13:27:46 Linha Valida Inserida");

  std::string extra;
  std::getline(leitor, extra);
  REQUIRE(extra.empty() == true);
  leitor.close();

  std::remove("log_linhas_mistas.txt");
  std::remove("lista_linhas_mistas.txt");
  std::remove("total_log_linhas_mistas.txt");
}

TEST_CASE("T21: Regex_MensagemTamanho1A100", "[caixa_aberta]") {
  // Regex mapeando o formato: data hora mensagem(1 ate 100 chars)
  std::regex padrao_log("^\\d+/\\d+/\\d+ \\d+:\\d+:\\d+ .{1,100}$");

  std::string linha_valida = "16/1/2026 13:27:46 Exemplo correto de log";
  std::string linha_longa = "16/1/2026 13:27:46 " + std::string(101, 'a');

  REQUIRE(std::regex_match(linha_valida, padrao_log) == true);
  REQUIRE(std::regex_match(linha_longa, padrao_log) == false);
}
