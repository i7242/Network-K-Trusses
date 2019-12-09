#!/bin/bash
names=(
facebook_combined.txt
musae_git_edges.csv
soc-Epinions1.txt
Slashdot0811.txt
Slashdot0902.txt
)
echo "using data: ${names[$1]}"

./nucleus ../data/${names[$1]} 23 NO
cat ${names[$1]}_23_K
