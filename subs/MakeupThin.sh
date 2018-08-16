#!/bin/bash

folder=$1
sample=$2
proc=$3

export MAKEUP=$proc
echo $MAKEUP

echo $folder $sample $proc
#jobsub_submit -N 1 -M --OS=SL6 --group=dune --memory=1GB --timeout=600s -e $proc --resource-provides=usage_model=OPPORTUNISTIC file:///dune/app/users/calcuttj/geant/GeantReweight/subs/$folder/$sample
jobsub_submit -N 1 -M --OS=SL6 --group=dune --memory=1GB --timeout=600s -e MAKEUP --resource-provides=usage_model=OPPORTUNISTIC file:///dune/app/users/calcuttj/geant/GeantReweight/subs/$folder/$sample
