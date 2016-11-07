# cits4419-dsdv
In order to create an experiment, we use the pristine implementation of the DSDV protocol based on model provided by the ns-3 simulator. Experiments are created by copying the pristine folder into the ns3/scratch directory. Graphs can be generated using the ./experiment{N}-graph.sh.

## How to use
This assume you have installed ns-3.25 according to their tutorial.
```
git clone http://github.com/SuperPanda/cits4419-dsdv
cd cits4419-dsdv
./copy-to-scratch <YOUR SCRATCH FOLDER>
cd <YOUR SCRATCH FOLDER>
# Make sure this file has uncommented all experiments you want to run
./experiment.sh
```
We have also provided scripts to allow distributed workloads when running experiments with more data points. Just run 
```
./distributed-experiment.sh <experiment number> <node number>
```
Node number is currently set to a number between 1 and the length of the nodeSpeed parameter array (which is currently 3 elements long: 1m/s, 10m/s, 20m/s).

## DSDV Experiments
See experiment.sh for commands used.
### Experiment 1. Full updates only
Compares the unmodified DSDV protocol vs Full update only implementation.
### Experiment 2. Effects of Greedy Nodes
Compares the effects of 5, 10 and 15 node not forwarding updates.
### Experiment 3. Effects of Incomplete Route Table Information
Compares the throughput of the DSDV protocol with each node having a probability of 0.4, 0.5 and 0.6 that a route table column is missing.
### Experiment 4. Effects of Ignoring the Sequence Numbers
Measures the effects of ignoring the role of sequence numbers in DSDV.
### Experiment 5. Search for optimal periodic update period value
In XYZ's [Add citation here] paper, there was called for further research into finding the optimal period for periodic updates. We have measured the performance of different values and compared each against the default 15s in the DSDV model from NS-3.

## Results from the small-scale experiments
### Experiment 1. Full updates only
![Results](https://github.com/SuperPanda/cits4419-dsdv/blob/master/experiment1-results.png?raw=true)
### Experiment 2. Effects of Greedy Nodes
![Results](https://github.com/SuperPanda/cits4419-dsdv/blob/master/experiment2-results.png?raw=true)
### Experiment 3. Effects of Incomplete Route Table Information
![Results](https://github.com/SuperPanda/cits4419-dsdv/blob/master/experiment3-results.png?raw=true)
### Experiment 4. Effects of Ignoring the Sequence Numbers
![Results](https://github.com/SuperPanda/cits4419-dsdv/blob/master/experiment4-results.png?raw=true)
### Experiment 5. Search for optimal periodic update period value
![Results](https://github.com/SuperPanda/cits4419-dsdv/blob/master/experiment5-results.png?raw=true)
