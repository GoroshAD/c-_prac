import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv('results_parallel.csv')

big = data[data[' n'] == 25000]
small = data[data[' n'] == 200]
proc_big, proc_small = big['p'], small['p']
time_big, time_small = big[' time'], small[' time']
quality_big, quality_small = big[' qual'], small[' qual'] 

plt.figure(figsize=(12, 10))
plt.subplot(2, 2, 1)
plt.plot(proc_big, time_big, marker='o')
plt.title('Время выполнения от количества потоков (N=25000, M=1500)')
plt.xlabel('Количество потоков')
plt.ylabel('Время (с)')
plt.grid(True)

plt.subplot(2, 2, 2)
plt.plot(proc_small, time_small, marker='o')
plt.title('Время выполнения от количества потоков (N=200, M=15)')
plt.xlabel('Количество потоков')
plt.ylabel('Время (с)')
plt.grid(True)

plt.subplot(2, 2, 3)
plt.plot(proc_big, quality_big, marker='o', color='orange')
plt.title('Качество от количества потоков (N=25000, M=1500)')
plt.xlabel('Количество потоков')
plt.ylabel('Качество')
plt.grid(True)

plt.subplot(2, 2, 4)
plt.plot(proc_small, quality_small, marker='o', color='orange')
plt.title('Качество от количества потоков (N=200, M=15)')
plt.xlabel('Количество потоков')
plt.ylabel('Качество')
plt.grid(True)

plt.tight_layout()
plt.savefig('results_parallel.eps', format='eps')