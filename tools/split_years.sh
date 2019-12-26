#!/bin/bash
i=4
for file in tas*.nc; do
	name="f$(($i/4))_"
	echo "$name" "$i"
	cdo splityear $file $name
	i=$(($i+1))
done

for (( j=1971; j<=2010; j++ )); do
	mkdir "../$j"
	for (( k=0; k<=14; k++ )); do
		mv "f${k}_$j.nc" "../$j/f$k.nc"
	done
done
