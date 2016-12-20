#!/bin/bash
echo "enter filename with extension"
read filename
echo "Enter the number of nodes in the network" 
read nodeCount
cover=$filename"_cover_i.txt"
adjlist="Adjlist_"$filename
membershipFile="Membership_"$filename
gcc LINCOM_nsv_disjoint.c -o output
gcc Modularity.c -o modularity
gcc changeLouvain.c -o change
gcc cov_gen.c -o cover_generator
for i in $(seq 0.4 0.05 0.85)
do
	echo "=============================================================="
	echo "INS value is : $i"
	./output $filename $i
	echo "=============================================================="
	echo "Disjoint modularity before mod max"
	./modularity $filename $cover
	mv $cover $i$cover
	echo "=============================================================="
	input="louvain_input_"$filename;
	./convert -i $input -o graph.bin
	./community graph.bin -l -1 -v > graph.tree
	./hierarchy graph.tree
	echo "=============================================================="
	echo "enter the level which you consider for generating final cover"
	read level
	./hierarchy graph.tree -l $level > "final_cover"$i$filename
	echo "Enter the no. of communities in the cover fed to louvain"
	read numberOfComm
	./change "final_cover"$i$filename "FINAL"$i$filename $numberOfComm
	echo "Enter the no. of communities finally detected"
	read detectedComm
	./cover_generator $numberOfComm $nodeCount $membershipFile "FINAL"$i$filename "COVER"$i$filename $detectedComm
	echo "Disjoint modularity after mod max"
	./modularity $filename "COVER"$i$filename
	echo "=============================================================="
done




