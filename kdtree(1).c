#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <assert.h>

typedef struct _reg {
    float embedding[128];
    char id[100];
} treg;

void *aloca_reg(float embedding[128], char id[]) {
    treg *reg = malloc(sizeof(treg));
    memcpy(reg->embedding, embedding, sizeof(float) * 128);
    strncpy(reg->id, id, 99);
    reg->id[99] = '\0';
    return reg;
}

int comparador(void *a, void *b, int pos) {
    float diff = ((treg *)a)->embedding[pos] - ((treg *)b)->embedding[pos];
    return (diff > 0) - (diff < 0);
}

double distancia(void *a, void *b) {
    double soma = 0;
    for (int i = 0; i < 128; i++) {
        double diff = ((treg *)a)->embedding[i] - ((treg *)b)->embedding[i];
        soma += diff * diff;
    }
    return soma;
}


typedef struct _node {
    void *key;
    struct _node *esq;
    struct _node *dir;
} tnode;

typedef struct _arv {
    tnode *raiz;
    int (*cmp)(void *, void *, int);
    double (*dist)(void *, void *);
    int k;
} tarv;

void kdtree_constroi(tarv *arv, int (*cmp)(void *, void *, int), double (*dist)(void *, void *), int k) {
    arv->raiz = NULL;
    arv->cmp = cmp;
    arv->dist = dist;
    arv->k = k;
}

void _kdtree_insere(tnode **raiz, void *key, int (*cmp)(void *, void *, int), int profund, int k) {
    if (*raiz == NULL) {
        *raiz = malloc(sizeof(tnode));
        (*raiz)->key = key;
        (*raiz)->esq = NULL;
        (*raiz)->dir = NULL;
    } else {
        int pos = profund % k;
        if (cmp((*raiz)->key, key, pos) < 0) {
            _kdtree_insere(&((*raiz)->dir), key, cmp, profund + 1, k);
        } else {
            _kdtree_insere(&((*raiz)->esq), key, cmp, profund + 1, k);
        }
    }
}

void kdtree_insere(tarv *arv, void *key) {
    _kdtree_insere(&(arv->raiz), key, arv->cmp, 0, arv->k);
}

void _kdtree_destroi(tnode *node) {
    if (node != NULL) {
        _kdtree_destroi(node->esq);
        _kdtree_destroi(node->dir);
        free(node->key);
        free(node);
    }
}

void kdtree_destroi(tarv *arv) {
    _kdtree_destroi(arv->raiz);
}

void _kdtree_busca(tarv *arv, tnode **atual, void *key, int profund, double *menor_dist, tnode **menor) {
    if (*atual != NULL) {
        double dist_atual = arv->dist((*atual)->key, key);
        if (dist_atual < *menor_dist) {
            *menor_dist = dist_atual;
            *menor = *atual;
        }
        int pos = profund % arv->k;
        int comp = arv->cmp(key, (*atual)->key, pos);

        tnode **lado_principal = (comp < 0) ? &((*atual)->esq) : &((*atual)->dir);
        tnode **lado_oposto = (comp < 0) ? &((*atual)->dir) : &((*atual)->esq);

        _kdtree_busca(arv, lado_principal, key, profund + 1, menor_dist, menor);

        float eixo_diff = ((treg *)key)->embedding[pos] - ((treg *)(*atual)->key)->embedding[pos];
        if (eixo_diff * eixo_diff < *menor_dist) {
            _kdtree_busca(arv, lado_oposto, key, profund + 1, menor_dist, menor);
        }
    }
}

tnode *kdtree_busca(tarv *arv, void *key) {
    tnode *menor = NULL;
    double menor_dist = DBL_MAX;
    _kdtree_busca(arv, &(arv->raiz), key, 0, &menor_dist, &menor);
    return menor;
}

treg buscar_mais_proximo(tarv *arv, treg query) {
    double menor_dist = DBL_MAX;
    tnode *menor = NULL;
    _kdtree_busca(arv, &(arv->raiz), &query, 0, &menor_dist, &menor);
    return *((treg *)(menor->key));
}

tarv arvore_global;

tarv *get_tree() {
    return &arvore_global;
}

void inserir_ponto(treg p) {
    treg *novo = malloc(sizeof(treg));
    *novo = p;
    kdtree_insere(&arvore_global, novo);
}

void kdtree_construir() {
    arvore_global.k = 128;
    arvore_global.dist = distancia;
    arvore_global.cmp = comparador;
    arvore_global.raiz = NULL;
}

void test_constroi() {
    tarv arv;
    float emb1[128] = {0};
    float emb2[128] = {1};
    tnode node1, node2;

    node1.key = aloca_reg(emb1, "Pessoa A");
    node2.key = aloca_reg(emb2, "Pessoa B");

    kdtree_constroi(&arv, comparador, distancia, 128);

    assert(arv.raiz == NULL);
    assert(arv.k == 128);
    assert(arv.cmp(node1.key, node2.key, 0) == -1);
    assert(strcmp(((treg *)node1.key)->id, "Pessoa A") == 0);
    assert(strcmp(((treg *)node2.key)->id, "Pessoa B") == 0);

    free(node1.key);
    free(node2.key);
}

int main(void) {
    test_constroi();
    printf("SUCCESS!!\n");
    return EXIT_SUCCESS;
}
