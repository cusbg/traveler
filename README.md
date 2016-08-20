# TRAVeLer - Template RnA VisuaLization

## Requirements:
- gcc with support of c++11

## Download:
Use `git clone https://github.com/rikiel/bc traveler` to download project

## Compiling:
    cd traveler/src
    make build

## Usage:
    traveler [-h|--help]
	traveler [OPTIONS] <TREES>

	TREES:
		<-mt|--match-tree> FILE_FASTA
		<-tt|--template-tree [--type DOCUMENT_TYPE]> DOCUMENT FILE_FASTA

        FILE_FASTA is in format like in example below
	DOCUMENT only supported type is .ps format from CRW database*, but other can be implemented and specified by DOCUMENT_TYPE argument

	*(http://www.rna.icmb.utexas.edu/DAT/3A/Summary/index.php)

	OPTIONS:
	[-a|--all [--overlaps] [--colored] <FILE_OUT>]
            # compute TED and draw images to FILE_OUT
            # if optional argument --overlaps is present, compute overlaps in image and highlight them
            # for detailed use of --colored argument, see COLOR CODING section
        [-t|--ted <FILE_MAPPING_OUT>]
            # run only TED, save mapping table to file FILE_MAPPING_OUT
        [-d|--draw [--overlaps] [--colored] <FILE_MAPPING_IN> <FILE_OUT>]
            # use mapping in FILE_MAPPING_IN and draw images to FILE_OUT
            # if optional argument --overlaps is present, compute overlaps in image and highlight them
            # for detailed use of --colored argument, see COLOR CODING section

	COLOR CODING:
	    We use following color coding of nucleotides in molecule:
		Inserted bases are marked red
		Edited bases are green (e.g. template had base 'A', and other had on same position 'C', so 'C' will be green)
		Reinserted bases - program need to redraw simple structure (like hairpin) - are blue
		Rotated - like reinserted, but when redrawing multibranch loop - all branches are rotated to lie on circles - they are brown



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

### Example3 - prints image using precomputed mapping from previous example
	$ INDIR="../InFiles/"
	$ OUTDIR="/tmp/"
	$ ./build/traveler \
		--match-tree $INDIR/mouse.fasta \
		--template-tree $INDIR/human.ps $INDIR/human.fasta \
		--draw --overlaps --mapping $OUTDIR/mouse_to_human.map $OUTDIR/mouse_to_human

    $ # generate .svg and .ps files
    $ # checks also if output molecule has overlaps and draws them in output image


#### Note:
Options --ted and --draw are for separated runs of application, because computations of TED may take a while.

