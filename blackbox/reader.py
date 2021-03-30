import matplotlib.pyplot as plt
import sys

try:
  path = sys.argv[1]
except IndexError:
  print('Error: Expected path to blackbox file')
  sys.exit(0)

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

fig, (ax1, ax2) = plt.subplots(2)
fig.suptitle('Light Sensor Readings vs Time')
ax1.set_title('Upper')
ax1.plot(data[3])
ax2.set_title('Lower')
ax2.plot(data[4])
plt.show()
