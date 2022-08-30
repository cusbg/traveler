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
