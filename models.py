from pydantic import BaseModel
from typing import List

class PontoEntrada(BaseModel):
    embedding: List[float]  # Vetor com 128 floats
    nome: str               # Nome ou ID da pessoa

class PontoSaida(BaseModel):
    embedding: List[float]
    nome: str
