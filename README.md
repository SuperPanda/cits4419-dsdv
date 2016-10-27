# cits4419-dsdv
Modified code to experiment with the DSDV protocol for use with the ns-3 simulator. All code should be slight modifications of unmodified-dsdv.cc or the protocols
## DSDVExperiments
See experiment.sh for commands used.
### Experiment 1. Full updates only
Compares the unmodified DSDV protocol vs Full update only implementation.
### Experiment 2. Effects of Greedy Nodes
Compares the effects of 5, 10 and 15 node not forwarding updates.
### Experiment 3. Effects of Incomplete Route Table Information
Compares the throughput of the DSDV protocol with each node having a probability of 0.4, 0.5 and 0.6 that a route table column is missing.
### Experiment 4. Effects of Ignoring the Sequence Numbers
Measures the effects of ignoring the role of sequence numbers in DSDV.
