import sys
import gzip
import argparse
import logging


def error_exit(message):
    logging.error(message)
    sys.exit()


def open_file(file_name, mode="r"):
    access_type = mode
    if sys.version_info >= (3,): access_type = mode + "t"
    if file_name.endswith("gz"):
        return gzip.open(file_name, access_type)
    else:
        return open(file_name, access_type)


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


def main():
    with open_file(args.input, "r") as fr:
        s = read_structure(fr)
        with (sys.stdout if args.output is None else open_file(args.output, "w")) as fw:
            fw.write('> generated from {} by bpseq2fasta\n'.format(args.input))
            fw.write('{}\n'.format(s[0]))
            fw.write('{}\n'.format(s[1]))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument("-i", "--input",
                        required=True,
                        metavar='FILE',
                        help="Input structure in bpseq format.")
    parser.add_argument("-o", "--output",
                        metavar='FILE',
                        help="Output file name for the FASTA file. "
                             "If non entered, the standard output will be used.")

    args = parser.parse_args()

    logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s [%(levelname)s] %(module)s - %(message)s',
        datefmt='%H:%M:%S')

    main()