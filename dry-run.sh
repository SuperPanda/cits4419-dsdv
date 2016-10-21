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
    # this runs unmodified-dsdv.cc and outputs to unmodifiedDSDV.dat instead 
    # of screen
    #./../waf --run "unmodified-dsdv" > unmodifiedDSDV.dat 2>&1
    # reads in a bunch of output from the simulation, and organises the data needed
    ## python parse-data.py -filein unmodifiedDSDV.dat > unmodifiedDSDV.stat
    # show the progress (update next line accordingly)
    ## tail -n 13 unmodifiedDSDV.dat
    ## cat unmodifiedDSDV.stat
    #./../waf --run "full-updates-only-dsdv" > fullUpdatesDSDV.dat 2>&1
    echo "Starting experiment 2 with ${n} CBR nodes @ ${s}m/s"
    for i in "${experiment2[@]}"
    do
      echo "Experiment 2: disabled updating for ${i} nodes"
      #./../waf --run "disable-nodes-updating-dsdv --num-disabled-nodes ${i}" > disable-${i}-nodes-updating-dsdv.dat 2>&1
      #python parse-data.py --filein disable-${i}-nodes-updating-dsdv.dat >  disable-${i}-nodes-updating-dsdv.stat
      #tail -n [num of relevant lines] [filename.dat]
    done
    echo "Starting experiment 3 with ${n} CBR nodes @ ${s}m/s"
    for i in "${experiment3[@]}"
    do
      echo "Experiment 3: Ignore Route Table Column (probability ${i})"
      #./../waf --run "ignore-routetable-column-dsdv --ignore-probability ${i}" > ignore-${i}-routetable-column-dsdv.dat 2>&1
    done
    echo "Starting experiment 4 with ${n} CBR nodes @ ${s}m/s: Ignoring sequence numbers in DSDV"
    #./../waf --run "ignore-sequence-number-dsdv" > ignore-sequence-number-dsdv.dat 2>&1
  done
done
echo "Done!"
