# PROJECT


## Prerequisites:
	No prerequisites are needed

## Compiling:
To compile project, run `make build` in src/ directory.

## Usage:
    traveler [-h|--help]
	traveler [OPTIONS] <TREES>

	TREES:
		<-mt|--match-tree> FASTAFile
		<-tt|--template-tree> PSFile FASTAFile

        FASTAFile is in format like in example below
		PSFile contains post-script file*

		* the only supported .ps format is from database (http://www.rna.icmb.utexas.edu/DAT/3A/Summary/index.php)

	OPTIONS:
		[-a|--all <FILE_OUT>]
        [-t|--ted <FILE_DISTANCES_OUT> <FILE_MAPPING_OUT>]
        [-d|--draw --mapping <FILE_MAPPING_IN> [--overlaps]]
        [--debug]


### Example0 - FASTA file format
    $ INDIR="../InFiles/"
    $ cat $INDIR/mouse.fasta
        >mouse
        UACCUGGUUGAUCCUGCCAGUAGCAUAUGCUUGUCUCAAAGAUUAAGCCAUGCAUGUCUAAGUACGCACGGCCGGUACAG
        UGAAACUGCGAAUGGCUCAUUAAAUCAGUUAUGGUUCCUUUGGUCGCUCGCUCCUCUCCUACUUGGAUAACUGUGGUAAU
        . . .
        ...(((((.......))))).((((((((((.(((((((((.....(((.(((..((...(((....((..........)
        )...)))))......(((......((((..((..((....(((..................((((....(((((((....
        . . .
    FASTA file should contain line starting with '>' and name of molecule
    other lines are filled with LABELS and BRACKETS in dot-bracket notation of secondary structure pairing
    match-tree must contain both LABELS and BRACKETS, templated-tree need only BRACKETS

### Example1 - prints .svg/.ps image of mouse mapped to human
	$ INDIR="../InFiles/"
	$ OUTDIR="/tmp/"
	$ ./build/traveler \
		--match-tree $INDIR/mouse.fasta \
		--template-tree $INDIR/human.ps $INDIR/human.fasta \
		--all $OUTDIR/mouse_to_human

### Example2 - compute only distances and mapping between mouse and human
	$ INDIR="../InFiles/"
	$ OUTDIR="/tmp/"
	$ ./build/traveler \
		--match-tree $INDIR/mouse.fasta \
		--template-tree $INDIR/human.ps $INDIR/human.fasta \
		--ted $OUTDIR/mouse_to_human.ted $OUTDIR/mouse_to_human.map

### Example3 - prints .svg/.ps image using precomputed mapping from Example2
	$ INDIR="../InFiles/"
	$ OUTDIR="/tmp/"
	$ ./build/traveler \
		--match-tree $INDIR/mouse.fasta \
		--template-tree $INDIR/human.ps $INDIR/human.fasta \
		--draw --mapping $OUTDIR/mouse_to_human.map --overlaps
    $ # checks also if output molecule has overlaps and draws them in output image


#### Note:
Options --ted and --draw are for separated runs of application, because computations of TED may take a while.

