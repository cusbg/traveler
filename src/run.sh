#!/bin/bash

#FILES1="african_frog mouse rabbit human"
FILES1="african_frog artemia_salina blue_mussel cicadas cicade echinococcus_granulosus fruit_fly human kenyan_frog microciona_prolifera mnemiopsis_leidyi mouse rabbit rat scorpion sea_scallop tripedalia_cystophora"
FILES2="african_frog artemia_salina blue_mussel cicadas cicade echinococcus_granulosus fruit_fly human kenyan_frog microciona_prolifera mnemiopsis_leidyi mouse rabbit rat scorpion sea_scallop tripedalia_cystophora"

EXECUTABLE="build/program ${DEBUG}"
DIR=precomputed

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
	    --ps --mapping ${file}.map --overlaps build/files/${file1}-${file2}.ps \
        || fail_function

}

run_all() {
    init_variables

    ${EXECUTABLE} \
        ${tt} ${mt} \
        --rted --strategies ${file}.rted \
        --gted --ted-out ${file}.ted --mapping ${file}.map \
        --ps ${file}.ps \
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
	done
}

if [ "$1" = "debug" ]
then
    EXECUTABLE="gdb --args ${EXECUTABLE}"
    ${EXECUTABLE}
    exit 0
fi

I=
for file1 in $FILES1
do
	run_loop&
done
