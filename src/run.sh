#~/bin/bash

FILES="$(ls precomputed/*ps | sed 's@\.ps@@;s@precomputed/@@')"
EXECUTABLE=build/program
DIR=precomputed

fail_function() {
    echo execution failed
    exit 1
}

run_rted() {
    file1=$1
    file2=$2
    file="${DIR}/${file1}-${file2}"
    tt="--template-tree ${DIR}/${file1}.ps ${DIR}/${file1}.fold --name ${file1}"
    mt="--match-tree ${DIR}/${file2}.seq ${DIR}/${file2}.fold --name ${file2}"

    ${EXECUTABLE} \
            ${tt} ${mt} \
            --rted --strategies ${file}.rted \
            || fail_function
}

run_gted() {
    file1=$1
    file2=$2
    file="${DIR}/${file1}-${file2}"
    tt="--template-tree ${DIR}/${file1}.ps ${DIR}/${file1}.fold --name ${file1}"
    mt="--match-tree ${DIR}/${file2}.seq ${DIR}/${file2}.fold --name ${file2}"

    #${EXECUTABLE} \
            #${tt} ${mt} \
            #--gted --strategies ${file}.rted --ted-out ${file}.ted \
            #|| fail_function

    ${EXECUTABLE} \
            ${tt} ${mt} \
            --gted --ted-in ${file}.ted --mapping ${file}.map \
            || fail_function
}

run_ps() {
    file1=$1
    file2=$2
    file="${DIR}/${file1}-${file2}"
    tt="--template-tree ${DIR}/${file1}.ps ${DIR}/${file1}.fold --name ${file1}"
    mt="--match-tree ${DIR}/${file2}.seq ${DIR}/${file2}.fold --name ${file2}"

    ${EXECUTABLE} \
	    ${tt} ${mt} \
	    --ps --mapping ${file}.map build/files/${file1}-${file2}.ps

}

run_all() {
    file1=$1
    file2=$2
    DIROUT=build/files/run/
    file="${DIROUT}/${file1}-${file2}"
    tt="--template-tree ${DIR}/${file1}.ps ${DIR}/${file1}.fold --name ${file1}"
    mt="--match-tree ${DIR}/${file2}.seq ${DIR}/${file2}.fold --name ${file2}"

    mkdir -p ${DIROUT}

    ${EXECUTABLE} \
        ${tt} ${mt} \
        --rted --strategies ${file}.rted \
        --gted --ted-out ${file}.ted --mapping ${file}.map \
        --ps ${file}.ps
}

f() {
    run_all "frog" "human"

    exit 0
}

if [ "$1" = "debug" ]
then
    EXECUTABLE="gdb --args ${EXECUTABLE}"
fi

f

for file1 in $FILES
do
    for file2 in $FILES
    do
	#run_gted ${file1} ${file2}
	run_ps ${file1} ${file2}
    done
done
