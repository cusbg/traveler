#!/bin/bash

TRAVELER_DIR=../bin/

TMP_DIR=data/tmp/
TGT_DIR=data/tgt/
OUT_DIR=out/

visual_only=false
while getopts v flag
do
    case "${flag}" in
        v) visual_only=true;;
    esac
done


TGTS=( human URS0000000306_562 URS00000B1E10_489619-d.16.b.B.japonicum URS00000B9D9D_471852-d.5.b.A.madurae URS00000B14F2_575540-d.5.b.P.brasiliensis URS000000C6FF_36873-d.16.b.Burkholderia.sp URS00000AA4F3_76731-d.16.b.Burkholderia.sp )
TMPS=( fruit_fly d.16.b.E.coli d.16.b.B.japonicum d.5.b.A.madurae d.5.b.P.brasiliensis d.16.b.Burkholderia.sp d.16.b.Burkholderia.sp )

for((i=0;i<${#TGTS[@]};i++))
do
    TGT=${TGTS[$i]}
    TMP=${TMPS[$i]}

    echo "`date`: Working on ${TGT} using ${TMP} as a template"

    if [ "$visual_only" = false ] ; then
        ${TRAVELER_DIR}traveler --target-structure ${TGT_DIR}${TGT}.fasta  --template-structure ${TMP_DIR}${TMP}.ps ${TMP_DIR}/${TMP}.fasta --ted ${OUT_DIR}${TGT}.map
    fi
    ${TRAVELER_DIR}traveler --target-structure ${TGT_DIR}${TGT}.fasta  --template-structure ${TMP_DIR}${TMP}.ps ${TMP_DIR}/${TMP}.fasta --draw ${OUT_DIR}${TGT}.map ${OUT_DIR}/${TGT}
done

TGTS=( J01436.1456.1522 add_bp_to_stem add_bp_to_stem_pn )
TMPS=( RF00005_Eukaryota-5E6M base base )

for((i=0;i<${#TGTS[@]};i++))
do
    TGT=${TGTS[$i]}
    TMP=${TMPS[$i]}

    echo "`date`: Working on ${TGT} using ${TMP} as a template"

    if [ "$visual_only" = false ] ; then
        ${TRAVELER_DIR}traveler --target-structure ${TGT_DIR}${TGT}.fasta  --template-structure --file-format varna ${TMP_DIR}${TMP}.svg ${TMP_DIR}/${TMP}.fasta --ted ${OUT_DIR}${TGT}.map
    fi
    ${TRAVELER_DIR}traveler --target-structure ${TGT_DIR}${TGT}.fasta  --template-structure --file-format varna ${TMP_DIR}${TMP}.svg ${TMP_DIR}/${TMP}.fasta --draw ${OUT_DIR}${TGT}.map ${OUT_DIR}/${TGT}
done

TGTS=( URS00008E3949_44689-DD_28S_3D Oceanobacillus_iheyensis-EC_SSU_3D )
TMPS=( DD_28S_3D EC_SSU_3D )

for((i=0;i<${#TGTS[@]};i++))
do
    TGT=${TGTS[$i]}
    TMP=${TMPS[$i]}

    echo "`date`: Working on ${TGT} using ${TMP} as a template"

    if [ "$visual_only" = false ] ; then
        ${TRAVELER_DIR}traveler --target-structure ${TGT_DIR}${TGT}.fasta  --template-structure --file-format traveler ${TMP_DIR}${TMP}.tr ${TMP_DIR}/${TMP}.fasta --ted ${OUT_DIR}${TGT}.map
    fi
    ${TRAVELER_DIR}traveler --target-structure ${TGT_DIR}${TGT}.fasta  --template-structure --file-format traveler ${TMP_DIR}${TMP}.tr ${TMP_DIR}/${TMP}.fasta --draw ${OUT_DIR}${TGT}.map ${OUT_DIR}/${TGT}
done

TGTS=( URS000075EC78_9606-HS_LSU_3D URS000080E357_9606-mHS_LSU_3D )
TMPS=( HS_LSU_3D mHS_LSU_3D )

for((i=0;i<${#TGTS[@]};i++))
do
    TGT=${TGTS[$i]}
    TMP=${TMPS[$i]}

    echo "`date`: Working on ${TGT} using ${TMP} as a template"

    ${TRAVELER_DIR}traveler --target-structure ${TGT_DIR}${TGT}.fasta  --template-structure --file-format traveler ${TMP_DIR}${TMP}.tr ${TMP_DIR}/${TMP}.fasta --draw ${TGT_DIR}/${TGT}.map ${OUT_DIR}/${TGT}
done

for f in out/*.json; do python3 ../utils/json2svg.py -i $f -o ${f/.json/.json.svg}; done

