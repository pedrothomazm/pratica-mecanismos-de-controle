import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

def read_results(filename):
    """
    Lê os resultados de um arquivo CSV, assumindo uma estrutura específica sem linha de cabeçalho.
    
    Parâmetros:
        filename (str): O caminho para o arquivo CSV.
        
    Retorna:
        DataFrame: Um DataFrame do pandas contendo os dados lidos.
    """
    return pd.read_csv(filename)


def plot_results(balanced_df, not_balanced_df, title, filename):
    """
    Plota os resultados de execução para versões balanceadas e não balanceadas.
    
    Parâmetros:
        balanced_df (DataFrame): DataFrame contendo os resultados da versão balanceada.
        not_balanced_df (DataFrame): DataFrame contendo os resultados da versão não balanceada.
        title (str): Título para o gráfico.
        visualization_name (str): Nome da imagem salva. 
    """
    plt.figure(figsize = (10, 6))
    
    # Plotando os resultados de cada arquivo.
    plt.plot(balanced_df["Threads"], 
             balanced_df["Tempo"], 
             color = "teal", 
             marker = "o",
             label = "Balanceado", 
             linewidth = 2)
    plt.plot(not_balanced_df["Threads"], 
             not_balanced_df["Tempo"], 
             color = "coral", 
             marker = "o",
             label = "Não Balanceado", 
             linewidth = 2)

    # Ajuste para os limites dos eixos baseados nos dados.
    max_x = max(balanced_df["Threads"].max(), not_balanced_df["Threads"].max())     # Maior valor do eixo X
    max_y = max(balanced_df["Tempo"].max(), not_balanced_df["Tempo"].max())         # Maior valor do eixo Y

    # Define os limites superior e inferior dos eixos
    plt.xlim(0, int(max_x) + int(max_x) / 20)
    plt.ylim(0, int(max_y) + int(max_y) / 5)

    # Define intervalos apropriados nos eixos X e Y, considerando o domínio dos dados
    plt.xticks(np.arange(0, max_x + int(max_x) / 20, 1))
    plt.yticks(np.arange(0, max_y + int(max_y) / 5, 0.1))

    # Personalizando títulos e rótulos dos eixos
    plt.xlabel("Número de Threads", 
               fontsize = 14, 
               color = "darkslategray", 
               fontweight = "bold", 
               fontname = "Arial")
    plt.ylabel("Tempo de Execução (seg)", 
               fontsize = 14, 
               color = "darkslategray", 
               fontweight = "bold", 
               fontname = "Arial")
    plt.title(title, 
              fontsize = 15, 
              pad = 20, 
              color = "darkslategray", 
              fontweight = "bold", 
              fontname = "Arial")

    plt.legend()
    plt.grid(True)
    # Salva a imagem
    plt.savefig(filename, dpi = 300)
    plt.show()


if __name__ == "__main__":
    # Arquivos contendo os dados para plotagem.
    balanced_filename = "threadsBalanced.csv"
    not_balanced_filename = "threadsUnBalanced.csv"
    
    # Lendo os dados dos arquivos e preparando os DataFrames.
    balanced_df = read_results(balanced_filename)
    not_balanced_df = read_results(not_balanced_filename)
    
    # Título do gráfico
    title = "Impacto do Balanceamento de Carga no Desempenho de Processamento"
    visualization_name = "Visualização.png"

    # Gerando o gráfico com os resultados.
    plot_results(balanced_df, not_balanced_df, title, visualization_name)

