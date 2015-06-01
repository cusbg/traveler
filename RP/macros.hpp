
#define FILES                       {"human", "mouse", "rabbit", "frog"}
#define PS_IN(val)                  "build/files/" + val + ".ps"
#define PS_OUT(val1, val2)          "build/files/" + val1 + "-" + val2 + ".out.ps"
#define SVG_OUT(val)                "build/files/" + val + ".svg"
#define SEQ(val)                    "build/files/" + val + ".seq"
#define RNAFOLD(val)                "build/files/" + val + ".RNAfold.fold"
#define FOLD(val)                   "build/files/" + val + ".fold"
#define FOLD_IN(val)                "../InFiles/" + val + ".fold"
#define MAP(val1, val2)             "build/files/mappings/" + val1 + "-" + val2 + ".map"
#define RTED_MAP_FILE(rna1, rna2)   "build/files/mappings/map_" + rna1.name + "-" + rna2.name + ".map"
#define RTED_RUN_FILE(i, name)      "build/files/rted/run_rted."#i"." + name

#define PRINT_MAPPING(vector) \
    for (auto val : vector) \
        cout << val.from << " " << val.to << endl

