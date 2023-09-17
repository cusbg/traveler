import sys
import argparse
import logging
import json
import re
from typing import Dict, Tuple

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


def get_color_class(attribute, val):
    return re.sub('[.,*]', '_', f'color-{attribute}-{val}')


def get_color_attributes(params):
    return params["coloring"].keys() if params else []


def rgb2triplet(rgb: str) -> Tuple[int, int, int]:
    r, g, b = re.search('\((.*),(.*),(.*)\)', rgb).group(1, 2, 3)
    return int(r), int(g), int(b)


def rgba2rgb(rgb: Tuple[int, int, int], a: float = 0.6) -> str:
    background = (255, 255, 255)

    rgb_blended = [0, 0, 0]
    rgb_blended[0] = int(rgb[0] * a + (1.0 - a) * background[0])
    rgb_blended[1] = int(rgb[1] * a + (1.0 - a) * background[1])
    rgb_blended[2] = int(rgb[2] * a + (1.0 - a) * background[2])

    return f'rgb({rgb_blended[0]}, {rgb_blended[1]}, {rgb_blended[2]})'


def classes_defs(labels_template, coloring, font_size):
    color_classes = []
    for attribute, values in coloring.items():
        for v, rgb in values["values"].items():
            color_classes.append({
                'name': get_color_class(attribute=attribute, val=v),
                'fill': rgba2rgb(rgb2triplet(rgb))
            })

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
        'name': 'bp-line-predicted',
        'stroke-dasharray': '2'
    },{
        'name': 'residue-circle',
        'fill': 'rgb(255,255,255)'
    }, {
        'name': 'res-line',
        'stroke': GRAY
    }, {
        'name': 'pseudoknot_segment1',
        'stroke-linecap': 'round',
        'stroke-opacity': 0.4,
        'stroke-width': font_size*1.2,
        'stroke': GRAY
    }, {
        'name': 'pseudoknot_segment2',
        'stroke-linecap': 'round',
        'stroke-opacity': 0.4,
        'stroke-width': font_size*1.2,
        'stroke': GRAY
    }, {
        'name': 'pseudoknot_connection',
        'stroke-linecap': 'round',
        'stroke-opacity': 0.2,
        'stroke-width': 1.5,
        'stroke': GRAY
    }
    ] + color_classes


def get_font_size(classes):
    return (float)((list(filter(lambda x: x["name"] == "font", classes))[0]["font-size"]).replace("px", ""))


def classes_to_svg(data, labels_template, params):

    classes = data['classes'] + classes_defs(labels_template, params["coloring"] if params else {}, get_font_size(data['classes']))

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


def residues_to_svg(rna, dim: Dimensions, res_pos: Dict[int, Point], res_info: Dict[int, Dict], font_size, params):

    color_attributes = get_color_attributes(params)
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
        is_circle  = False
        for attr in res['info'].keys():
            if attr in color_attributes:
                residues += f'<circle cx="{p.x}" cy="{p.y}" class="residue-circle {get_color_class(attr, res["info"][attr])}" r="{font_size * 0.75}"/>\n'
                is_circle = True
        if not is_circle:
            # in case there is no info attribute with color, we need to add white circle to cover the lines which go to the center of the letters
            residues += f"<circle cx=\"{p.x}\" cy=\"{p.y}\" class=\"residue-circle\" r=\"{font_size*0.75}\"/>\n"


    # Residue letters
    for res in rna['sequence']:
        p = Point(round(float(res['x']), 2), round(float(res['y']), 2)) + MARGIN
        res_pos[res['residueIndex']] = p
        res_info[res['residueIndex']] = res
        dim.update(p)
        cls = " ".join(res['classes'])
        title = f"Position: {res['residueIndex']} (position.label in template: {res['info']['templateResidueIndex']}.{res['info']['templateResidueName']})"
        for attr in res['info'].keys():
            if attr in color_attributes:
                # cls = f'{cls} {get_color_class(attr, res["info"][attr])}'
                title = f'{title}\n{params["coloring"][attr]["label"]}: {res["info"][attr]}'

        residues += f"<g>" \
                    f"<title>{title}</title>" \
                    f"<text x=\"{p.x}\" y=\"{p.y}\" class=\"{cls}\" >{res['residueName']}</text>" \
                    f"</g>\n"

    residues += '</g>'

    return residues


def bps_pn_to_svg(rna, res_pos: Dict[int, Point], res_info: Dict[int, Dict]):
    
    segments = []
    segment = None
    for bp in rna['basePairs']:        
        if 'info' in bp and 'type' in bp['info'] and bp['info']['type'] == "pseudoknot":
            r1 = bp['residueIndex1']
            r2 = bp['residueIndex2']
            if segment is not None and segment['interval1']['end'] + 1 == r1 and r2 + 1 == segment['interval2']['begin']:
                segment['interval1']['end'] = r1
                segment['interval2']['begin'] = r2
            else:
                if segment is not None:
                    segments.append(segment)
                segment = {
                    'interval1': {
                        'begin': r1,
                        'end': r1
                    },
                    'interval2': {
                        'begin': r2,
                        'end': r2
                    }
                }
    if segment is not None:
        segments.append(segment)

    bps = '<g class="bps-pn">'
    for segment in segments:
        ix = [
            [segment['interval1']['begin'], segment['interval1']['end']],
            [segment['interval2']['begin'], segment['interval2']['end']]
        ]
        p = [
            [res_pos[ix[0][0]], res_pos[ix[0][1]]],
            [res_pos[ix[1][0]], res_pos[ix[1][1]]]
        ]
        lbl = [
            ''.join([res_info[i]['residueName'] for i in range(ix[0][0], ix[0][1]+1)]),
            ''.join([res_info[i]['residueName'] for i in range(ix[1][0], ix[1][1]+1)])
        ]
        
        title = f'Pseudoknot {ix[0][0]}:{ix[0][1]}--{ix[1][0]}:{ix[1][1]}({lbl[0]}--{lbl[1]})'
        for s_ix in [0,1]:            
            bps += f'<polyline points="{p[s_ix][0].x} {p[s_ix][0].y}, {p[s_ix][1].x} {p[s_ix][1].y}" class="pseudoknot_segment{s_ix+1}" title="{title}"/>'            
        bps += f'<polyline points="{p[0][0].x} {p[0][0].y}, {p[1][0].x} {p[1][0].y}" class="pseudoknot_connection" title="{title}"/>'            

    bps += "</g>"
    return bps        


def bps_to_svg(rna, res_pos: Dict[int, Point], res_info: Dict[int, Dict]):
    """

    Args:
        rna (_type_): ['rnaComplexes'][0]['rnaMolecules'][0] part of the JSON schema.
        res_pos (Dict[int, Point]): Positions of the residues which are needed to correctly position the base pairs.

    Returns:
        string: part of svg with the base pairing rendering instructions
    """

    bps = '<g class="bps">'
    for bp in rna['basePairs']:
        r1 = bp['residueIndex1']
        r2 = bp['residueIndex2']
        p1 = res_pos[r1]
        p2 = res_pos[r2]        
        cls = 'class = "{}"'.format(" ".join(bp['classes'])) if 'classes' in bp else ''
        bps += f'<line x1="{p1.x}" y1="{p1.y}" x2="{p2.x}" y2="{p2.y}" {cls} />\n'
    bps += '</g>'

    return bps


def labels_to_svg(rna, dim: Dimensions):

    svg_labels = '<g class="labels">\n'
    for lbl in rna['labels']:
        lbl_content = lbl['labelContent']
        lbl_line = lbl['labelLine']
        svg_labels += '<g class="label">\n'
        p = Point(lbl_content["x"],lbl_content["y"]) + MARGIN
        p1 = Point(lbl_line["points"][0]["x"], lbl_line["points"][0]["y"]) + MARGIN
        p2 = Point(lbl_line["points"][1]["x"], lbl_line["points"][1]["y"]) + MARGIN
        dim.update(p)
        dim.update(p1)
        dim.update(p2)
        svg_labels += f'<text x="{p.x}" y="{p.y}" class="{" ".join(lbl_content["classes"])}" >{lbl_content["label"]}</text>\n'
        svg_labels += f'<line x1="{p1.x}" y1="{p1.y}" x2="{p2.x}" y2="{p2.y}" class="{" ".join(lbl_line["classes"])}" />\n'
        svg_labels += '</g>'

    svg_labels += '</g>\n'

    return svg_labels


def to_svg(data, labels_template, params):
    
    dim = Dimensions()

    rna = data['rnaComplexes'][0]['rnaMolecules'][0]
    svg_classes = classes_to_svg(data, labels_template, params)
    res_pos: Dict[int, Point] = {}
    res_info: Dict[int, Dict] = {}
    residues = residues_to_svg(rna=rna, dim=dim, res_pos=res_pos, res_info=res_info, font_size=get_font_size(data['classes']), params=params)
    bps = bps_to_svg(rna, res_pos, res_info)
    bps_pn = bps_pn_to_svg(rna, res_pos, res_info)
    svg_labels = labels_to_svg(rna, dim)

    svg = f'<svg xmlns="http://www.w3.org/2000/svg" width="{dim.p2.x + 2*MARGIN.x}" height="{dim.p2.y + 2*MARGIN.y}">\n'
    svg += svg_classes + '\n'
    svg += bps + '\n'
    svg += residues + '\n'
    svg += svg_labels + '\n'
    svg += bps_pn + '\n'
    svg += '</svg>'
    return svg


def main():

    with open(args.input, "r") as fr:
        data = json.load(fr)
        params = {}
        if args.params:
            with open(args.params, "r") as fp:
                params = json.load(fp)
        with (sys.stdout if args.output is None else open(args.output, "w")) as fw:
            fw.write(to_svg(data, args.labels_template, params))


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
    parser.add_argument("-p", "--params",
                        metavar='FILE',
                        help="Params file")
    parser.add_argument("-l", "--labels-template",
                        action='store_true',
                        help="If set, the numbering labels will be based on numbering labels from template (e.g. Sprinzl positions for tRNA). ")

    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s [%(levelname)s] %(module)s - %(message)s',
        datefmt='%H:%M:%S')

    main()