from fastapi import FastAPI #fast api para poder usar o cod no colab
from models import PontoEntrada
from kdtree import inserir_face, reconhecer_face

app = FastAPI()

@app.post("/enviar/")
def enviar(ponto: PontoEntrada):
    try:
        inserir_face(ponto.nome, ponto.embedding) #chama p adicionar na arvore
        return {"status": "inserido com sucesso"}
    except Exception as e:
        return {"erro": str(e)}

@app.post("/reconhecer/")
def reconhecer(ponto: PontoEntrada):
    try:
        resultados = reconhecer_face(ponto.embedding, n=3) #buscar os 3 vizinho mais proximos
        return {"resultados": resultados}
    except Exception as e:
        return {"erro": str(e)}
