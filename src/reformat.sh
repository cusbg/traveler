#!/bin/sh

SOURCES="$(find . -name *.cpp)"
HEADERS="$(find . -name *.hpp)"
MAKEFILES="$(find ./*/ -name Makefile)"

get_names() {
    file="$1"
    filename="$(echo $file | sed 's@.*/@@')"
}

reformat_makefile() {
    {
        echo "#"
        echo "# Richard Eliáš <richard.elias@matfyz.cz>"
        echo "#"
        echo
        cat ${file} | grep "^MODULE"
        cat ${file} | grep "^ROOTDIR"
        cat ${file} | grep "^RELEASE"
        echo
        echo "%:"
        echo "	make --directory=\${ROOTDIR} --file=Makefile \$@"
        echo
        cat ${file} | grep "^include"
        echo
    } > "${file}.2"
    mv "${file}.2" "${file}"
}

for f in ${MAKEFILES}
do
    get_names "$f"
    reformat_makefile "$f"
done
