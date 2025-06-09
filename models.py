from pydantic import BaseModel
from typing import List

class PontoEntrada(BaseModel):
    embedding: List[float]  #lista com os 128 floats
    nome: str               #nome pessoa

class PontoSaida(BaseModel):
    embedding: List[float]
    nome: str
