#import Gnuplot
from matplotlib import pyplot as plt
import numpy as np
import sys
dt = np.dtype([('cbrNodes',int),('nodeSpeed',float),('throughput',float)])
a = []
for i in range(1,len(sys.argv)):
  print("loading " + sys.argv[i] + "...")
  # numCBRNodes, nodeSpeed, throughput
  b = np.genfromtxt(open(sys.argv[i],'r'),delimiter=",",skiprows=1, dtype=dt)
  a.append((b['cbrNodes'].item(0),b['nodeSpeed'].item(0),b['throughput'].item(0)))
list.sort(a)
print(a)
setOfNodeSpeeds = set([a1[1] for a1 in a])

# Start plotting
counter = 0
combinations = ['r-', 'g-', 'b-', 'ro', 'go', 'bo']
plt.title('Experiment 1: Throughput of unmodified DSDV protocol')
plt.xlabel('Number of nodes generating CBR traffic')
plt.ylabel('Throughput (pkts rx / pkts tx)')
plt.axis([0,35,0,1.0])
leg = []
for nodeSpeed in setOfNodeSpeeds:
  print(nodeSpeed)
  # add subplot for each node speed
  x = []
  y = []
  for item in [a1 for a1 in a]:
    if nodeSpeed == item[1]:
      x.append(item[0])
      y.append(item[2])
  
  lab = 'Node Mobility Speed of ' + str(nodeSpeed) + 'm/s'
  plt.plot(x,y,combinations[counter%len(combinations)],label=lab)
  plt.plot(x,y,combinations[counter+3%len(combinations)])
  counter+=1
  print([x, y])
#plt.legend(leg)
plt.legend()
plt.show()
