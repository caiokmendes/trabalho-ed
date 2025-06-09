#esse programa permtie integrar o C e o python

import ctypes
from typing import List

DIM = 128
MAX_ID = 100

lib = ctypes.CDLL("./libkdtree.so")  #carregando a biblioteca feita em C

class TReg(ctypes.Structure):
    _fields_ = [
        ("embedding", ctypes.c_float * DIM),
        ("nome", ctypes.c_char * MAX_ID)
    ]

lib.inserir.argtypes = [ctypes.POINTER(TReg)]
lib.inserir.restype = None

lib.buscar_vizinhos.argtypes = [(ctypes.c_float * DIM), ctypes.c_int, ctypes.POINTER(TReg)]
lib.buscar_vizinhos.restype = ctypes.c_int

def inserir_face(nome: str, embedding: List[float]):
    t = TReg()
    for i in range(DIM):
        t.embedding[i] = embedding[i]
    t.nome = nome.encode("utf-8")
    lib.inserir(ctypes.byref(t))

def reconhecer_face(embedding: List[float], n: int = 1) -> List[str]:
    emb = (ctypes.c_float * DIM)(*embedding)
    resultados = (TReg * n)()
    num = lib.buscar_vizinhos(emb, n, resultados)
    return [resultados[i].nome.decode("utf-8") for i in range(num)]
