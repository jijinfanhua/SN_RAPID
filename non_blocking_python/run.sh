#!/bin/bash
for i in {0..19}
do
  for j in {0..3}
  do
    python3 resubmit_writeback.py $i $j  > log_${i}_${j}.txt &
  done
done