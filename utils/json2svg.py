import sys
import gzip
import argparse
import logging
import json
from typing import Dict


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y


class Dimensions:
    def __init__(self):
        self.p1 = Point(sys.maxsize, sys.maxsize)
        self.p2 = Point(-sys.maxsize, -sys.maxsize)
        
    def update(self, x, y):
        if x < self.p1.x:
            self.p1.x = x
        if y < self.p1.y:
            self.p1.y = y
        if x > self.p2.x:
            self.p2.x = x
        if y > self.p2.y:
            self.p2.y = y


MARGIN = Point(10, 10)


def error_exit(message):
    logging.error(message)
    sys.exit()


def classes_defs():
    return [{
        'name': 'text-black',
        'fill': 'rgb(0, 0, 0)'
    }, {
        'name': 'text-red',
        'fill': 'rgb(255, 0, 255)'
    }, {
        'name': 'text-green',
        'fill': 'rgb(0, 255, 0)'
    }, {
        'name': 'text-blue',
        'fill': 'rgb(0, 0, 255)'
    }, {
        'name': 'text-gray',
        'fill': 'rgb(204, 204, 204)'
    }, {
        'name': 'text-brown',
        'fill': 'rgb(211.65, 104.55, 30.6)'
    },  {
        'element-wise': True,
        'name': 'text',
        'fill': 'rgb(0, 0, 0)',
        'text-anchor': 'middle',
        'dominant-baseline': 'middle'
    }, {
        'name': 'circle-black',
        'stroke': 'rgb(0, 0, 0)',
        'fill': 'none'
    }, {
        'name': 'circle-red',
        'stroke': 'rgb(255, 0, 255)',
        'fill': 'none'
    }, {
        'name': 'circle-green',
        'stroke': 'rgb(0, 255, 0)',
        'fill': 'none'
    }, {
        'name': 'circle-blue',
        'stroke': 'rgb(0, 0, 255)',
        'fill': 'none'
    }, {
        'name': 'circle-gray',
        'stroke': 'rgb(204, 204, 204)',
        'fill': 'none'
    }, {
        'name': 'circle-brown',
        'stroke': 'rgb(211.65, 104.55, 30.6)',
        'fill': 'none'
    }, {
        'name': 'circle',
        'stroke': 'rgb(0, 0, 0)'
    }, {
        'name': 'numbering-label',
        'fill': 'rgb(204, 204, 204)'
    }, {
        'name': 'numbering-line',
        'stroke': 'rgb(204, 204, 204)'
    }, {
        'name': 'template',
        'visibility': 'hidden'
    }, {
        'name': 'bp-line',
        'stroke': 'rgb(0, 0, 0)'
    }, {
        'name': 'residue-circle',
        'fill': 'rgb(255,255,255)'
    }
    ]


def get_font_size(classes):
    return (float)((list(filter(lambda x: x["name"] == "font", classes))[0]["font-size"]).replace("px", ""))


def classes_to_svg(data):

    classes = data['classes'] + classes_defs()

    svg_classes = '<style type="text/css" >\n'
    svg_classes += '<![CDATA[\n'
    for cls in classes:
        # text.brown {fill: rgb(211.65, 104.55, 30.6); }
        content = ""
        for k, v in cls.items():
            if k != 'name':
                content += f'{k}: {v}; '

        selector = cls["name"] if 'element-wise' in cls else '.' + cls["name"]
        svg_classes += f'{selector} {{ {content} }}\n'

    svg_classes += ']]>\n'
    svg_classes += '</style>\n'

    return svg_classes


def residues_to_svg(rna, dim: Dimensions, res_pos: Dict[int, Point], font_size):

    residues = '<g class="residues">'

    for res in rna['sequence']:
        x = round(float(res['x']), 2) + MARGIN.x
        y = round(float(res['y']), 2) + MARGIN.y
        residues += f"<circle cx=\"{x}\" cy=\"{y}\" class=\"residue-circle\" r=\"{font_size*0.75}\"/>\n"
    for res in rna['sequence']:
        x = round(float(res['x']), 2) + MARGIN.x
        y = round(float(res['y']), 2) + MARGIN.y
        res_pos[res['residueIndex']] = Point(x, y)
        dim.update(x, y)
        cls = " ".join(res['classes'])
        residues += f"<g>" \
                    f"<title>{res['residueIndex']} (position.label in template: {res['templateResidueIndex']}.{res['templateResidueName']}')</title>" \
                    f"<text x=\"{x}\" y=\"{y}\" class=\"{cls}\" >{res['residueName']}</text>" \
                    f"</g>\n"
    residues += '</g>'

    return residues


def bps_to_svg(rna, res_pos: Dict[int, Point]):

    bps = '<g class="bps">'
    for bp in rna['basePairs']:
        r1 = bp['residueIndex1']
        r2 = bp['residueIndex2']
        p1 = res_pos[r1]
        p2 = res_pos[r2]
        cls = " ".join(bp['classes'])
        bps += f'<line x1="{p1.x}" y1="{p1.y}" x2="{p2.x}" y2="{p2.y}" class="{cls}" />\n'
    bps += '</g>'

    return bps


def labels_to_svg(rna, dim: Dimensions):

    svg_labels = '<g class="labels">\n'
    for lbl in rna['labels']:
        lbl_content = lbl['labelContent']
        lbl_line = lbl['labelLine']
        svg_labels += '<g class="label">\n'
        x = lbl_content["x"] + MARGIN.x
        y = lbl_content["y"] + MARGIN.y
        x1 = lbl_line["x1"] + MARGIN.x
        y1 = lbl_line["y1"] + MARGIN.y
        x2 = lbl_line["x2"] + MARGIN.x
        y2 = lbl_line["y2"] + MARGIN.y
        dim.update(x, y)
        dim.update(x1, y1)
        dim.update(x2, y2)
        svg_labels += f'<text x="{x}" y="{y}" class="{" ".join(lbl_content["classes"])}" >{lbl_content["label"]}</text>\n'
        svg_labels += f'<line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" class="{" ".join(lbl_line["classes"])}" />\n'
        svg_labels += '</g>'

    svg_labels += '</g>\n'

    return svg_labels


def to_svg(data):
    
    dim = Dimensions()

    rna = data['rnaComplexes'][0]['rnaMolecules'][0]
    svg_classes = classes_to_svg(data)
    res_pos: Dict[int, Point] = {}
    residues = residues_to_svg(rna, dim, res_pos, get_font_size(data['classes']))
    bps = bps_to_svg(rna, res_pos)
    svg_labels = labels_to_svg(rna, dim)

    svg = f'<svg xmlns="http://www.w3.org/2000/svg" width="{dim.p2.x + 2*MARGIN.x}" height="{dim.p2.y + 2*MARGIN.y}">\n'
    svg += svg_classes + '\n'
    svg += bps + '\n'
    svg += residues + '\n'
    svg += svg_labels + '\n'
    svg += '</svg>'
    return svg


def main():

    with open(args.input, "r") as fr:
        data = json.load(fr)
        with (sys.stdout if args.output is None else open(args.output, "w")) as fw:
            fw.write(to_svg(data))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument("-i", "--input",
                        required=True,
                        metavar='FILE',
                        help="Input structure in the JSON format.")
    parser.add_argument("-o", "--output",
                        metavar='FILE',
                        help="Output file name for the SVG file. "
                             "If non entered, the standard output will be used.")

    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s [%(levelname)s] %(module)s - %(message)s',
        datefmt='%H:%M:%S')

    main()