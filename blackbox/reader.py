import matplotlib.pyplot as plt
import sys
from numpy import sqrt


## Plots layout
fig = plt.figure()
fig.suptitle('Blackbox Sensor Readings vs Time')
gs = fig.add_gridspec(3, 2)

map_ax = fig.add_subplot(gs[:, 1])
map_ax.set_title('Odometry State (in)')
map_ax.set_aspect('equal')
map_ax.set_xlabel('X')
map_ax.set_ylabel('Y')

uls_ax = fig.add_subplot(gs[0,0])
uls_ax.set_title('Upper Light Sensor')

lls_ax = fig.add_subplot(gs[1,0])
lls_ax.set_title('Lower Light Sensor')

speed_ax = fig.add_subplot(gs[2,0])
speed_ax.set_title('Robot Speed (in/sec)')


## Read and plot data
colors = ['C' + str(n) for n in range(0,10)]
for path, color in zip(sys.argv[1:], colors):
  data = []
  with open(path) as file:
    for line in file:
      blockstr = line.split(',')
      block = []
      for point in blockstr:
        try:
          block.append(float(point))
        except ValueError:
          pass
      data.append(block)

  map_ax.scatter(data[0], data[1], c=color, label=path)
  uls_ax.plot(data[3], c=color)
  lls_ax.plot(data[4], c=color)
  speed_data = []
  for i in range(1, len(data[0])):
    speed_data.append(sqrt((data[0][i] - data[0][i-1])**2 + (data[1][i] - data[1][i-1])**2) / .1)
  speed_ax.plot(speed_data, c=color)

map_ax.legend()

plt.show()
