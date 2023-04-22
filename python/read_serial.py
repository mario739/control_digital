import time
import serial
import numpy as np
import matplotlib.pyplot as plt

tf = 100
ts = 0.1
t = np.arange(0, tf+ts, ts)
fig = plt.figure()
print(t)
q = t.size
p = np.zeros(q)
print(p)
dev = serial.Serial("COM4", 115200)


def risetime(data: np.ndarray, time: np.ndarray):
    v10 = (data.max() - data.min())*.1 + data.min()
    v90 = (data.max() - data.min())*.9 + data.min()
    t10_i = np.argmin(np.abs((data - v10)))
    t90_i = np.argmin(np.abs((data - v90)))
    return time[t90_i] - time[t10_i], (t10_i, t90_i)


print("recolectando datos")

for k in range(q):
    tic = time.time()
    p[k] = dev.readline().decode('ascii')

print(p)
print("datos recolectados")
rs, (t1, t2) = risetime(p, t)
print(rs)
plt.plot(t, p, '-')
plt.plot(t[t1], p[t1], 'o', markersize=20)
plt.plot(t[t2], p[t2], 'o', markersize=20)

plt.show()
