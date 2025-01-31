import matplotlib.pyplot as plt
import numpy as np
import os
N_points = 1000
n_bins = 20

y = np.arange(1000)
y = y / 200
y = y-2.5
y = -y*y
y = 8* np.exp(y)

fig, ax = plt.subplots()

ax.set_xlabel(f"hour of the day")
ax.set_ylabel("cars parked")
ax.set_title("An Exemplary Day")


ax.plot(np.arange(0, 23, 0.023), y)
fig.savefig("plot.png")
