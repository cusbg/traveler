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
		<-gs|--target-structure> DBN_FILE
		<-ts|--template-structure [--file-format FILE_FORMAT]> IMAGE_FILE DBN_FILE

	DBN_FILE (Varna/DotBracketNotation) is in format like in example below
	IMAGE_FILE* - visualization of template molecule, type of file can be specified by FILE_FORMAT argument

	OPTIONS:
		[-a|--all] [--overlaps] FILE_OUT
			# compute TED and draw images to FILE_OUT
			# if optional argument --overlaps is present, compute overlaps in image and highlight them
		[-t|--ted <FILE_MAPPING_OUT>]
			# run only TED, save mapping table to file FILE_MAPPING_OUT
		[-d|--draw] [--overlaps] FILE_MAPPING_IN FILE_OUT
			# use mapping in FILE_MAPPING_IN and draw images to FILE_OUT
			# if optional argument --overlaps is present, compute overlaps in image and highlight them

	COLOR CODING:
		We use following color coding of nucleotides in molecule:
		Inserted bases are marked red
		Edited bases are green (e.g. template had base 'A', and other had on same position 'C', so 'C' will
		be green)
		Reinserted bases - program need to redraw simple structure (like hairpin) - are blue
		Rotated - like reinserted, but when redrawing multibranch loop - all branches are rotated to lie on
		circles - they are brown

#### Note:
Only supported type of IMAGE\_FILE is post-script (ps) from [CRW](http://www.rna.icmb.utexas.edu/DAT/3A/Summary/index.php),
but other can be implemented and specified by FILE\_FORMAT argument.

### Example 0.A - download test files
	$ mkdir 18S/
	$ cd 18S/
	$ wget --recursive --no-directories --no-parent \
        http://richard.ba30.eu/traveler/img/input/18S/

In other examples, we will use 18S/ directory as INDIR, OUTDIR will be /tmp/

### Example 0.B - Varna/DBN file format
	$ cat $INDIR/mouse.fasta
		>mouse
		UACCUGGUUGAUCCUGCCAGUAGCAUAUGCUUGUCUCAAAGAUUAAGCCAUGCAUGUCUAAGUACGCACGGCCGGUACAG
		UGAAACUGCGAAUGGCUCAUUAAAUCAGUUAUGGUUCCUUUGGUCGCUCGCUCCUCUCCUACUUGGAUAACUGUGGUAAU
		. . .
		...(((((.......))))).((((((((((.(((((((((.....(((.(((..((...(((....((..........)
		)...)))))......(((......((((..((..((....(((..................((((....(((((((....
		. . .
	FASTA (Varna/DBN) file should contain line starting with '>' and name of molecule, without any blank character
	other lines are filled with LABELS and BRACKETS in dot-bracket notation of secondary structure pairing
	match-tree must contain both LABELS and BRACKETS, templated-tree need only BRACKETS

### Example 1 - prints .svg/.ps image of mouse mapped to human
	$ ./build/traveler \
		--match-tree $INDIR/mouse.fasta \
		--template-tree $INDIR/human.ps $INDIR/human.fasta \
		--all $OUTDIR/mouse_draw-to_human

### Example 2 - compute only distances and mapping between mouse and human
	$ ./build/traveler \
		--match-tree $INDIR/mouse.fasta \
		--template-tree $INDIR/human.ps $INDIR/human.fasta \
		--ted $OUTDIR/mouse_draw-to_human.map

### Example 3 - prints images using precomputed mapping from previous example
	$ ./build/traveler \
		--match-tree $INDIR/mouse.fasta \
		--template-tree $INDIR/human.ps $INDIR/human.fasta \
		--draw --overlaps $OUTDIR/mouse_draw-to_human.map $OUTDIR/mouse_draw-to_human

	$ # generate 4 files - .svg and .ps files, both with/without colored bases (see COLOR CODING section)
	$ # checks also if output molecule has overlaps and draws them in output image


#### Note:
Options --ted and --draw are for separated runs of application, because computations of TED may take a while
and more types of images can be generated (with overlaps).

