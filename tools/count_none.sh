#!/bin/bash
FILE=$1
i=0
while read line; do
	if [[ ${line##* } == "-9999" ]] || [[ ${line##* } == "1e+20" ]]; then
		(( i++ ))
	fi
done < $FILE
echo $i
