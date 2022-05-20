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

``
python3 json2svg.py -i test_data/mouse_from_human.json -o test_data/mouse_from_human.json.svg
``
