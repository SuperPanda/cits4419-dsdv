# cits4419-dsdv
In order to create an experiment, we use the pristine implementation of the DSDV protocol based on model provided by the ns-3 simulator. Experiments are created by copying the pristine folder into the ns3/scratch directory. Graphs can be generated using the ./experiment{N}-graph.sh.
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
