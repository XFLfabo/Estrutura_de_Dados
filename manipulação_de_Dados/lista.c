#include "lista.h"

//Função para criar a lista com capacidade inicial C
Lista * CriarLista(int C){
    Lista * L = (Lista *) malloc(sizeof(Lista));
    if(L == NULL){
        printf("ERRO: não há memória para a lista\n");
        return NULL;
    }

    L->Tamanho = 0;
    L->Capacidade = C;
    L->Dados = (Registro *) malloc(C * sizeof(Registro));

    if(L->Dados == NULL){
        printf("ERRO: não foi possível alocar memória para o vetor!\n");
        free(L);
        return NULL;
    }
    return L;
}

//Função para destruir a lista e liberar memória
void DestruirLista(Lista * L){
    if(L == NULL) return;
    free(L->Dados);
    free(L);
}

//Função para adicionar um registro á lista
void AdicionarRegistro(Lista * L, Registro R){
    if(L == NULL){
        printf("ERRO: lista não inicializada!\n");
        return;
    }

    //Se a lista estiver cheia, a capacidade dobra de tamanho
    if(L->Tamanho == L->Capacidade){
        int novaCapacidade = L->Capacidade * 2;
        Registro * novosDados = (Registro *) realloc(L->Dados, novaCapacidade * sizeof(Registro));
        if(novosDados == NULL){
            printf("ERRO: não foi possivel redimencionar a lista\n");
            return;
        }
        L->Dados = novosDados;
        L->Capacidade = novaCapacidade;
    }
    L->Dados[L->Tamanho] = R;
    L->Tamanho++;

}