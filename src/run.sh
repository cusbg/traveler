#!/bin/bash

ALL="african_frog artemia_salina blue_mussel cicadas echinococcus_granulosus fruit_fly human kenyan_frog microciona_prolifera mnemiopsis_leidyi mouse rabbit rat scorpion sea_scallop tripedalia_cystophora"
FILES1=${ALL}
FILES2=${ALL}
FILES1="african_frog"
FILES2="cicadas"
#FILES2="artemia_salina"

EXECUTABLE="build/traveler"
DIR="/tmp/"
DIR_IMG="build/files"
ONCE=false

#OVERLAPS="--overlaps"

#mv ${EXECUTABLE} ${EXECUTABLE}2
#EXECUTABLE="${EXECUTABLE}2"

if [ "$1" = "debug" ]
then
    EXECUTABLE="gdb --args ${EXECUTABLE}"
    ONCE=true
fi

fail_function() {
    {
        date
        echo "execution of ${file1} <-> ${file2} failed"
    } >> build/logs/fail.log
}

init_variables() {
    file_in="${DIR}/${file1}-${file2}"
    file_out="${DIR_IMG}/${file1}-${file2}"
    debug="--debug"
    tt="--template-tree ${DIR}/${file1}.ps ${DIR}/${file1}.fasta"
    mt="--match-tree ${DIR}/${file2}.fasta"
}

run_ted() {
    ${EXECUTABLE} \
        ${tt} ${mt} ${debug} \
        --ted ${file_out}.map \
        || fail_function
}

run_draw() {
    init_variables

    ${EXECUTABLE} \
        ${tt} ${mt} ${debug} \
	    --draw ${COLORED} ${OVERLAPS} ${file_in}.map ${file_out} \
	|| fail_function
}

run_all() {
    init_variables

    ${EXECUTABLE} \
        ${tt} ${mt} ${debug} \
        --all ${COLORED} ${OVERLAPS} ${file_out} \
        --debug \
        || fail_function
}

run() {
    init_variables

    #run_ted
    run_draw
    #run_all

    #{
        #date
        #echo "run OK ${file1} <-> ${file2}"
    #} >> build/logs/finished.log
}
run_loop() {
	for file2 in $FILES2
	do
		run

        if [ "$ONCE" = true ]
        then
            exit
        fi
	done
}

for file1 in $FILES1
do
	run_loop
done
