#import Gnuplot

from matplotlib import pyplot as plt
import numpy as np
import sys
from os import listdir
from os.path import isfile, join
listOfDirs = []
listOfExperiments = []
state = 0
for i in range(1,len(sys.argv)):
  if (state == 0):
    listOfExperiments.append(sys.argv[i])
    state = 1
  elif (state == 1):
    listOfDirs.append(sys.argv[i])
    state = 0

combinations = ['r-', 'g-', 'b-', 'ro', 'go', 'bo']
fig = plt.figure()
#ax = fig.add_subplot(111)

dt = np.dtype([('cbrNodes',int),('nodeSpeed',float),('throughput',float)])
allFiles = [[join(d,f) for f in listdir(d) if isfile(join(d,f))] for d in listOfDirs]
for i in range(0,len(allFiles)):
  fileList = allFiles[i]
  a = []
  for f in fileList:
    print("loading " + f + "...")
    # numCBRNodes, nodeSpeed, throughput
    b = np.genfromtxt(open(f,'r'),delimiter=",",skiprows=1, dtype=dt)
    a.append((b['cbrNodes'].item(0),b['nodeSpeed'].item(0),b['throughput'].item(0)))
  list.sort(a)
  setOfNodeSpeeds = set([a1[1] for a1 in a])

  # Start plotting
  counter = 0
  ax = fig.add_subplot(100+len(listOfExperiments)*10+(i+1))
  ax.set_title(listOfExperiments[i])
  #plt.title('Experiment 1: Throughput of unmodified DSDV protocol')
  ax.set_xlabel('Number of nodes generating CBR traffic')
  ax.set_ylabel('Throughput (pkts rx / pkts tx)')
  ax.grid(True)
  ax.axis([0,41,0,1.0])
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
    ax.plot(x,y,combinations[counter%len(combinations)],label=lab)
    ax.plot(x,y,combinations[counter+3%len(combinations)])
    counter+=1
    print([x, y])
#plt.legend(loc='lower center', bbox_to_anchor=(0.5,-5a), fancybox=True, shadow=True, ncol=3)a
#aplt.grid(True)
plt.legend()
plt.show()
