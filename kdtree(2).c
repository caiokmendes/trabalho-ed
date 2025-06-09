#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DIM 128
#define MAX_ID 100

typedef struct {
    float embedding[DIM];  //representa um rosto com embbedings
    char nome[MAX_ID];     //e nome ou id
} treg;

typedef struct No {
    treg ponto;
    struct No *esq, *dir;
    int eixo;
} No;

typedef struct {
    No *raiz;
} KDTree;

typedef struct {
    float distancia;
    void *key;
} HeapItem;


static treg *resultado_buffer = NULL;
static int num_resultados = 0;



float distancia2(float *a, float *b) { //calcula a dist euclidiana
    float d = 0.0;                      //entre dois embeddings
    for (int i = 0; i < DIM; i++) {
        float diff = a[i] - b[i];
        d += diff * diff;
    }
    return d;
}

No* novo_no(treg ponto, int eixo) {
    No *no = (No *)malloc(sizeof(No));
    no->ponto = ponto;
    no->esq = no->dir = NULL;
    no->eixo = eixo;
    return no;
}

void inserir_rec(No **raiz, treg ponto, int profundidade) {
    if (*raiz == NULL) {
        *raiz = novo_no(ponto, profundidade % DIM);
        return;
    }

    int eixo = (*raiz)->eixo;
    //comparacao por um dos val do embedding
    if (ponto.embedding[eixo] < (*raiz)->ponto.embedding[eixo]) {
        inserir_rec(&((*raiz)->esq), ponto, profundidade + 1);
    } else {
        inserir_rec(&((*raiz)->dir), ponto, profundidade + 1);
    }
}


//busca os n vizinhos mais prox d um vetor usando heap
void buscar_n_vizinhos_rec(No *raiz, treg *alvo, int n, HeapItem *heap, int *heap_size) {
    if (raiz == NULL) return;

    float d = distancia2(raiz->ponto.embedding, alvo->embedding);

    if (*heap_size < n) {
        heap[*heap_size].distancia = d;
        heap[*heap_size].key = &(raiz->ponto);
        (*heap_size)++;
    } else {
        int max_idx = 0;
        for (int i = 1; i < n; i++)
            if (heap[i].distancia > heap[max_idx].distancia)
                max_idx = i;

        if (d < heap[max_idx].distancia) {
            heap[max_idx].distancia = d;
            heap[max_idx].key = &(raiz->ponto);
        }
    }

    int eixo = raiz->eixo;
    float diff = alvo->embedding[eixo] - raiz->ponto.embedding[eixo];
    No *primeiro = diff < 0 ? raiz->esq : raiz->dir;
    No *segundo = diff < 0 ? raiz->dir : raiz->esq;

    buscar_n_vizinhos_rec(primeiro, alvo, n, heap, heap_size);
    if (*heap_size < n || fabs(diff) * fabs(diff) < heap[0].distancia) {
        buscar_n_vizinhos_rec(segundo, alvo, n, heap, heap_size);
    }
}



static KDTree arvore = { NULL };

void inserir(treg *p) {
    inserir_rec(&(arvore.raiz), *p, 0);
}

//receebe o vetor e retorna os n vizinhos mais prox no buffer
int buscar_vizinhos(float embedding[DIM], int n, treg *resultados) {
    if (resultado_buffer) {
        free(resultado_buffer);
        resultado_buffer = NULL;
    }

    resultado_buffer = malloc(sizeof(treg) * n);
    if (!resultado_buffer) return 0;

    treg consulta;
    memcpy(consulta.embedding, embedding, sizeof(float) * DIM);
    strcpy(consulta.nome, "");

    HeapItem heap[n];
    int heap_size = 0;

    buscar_n_vizinhos_rec(arvore.raiz, &consulta, n, heap, &heap_size);

    num_resultados = heap_size;
    for (int i = 0; i < heap_size; i++) {
        resultado_buffer[i] = *((treg *)heap[i].key);
        resultados[i] = resultado_buffer[i];
    }

    return heap_size;
}

int obter_num_resultados() {
    return num_resultados;
}

const char* obter_id_resultado(int idx) {
    if (idx < 0 || idx >= num_resultados) return NULL;
    return resultado_buffer[idx].nome;
}
