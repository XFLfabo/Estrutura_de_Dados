# Documentação do Projeto - Manipulação de Arquivos em C

## Introdução
Este projeto foi desenvolvido seguindo os padrões de codificação e estruturas de dados ensinados em sala de aula. O sistema realiza a leitura, concatenação e análise de indicadores da Justiça Eleitoral a partir de arquivos CSV.

## Estrutura do Projeto
O código segue o modelo de separação em três arquivos:
1	`lista.h`: Definição da estrutura `Registro` e da struct `Lista` baseada em **vetores dinâmicos** (ponteiro `Dados`, `Capacidade` e `Tamanho`).
2	`lista.c`: Implementação das funções `CriarLista`, `DestruirLista` e `AdicionarRegistro`. A função de adição inclui lógica de **redimensionamento dinâmico** (realloc) para garantir que todos os dados sejam carregados independentemente do volume.
3	`main.c`: Lógica de interface com o usuário, parser de CSV adaptado para lidar com campos entre aspas e as funções de processamento de dados.

## Funcionalidades
- **Concatenação de Arquivos**: Varre o diretório, une os CSVs em cvs.csv e popula a lista na memória.
- **Geração de Resumo**: Calcula as metas (Meta1, Meta2A, Meta2Ant, Meta4A e Meta4B) por tribunal, exportando para resumo.csv.
- **Filtro por Município**: Gera um arquivo CSV personalizado baseado na entrada do usuário.

## Técnicas Aplicadas
- **Alocação Dinâmica**: Uso de malloc, free e realloc para gerenciamento de memória.
- **Manipulação de Strings**: Uso de strcpy, strcmp, strcasecmp e strstr.
- **Arquivos**: Uso de fopen, fgets, fprintf e fclose para manipulação persistente.
- **TAD**: Encapsulamento da lógica da lista em arquivos separados para modularização.

## Como Compilar e Executar
```bash
gcc main.c lista.c -o sistema_tribunal.exe
.\sistema_tribunal
```
