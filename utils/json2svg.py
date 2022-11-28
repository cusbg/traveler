import sys
import argparse
import logging
import json
from typing import Dict


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __add__(self, other):
        return Point(self.x + other.x, self.y + other.y)


class Dimensions:
    def __init__(self):
        self.p1 = Point(sys.maxsize, sys.maxsize)
        self.p2 = Point(-sys.maxsize, -sys.maxsize)
        
    def update(self, p:Point):
        if p.x < self.p1.x:
            self.p1.x = p.x
        if p.y < self.p1.y:
            self.p1.y = p.y
        if p.x > self.p2.x:
            self.p2.x = p.x
        if p.y > self.p2.y:
            self.p2.y = p.y


MARGIN = Point(10, 10)
GRAY = 'rgb(204, 204, 204)'


def error_exit(message):
    logging.error(message)
    sys.exit()


def classes_defs(labels_template):
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
        'stroke': GRAY,
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
        'fill': GRAY
    }, {
        'name': 'numbering-line',
        'stroke': GRAY
    }, {
        'name': 'sequential' if labels_template else 'template',
        'visibility': 'hidden'
    }, {
        'name': 'bp-line',
        'stroke': 'rgb(0, 0, 0)'
    }, {
        'name': 'residue-circle',
        'fill': 'rgb(255,255,255)'
    }, {
        'name': 'res-line',
        'stroke': GRAY
    },
    ]


def get_font_size(classes):
    return (float)((list(filter(lambda x: x["name"] == "font", classes))[0]["font-size"]).replace("px", ""))


def classes_to_svg(data, labels_template):

    classes = data['classes'] + classes_defs(labels_template)

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

    # Lines connected residues in the sequence are drawn
    p_prev: Point = None
    for res in rna['sequence']:
        p = Point(round(float(res['x']), 2), round(float(res['y']), 2)) + MARGIN
        if p_prev:
            residues += f'<line x1="{p_prev.x}" y1="{p_prev.y}" x2="{p.x}" y2="{p.y}" class="bp-line res-line" />\n'
        p_prev = p

    # Circles forming the background of the residues (these need to come AFTER the connecting lines as they need to be "above" them and SVG does not have z-index)
    for res in rna['sequence']:
        p = Point(round(float(res['x']), 2), round(float(res['y']), 2)) + MARGIN
        residues += f"<circle cx=\"{p.x}\" cy=\"{p.y}\" class=\"residue-circle\" r=\"{font_size*0.75}\"/>\n"

    # Residue letters
    for res in rna['sequence']:
        p = Point(round(float(res['x']), 2), round(float(res['y']), 2)) + MARGIN
        res_pos[res['residueIndex']] = p
        dim.update(p)
        cls = " ".join(res['classes'])
        residues += f"<g>" \
                    f"<title>{res['residueIndex']} (position.label in template: {res['info']['templateResidueIndex']}.{res['info']['templateResidueName']})</title>" \
                    f"<text x=\"{p.x}\" y=\"{p.y}\" class=\"{cls}\" >{res['residueName']}</text>" \
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
        p = Point(lbl_content["x"],lbl_content["y"]) + MARGIN
        p1 = Point(lbl_line["x1"], lbl_line["y1"]) + MARGIN
        p2 = Point(lbl_line["x2"], lbl_line["y2"]) + MARGIN
        dim.update(p)
        dim.update(p1)
        dim.update(p2)
        svg_labels += f'<text x="{p.x}" y="{p.y}" class="{" ".join(lbl_content["classes"])}" >{lbl_content["label"]}</text>\n'
        svg_labels += f'<line x1="{p1.x}" y1="{p1.y}" x2="{p2.x}" y2="{p2.y}" class="{" ".join(lbl_line["classes"])}" />\n'
        svg_labels += '</g>'

    svg_labels += '</g>\n'

    return svg_labels


def to_svg(data, labels_template):
    
    dim = Dimensions()

    rna = data['rnaComplexes'][0]['rnaMolecules'][0]
    svg_classes = classes_to_svg(data, labels_template)
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
            fw.write(to_svg(data, args.labels_template))


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
    parser.add_argument("-l", "--labels-template",
                        action='store_true',
                        help="If set, the numbering labels will be based on numbering labels from template (e.g. Sprinzl positions for tRNA). ")

    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s [%(levelname)s] %(module)s - %(message)s',
        datefmt='%H:%M:%S')

    main()