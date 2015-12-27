# ICBCB TESTS

## FILES
archive `icbcb.tar.gz`

## TESTS
We ran application on input molecules from src/ directory.
Test outputs new post-script files saved in output/ directory and log file `overlaps-sorted.log` containing how many overlaps were detected in files.

## CONCLUSION
We can see, that almost 1/3 output molecules are with no crossings, about 2/3 are with less than 3 crossings. In many molecules are detected crossings in unpaired bases list, but there is any (it detects +/- 10 overlaps; for more information see overlap algorithm).
Because of a few bugs in application, many (about 70) molecule pairs dont succeed computation. We will search and remove them in time..

### OVERLAP DETECTION
Our overlap detection algorithm is based on crossings between RNA outer edges and has some defects. If molecule contains list of unpaired bases, it often detects overlaps, but there is any. If there is large overlapping, but only 2 crossings between edges, only 2 are added to log count.

### HOW TO GET OUTPUT FROM INPUT
	As described in man README project page, run:
	$ # SET VARIABLES:
	$ INDIR="src/"
	$ OUTDIR="/tmp/"
	$ TEMPLATE="rabbit"
	$ MATCH="human"
	$ # RUN:
	$ ./build/program \
		--match-tree "$INDIR/$MATCH.seq" "$INDIR/$MATCH.fold" --name "$MATCH" \
		--template-tree "$INDIR/$TEMPLATE.ps" "$INDIR/$TEMPLATE.fold" --name "$TEMPLATE" \
		--all "$OUTDIR/$TEMPLATE-matched-to-$MATCH.ps"
