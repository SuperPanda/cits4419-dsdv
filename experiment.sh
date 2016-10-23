## WARNING: Not yet tested
declare -a experiment2=("5" "10" "15")
declare -a experiment3=("0.4" "0.5" "0.6")
declare -a speed=("1" "10" "20")
declare -a cbrNodes=("10" "20" "30")

for n in "${cbrNodes[@]}"
do
  for s in "${speed[@]}"
  do
    echo "Starting experiment 1 with ${n} CBR nodes @ ${s}m/s"
    echo "Starting simulation with unmodified DSDV protocol"
    #./../waf --run "unmodified-dsdv --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment1-dsdv-unmodified-cbrNodes[${n}]-nodeSpeed[${s}].stat"
    echo "Experiment 1 full updates"
    #./../waf --run "full-updates-dsdv-experiment1 --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment1-dsdv-full-updates-cbrNodes[${n}]-nodeSpeed[${s}].stat"
    echo "Starting experiment 2 with ${n} CBR nodes @ ${s}m/s"
    for i in "${experiment2[@]}"
    do
      echo "Experiment 2 [incomplete]: disabled updating for ${i} nodes"
      ./../waf --run "disable-nodes-updating-dsdv --nDisabledNodes ${i} --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment2-disable-nodes-updating-cbrNodes[${n}]-nodeSpeed[${s}].stat"
    done
    #echo "Starting experiment 3 with ${n} CBR nodes @ ${s}m/s"
    for i in "${experiment3[@]}"
    do
      echo "Experiment 3 [incomplete]: Ignore Route Table Column (probability ${i})"
      #./../waf --run "ignore-routetable-column-dsdv --ignore-probability ${i} --cbrNodes=${n} --nodeSpeed=${s}" > ignore-${i}-routetable-column-dsdv.dat 2>&1
    done
    echo "Starting experiment 4 [incomplete] with ${n} CBR nodes @ ${s}m/s: Ignoring sequence numbers in DSDV"
    #./../waf --run "ignore-sequence-number-dsdv --cbrNodes=${n} --nodeSpeed=${s}" > ignore-sequence-number-dsdv.dat 2>&1
  done
done
echo "Done!"
