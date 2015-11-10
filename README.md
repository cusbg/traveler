# PROJECT


## Prerequisites:
	No prerequisites are needed

## Compiling:
To compile project, run `make build` in src/ directory.

## Usage:
	<program> [OPTIONS] <TREES>

	TREES:
		<-mt|--match-tree> SEQFile FOLDFile
		<-tt|--template-tree> PSFile FOLDFile

		SEQFile contains RNA sequence
		FOLDFile contains folding in dot-bracket notation
		PSFile contains post-script file*

		* the only supported .ps format is from database (http://www.rna.icmb.utexas.edu/DAT/3A/Summary/index.php)

	OPTIONS:
		[-a|--all <FILE-OUT-PS>]
		[-r|--rted [--strategies <FILE-OUT>]]
		[-g|--gted
			[--strategies <FILE-IN>]
			[--ted-in <FILE-IN>]
			[--ted-out <FILE-OUT>]
				[--mapping <FILE-OUT>]]
		[--ps [--mapping <FILE-IN>] <FILE-OUT>]

### Example:
	$ INDIR="../InFiles/"
	$ OUTDIR="/tmp/"
	$ ./build/program \
		--match-tree $INDIR/frog.seq $INDIR/frog.fold --name frog \
		--template-tree $INDIR/human.ps $INDIR/human.fold --name human \
		--all $OUTDIR/frog_to_human.ps

#### Note:
Options --gted, --rted, --ps are for separated runs of application, because computations may take a while.
