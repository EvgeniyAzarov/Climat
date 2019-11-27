#!/bin/bash
i=4
for file in *.nc; do
	name="f$(($i/4))_"
	echo "$name"
	cdo splityear $file $name
	i=$(($i+1))
done
