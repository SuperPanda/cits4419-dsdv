#import Gnuplot

from matplotlib import pyplot as plt
import numpy as np
import sys
from os import listdir
from os.path import isfile, join

listOfDirs = [] # the first directory will be of the control experiment
listOfExperiments = [] # the first experiment is the control experiment

state = 0
for i in range(1,len(sys.argv)):
  if (state == 0):
    listOfExperiments.append(sys.argv[i])
    state = 1
  elif (state == 1):
    listOfDirs.append(sys.argv[i])
    state = 0

combinations = ['r-', 'g-', 'b-', 'ro', 'go', 'bo']
combinations_original = ['r--', 'g--.', 'b--', 'rx', 'gx', 'bx']
fig = plt.figure()
#ax = fig.add_subplot(111)

dt = np.dtype([('silentNodes',int),('cbrNodes',int),('nodeSpeed',float),('throughput',float)])
dt_original = np.dtype([('cbrNodes',int),('nodeSpeed',float),('throughput',float)])
allFiles = [[join(d,f) for f in listdir(d) if isfile(join(d,f))] for d in listOfDirs]
a = []
for i in range(0,len(allFiles)):
  fileList = allFiles[i]
  for f in fileList:
    print("loading " + f + "...")
    # numCBRNodes, nodeSpeed, throughput
    if (i == 0):
      b = np.genfromtxt(open(f,'r'),delimiter=",",skiprows=1, dtype=dt_original)
      a.append((0,b['cbrNodes'].item(0),b['nodeSpeed'].item(0),b['throughput'].item(0)))
    else:
      b = np.genfromtxt(open(f,'r'),delimiter=",",skiprows=1, dtype=dt)
      a.append((b['silentNodes'].item(0),b['cbrNodes'].item(0),b['nodeSpeed'].item(0),b['throughput'].item(0)))
  list.sort(a)


#print(a)
#print("part 1")
#print([(a1[1],a1[2],a1[3]) for a1 in a])
#print("part 2")
#print([(a1[1],a1[2],a1[3])])
originalDataSet = set()
for a1 in a:
  print("a1:")
  print(a1)
  if a1[0] == 0:
    originalDataSet.add((a1[1],a1[2],a1[3]))
print(originalDataSet)
# cbrNodes, nodeSpeeds and throughput
#originalDataSet = set([(a1[1],a1[2],a1[3]) for a1 in a if a1[0] == 0])

#setOfNodeSpeeds = set([a1[2] for a1 in a if a1[0] == 0])
#set(a1[1] for a1 in a if a1[0] == 0)
originalDataMap = {}
for item in originalDataSet:
  keyName = str(item[0]) + "," + str(item[1])
  originalDataMap[keyName] = item[2]
  print("cbrNodes: " + str(item[0]) + "\tnodeSpeed: " + str(item[1]) + "\tthroughput: "+ str(item[2]))
print(originalDataMap)

for i in range(1,len(allFiles)):
  setOfNodeSpeeds = set([a1[2] for a1 in a])
  setOfSilentNodes = set([a1[0] for a1 in a if a1[0] != 0])
  # (cbrNodes,nodeSpeed) -> throughput
  subplot_id = 0
  l1 = None;
  l2 = None;
  l3 = None;
  l4 = None;
  # Start plotting
  for nSilentNodes in setOfSilentNodes:
    subplot_id += 1
    ax = fig.add_subplot(len(setOfSilentNodes)*10+100+subplot_id)
    ax.set_title("Throughput with " + str(nSilentNodes) + " uncooperative nodes")
    #plt.title('Experiment 1: Throughput of unmodified DSDV protocol')
    ax.set_xlabel('Number of nodes generating CBR traffic')
    ax.set_ylabel('Throughput (pkts rx / pkts tx)')
    ax.grid(True)
    ax.axis([0,41,0,1.0])
    line_id = 0
    for nodeSpeed in setOfNodeSpeeds:
      line_id += 1
      print("nodespeed")
      print(nodeSpeed)
      # add subplot for each node speed
      x = []
      y = []
      y_original_result = []
      for item in [a1 for a1 in a]:
        if nodeSpeed == item[2] and nSilentNodes == item[0]:
          x.append(item[1])
          y.append(item[3])
          print("originalDataMap.get("+str(item[1])+","+str(nodeSpeed)+")")
          print(originalDataMap.get(str(item[1])+","+str(nodeSpeed)))
          y_original_result.append(originalDataMap.get(str(item[1])+","+str(nodeSpeed)))
          
      print("x")
      print(x)
      print("y")
      print(y)
      print("y_original_result")
      print(y_original_result)
      lab = 'Node Mobility Speed of ' + str(nodeSpeed) + 'm/s (with uncooperative nodes)'
      l1 = ax.plot(x,y,combinations[(line_id)%len(combinations)],label=lab)
      l2 = ax.plot(x,y,combinations[(line_id+3)%len(combinations)])
      lab = 'Node Mobility Speed of ' + str(nodeSpeed) + 'm/s (with original DSDV implementation)'
      l3  = ax.plot(x,y_original_result,combinations_original[line_id%len(combinations_original)],label=lab)
      x_original = []
      l4 = ax.plot(x,y_original_result,combinations_original[(line_id+3)%len(combinations_original)])   
    print([x, y, y_original_result])
#lt.legend(loc='lower center', bbox_to_anchor=(0.5,-5a), fancybox=True, shadow=True, ncol=3)a
#aplt.grid(True)
#plt.legend(bbox_transform=plt.gcf().transFigure,bbox_to_anchor=(1.0,1.0))
plt.legend()
#plt.tight_layout()
  #fig.legend(l1,l2,loc='bottom')
plt.show()
