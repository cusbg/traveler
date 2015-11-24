#~/bin/bash

FILES="$(ls precomputed/*ps | sed 's@\.ps@@;s@precomputed/@@')"
#DEBUG="--debug"
EXECUTABLE="build/program ${DEBUG}"
DIR=precomputed

fail_function() {
    echo execution failed
    exit 1
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
	    --ps --mapping ${file}.map build/files/${file1}-${file2}.ps \
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

run_tests() {
    init_variables

}

if [ "$1" = "debug" ]
then
    EXECUTABLE="gdb --args ${EXECUTABLE}"
    ${EXECUTABLE}
    exit(0)
fi

run() {
	init_variables

	#run_rted
	#run_gted_full
	run_ps
}


for file1 in $FILES
do
    for file2 in $FILES
    do
	    run&
	#run_rted
	#run_gted_full
	#run_gted_mapping
        #run_ps
    done
done
