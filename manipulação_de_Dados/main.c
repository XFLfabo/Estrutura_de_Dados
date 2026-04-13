#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <locale.h>
#include "lista.h"


//Variáveis globais para rastrear arquivos de muinicípios
char arquivos_municipios[100][MAX_PATH];
int qtd_arquivos_municipios = 0;

//Função para remover aspas de uma string CSV
void removerAspas(char* str){
    int i, j = 0;
    for(i = 0; str[i] != '\0'; i++){
        if(str[i] != '\"'){
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

//Função 1: concatenar arquivos e carregar na lista
void concatenarArquivos(const char * diretorio, Lista* L){
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char busca[MAX_PATH];

    //Prepara o padrão de busca 
    sprintf(busca, "%s/*.csv", diretorio);

    hFind = FindFirstFile(busca, &findFileData);
    if(hFind == INVALID_HANDLE_VALUE){
        printf("ERRO: não foi possível encontrar a pasta ou arquivos CSV em: %s\n", diretorio);
        return;
    }

    FILE* f_out = fopen("csv.csv", "w");
    if(f_out == NULL){
        printf("Erro ao criar arquivo cvs.csv\n");
        FindClose(hFind);
        return;
    }

    int cabecalho_escrito = 0;
    do{
        char caminho[MAX_PATH];
        sprintf(caminho, "%s/%s", diretorio, findFileData.cFileName);

        FILE* f_in = fopen(caminho, "r");
        if(f_in){
            char linha[2048];
            
            if(fgets(linha, sizeof(linha), f_in)){
                if(!cabecalho_escrito){
                    fprintf(f_out, "%s", linha);
                    cabecalho_escrito = 1;
                }
            }

            while(fgets(linha, sizeof(linha), f_in)){
                fprintf(f_out, "%s", linha);

                Registro r;
                char temp[2048];
                strcpy(temp, linha);
                memset(&r, 0, sizeof(Registro));

                int col = 0;
                char* ptr = temp;
                char* start = temp;
                int in_quotes = 0;

                while(1){
                    if(*ptr == '\"') in_quotes = !in_quotes;
                    if ((*ptr == ',' && !in_quotes) || *ptr == '\n' || *ptr == '\r' || *ptr == '\0'){
                        char last = *ptr;
                        *ptr = '\0';
                        removerAspas(start);

                        //Remove espaços em branco no início e fim do campo extraído
                        char *field = start;
                        while(isspace((unsigned char)*field)) field++;
                        char *end = field + strlen(field) -1;
                        while(end > field && isspace((unsigned char)*end)) *end-- = '\0';

                        switch(col){
                            case 0: strncpy(r.sigla_tribunal, field, 49); break;
                            case 1: strncpy(r.procedimento, field, 99); break;
                            case 2: strncpy(r.ramo_justica, field, 99); break;
                            case 3: strncpy(r.sigla_grau, field, 9); break;
                            case 4: strncpy(r.uf_oj, field, 9); break;
                            case 5: strncpy(r.municipio_oj, field, 99); break;
                            case 6: r.id_ultimo_oj = atoi(field); break;
                            case 7: strncpy(r.nome, field, 199); break;
                            case 8: strncpy(r.mesano_cnml, field, 19); break;
                            case 9: strncpy(r.mesano_sent, field, 19); break;
                            case 10: r.casos_novos_2026 = atoi(field); break;
                            case 11: r.julgados_2026 = atoi(field); break;
                            case 12: r.prim_sent2026 = atoi(field); break;
                            case 13: r.suspensos_2026 = atoi(field); break;
                            case 14: r.dessobrestados_2026 = atoi(field); break;
                            case 15: r.cumprimento_meta1 = atoi(field); break;
                            case 16: r.distm2_a = atoi(field); break;
                            case 17: r.julgm2_a = atoi(field); break;
                            case 18: r.suspm2_a = atoi(field); break;
                            case 19: r.cumprimento_meta2a = atoi(field); break;
                            case 20: r.distm2_ant = atoi(field); break;
                            case 21: r.julgm2_ant = atoi(field); break;
                            case 22: r.suspm2_ant = atoi(field); break;
                            case 23: r.desom2_ant = atoi(field); break;
                            case 24: r.cumprimento_meta2ant = atoi(field); break;
                            case 25: r.distm4_a = atoi(field); break;
                            case 26: r.julgm4_a = atoi(field); break;
                            case 27: r.suspm4_a = atoi(field); break;
                            case 28: r.cumprimento_meta4a = atoi(field); break;
                            case 29: r.distm4_b = atoi(field); break;
                            case 30: r.julgm4_b = atoi(field); break;
                            case 31: r.suspm4_b = atoi(field); break;
                            case 32: r.cumprimento_meta4b = atoi(field); break;
                        }
                        if(last == '\0' || last == '\n' || last == '\r') break;
                        start = ptr + 1;
                        col++;
                    }
                    ptr++;
                }
                AdicionarRegistro(L, r);
            }
            fclose(f_in);
        }
    }while(FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    fclose(f_out);
    printf("\nSucesso: arquivos concatenados em 'cvs.csv'!\n");
    printf("Total de registros carregados na memória: %d\n", L->Tamanho);
}

//Estrutura para ajudar no resumo por tribunal
typedef struct {
    char sigla[50];
    long julgados_2026;
    long casos_novos_2026;
    long dessobrestados_2026;
    long suspensos_2026;
    long julgm2_a;
    long distm2_a;
    long suspm2_a;
    long julgm2_ant;
    long distm2_ant;
    long suspm2_ant;
    long desom2_ant;
    long julgm4_a;
    long distm4_a;
    long suspm4_a;
    long julgm4_b;
    long distm4_b;
    long suspm4_b;
} ResumoTribunal;

//Função 2: gerar resumo por tribunal
void gerarResumo(Lista* L){
    if(L == NULL || L->Tamanho == 0){
        printf("Aviso: a lista está vazia! carregue os dados primeiro (opção 1).\n");
        return;
    }

    ResumoTribunal* resumos = NULL;
    int qtd_tribunais = 0;

    for(int i = 0; i < L->Tamanho; i++){
        int encontrado = -1;
        for(int j = 0; j < qtd_tribunais; j++){
            if(strcmp(resumos[j].sigla, L->Dados[i].sigla_tribunal) == 0){
                encontrado = j;
                break;
            }
        }

        if(encontrado == -1){
            qtd_tribunais++;
            resumos = (ResumoTribunal*)realloc(resumos, qtd_tribunais * sizeof(ResumoTribunal));
            mumset(&resumos[qtd_tribunais-1], 0, sizeof(ResumoTribunal));
            strcpy(resumos[qtd_tribunais-1].sigla, L->Dados[i].sigla_tribunal);
            encontrado = qtd_tribunais - 1;
        }

        resumos[encontrado].julgados_2026 += L->Dados[i].julgados_2026;
        resumos[encontrado].casos_novos_2026 += L->Dados[i].casos_novos_2026;
        resumos[encontrado].dessobrestados_2026 += L->Dados[i].dessobrestados_2026;
        resumos[encontrado].suspensos_2026 += L->Dados[i].suspensos_2026;
        resumos[encontrado].julgm2_a += L->Dados[i].julgm2_a;
        resumos[encontrado].distm2_a += L->Dados[i].distm2_a;
        resumos[encontrado].suspm2_a += L->Dados[i].suspm2_a;
        resumos[encontrado].julgm2_ant += L->Dados[i].julgm2_ant;
        resumos[encontrado].distm2_ant += L->Dados[i].distm2_ant;
        resumos[encontrado].suspm2_ant += L->Dados[i].suspm2_ant;
        resumos[encontrado].desom2_ant += L->Dados[i].desom2_ant;
        resumos[encontrado].julgm4_a += L->Dados[i].julgm4_a;
        resumos[encontrado].distm4_a += L->Dados[i].distm4_a;
        resumos[encontrado].suspm4_a += L->Dados[i].suspm4_a;
        resumos[encontrado].julgm4_b += L->Dados[i].julgm4_b;
        resumos[encontrado].distm4_b += L->Dados[i].distm4_b;
        resumos[encontrado].suspm4_b += L->Dados[i].suspm4_b;
    }

    FILE* f_res = fopen("resumo.csv", "w");
    if(f_res == NULL) return;
    fprintf(f_res, "sigla_tribunal,Meta1,Meta2A,Meta2Ant,Meta4A,Meta4B\n");

    for(int i = 0; i < qtd_tribunais; i++){
        double m1 = 0, m2a = 0, m2ant = 0, m4a = 0, m4b = 0;
        long div1 = resumos[i].casos_novos_2026 + resumos[i].dessobrestados_2026 - resumos[i].suspensos_2026;
        if(div1 != 0) m1 = ((double)resumos[i].julgados_2026 / div1) * 100;
        long div2a = resumos[i].distm2_a - resumos[i].suspm2_a;
        if(div2a != 0) m2a = ((double)resumos[i].julgm2_a / div2a) * (1000.0/7.0);
        long div2ant = resumos[i].distm2_ant - resumos[i].suspm2_ant - resumos[i].desom2_ant;
        if(div2ant != 0) m2ant = ((double)resumos[i].julgm2_ant / div2ant) * 100;
        long div4a = resumos[i].distm4_a - resumos[i].suspm4_a;
        if(div4a != 0) m4a = ((double)resumos[i].julgm4_a / div4a) * 100;
        long div4b = resumos[i].distm4_b - resumos[i].suspm4_b;
        if(div4b != 0) m4b = ((double)resumos[i].julgm4_b / div4b) * 100;
        fprintf(f_res, "%s,%.2f,%.2f,%.2f,%.2f,%.2f\n", resumos[i].sigla, m1, m2a, m2ant, m4a, m4b);
    }
    fclose(f_res);
    free(resumos);
    printf("Sucesso: arquivo 'resumo.csv' gerado.\n");
}

//Função 3: filtrar por município
void filtrarPorMunicipio(Lista* L, const char* municipio){
    if(L == NULL || L->Tamanho == 0){
        printf("Aviso: a lista está vazia! carregue os dados primeiro.\n");
        return;
    }
    char nome_arquivo[MAX_PATH];
    sprintf(nome_arquivo, "%s.csv", municipio);
    FILE* f_mun = fopen(nome_arquivo, "w");
    if(f_mun == NULL) return;

    fprintf(f_mun, "sigla_tribunal,procedimento,ramo_justica,sigla_grau,uf_oj,municipio_oj,id_ultimo_oj,nome,mesano_cnm1,mesano_sent,casos_novos_2026,julgados_2026,prim_sent2026,suspensos_2026,dessobrestados_2026,cumprimento_meta1,distm2_a,julgm2_a,suspm2_a,cumprimento_meta2a,distm2_ant,julgm2_ant,suspm2_ant,desom2_ant,cumprimento_meta2ant,distm4_a,julgm4_a,suspm4_a,cumprimento_meta4a,distm4_b,julgm4_b,suspm4_b,cumprimento_meta4b\n");
    
    int cont = 0;
    for(int i = 0; i < L->Tamanho; i++){
        if(strcasecmp(L->Dados[i].municipio_oj, municipio) == 0){
            fprintf(f_mun, "%s,%s,%s,%s,%s,%s,%d,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                L->Dados[i].sigla_tribunal, L->Dados[i].procedimento, L->Dados[i].ramo_justica,
                L->Dados[i].sigla_grau, L->Dados[i].uf_oj, L->Dados[i].municipio_oj,
                L->Dados[i].id_ultimo_oj, L->Dados[i].nome, L->Dados[i].mesano_cnml,
                L->Dados[i].mesano_sent, L->Dados[i].casos_novos_2026, L->Dados[i].julgados_2026,
                L->Dados[i].prim_sent2026, L->Dados[i].suspensos_2026, L->Dados[i].dessobrestados_2026,
                L->Dados[i].cumprimento_meta1, L->Dados[i].distm2_a, L->Dados[i].julgm2_a,
                L->Dados[i].suspm2_a, L->Dados[i].cumprimento_meta2a, L->Dados[i].distm2_ant,
                L->Dados[i].julgm2_ant, L->Dados[i].suspm2_ant, L->Dados[i].desom2_ant,
                L->Dados[i].cumprimento_meta2ant, L->Dados[i].distm4_a, L->Dados[i].julgm4_a,
                L->Dados[i].suspm4_a, L->Dados[i].cumprimento_meta4a, L->Dados[i].distm4_b,
                L->Dados[i].julgm4_b, L->Dados[i].suspm4_b, L->Dados[i].cumprimento_meta4b);
            cont++;
        }
    }
    fclose(f_mun);
    printf("Sucesso: arquivo '%s' gerado com %d ocorrências.\n", nome_arquivo, cont);

    if(qtd_arquivos_municipios < 100){
        int ja_existe = 0;
        for(int j=0; j<qtd_arquivos_municipios; j++){
            if(strcmp(arquivos_municipios[j], nome_arquivo) == 0) ja_existe = 1;
        }
        if(!ja_existe){
            strcpy(arquivos_municipios[qtd_arquivos_municipios], nome_arquivo);
            qtd_arquivos_municipios++;
        }
    }
}

//Função 4: remover arquivos gerados
void removerArquivosGerados(){
    int removidos = 0;
    if(DeleteFile("cvs.csv")){printf("Arquivo 'cvs.csv' removido.\n"); removidos++;}
    if(DeleteFile("resumo.csv")){printf("Arquivo 'resumo.csv' removido.\n"); removidos++;}
    for(int i = 0; i < qtd_arquivos_municipios; i++){
        if(DeleteFile(arquivos_municipios[i])){
            printf("Arquivo '%s' removido.\n", arquivos_municipios[i]);
            removidos++;
        }
    }
    qtd_arquivos_municipios = 0;
    if(removidos > 0) printf("\nLimpeza concluida! %d arquivos apagados.\n", removidos);
    else printf("\nNenhum arquivo gerado foi encontrado para remover.\n");
}

int main(){
    setlocale(LC_ALL, "Portuguese");
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    Lista * minhaLista = CriarLista(1000);
    int opcao;
    char municipio[100];

    do{
        system("cls");
        printf("========================================\n");
        printf("     SISTEMA DE JUSTIÇA ELEITORAL       \n");
        printf("========================================\n");
        printf("1. Concatenar e Carregar Dados\n");
        printf("2. Gerar Resumo (resumo.csv)\n");
        printf("3. Filtrar por Município\n");
        printf("4. Limpar Arquivos Gerados\n");
        printf("0. Sair\n");
        printf("----------------------------------------\n");
        printf("Escolha: ");

        if(scanf("%d", &opcao) != 1){while(getchar()!= '\n'); continue;}
        getchar();

        switch(opcao){
            case 1:
                concatenarArquivos("./Base-de-Dados", minhaLista);
                if(minhaLista->Tamanho == 0) concatenarArquivos("./Base de Dados", minhaLista);
                system("pause");
                break;
            case 2:
                gerarResumo(minhaLista);
                system("pause");
                break;
            case 3:
                printf("Nome do município: ");
                fgets(municipio, sizeof(municipio), stdin);
                municipio[strcspn(municipio, "\n")] = 0;
                filtrarPorMunicipio(minhaLista, municipio);
                system("pause");
                break;
            case 4:
                removerArquivosGerados();
                system("pause");
                break;
            case 0:
                printf("Encerrando sistema...\n");
                break;
            default:
                printf("Opção invalida!\n");
                system("pause");
        }
    }while(opcao != 0);

    DestruirLista(minhaLista);
    return 0;
}