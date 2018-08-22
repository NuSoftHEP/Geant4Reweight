#!/bin/bash

input=$1
folder=$2 #Sim or Thin, etc.
timeout=$3
memory=$4

echo "$folder"
infile=${input}
while read line; do
  echo $line
  stringarray=($line)
  echo ${stringarray[0]}
  sample=${stringarray[0]}
  export INPUTDIR="${folder}/${sample}"
  echo ${INPUTDIR}
  export INPUTFILE=${stringarray[1]}
  echo ${INPUTFILE}
  export INEL=${stringarray[2]}
  echo ${INEL}
  export ELAST=${stringarray[3]}
  echo ${ELAST}
  export OUTPUTFILE=${stringarray[4]}
  echo ${OUTPUTFILE}
  
  jobsub_submit -N 1 -M --OS=SL6 --group=dune --memory=$memory --timeout=$timeout -e INPUTDIR -e INPUTFILE -e OUTPUTFILE -e INEL -e ELAST --resource-provides=usage_model=OPPORTUNISTIC file:///dune/app/users/calcuttj/geant/GeantReweight/subs/rw_sub_base.txt
 # done
done <$infile  

#echo $folder $sample $proc
#jobsub_submit -N 1 -M --OS=SL6 --group=dune --memory=1GB --timeout=600s -e MAKEUP --resource-provides=usage_model=OPPORTUNISTIC file:///dune/app/users/calcuttj/geant/GeantReweight/subs/$folder/$sample
