# Analisador léxico para a linguagem KRNEL.

Esse analisador tem como objetivo verificar os tokens de um arquivo de entrada e separa-los em suas devidas categorias.

# Como usar?

## Compilando

###  Linux
  1. Utilize um compilador como g++, instalando com ``` sudo apt install g++ ``` em sistemas baseados em Debian.
  2. Para compilar utilize ``` g++ lexer.cpp -o lexer.o ```
  3. Após compilar, execute com ``` ./lexer.o ```

## Utilização

  1. Tenha o arquivo para ser analisado no diretório do executável;
  2. Após executar, aparecerá um prompt solicitando o nome do arquivo;
  3. Finalizando a análise, o programa retorna no terminal se obteve sucesso;
  4. Um arquivo de output com extensão .krn será criado no diretório do executável.
