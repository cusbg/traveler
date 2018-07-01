import sys
import gzip
import argparse


def open_file(file_name, mode="r"):
    access_type = mode
    if sys.version_info >= (3,): access_type = mode + "t"
    if file_name.endswith("gz"):
        return gzip.open(file_name, access_type)
    else:
        return open(file_name, access_type)


def read_structure(f):

    struct = ""
    for line in f:
        struct += line.strip()
    return struct


def parse_definition(d):
    positions = []
    for x in d.split(','):
        if '-' in x:
            sx = x.split('-')
            positions += list(range(int(sx[0]), int(sx[1])+1))
        else:
            positions.append(int(x))
    return positions


def modify_structure(str, d):
    positions = parse_definition(d)

    s = list(str)

    i = 0
    j = len(s) - 1

    left_brackes = []
    pairs = []

    for i in range(len(s)):
        if s[i] == '(':
            left_brackes.append(i)
        if s[i] == ')':
            lb = left_brackes.pop()
            pairs.append([lb,i])

    for p in pairs:
        if p[0] in positions or p[1] in positions:
            s[p[0]] = '.'
            s[p[1]] = '.'


    # while i < j:
    #     if s[i] != '.' and s[j] != '.':
    #         if i in positions or j in positions:
    #             s[i] = '.'
    #             s[j] = '.'
    #         i += 1
    #         j -= 1
    #
    #     if s[i] == '.':
    #         i += 1
    #     if s[j] == '.':
    #         j -= 1
    #
    return ''.join(s)


def main():
    with (sys.stdin if args.input is None else open_file(args.input, "r")) as fr:
        s = read_structure(fr)
        s = modify_structure(s, args.remove_definition)
        with (sys.stdout if args.output is None else open_file(args.output, "w")) as fw:
            fw.write('{}\n'.format(s))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument("-i", "--input",
                        metavar='FILE',
                        help="Input structure")
    parser.add_argument("-o", "--output",
                        metavar='FILE',
                        help="Output file name"
                             "If non entered, the standard output will be used.")
    parser.add_argument("-d", "--remove-definition",
                        metavar='STRING',
                        required=True,
                        help="List of zero-indexed positions or ranges separated by colon which should be unpaired."
                             "Range needs to be in the form X-Y.")

    args = parser.parse_args()

    main()