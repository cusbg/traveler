#!/bin/bash

ALL="african_frog artemia_salina blue_mussel cicadas echinococcus_granulosus fruit_fly human kenyan_frog microciona_prolifera mnemiopsis_leidyi mouse rabbit rat scorpion sea_scallop tripedalia_cystophora"
FILES1=${ALL}
FILES2=${ALL}
FILES1="artemia_salina"
FILES2="cicadas"
#FILES1=cicadas
#FILES2=echinococcus_granulosus

EXECUTABLE="build/traveler"
DIR=precomputed
ONCE=false

mv ${EXECUTABLE} ${EXECUTABLE}2
EXECUTABLE="${EXECUTABLE}2"

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
    file="${DIR}/${file1}-${file2}"
    tt="--template-tree ${DIR}/${file1}.ps ${DIR}/${file1}.fold --name ${file1}"
    mt="--match-tree ${DIR}/${file2}.seq ${DIR}/${file2}.fold --name ${file2}"
}

run_rted() {
    init_variables

    ${EXECUTABLE} \
        ${tt} ${mt} \
        --rted --strategies ${file}.rted \
        || fail_function
}

run_gted_full() {
    init_variables

    ${EXECUTABLE} \
        ${tt} ${mt} \
        --gted --strategies ${file}.rted --ted-out ${file}.ted --mapping ${file}.map \
        || fail_function

}

run_gted_mapping() {
    init_variables

    ${EXECUTABLE} \
        ${tt} ${mt} \
        --gted --ted-in ${file}.ted --mapping ${file}.map \
        || fail_function
}

run_ps() {
    init_variables

    ${EXECUTABLE} \
	    ${tt} ${mt} \
	    --image --mapping ${file}.map --overlaps build/files/${file1}-${file2} \
        || fail_function
}

run_all() {
    init_variables

    ${EXECUTABLE} \
        ${tt} ${mt} \
        --rted --strategies ${file}.rted \
        --gted --ted-out ${file}.ted --mapping ${file}.map \
        --image ${file} \
        || fail_function
}

run() {
    init_variables

    #run_rted
    #run_gted_full
    run_ps

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
