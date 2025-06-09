from fastapi import FastAPI
from models import PontoEntrada
from kdtree import inserir_face, reconhecer_face

app = FastAPI()

@app.post("/enviar/")
def enviar(ponto: PontoEntrada):
    try:
        inserir_face(ponto.nome, ponto.embedding)
        return {"status": "inserido com sucesso"}
    except Exception as e:
        return {"erro": str(e)}

@app.post("/reconhecer/")
def reconhecer(ponto: PontoEntrada):
    try:
        resultados = reconhecer_face(ponto.embedding, n=3)
        return {"resultados": resultados}
    except Exception as e:
        return {"erro": str(e)}
