import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

data = pd.read_csv('results_cauchy_parallel.csv')

def create_heatmap(data, title, eps_filename):
    heatmap_data = data.pivot_table(index=" m", columns="n", values=" time")
    
    plt.figure(figsize=(10, 8))
    sns.heatmap(heatmap_data, cmap="Reds", annot=False, fmt=".2f", cbar=True)

    plt.title(title)
    plt.xlabel("N (количество работ)")
    plt.ylabel("M (количество процессоров)")
    plt.gca().invert_yaxis()
    plt.savefig(eps_filename, format='eps', dpi=300)
    plt.close()

for algo, title, eps_filename in [
    #('Boltzmann', 'Тепловая карта для алгоритма Больцмана', 'boltzmann_heatmap.eps'),
    ('Cauchy', 'Тепловая карта для алгоритма Коши', 'cauchy_heatmap.eps'),
    #('Logarithmic Cauchy', 'Тепловая карта для логарифмического алгоритма Коши', 'log_cauchy_heatmap.eps')
]:
    create_heatmap(data, title, eps_filename)