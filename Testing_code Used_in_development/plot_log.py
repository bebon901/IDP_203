import matplotlib.pyplot as plt
import numpy as np

f = open("log.txt", "r")
x = []
y = []
data = f.read().split("\n")
for item in data:
    if not("TURN" in item):
        x.append(item.split(",")[0])
        y.append(item.split(",")[1])

plt.plot(x, y)
plt.savefig("matplotlib.png")
plt.show()
