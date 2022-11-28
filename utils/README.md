# Traveler utilities

## traveler2xrna

Export of Traveler's XML output to XRNA.

```
traveler2xrna -i test_data/mouse_from_human.colored.xml -o test_data/mouse_from_human.colored.xrna
```

To specify residue font size, label font size or thickness of lines of labels, run:

```
traveler2xrna -i test_data/mouse_from_human.colored.xml -o test_data/mouse_from_human.colored.xrna -rfs 21 -lfs 5 -ll 0.1
```

## json2svg

Converts the Travaler's JSON output (in the [RNA2D data schema format](https://github.com/LDWLab/RNA2D-data-schema/))
to SVG.

If the `-l` flag is set, numbering labels from template will be used instead of sequential positions from target. This
is useful, for example, in case of tRNA where users are used to the Sprinzl positions. Here, for instance, position 20a is
used instead of 21. So if the 21st residue is mapped onto a residue of target with visible label, that label should show 20a 
irrespective of its position in the target.

``
python3 json2svg.py -i test_data/mouse_from_human.json -o test_data/mouse_from_human.json.svg
``

## encrich_json

Takes as input an [RNA2D schema](https://github.com/LDWLab/RNA2D-data-schema/) JSON file and annotations in 
a [TSV file](test_data/mouse_from_human.annotations.tsv) and adds the annotations found in the TSV to the
JSON. The resulting JSON is then either outputted into the console or into a file (the ``-o`` option).
The TSV needs to contain residue index (``residue_index``) and residue name (``residue_name``) columns which specify
which residues should be enriched. This happens by adding the annotations into the ``info`` field of the given
residue. 

If a residue name does not match the name of the corresponding residue in the input JSON, an exception
is raised.

If given annotation is already present for a residue, an exception is raised. This behavior can be overriden by specifying the ``-f`` 
(``--force``) argument.

``
python3 enrich_json.py -ij test_data/mouse_from_human.json -id test_data/mouse_from_human.annotations.tsv
``

