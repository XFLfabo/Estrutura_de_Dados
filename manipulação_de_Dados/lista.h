#ifndef LISTA_H
#define LISTA_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Estrutura que define um registro da justiça eleitoral
typedef struct{
    char sigla_tribunal[50];
    char procedimento[100];
    char ramo_justica[100];
    char sigla_grau[10];
    char uf_oj[10];
    char municipio_oj[100];
    int id_ultimo_oj;
    char nome[200];
    char mesano_cnml[20];
    char mesano_sent[20];
    int casos_novos_2026;
    int julgados_2026;
    int prim_sent2026;
    int suspensos_2026;
    int dessobrestados_2026;
    int cumprimento_meta1;
    int distm2_a;
    int julgm2_a;
    int suspm2_a;
    int cumprimento_meta2a;
    int distm2_ant;
    int julgm2_ant;
    int suspm2_ant;
    int desom2_ant;
    int cumprimento_meta2ant;
    int distm4_a;
    int julgm4_a;
    int suspm4_a;
    int cumprimento_meta4a;
    int distm4_b;
    int julgm4_b;
    int suspm4_b;
    int cumprimento_meta4b;
} Registro;

//Definição da struct lista
typedef struct{
    Registro * Dados;
    int Capacidade;
    int Tamanho;
} Lista;

//Operações da lista
Lista * CriarLista(int C);
void DestruirLista(Lista * L);
void AdicionarRegistro(Lista * L, Registro R);

#endif