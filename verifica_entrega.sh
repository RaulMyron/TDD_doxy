#!/bin/bash

echo "============================================="
echo "VERIFICADOR DE ENTREGA - TP2 (CIC0198)"
echo "============================================="

echo -e "\n[1/6] Limpando e compilando testes..."
make clean
make test
if [ $? -ne 0 ]; then echo "FALHA: Erro nos testes."; exit 1; fi

echo -e "\n[2/6] Rodando analise estatica (Linter e Cppcheck)..."
make lint
if [ $? -ne 0 ]; then echo "FALHA: Erro no cpplint."; exit 1; fi
make check
if [ $? -ne 0 ]; then echo "FALHA: Erro no cppcheck."; exit 1; fi

echo -e "\n[3/6] Rodando analise de memoria (Valgrind)..."
make valgrind
if [ $? -ne 0 ]; then echo "FALHA: Vazamento de memoria detectado."; exit 1; fi

echo -e "\n[4/6] Verificando cobertura de codigo (Gcov)..."
make coverage
if [ $? -ne 0 ]; then echo "FALHA: Erro na geracao de cobertura."; exit 1; fi

echo -e "\n[5/6] Gerando documentacao (Doxygen)..."
doxygen
if [ ! -d "html" ]; then echo "FALHA: Diretorio html do Doxygen nao foi gerado."; exit 1; fi

echo -e "\n[6/6] Verificando artefatos e controle de versao..."
if [ ! -d ".git" ]; then
    echo "FALHA: Diretorio .git nao encontrado!"
else
    COMMITS=$(git rev-list --count HEAD)
    if [ "$COMMITS" -ge 30 ]; then
        echo "[OK] Commits minimos atingidos ($COMMITS)."
    else
        echo "FALHA: Commits insuficientes ($COMMITS/30)."
    fi
fi

if [ ! -f "leiame.txt" ]; then echo "FALHA: leiame.txt ausente!"; else echo "[OK] leiame.txt encontrado."; fi

echo "============================================="
echo "VERIFICACAO CONCLUIDA. SE NAO HOUVER FALHAS ACIMA, O CODIGO ESTA PRONTO."
echo "Lembre-se de compilar o PDF no Overleaf e colocar na raiz antes de zipar."
echo "============================================="
