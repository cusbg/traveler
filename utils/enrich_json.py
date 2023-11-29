import sys
import argparse
import logging
import json
import csv

RESIDUE_INDEX = 'residue_index'
RESIDUE_NAME = 'residue_name'

def reserved_name(name: str):
    return name.lower() in [RESIDUE_NAME, RESIDUE_INDEX]

def error_exit(message):
    logging.error(message)
    sys.exit()


def load_inputs(fn_json, fn_data):

    with open(fn_json, "r") as f:
        data_json = json.load(f)
    with open(fn_data, "r") as f:
        data_enrich = []
        for row in csv.DictReader(f, delimiter='\t'):
            data_enrich.append(row)

    return data_json, data_enrich


def map_res_index_to_ix(data):
    mapping = {}
    for i in range(len(data)):
        mapping[int(data[i][RESIDUE_INDEX])] = i
    return mapping


def int_or_fl(value):
    try:
        value = int(value)
    except ValueError:
        try:
            value = float(value)            
        except ValueError:
            pass
    return value


def enrich_structure(structure, data, force_overwrite: bool):
    res_ix_ix = map_res_index_to_ix(data=data)
    for res in structure['rnaComplexes'][0]['rnaMolecules'][0]['sequence']:
        res_ix = res['residueIndex']
        if res_ix in res_ix_ix:
            annotations = data[res_ix_ix[res_ix]]
            info = res['info']
            res_name = res['residueName']
            if res_name.lower() != annotations[RESIDUE_NAME].lower():
                raise Exception(
                    "Residue name in the data file and the input JSON do not match (residueIndex {})".format(res_ix))
            for k, v in annotations.items():
                if k in info and not force_overwrite:
                    raise Exception("Annotation {} already present in the nucleotide with residueIndex {}".format(k, res_ix))
                if not reserved_name(k):
                    info[k] = int_or_fl(v)


def main(args):

    structure, data = load_inputs(fn_json=args.input_json, fn_data=args.input_data)
    enrich_structure(structure=structure, data=data, force_overwrite=args.force)
    with (sys.stdout if args.output is None else open(args.output, "w")) as fw:
        fw.write(json.dumps(structure, indent=2))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument("-ij", "--input-json",
                        required=True,
                        metavar='FILE',
                        help="Input structure in the JSON format.")
    parser.add_argument("-id", "--input-data",
                        required=True,
                        metavar='FILE',
                        help="Input data in the tab-delimited format.")
    parser.add_argument("-o", "--output",
                        metavar='FILE',
                        help="Output file name for the SVG file. "
                             "If non entered, the standard output will be used.")
    parser.add_argument("-f", "--force",
                        required=False,
                        action='store_true',
                        default=False,
                        help="Forces overwriting a field if it is already present in the input json file.")

    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s [%(levelname)s] %(module)s - %(message)s',
        datefmt='%H:%M:%S')

    main(args)