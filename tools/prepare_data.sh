#!/bin/bash
for file in ./*.nc; do
	tmp="${file%.nc}_tmp.nc"
#	cdo sellonlatbox,19,37,47,55 $file $tmp
	mv $file $tmp
	cdo remapbil,ukr_eobs_01.grid $tmp $file
	cdo sellonlatbox,22.125,40.125,44.375,52.375 $file $tmp
	cdo subc,273.15 $tmp $file
	cdo maskregion,ukraine.txt $file $tmp
	mv $tmp $file
	rm $tmp
done
