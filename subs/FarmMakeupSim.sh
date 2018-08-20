#!/bin/bash

input=$1
#folder=$1
IFS="_" read -ra folder_split <<< "$input"
echo "${folder_split[0]}"
echo "${folder_split[1]}"
folder=""
for split in "${folder_split[@]}"; do
  folder="$folder$split/" 
done
echo "$folder"
infile=${input}_bad_samples.txt
while read line; do
  echo $line
  stringarray=($line)
  echo ${stringarray[0]}
  sample=${stringarray[0]}
  for i in "${stringarray[@]:1:${#stringarray[@]}}"; do
    echo $i
    makeup=$i
    export MAKEUP=$makeup
    echo $MAKEUP
    jobsub_submit -N 1 -M --OS=SL6 --group=dune --memory=1GB --timeout=2000s -e MAKEUP --resource-provides=usage_model=OPPORTUNISTIC file:///dune/app/users/calcuttj/geant/GeantReweight/subs/$folder/$sample
  done
done <$infile  

#echo $folder $sample $proc
#jobsub_submit -N 1 -M --OS=SL6 --group=dune --memory=1GB --timeout=600s -e MAKEUP --resource-provides=usage_model=OPPORTUNISTIC file:///dune/app/users/calcuttj/geant/GeantReweight/subs/$folder/$sample
