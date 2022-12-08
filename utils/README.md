# Traveler utilities

## infernal2mapping.py

Takes the Infernal target-template information (`.afa` file) and generates mapping (see the main README for more details).
The input file contains the template sequence and information about mapping of its secondary structure onto the template
seoncary structure.

```
infernal2mapping.py -i test_data/URS00008E3949.traveler.afa -o test_data/URS00008E3949.map
```

However, due to the depairing process introduced in the R2DT pipeline, it can happen that the template structure
actually does not correctly maps to the sequence (some positions which should be paired, are not paired - see the rules
in the header of the `infernal2mapping.py` script) and the resulting mapping is actually not valid (the mapping proces
does not fail, but Traveler does). Thus, it is possible to add additional line that specifies
the original target structure.

```
infernal2mapping.py -i test_data/URS00008E3949.traveler.orig_ss.afag -o test_data/URS00008E3949.map
```

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

Passing a params file specifying the custom coloring of the diagram is possible. The coloring happens based on attributes
defined in the input json file in the `info` value of a residue. The params file contains the name of the attribute 
value, which determines the colors, plus RGB values for each such value. For example, ``enrich_json`` (see below) can add posterior 
probabilities from the Infernal `cmscan` utility. Then, the params file can look like this:


```
{
  "coloring": {
    "posterior_probability": {
      "label": "Infernal posterior probability",
      "values": {        
        "0.0": "rgb(255, 0, 0)",
        "1.0": "rgb(234, 0, 0)",
        "2.0": "rgb(213, 0, 0)",
        "3.0": "rgb(191, 0, 0)",
        "4.0": "rgb(170, 0, 0)",
        "5.0": "rgb(149, 0, 0)",
        "6.0": "rgb(128, 0, 0)",
        "7.0": "rgb(106, 0, 0)",
        "8.0": "rgb(85, 0, 0)",
        "9.0": "rgb(64, 0, 0)",
        "10.0": "rgb(43, 0, 0)",
        "*": "rgb(0, 0, 0)"
      }
    }
  }
}
```

Such definition could then be passed to `json2svg`:

```
python3 json2svg.py -p test_data/params.json -i test_data/URS000080E2F0_93929-RF01734.enriched.json -o test_data/URS000080E2F0_93929-RF01734.enriched.svg
python3 json2svg.py -p test_data/params.json -i test_data/URS00002A2E83_10090-HS_SSU_3D.enriched.json -o test_data/URS00002A2E83_10090-HS_SSU_3D.enriched.svg
```
The coloring definition will be turned into SVG classes, which will be assigned to corresponding
residues. To switch between the original coloring and the user-provided coloring, you will need to invalidate the class
`color-*` definitions in the SVG. 

Theoretically, one could provide multiple annotations based on which the diagram can be colored. However, switching
between them could be more complicated.

Information about the value of the parameters used for coloring is provided in the residue titles, which show upon 
hovering.

One limitation is that currently, only discrete values are supported, i.e., one needs to provide color definitions
for every possible value of the attribute.

## enrich_json

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

