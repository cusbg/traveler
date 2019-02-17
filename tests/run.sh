#!/bin/bash

TRAVELER_DIR=../bin/

TMP_DIR=data/tmp/
TGT_DIR=data/tgt/
OUT_DIR=out/

TGTS=( human URS0000000306_562 URS00000B1E10_489619-d.16.b.B.japonicum URS00000B9D9D_471852-d.5.b.A.madurae URS00000B14F2_575540-d.5.b.P.brasiliensis URS000000C6FF_36873-d.16.b.Burkholderia.sp URS00000AA4F3_76731-d.16.b.Burkholderia.sp )
TMPS=( fruit_fly d.16.b.E.coli d.16.b.B.japonicum d.5.b.A.madurae d.5.b.P.brasiliensis d.16.b.Burkholderia.sp d.16.b.Burkholderia.sp )

for((i=0;i<${#TGTS[@]};i++))
do
    TGT=${TGTS[$i]}
    TMP=${TMPS[$i]}
    
    echo "`date`: Working on ${TGT} using ${TMP} as a template"
    
    ${TRAVELER_DIR}traveler --target-structure ${TGT_DIR}${TGT}.fasta  --template-structure ${TMP_DIR}${TMP}.ps ${TMP_DIR}/${TMP}.fasta --ted ${OUT_DIR}${TGT}.map
    ${TRAVELER_DIR}traveler --target-structure ${TGT_DIR}${TGT}.fasta  --template-structure ${TMP_DIR}${TMP}.ps ${TMP_DIR}/${TMP}.fasta --draw ${OUT_DIR}${TGT}.map ${OUT_DIR}/${TGT}
done
