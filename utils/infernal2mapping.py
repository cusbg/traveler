import sys
import re
import argparse
import logging
import copy

def error_exit(message):
    logging.error(message)
    sys.exit()




def read_structure(f):

    seq = ""
    struct = ""
    for line in f:
        sLine = line.strip().split(" ")
        if len(sLine) == 3 and sLine[0].isdigit():
            sLine[0] = int(sLine[0])
            sLine[2] = int(sLine[2])
            seq += sLine[1]
            if sLine[2] == 0:
                struct += '.'
            elif sLine[0] < sLine[2]:
                struct += '('
            else:
                struct += ')'

    return [seq, struct]


def write_structure(s, f):
    f.writelines(s)

def remove_at(i, s):
    return s[:i] + s[i+1:]


def read_structures(f):
    f.readline()
    sq = f.readline()
    f.readline()
    str = f.readline()
    assert len(sq) == len(str)

    sq_tgt = sq.replace('-', '')
    pos_deleted = []
    for i, letter in enumerate(sq_tgt):
        if letter == '-':
        # if 'a' <= letter <= 'z':
            pos_deleted.append(i)


    str_tmp = str.replace('.', '').replace('~', '')
    str_tgt = str
    for i in pos_deleted[::-1]:
        str_tgt = remove_at(i, str_tgt)

    return sq, str, str_tgt, str_tmp

def get_mapping(sq, str, str_tgt, str_tmp):
    NotImplemented

def main():
    with open(args.input, "r") as fr:
        sq, str, str_tgt, str_tmp = read_structures(fr)
        with (sys.stdout if args.output is None else open(args.output, "w")) as fw:
            fw.write('> generated from {} by bpseq2fasta\n'.format(args.input))
            fw.write('{}\n'.format(s[0]))
            fw.write('{}\n'.format(s[1]))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument("-i", "--input",
                        required=True,
                        metavar='FILE',
                        help="Infernal FASTA-like .afa file with the alignment.")
    parser.add_argument("-o", "--output",
                        metavar='FILE',
                        help="Output file name for the Traveler-formatted mapping. "
                             "If non entered, the standard output will be used.")

    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s [%(levelname)s] %(module)s - %(message)s',
        datefmt='%H:%M:%S')

    main()