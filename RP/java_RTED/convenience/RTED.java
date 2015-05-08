//    Copyright (C) 2012  Mateusz Pawlik and Nikolaus Augsten
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

package convenience;

import java.util.LinkedList;

import util.LblTree;
import distance.RTED_InfoTree_Opt;

/**
 * 
 * This class provides several convenience methods for computing the tree edit distance and the minimal edit mapping.
 * 
 * @author Mateusz Pawlik
 *
 */
public class RTED {

	/**
	 * Computes the tree edit distance between two trees using the RTED algorithm.
	 * The costs of the edit operations are set to the default 1.
	 * 
	 * @param tree1 first tree given in the bracket notation.
	 * @param tree2 second tree given in the bracket notation.
	 * @return the tree edit distance between tree1 and tree2.
	 */
	public static double computeDistance(String tree1, String tree2) {
		RTED_InfoTree_Opt rted = new RTED_InfoTree_Opt(1, 1, 1);
		return rted.nonNormalizedTreeDist(LblTree.fromString(tree1), LblTree.fromString(tree2));
	}
	
	/**
	 * Computes the tree edit distance between two trees using the RTED algorithm and specified costs of edit operations.
	 * 
	 * @param tree1 first tree given in the bracket notation.
	 * @param tree2 second tree given in the bracket notation.
	 * @param costIns cost of inserting a node.
	 * @param costDel cost of deleting a node.
	 * @param costRen cost of renaming two nodes.
	 * @return the tree edit distance between tree1 and tree2.
	 */
	public static double computeDistance(String tree1, String tree2, double costIns, double costDel, double costRen) {
		RTED_InfoTree_Opt rted = new RTED_InfoTree_Opt(costIns, costDel, costRen);
		return rted.nonNormalizedTreeDist(LblTree.fromString(tree1), LblTree.fromString(tree2));
	}
	
	/**
	 * <p>
	 * Computes the minimal edit mapping between two trees. There might be multiple minimal edit mappings.
	 * This method computes only one of them.
	 * </p>
	 * <p>
	 * First, the tree distance matrix is computed using RTED algorithm and the default cost of edit operations.
	 * Second, the mapping is computed.
	 * </p>
	 * <p>
	 * The result is a list of edit operations represented by pairs of postorder IDs of the nodes.
	 * n and m are the the postorder IDs of the nodes from tree1 and tree2 respectively.
	 * There are three possible operations:<br />
	 * [n,m] - rename node n to m<br />
	 * [n,0] - delete node n<br />
	 * [0.m] - insert node m<br />
	 * </p>
	 * @param tree1 first tree given in the bracket notation.
	 * @param tree2 second tree given in the bracket notation.
	 * @return minimal edit mapping.
	 */
	public static LinkedList<int[]> computeMapping(String tree1, String tree2) {
		RTED_InfoTree_Opt rted = new RTED_InfoTree_Opt(1, 1, 1);
		rted.nonNormalizedTreeDist(LblTree.fromString(tree1), LblTree.fromString(tree2));
		return rted.computeEditMapping();
	}
	
	/**
	 * <p>
	 * Computes the minimal edit mapping between two trees. There might be multiple minimal edit mappings.
	 * This method computes only one of them.
	 * </p>
	 * <p>
	 * First, the tree distance matrix is computed using RTED algorithm with the specified costs of edit operations.
	 * Second, the mapping is computed.
	 * </p>
	 * <p>
	 * The result is a list of edit operations represented by pairs of postorder IDs of the nodes.
	 * n and m are the the postorder IDs of the nodes from tree1 and tree2 respectively.
	 * There are three possible operations:<br />
	 * [n,m] - rename node n to m<br />
	 * [n,0] - delete node n<br />
	 * [0.m] - insert node m<br />
	 * </p>
	 * @param tree1 first tree given in the bracket notation.
	 * @param tree2 second tree given in the bracket notation.
	 * @param costIns cost of inserting a node.
	 * @param costDel cost of deleting a node.
	 * @param costRen cost of renaming two nodes.
	 * @return minimal edit mapping.
	 */
	public static LinkedList<int[]> computeMapping(String tree1, String tree2, double costIns, double costDel, double costRen) {
		RTED_InfoTree_Opt rted = new RTED_InfoTree_Opt(costIns, costDel, costRen);
		rted.nonNormalizedTreeDist(LblTree.fromString(tree1), LblTree.fromString(tree2));
		return rted.computeEditMapping();
	}
}
