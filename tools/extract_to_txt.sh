#!/bin/bash
for (( i=1971; i<=2010; i++ )); do
	for (( j=0; j<=12; j++ )); do
		cdo outputtab,month,lon,lat,value,nohead "$i/f$j.nc" > "$i/f$j.txt"
	done
done 
