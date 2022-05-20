Tests run Traveler for some well-tested or difficult cases, to be able 
to compare how the output changes between versions. For now, the comparison is purely
visual and left to the user. To ease this process there are two scrips: `run.sh` and `compare.sh`.

`run.sh` generates the layouts. As in most cases, only the layouting algorithm 
changes between versions, the `.map` files (based on which the layout is computed) 
are available in the `out` directory and `run.sh` can be executed with the 
`-v` options which skips the mapping step (the most time-consuming part) and lays out
the structures based on the precomputed `.map` files.

WARNING: currently, Traveler generates both SVG and JSON outputs where the JSON can 
then be converted to SVG with the `json2svg` utility. 
As going through JSON will be the preferred way in the future, 
the tests use that path (JSON->SVG).

The second script is `compare.sh` which needs to be run only after the layouts are generated
and creates an html file where it pairs the  ref-` layouts 
(available in the `out` directory) with corresponding generated layouts. 
Thus, when evaluating the changes, one does not need to manually 
inspect the layouts one by one.
