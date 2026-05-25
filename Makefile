# Makefile -- Trabalho 2 de Tecnicas de Programacao 2 (CIC0198) - UnB
#
# Targets principais:
#   make            - compila o executavel de testes (testa_monitora_logs)
#   make test       - compila e roda os testes
#   make coverage   - compila com flags de cobertura, roda os testes
#                     e gera relatorios .gcov (precisa >= 80% por modulo)
#   make valgrind   - roda os testes sob Valgrind (precisa zero leaks)
#   make lint       - roda cpplint no codigo fonte (estilo Google)
#   make check      - roda cppcheck --enable=warning --suppress=*:catch.hpp
#   make doc        - gera documentacao HTML com Doxygen em doc/html
#   make clean      - remove artefatos de build e cobertura
#   make distclean  - clean + remove documentacao gerada

# Compilador e flags base
CXX       = g++
CXXSTD    = -std=c++14
WARN      = -Wall -Wextra -Wpedantic
CXXFLAGS  = $(CXXSTD) $(WARN) -g

# Flags de cobertura (gcov): geram .gcno na compilacao e .gcda na execucao.
# Sao adicionadas apenas ao alvo 'coverage' para nao poluir o build normal.
COVFLAGS  = -fprofile-arcs -ftest-coverage -O0

# Fontes do modulo (NAO inclui catch.hpp, que e header-only e e incluido
# apenas pelo arquivo de testes)
MOD_SRC   = monitora_logs.cpp
MOD_HDR   = monitora_logs.hpp

# Fontes do binario de testes
TEST_SRC  = testa_monitora_logs.cpp $(MOD_SRC)
TEST_BIN  = testa_monitora_logs

# Arquivos a serem verificados por cpplint (NAO inclui catch.hpp)
LINT_SRC  = $(MOD_SRC) $(MOD_HDR) testa_monitora_logs.cpp

# ----------------------------------------------------------------------------
# Build padrao
# ----------------------------------------------------------------------------
all: $(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(MOD_HDR) catch.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_SRC)

# ----------------------------------------------------------------------------
# Rodar testes
# ----------------------------------------------------------------------------
test: $(TEST_BIN)
	./$(TEST_BIN) --success

# ----------------------------------------------------------------------------
# Cobertura com gcov
# Usa compilacao em dois passos para que os arquivos .gcno/.gcda tenham
# o nome do source (monitora_logs.gcno) em vez do nome do binario.
# ----------------------------------------------------------------------------
coverage: clean-cov
	$(CXX) $(CXXFLAGS) $(COVFLAGS) -c monitora_logs.cpp -o monitora_logs.o
	$(CXX) $(CXXFLAGS) $(COVFLAGS) -c testa_monitora_logs.cpp \
	    -o testa_monitora_logs.o
	$(CXX) $(CXXFLAGS) $(COVFLAGS) -o $(TEST_BIN)_cov \
	    testa_monitora_logs.o monitora_logs.o
	./$(TEST_BIN)_cov
	gcov monitora_logs.cpp
	@echo ""
	@echo "=== Relatorio de cobertura ==="
	@echo "Verifique monitora_logs.cpp.gcov -- a cobertura deve ser >= 80%."

clean-cov:
	rm -f *.gcno *.gcda *.gcov $(TEST_BIN)_cov monitora_logs.o \
	      testa_monitora_logs.o

# ----------------------------------------------------------------------------
# Valgrind (memcheck)
# ----------------------------------------------------------------------------
valgrind: $(TEST_BIN)
	valgrind --leak-check=full --show-leak-kinds=all \
	         --track-origins=yes --error-exitcode=1 \
	         ./$(TEST_BIN)

# ----------------------------------------------------------------------------
# cpplint (estilo Google)
# Excluindo warnings do build/include_subdir e legal/copyright se necessario.
# ----------------------------------------------------------------------------
lint:
	cpplint --filter=-legal/copyright,-build/include_subdir,-build/header_guard $(LINT_SRC)

# ----------------------------------------------------------------------------
# cppcheck (analise estatica)
# ----------------------------------------------------------------------------
check:
	cppcheck --enable=warning --suppress=*:catch.hpp --inline-suppr --error-exitcode=1 \
                 monitora_logs.cpp testa_monitora_logs.cpp

# ----------------------------------------------------------------------------
# Doxygen
# ----------------------------------------------------------------------------
doc:
	@if [ ! -f Doxyfile ]; then \
	    echo "Doxyfile nao encontrado. Rode 'doxygen -g' e edite-o."; \
	    exit 1; \
	fi
	doxygen Doxyfile

# ----------------------------------------------------------------------------
# Limpeza
# ----------------------------------------------------------------------------
clean: clean-cov
	rm -f $(TEST_BIN) *.o

distclean: clean
	rm -rf doc html latex

.PHONY: all test coverage clean-cov valgrind lint check doc clean distclean
