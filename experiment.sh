## WARNING: Not yet tested
declare -a experiment2=("5" "10" "15")
declare -a experiment3=("0.4" "0.5" "0.6")
declare -a speed=("1" "10" "20")
declare -a cbrNodes=("10" "20" "30")

echo " "
echo "=================================="
echo "==        Experiment 1         =="
echo "================================="
echo " "
for s in "${speed[@]}"
do
  for n in "${cbrNodes[@]}"
  do
    echo "** Experiment 1. Unmodified DSDV protocol (cbrNode=${n}, nodeSpeed=${s} **"
    #./../waf --run "unmodified-dsdv --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment1-dsdv-unmodified-cbrNodes[${n}]-nodeSpeed[${s}].stat"
    echo "** Experiment 1. DSDV protocol with only full updates (cbrNode=${n}, nodeSpeed=${s} **"
    #./../waf --run "full-updates-dsdv-experiment1 --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment1-dsdv-full-updates-cbrNodes[${n}]-nodeSpeed[${s}].stat"
  done
done

echo " "
echo "================================="
echo "==        Experiment 2         =="
echo "================================="
echo " "
for i in "${experiment2[@]}"
do
  for s in "${speed[@]}"
  do
    for n in "${cbrNodes[@]}"
    do
     echo "** Experiment 2. Disabled DSDV updates in ${i} nodes (cbrNode=${n}, nodeSpeed=${s} **"
      #./../waf --run "disable-nodes-updating-dsdv-experiment2 --silentNodes=${i} --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment2-disable-nodes-updating-cbrNodes[${n}]-nodeSpeed[${s}]-silentNode[${i}].stat --printRoutingTable"
    done
  done
done

echo " "
echo "================================="
echo "==        Experiment 3         =="
echo "================================="
echo " "
for i in "${experiment3[@]}"
  do
  for s in "${speed[@]}"
  do
    for n in "${cbrNodes[@]}"
    do
      echo "** Experiment 3: Ignore Route Table Column (probability ${i}) **"
      ./../waf --run "experiment3-randomly-incomplete-route-tables --rtCorruptionProbability=${i} --printRoutingTable --cbrNodes=${n} --nodeSpeed=${s} --statsFileName=experiment3-randomly-incomplete-route-tables-cbrNodes[${n}]-nodeSpeed[${s}]-rtCorruptionProbability[${i}].stat"
    done
  done
done
#echo "Starting experiment 4 [incomplete] with ${n} CBR nodes @ ${s}m/s: Ignoring sequence numbers in DSDV"
 #./../waf --run "ignore-sequence-number-dsdv --cbrNodes=${n} --nodeSpeed=${s}" > ignore-sequence-number-dsdv.dat 2>&1

echo "Done!"
