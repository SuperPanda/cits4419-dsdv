scratchLocation=$1

cp -R experiment1-full-updates-dsdv $scratchLocation/
cp -R experiment2-disable-nodes-updating-dsdv $scratchLocation/
cp -R experiment3-randomly-incomplete-routing-tables $scratchLocation/
cp -R experiment4-ignore-seq-numbers $scratchLocation/
cp -R experiment5-optimal-update-time $scratchLocation/
cp *experiment.sh $scratchLocation/
cp unmodified-dsdv.cc $scratchLocation/


