"""
Reads Traveler's output layout XML and outputs XRNA XML version of the layout (http://rna.ucsc.edu/rnacenter/xrna/e.coli_5s.xrna.html).

Traveler XML output file looks like:

<structure>
<point x="138.308" y="-374.659" b="5'" id="0"/>
<point x="138.308" y="-366.664" b="U" id="1"/>
<point x="138.308" y="-358.668" b="A" id="2"/>
<point x="138.308" y="-350.673" b="C" id="3"/>
<point x="138.5" y="-342.68" b="C" id="4"/>
<line fromX="137.25" fromY="-336.839" toX="134.75" toY="-327.161" fromIx="3" toIx="19"/>
<point x="146.24" y="-340.68" b="U" id="5"/>
<line fromX="144.992" fromY="-334.838" toX="142.498" toY="-325.162" fromIx="4" toIx="18"/>
<point x="153.99" y="-338.68" b="G" id="6"/>
<line fromX="152.74" fromY="-332.839" toX="150.24" toY="-323.161" fromIx="5" toIx="17"/>
<point x="161.74" y="-336.69" b="G" id="7"/>
<line fromX="160.49" fromY="-330.849" toX="157.99" toY="-321.161" fromIx="6" toIx="16"/>
<point x="169.48" y="-334.69" b="U" id="8"/>
<line fromX="168.233" fromY="-328.848" toX="165.737" toY="-319.162" fromIx="7" toIx="15"/>
<point x="177.1" y="-340.6" b="U" id="9"/>
<point x="187.3" y="-340.1" b="G" id="10"/>
<point x="197.212" y="-361.942" b="10" associatedId="10"/>
<line fromX="188.952" fromY="-343.74" toX="194.491" toY="-355.946" associatedId="10"/>
<point x="194.7" y="-333" b="A" id="11"/>
<point x="195.84" y="-322.49" b="U" id="12"/>
<point x="188.25" y="-319.96" b="C" id="13"/>
<point x="180.66" y="-317.43" b="C" id="14"/>
<point x="172.6" y="-315.5" b="U" id="15"/>
<point x="164.49" y="-315.32" b="G" id="16"/>
<point x="156.74" y="-317.32" b="C" id="17"/>
<point x="148.99" y="-319.32" b="C" id="18"/>
<point x="141.25" y="-321.32" b="A" id="19"/>
<point x="133.5" y="-323.32" b="G" id="20"/>
<point x="127.502" y="-300.096" b="20" associatedId="20"/>
<line fromX="132.5" fromY="-319.449" toX="129.051" toY="-306.093" associatedId="20"/>
<point x="126.7" y="-325.1" b="U" id="21"/>
<point x="120.11" y="-328.92" b="A" id="22"/>
<line fromX="117.056" fromY="-323.961" toX="110.944" toY="-316.039" fromIx="21" toIx="652"/>
<point x="113.78" y="-333.8" b="G" id="23"/>
...
</structure>
"""

import sys
import argparse
import logging
import xml.etree.ElementTree as ET
import os
import io

from typing import List



class SETTINGS:
    RESIDUE_FONT_SIZE = 10
    LABEL_FONT_SIZE = 8
    LABEL_LINE_THICKNESS = 0.2

class COLOR:
    BLACK = "000000"
    RED = "FF0000"
    GREEN = "00FF00"
    BLUE = "0000FF"
    BROWN = "231709"


class Residue:
    def __init__(self, attrs):
        self.x = float(attrs['x'])
        self.y = float(attrs['y'])
        self.label = attrs['b']
        self.id = attrs['id']
        self.color = attrs['color']


def residue_color(r: Residue) -> str:
    if r.color.upper() == "BLACK":
        return COLOR.BLACK
    if r.color.upper() == "RED":
        return COLOR.RED
    if r.color.upper() == "GREEN":
        return COLOR.GREEN
    if r.color.upper() == "BLUE":
        return COLOR.BLUE
    if r.color.upper() == "brown":
        return COLOR.BROWN


class Label:
    def __init__(self, attrs, relative_to:Residue):
        self.relative_to = relative_to
        self.label = attrs['label'] if 'label' in attrs else attrs['b'] # 'b' in case this is the 5' or 3' node
        self.x = float(attrs['x']) - relative_to.x + len(self.label) * SETTINGS.LABEL_FONT_SIZE / 8
        self.y = float(attrs['y']) - relative_to.y
        self.id = attrs['associatedId']
        self.x1 = None
        self.y1 = None
        self.x2 = None
        self.y2 = None

    def add_line(self, attrs):
        self.x1 = float(attrs['fromX']) - self.relative_to.x - len(self.label) * SETTINGS.LABEL_FONT_SIZE / 8
        self.y1 = float(attrs['fromY']) - self.relative_to.y
        self.x2 = float(attrs['toX']) - self.relative_to.x
        self.y2 = float(attrs['toY']) - self.relative_to.y


class BasePair:
    def __init__(self, attrs):
        self.x1 = attrs['fromX']
        self.y1 = attrs['fromY']
        self.x2 = attrs['toX']
        self.y2 = attrs['toY']
        self.id1 = int(attrs['fromIx'])
        self.id2 = int(attrs['toIx'])

class Structure:
    def __init__(self, residues: List[Residue], labels: List[Label], bps: List[BasePair]):
        self.residues: List[Residue] = residues
        self.labels: List[Label] = labels
        self.base_pairs: List[BasePair] = bps

    def get_center(self) -> List:
        xs = [r.x for r in self.residues]
        ys = [r.y for r in self.residues]
        return [(max(xs) + min(xs)) / 2, (max(ys) + min(ys)) / 2]


def get_structure(xml_root) -> Structure:
    residues: List[Residue] = []
    labels: List[Label] = []
    bps: List[BasePair] = []

    for node in xml_root:
        attrs = node.attrib
        if node.tag == 'point':
            if "'" in attrs['b']:
                continue

            if 'id' in attrs:
                residues.append(Residue(attrs=attrs))
            else:
                # the label is associated with the last inserted residues TODO: (this should be correctly implemented by checking the associatedId)
                labels.append(Label(attrs=attrs, relative_to=residues[-1]))
        else: #line
            if 'associatedId' in attrs:
                # it is a line corresponding to the last inserted label
                labels[-1].add_line(attrs=attrs)
            else:
                bps.append(BasePair(attrs=attrs))

    return Structure(residues=residues, labels=labels, bps=bps)


# def get_labels(xml_root):
#     NotImplemented


def read_traveler_xml(f) -> Structure:

    tree = ET.fromstring(f.read())

    # residues = get_residues(tree)
    # labels = get_labels(tree)

    return get_structure(tree)


def convert_to_xrna(name: str, structure: Structure) -> str:

    # Header
    center = structure.get_center()
    xrna = "<ComplexDocument Name='{}'>\n".format(name)
    xrna += "<SceneNodeGeom CenterX='{}' CenterY='{}' />\n".format(-center[0], -center[1])
    xrna += "<Complex Name='{}'>\n".format(name)
    xrna += "<RNAMolecule Name='{}'>\n".format(name)

    # NucListData
    xrna += "<NucListData StartNucID='1' DataType='NucChar.XPos.YPos'>\n"
    for r in structure.residues:
        xrna += "{} {} {}\n".format(r.label, r.x, r.y)
    xrna += "</NucListData>\n"

    #Nuc

    # xrna += "<Nuc RefIDs='1-{}' Color='0' FontID='0' FontSize='{}' />\n".format(len(structure.residues)+1, SETTINGS.RESIDUE_FONT_SIZE)

    color_prev = None
    ix_prev = 1
    ix = 0
    for r in structure.residues:
        ix += 1
        color = residue_color(r)
        if color != color_prev and color_prev is not None:
            xrna += "<Nuc RefIDs='{}-{}' Color='{}' FontID='0' FontSize='{}' />\n".format(ix_prev, ix - 1, color_prev, SETTINGS.RESIDUE_FONT_SIZE)
            ix_prev = ix
        color_prev = color
    xrna += "<Nuc RefIDs='{}-{}' Color='{}' FontID='0' FontSize='{}' />\n".format(ix_prev, ix - 1, color_prev, SETTINGS.RESIDUE_FONT_SIZE)

    xrna += "<Nuc RefIDs='1-{}' IsSchematic='false' SchematicColor='0' SchematicLineWidth='1.5' SchematicBPLineWidth='1.0' />\n".format(len(structure.residues)+1)
    # xrna += "<Nuc RefID='120' FormatType='9' />\n"
    for l in structure.labels:
        xrna += "<Nuc RefID='{}'>\n".format(l.id)
        xrna += "<LabelList>\n"
        xrna += 's {} {} 0.0 {} 0 0 "{}"\n'.format(l.x, l.y, SETTINGS.LABEL_FONT_SIZE, l.label)
        xrna += 'l {} {} {} {} {} 0 0 0 0 0 0\n'.format(l.x1, l.y1, l.x2, l.y2, SETTINGS.LABEL_LINE_THICKNESS)
        xrna += "</LabelList>\n"
        xrna += "</Nuc>\n"
    xrna += "\n"
    xrna += "\n"

    # Basepair Helixes

    # xrna += "<BasePairs nucID='1' length='10' bpNucID='119' />\n"
    for bp in structure.base_pairs:
        xrna += "<BasePairs nucID='{}' length='1' bpNucID='{}' />\n".format(bp.id1+1, bp.id2+1)

    # Footer
    xrna += "</RNAMolecule>\n"
    xrna += "</Complex>\n"
    xrna += "</ComplexDocument>\n"

    return xrna


def process_settings():
    if args.residue_font_size:
        SETTINGS.RESIDUE_FONT_SIZE = int(args.residue_font_size)
    if args.residue_font_size:
        SETTINGS.LABEL_FONT_SIZE = int(args.label_font_size)
    if args.residue_font_size:
        SETTINGS.LABEL_LINE_THICKNESS = float(args.label_line)


def main():
    fn = args.input
    fname = os.path.basename(fn)
    fname = os.path.splitext(fname)[0]
    with open(fn, "r") as fr:
        structure = read_traveler_xml(f=fr)
        with (sys.stdout if args.output is None else io.open(args.output, "w", newline="\n")) as fw:
            fw.write(convert_to_xrna(structure=structure, name=fname))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument("-i", "--input",
                        required=True,
                        metavar='FILE',
                        help="Traveler layout output XML file")
    parser.add_argument("-o", "--output",
                        metavar='FILE',
                        help="Output file name for the XRNA-formatted layout. "
                             "If non entered, the standard output will be used.")
    parser.add_argument("-rfs", "--residue-font-size",
                        required=False,
                        default=SETTINGS.RESIDUE_FONT_SIZE,
                        metavar='INT',
                        help="Residue font size")
    parser.add_argument("-lfs", "--label-font-size",
                        required=False,
                        default=SETTINGS.LABEL_FONT_SIZE,
                        metavar='INT',
                        help="Label font size")
    parser.add_argument("-ll", "--label-line",
                        required=False,
                        default=SETTINGS.LABEL_LINE_THICKNESS,
                        metavar='FLOAT',
                        help="Label line thickness size")

    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s [%(levelname)s] %(module)s - %(message)s',
        datefmt='%H:%M:%S')

    process_settings()
    main()