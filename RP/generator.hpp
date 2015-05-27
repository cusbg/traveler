/*
 * File: generator.hpp
 *
 * Copyright (C) 2015 Richard Eliáš <richard@ba30.eu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#ifndef GENERATOR_HPP
#define GENERATOR_HPP


class generator
{
public:
    static void generate_files();

private:
    static void generate_seq_files();
    static void generate_fold_files();
    static void generate_ps_files();
    static void generate_mapping();
};

/*
    //static void generate_in_human();
    //static void generate_in_mouse();
    //static void generate_mapping_human_mouse();
*/
/*
    static void generate_RNAfold_fold_files()
    {
        APP_DEBUG_FNAME;

        logger.error("do not use!!!");
        abort();

        vector<string> vec = FILES;

        for (auto val : vec)
        {
            string fileIn = SEQ(val);
            string fileOut = FOLD(val);
            
            assert(exist_file(fileIn));
            string labels = read_file(fileIn);

            string brackets = run_RNAfold(labels);

            ofstream out(fileOut);
            out << brackets;
            assert(!out.fail());
        }
    }
*/
/*
void generator::generate_in_human()
{
    document doc = read_ps("build/files/human.ps");
    ofstream out;

    out.open("build/files/in_human.ps");
    out << doc.prolog << endl;
    out <<
        "(G) -49.93 -957.93 lwstring\n"
        "(C) -44.27 -963.59 lwstring\n"
        "(G) -38.62 -969.24 lwstring\n"
        "(C) -32.96 -974.9 lwstring\n"
        "(U) -23.7 -975.27 lwstring\n"
        "(G) -15.7 -980.29 lwstring\n"
        "(A) -10.61 -988.24 lwstring\n"
        "(C) -10.33 -997.53 lwstring\n"
        "(C) -4.67 -1003.18 lwstring\n"
        "(C) 0.98 -1008.84 lwstring\n"
        "(C) 6.64 -1014.5 lwstring\n"
        "(C) 12.3 -1020.15 lwstring\n"
        "(U) 17.95 -1025.81 lwstring\n"
        "(U) 23.98 -1031.34 lwstring\n"
        "(C) 23.81 -1039.81 lwstring\n"
        "(G) 17.84 -1045.8 lwstring\n"
        "(C) 9.39 -1045.99 lwstring\n"
        "(G) 3.81 -1039.95 lwstring\n"
        "(G) -1.85 -1034.3 lwstring\n"
        "(G) -7.5 -1028.64 lwstring\n"
        "(G) -13.16 -1022.98 lwstring\n"
        "(G) -18.82 -1017.33 lwstring\n"
        "(G) -24.47 -1011.67 lwstring\n"
        "(G) -33.47 -1010.54 lwstring\n"
        "(A) -41.02 -1005.55 lwstring\n"
        "(U) -45.95 -997.96 lwstring\n"
        "(G) -47.1 -989.04 lwstring\n"
        "(C) -52.76 -983.38 lwstring\n"
        "(G) -58.41 -977.73 lwstring\n"
        "(U) -64.07 -972.07 lwstring\n"
        ;
    out << endl;
    out << doc.epilog << endl;
    out.close();

    out.open("build/files/in_human.fold");
    out << 
        ".(((...(((((......)))))...))).";
    out.close();
}

void generator::generate_in_mouse()
{
    document doc = read_ps("build/files/mouse.ps");
    ofstream out;

    out.open("build/files/in_mouse.ps");
    out << doc.prolog << endl;
    out <<
        "(G) 112.07 -859.93 lwstring\n"
        "(C) 117.73 -865.59 lwstring\n"
        "(G) 123.38 -871.24 lwstring\n"
        "(C) 129.04 -876.90 lwstring\n"
        "(U) 138.30 -877.27 lwstring\n"
        "(G) 146.30 -882.29 lwstring\n"
        "(A) 151.39 -890.24 lwstring\n"
        "(C) 151.67 -899.53 lwstring\n"
        "(C) 157.33 -905.18 lwstring\n"
        "(C) 162.98 -910.84 lwstring\n"
        "(C) 168.64 -916.50 lwstring\n"
        "(C) 174.30 -922.15 lwstring\n"
        "(C) 179.95 -927.81 lwstring\n"
        "(U) 189.22 -929.92 lwstring\n"
        "(U) 194.45 -938.37 lwstring\n"
        "(C) 193.31 -948.24 lwstring\n"
        "(C) 186.31 -955.28 lwstring\n"
        "(C) 176.44 -956.47 lwstring\n"
        "(G) 167.97 -951.29 lwstring\n"
        "(G) 165.81 -941.95 lwstring\n"
        "(G) 160.15 -936.30 lwstring\n"
        "(G) 154.50 -930.64 lwstring\n"
        "(G) 148.84 -924.98 lwstring\n"
        "(G) 143.18 -919.33 lwstring\n"
        "(G) 137.53 -913.67 lwstring\n"
        "(G) 128.53 -912.54 lwstring\n"
        "(A) 120.98 -907.55 lwstring\n"
        "(U) 116.05 -899.96 lwstring\n"
        "(G) 114.90 -891.04 lwstring\n"
        "(C) 109.24 -885.38 lwstring\n"
        "(G) 103.59 -879.73 lwstring\n"
        "(U) 97.93 -874.07 lwstring\n"
        ;
    out << endl;
    out << doc.epilog << endl;
    out.close();

    out.open("build/files/in_mouse.fold");
    out << 
        ".(((...((((((......))))))...))).";
    out.close();
}

void generator::generate_mapping_human_mouse()
{
}
*/

#endif /* !GENERATOR_HPP */

