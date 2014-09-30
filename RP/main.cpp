/*
 * main.cpp
 * Copyright (C) 2014 Richard Eliáš <richard@ba30.eu>
 *
 * Distributed under terms of the MIT license.
 */

#include <iostream>
#include "types.hpp"
#include "tree_hh/tree.hh"
#include "tree_hh/tree_util.hh"
#include "node.hpp"
#include "tree_base.hpp"
#include "tree.hpp"

using namespace std;



int main(int argc, char** argv)
{
    tree<node<string> > t;
    
    cout << t.size() << endl;
    cout << node<string>().get_id() << endl;
    logger.info("LOGGER");
    logger.debugStream() << "text";
	return 0;
}





