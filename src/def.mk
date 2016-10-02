#
# Richard Eliáš <richard.elias@matfyz.cz>
#

LOG_FILE				= "/tmp/traveler.log"

CC                      = g++
DEBUG                   = -g -Wall
CFLAGS                  = -std=c++11 -c ${DEBUG} ${RELEASE} -I${ROOTDIR}/include/ -I${ROOTDIR}/include/tests/ -DLOG_FILE=\\\"${LOG_FILE}\\\"
LFLAGS                  = ${DEBUG} ${RELEASE} -std=c++11
SHELL                   = /bin/bash -o pipefail

