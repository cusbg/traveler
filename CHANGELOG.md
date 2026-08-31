# Changelog

The project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

##  [Unreleased]

### Added

### Removed

### Fixed

- `numbering-label` no longer leaks from a labeled point onto subsequent unlabeled points on partially-labeled templates
- `--numbering`/`-n` no longer silently drops the last position in the list (and no longer falls back to defaults when only one position is given)

##  [3.1.0] - 2026-08-26

### Added

- Infernal2mapping supports original secondary structure.
- Support of custom coloring.
- Ability to enrich RNA2D JSON files.
- Ability to specify template-based numbering.
- Support of pseudoknots.
- Pseudoknot connections are now drawn as one line per base pair instead of a single summary line per segment.
- `-pksl`/`--pseudoknot-single-line` option to restore the previous single-summary-line pseudoknot rendering.
- `utils/json2svg.py` now also draws one pseudoknot connection line per base pair by default, with a matching `-pksl`/`--pseudoknot-single-line` opt-out flag.

### Removed

### Fixed

- Canonical basePairType in output JSON
- Removed titles from numbering labels
- Hidden elements in SVG (template labels and lines) showing up 
- Propagation of NaN values in annotations (during the enrichment) to SVG classes

##  [3.0.0]

### Added

- JSON output by supporting the [RNA2D-data-schema](https://github.com/LDWLab/RNA2D-data-schema/)
- svg2json utility

### Removed

- Support of XML output
- Support of non-colored output

### Fixed

