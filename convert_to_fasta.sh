#!/bin/bash

FILES="african_frog artemia_salina blue_mussel cicadas echinococcus_granulosus fruit_fly human kenyan_frog microciona_prolifera mnemiopsis_leidyi mouse rabbit rat scorpion sea_scallop tripedalia_cystophora"
DIR="./InFiles/"

set_length() {
	j=0
	str=$1
	for (( i=0; i<${#str}; i++ ))
	do
		echo -n "${str:$i:1}"
		j="$(expr $j + 1)"
		if [ "$j" = 80 ]
		then
			echo
			j=0
		fi
	done
	echo
}

for file in ${FILES}
do
	f="${DIR}/${file}"
	{
		echo ">${file}"
		set_length "$(cat ${f}.seq)"
		set_length "$(cat ${f}.fold)"
	} > ${f}.fasta
	cat ${f}.fasta
done

