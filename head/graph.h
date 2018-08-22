/*
 * OOPS - Particle Swarm Optimization for the Arc Orienteering Problem
 * Copyright (C) 2018	Manuel Weitzman
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __graph_h__
#define __graph_h__

#include <unordered_map>
#include <vector>
#include "edge.h"
#include "gtree.h"

class Graph {
private:
	unsigned int _start;

	std::vector< std::unordered_map<unsigned int, Edge> > _costs_rewards;
	std::vector< std::vector<double> > _min_costs;
	std::vector< std::vector<unsigned int> > _max_rewards;
	std::vector < std::vector<unsigned int> > _paths;
	std::vector<unsigned int> _blacklist;
	GTree _MST;

	void make_floyd_warshall(void);
	void make_mst(void);
public:
	Graph(unsigned int, unsigned int);

	std::vector<unsigned int> &blacklist(void);
	void add_edge(unsigned int, unsigned int, double, unsigned int);
	void analyze(bool);
	std::vector<unsigned int> preorder(std::vector<double> const &, std::vector<bool> const &);

	unsigned int size(void) const;
	unsigned int start(void) const;
	Edge edge(unsigned int, unsigned int) const;
	double min_cost(unsigned int, unsigned int) const;
	unsigned int max_reward(unsigned int, unsigned int) const;
	std::vector<unsigned int> best_path(unsigned int, unsigned int) const;
};

#endif
