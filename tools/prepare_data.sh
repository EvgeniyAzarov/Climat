#!/bin/bash
for file in ./*.nc
do
	tmp="${file%.nc}_tmp.nc"
	cdo sellonlatbox,22.125,40.125,44.375,52.375 $file $tmp
	cdo remapbil,ukr_eobs_01.grid $tmp $file
	cdo subc,273.15 $file $tmp
	cdo maskregion,ukraine.txt $tmp $file
	rm $tmp
#	mv $tmp $file
done
