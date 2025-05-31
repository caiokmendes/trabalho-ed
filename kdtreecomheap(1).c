#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
    float x, y;
    char nome[100];
} treg;

typedef struct node {
    void *key;
    struct node *esq, *dir;
} tnode;

typedef struct {
    tnode *raiz;
    int (*cmp)(void*, void*, int);
    double (*dist)(void*, void*);
    int k;
} tarv;

treg *aloca_reg(float x, float y, char *nome) {
    treg *p = malloc(sizeof(treg));
    p->x = x;
    p->y = y;
    strcpy(p->nome, nome);
    return p;
}

int comparador(void *a, void *b, int pos) {
    treg *pa = a;
    treg *pb = b;
    if (pos == 0) return (pa->x > pb->x) - (pa->x < pb->x);
    return (pa->y > pb->y) - (pa->y < pb->y);
}

double distancia(void *a, void *b) {
    treg *pa = a;
    treg *pb = b;
    double dx = pa->x - pb->x;
    double dy = pa->y - pb->y;
    return dx * dx + dy * dy; 
}

void _kdtree_insere(tnode **p, void *key, int profund, int k, int (*cmp)(void*, void*, int)) {
    if (*p == NULL) {
        *p = malloc(sizeof(tnode));
        (*p)->key = key;
        (*p)->esq = (*p)->dir = NULL;
        return;
    }
    int pos = profund % k;
    int c = cmp(key, (*p)->key, pos);
    if (c < 0) _kdtree_insere(&(*p)->esq, key, profund+1, k, cmp);
    else _kdtree_insere(&(*p)->dir, key, profund+1, k, cmp);
}

void kdtree_constroi(tarv *arv, int (*cmp)(void*, void*, int), double (*dist)(void*, void*), int k) {
    arv->raiz = NULL;
    arv->cmp = cmp;
    arv->dist = dist;
    arv->k = k;
}

void kdtree_insere(tarv *arv, void *key) {
    _kdtree_insere(&arv->raiz, key, 0, arv->k, arv->cmp);
}

void _kdtree_destroi(tnode *p) {
    if (p == NULL) return;
    _kdtree_destroi(p->esq);
    _kdtree_destroi(p->dir);
    free(p->key);
    free(p);
}

void kdtree_destroi(tarv *arv) {
    _kdtree_destroi(arv->raiz);
    arv->raiz = NULL;
}

typedef struct {
    double dist;
    void *key;
} HeapItem;

typedef struct {
    HeapItem *itens;
    int capacidade;
    int tamanho;
} MaxHeap;

MaxHeap *criar_heap(int capacidade) {
    MaxHeap *h = malloc(sizeof(MaxHeap));
    h->capacidade = capacidade;
    h->tamanho = 0;
    h->itens = malloc(sizeof(HeapItem) * capacidade);
    return h;
}

void destruir_heap(MaxHeap *h) {
    free(h->itens);
    free(h);
}

void heapify_para_baixo(MaxHeap *h, int i) {
    int maior = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq < h->tamanho && h->itens[esq].dist > h->itens[maior].dist)
        maior = esq;
    if (dir < h->tamanho && h->itens[dir].dist > h->itens[maior].dist)
        maior = dir;

    if (maior != i) {
        HeapItem tmp = h->itens[i];
        h->itens[i] = h->itens[maior];
        h->itens[maior] = tmp;
        heapify_para_baixo(h, maior);
    }
}

void heapify_para_cima(MaxHeap *h, int i) {
    if (i == 0) return;
    int pai = (i - 1) / 2;
    if (h->itens[i].dist > h->itens[pai].dist) {
        HeapItem tmp = h->itens[i];
        h->itens[i] = h->itens[pai];
        h->itens[pai] = tmp;
        heapify_para_cima(h, pai);
    }
}

void inserir_heap(MaxHeap *h, double dist, void *key) {
    if (h->tamanho < h->capacidade) {
        h->itens[h->tamanho].dist = dist;
        h->itens[h->tamanho].key = key;
        heapify_para_cima(h, h->tamanho);
        h->tamanho++;
    } else if (dist < h->itens[0].dist) {
        h->itens[0].dist = dist;
        h->itens[0].key = key;
        heapify_para_baixo(h, 0);
    }
}

void _kdtree_busca_n_vizinhos(tarv *arv, tnode *no, void *key, int profund, MaxHeap *heap) {
    if (no == NULL) return;
    double d = arv->dist(no->key, key);
    inserir_heap(heap, d, no->key);

    int pos = profund % arv->k;
    int comp = arv->cmp(key, no->key, pos);

    tnode *lado1 = comp < 0 ? no->esq : no->dir;
    tnode *lado2 = comp < 0 ? no->dir : no->esq;

    _kdtree_busca_n_vizinhos(arv, lado1, key, profund + 1, heap);

    double delta;
    if (pos == 0)
        delta = ((treg*)key)->x - ((treg*)no->key)->x;
    else
        delta = ((treg*)key)->y - ((treg*)no->key)->y;

    if (heap->tamanho < heap->capacidade || (delta * delta) < heap->itens[0].dist)
        _kdtree_busca_n_vizinhos(arv, lado2, key, profund + 1, heap);
}

void buscar_n_vizinhos(tarv *arv, void *key, int n, HeapItem *resultado) {
    MaxHeap *heap = criar_heap(n);
    _kdtree_busca_n_vizinhos(arv, arv->raiz, key, 0, heap);
    for (int i = 0; i < heap->tamanho; i++)
        resultado[i] = heap->itens[i];
    destruir_heap(heap);
}

void test_busca_n_vizinhos() {
    tarv arv;
    kdtree_constroi(&arv, comparador, distancia, 2);
    kdtree_insere(&arv, aloca_reg(10,10,"a"));
    kdtree_insere(&arv, aloca_reg(20,20,"b"));
    kdtree_insere(&arv, aloca_reg(1,10,"c"));
    kdtree_insere(&arv, aloca_reg(3,5,"d"));
    kdtree_insere(&arv, aloca_reg(7,15,"e"));
    kdtree_insere(&arv, aloca_reg(4,11,"f"));

    treg *query = aloca_reg(5,10,"query");

    int n = 3;
    HeapItem resultado[3];
    buscar_n_vizinhos(&arv, query, n, resultado);

    printf("\n%d vizinhos mais pr\303\263ximos de %s:\n", n, query->nome);
    for (int i = 0; i < n; i++) {
        treg *viz = resultado[i].key;
        printf("%s (%.2f)\n", viz->nome, resultado[i].dist);
    }

    free(query);
    kdtree_destroi(&arv);
}

int main() {
    test_busca_n_vizinhos();
    return 0;
}
