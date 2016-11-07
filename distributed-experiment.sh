experimentId=$1
nodeId=$2
nodeId=$(($nodeId-1))

# Number of uncooperative nodes 
declare -a experiment2=("5" "10" "15")
# Probability of one random column corrupted in a node
declare -a experiment3=("0.3" "0.4" "0.5")
# The update interval time
declare -a experiment5=("5" "10" "20" "25")

declare -a speed=("1" "10" "20")
declare -a cbrNodes=("1" "10" "20" "30" "40" "50")

s=${speed[$nodeId]}
echo "Running experiment $1 with node $nodeId using nodeSpeed $s"

if [[ $experimentId -eq 1 ]]; 
then
  echo " "
  echo "=================================="
  echo "==        Experiment 1          =="
  echo "=================================="
  echo " "
  for n in "${cbrNodes[@]}"
  do
    echo "** Experiment 1. Unmodified DSDV protocol (cbrNode=${n}, nodeSpeed=${s}) **"
    ./../waf --run "unmodified-dsdv --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment1-dsdv-unmodified-cbrNodes[${n}]-nodeSpeed[${s}].stat"
    echo "** Experiment 1. DSDV protocol with only full updates (cbrNode=${n}, nodeSpeed=${s}) **"
    ./../waf --run "full-updates-dsdv-experiment1 --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment1-dsdv-full-updates-cbrNodes[${n}]-nodeSpeed[${s}].stat"
  done
fi

if [[ $experimentId -eq 2 ]]; 
then
  echo " "
  echo "================================="
  echo "==        Experiment 2         =="
  echo "================================="
  echo " "
  for i in "${experiment2[@]}"
  do
    for n in "${cbrNodes[@]}"
    do
     echo "** Experiment 2. Disabled DSDV updates in ${i} nodes (cbrNode=${n}, nodeSpeed=${s}) **"
     ./../waf --run "experiment2-disable-nodes-updating-dsdv --silentNodes=${i} --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment2-disable-nodes-updating-cbrNodes[${n}]-nodeSpeed[${s}]-silentNode[${i}].stat --printRoutingTable"
    done
  done
fi


if [[ $experimentId -eq 3 ]]; 
then
  echo " "
  echo "================================="
  echo "==        Experiment 3         =="
  echo "================================="
  echo " "
  for i in "${experiment3[@]}"
  do
    for n in "${cbrNodes[@]}"
    do
      echo "** Experiment 3: Ignore Route Table Column (rtCorruptionProbability=${i}, cbrNode=${n}, nodeSpeed=${s}) **"
      ./../waf --run "experiment3-randomly-incomplete-routing-tables --rtCorruptionProbability=${i} --printRoutingTable --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment3-randomly-incomplete-routing-tables-cbrNodes[${n}]-nodeSpeed[${s}]-rtCorruptionProbability[${i}].stat"
    done
  done
fi

if [[ $experimentId -eq 4 ]]; 
then
  echo " "
  echo "=================================="
  echo "==        Experiment 4         =="
  echo "================================="
  echo " "
  for n in "${cbrNodes[@]}"
  do
    echo "** Experiment 4. DSDV protocol ignore sequence number (cbrNode=${n}, nodeSpeed=${s}) **"
    ./../waf --run "experiment4-ignore-seq-numbers --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment4-ignore-seq-numbers-cbrNodes[${n}]-nodeSpeed[${s}].stat"
  done
fi

if [[ $experimentId -eq 5 ]]; 
then
  echo " "
  echo "================================="
  echo "==        Experiment 5         =="
  echo "================================="
  echo " "
  for i in "${experiment5[@]}"
  do
    for n in "${cbrNodes[@]}"
    do
     echo "** Experiment 5. Finding optimal periodic update interval (periodicUpdateInterval=${i}, cbrNode=${n}, nodeSpeed=${s}) **"
     ./../waf --run "experiment5-optimal-update-time --periodicUpdateInterval=${i} --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment5-optimal-update-time-cbrNodes[${n}]-nodeSpeed[${s}]-periodicUpdateInterval[${i}].stat --printRoutingTable"
    done
  done
fi


echo "Done!"
