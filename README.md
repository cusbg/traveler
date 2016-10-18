# traveler - Template-based RnA VisuaLization

## Requirements:
- gcc with support of c++11

## Download:
Use `git clone https://github.com/rikiel/traveler` to download project

## Build:
	cd traveler/src
	make build

The binaries will be copied into traveler/bin. To navigate there from the src directory use: `cd ../bin`

## Usage:
	traveler [-h|--help]
	traveler [OPTIONS] <STRUCTURES>

	STRUCTURES:
		<-gs|--target-structure> DBN_FILE
		<-ts|--template-structure [--file-format FILE_FORMAT]> IMAGE_FILE DBN_FILE

	DBN_FILE (Varna/DotBracketNotation) is in format like in example below
	IMAGE_FILE* - visualization of template molecule, type of file can be specified by FILE_FORMAT argument

	OPTIONS:
		[-a|--all] [--overlaps] OUT_PREFIX
			# computes mapping (TED) and outputs target leayout as both .ps and .svg image to files with prefix OUT_PREFIX
			# with the optional --overlaps argument, overlaps in the layout are identified and highlited
		[-t|--ted <FILE_MAPPING_OUT>]
			# runs mapping (TED) only and saves mapping table to FILE_MAPPING_OUT file
		[-d|--draw] [--overlaps] FILE_MAPPING_IN OUT_PREFIX
			# use mapping in FILE_MAPPING_IN and outputs layout as both .ps and .svg image to files with prefix OUT_PREFIX
			# if optional argument --overlaps is present overlaps in the layout are identified and highlighted

	COLOR CODING:
		Traveler uses the following color coding of nucleotides:
		* Red - inserted bases
		* Green - edited bases - e.g. the template has adenosine at a position while target has cytosine at the same position and therefore cytosine will be green)
		* Blue - reinserted bases - happens when traveler needs to redraw simple structures like hairpins (for example due to the change in the number of bases)
		* Brown - rotated parts - similar situation to reinserted bases but takes place when redrawing multibranch loop (in that case all branches are rotated to lie on a circle)

#### Note:

Two types of template IMAGE\_FILE are currectly supported by Traveler:
	
* PostScript (crw) from [CRW](http://www.rna.icmb.utexas.edu/DAT/3A/Summary/index.php)
* VARNA (varna) format of SVG images produced by tool [VARNA](http://varna.lri.fr/)

Other extractors of RNA structure can be implemented and specified by the FILE\_FORMAT argument.

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

### Example 1: Visualize mouse 18S rRNA using human 18S rRNA as template using CRW ps image as the template layout.
	$ ./build/traveler \
		--target-structure $INDIR/mouse.fasta \
		--template-structure $INDIR/human.ps $INDIR/human.fasta \
		--all $OUTDIR/mouse_draw-to_human

### Example 2: Compute TED distance and mapping between human 18S rRNA (template) and mouse 18S rRNA (target).
	$ ./build/traveler \
		--target-structure $INDIR/mouse.fasta \
		--template-structure $INDIR/human.ps $INDIR/human.fasta \
		--ted $OUTDIR/mouse_draw-to_human.map

### Example 3: Generate visualization for the mapping generated in Example 2.
	$ ./build/traveler \
		--target-structure $INDIR/mouse.fasta \
		--template-structure $INDIR/human.ps $INDIR/human.fasta \
		--draw --overlaps $OUTDIR/mouse_draw-to_human.map $OUTDIR/mouse_draw-to_human

	$ # generates 4 files - .svg and .ps files, both with/without colored bases (see COLOR CODING section)
	$ # checks also if output molecule has overlaps and draws them in output image


#### Note:
Options --ted and --draw serve for separatation of mapping and visualization since TED computation and on the other hand, Traveler allows for multiple output visualization (coloring, overlaps).
 
## Support for other imput images: How to implement own extractor
As we said, we support two types of input images - crw and varna. There are three steps you need to satisfy, when you want to support other image types.

* You need to implement `extractor` interface and it's method `extract`. Method should obtain all nucleotides (the primary structure) and their position in image (points) from given file.
* In the class, you need to implement method `get_type`, that should only return extractor's type that is used in IMAGE\_FILE argument.
* Adds your new extractor to method `extractor.get_all_extractors()`

For more ideas, how it should be implemented, see usage of `crw_extractor` and `varna_extractor`.





