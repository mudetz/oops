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

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <numeric>
#include <utility>

#include "edge.h"
#include "graph.h"
#include "union_find.h"


Graph::Graph(unsigned int size, unsigned int start)
	: _start(start)
	, _costs_rewards(size)
	, _min_costs(size, std::vector<double>(size, INFINITY))
	, _max_rewards(size, std::vector<unsigned int>(size, 0))
	, _paths(size, std::vector<unsigned int>(size, size))
	, _blacklist()
	, _MST(start)

{
	assert(size != 0);
	assert(start < size);
}

void Graph::make_floyd_warshall(void) {
	// Initialize known costs & rewards
	for (size_t i = 0; i < _costs_rewards.size(); i++) {
		for (auto &j : _costs_rewards.at(i)) {
			_min_costs.at(i).at(j.first) = j.second.cost();
			_max_rewards.at(i).at(j.first) = j.second.reward();
			_paths.at(i).at(j.first) = j.first;
		}
	}

	// Get min costs via Dynamic Programming
	auto &d = _min_costs;
	auto &m = _max_rewards;
	auto &p = _paths;
	for (size_t k = 0; k < _min_costs.size(); k++) {
		for (size_t i = 0; i < _min_costs.size(); i++) {
			for (size_t j = 0; j < _min_costs.size(); j++) {
				if (d[i][j] > d[i][k] + d[k][j]) {
					d[i][j] = d[i][k] + d[k][j];
					m[i][j] = m[i][k] + m[k][j];
					p[i][j] = p[i][k];
				}
			}
		}
	}

	// Forbid remaining still
	for (size_t i = 0; i < _min_costs.size(); i++) {
		_min_costs.at(i).at(i) = INFINITY;
		_max_rewards.at(i).at(i) = 0;
		_paths.at(i).at(i) = _paths.size();
	}

	// Blacklist unconnected nodes
	auto cond = [](auto const &x) { return !std::isinf(x); };
	for (size_t i = 0; i < d.size(); i++)
		if (std::find_if(d[i].begin(), d[i].end(), cond) == d[i].end())
			_blacklist.push_back(i);
};

void Graph::make_mst(void) {
	// Comparison function for priority queue
	auto cmp = [this](auto &a, auto &b) {
		double x = INFINITY;
		double y = INFINITY;

		if (_costs_rewards.at(a.first).count(a.second))
			x = _costs_rewards.at(a.first).at(a.second).cost();

		if (_costs_rewards.at(b.first).count(b.second))
			y = _costs_rewards.at(b.first).at(b.second).cost();

		return x < y;
	};

	// Edges sorted by weight
	std::vector< std::pair<unsigned int, unsigned int> > E;
	for (size_t i = 0; i < _costs_rewards.size(); i++)
		for (auto &j : _costs_rewards.at(i))
			if (i < j.first)
				E.emplace_back(i, j.first);
	std::sort(E.begin(), E.end(), cmp);

	// Container for MST edges
	std::vector< std::pair<unsigned int, unsigned int> > MST_E;

	// Create Union-Find
	UnionFind uf(_costs_rewards.size());

	// Do Kruskal's algorithm
	for (auto &e : E) {
		if (uf.find(e.first) != uf.find(e.second)) {
			MST_E.push_back(e);
			uf.unite(e.first, e.second);
		}
	}

	// Add found edges to MST
	_MST.add_edges(MST_E);
};

std::vector<unsigned int> Graph::best_path(unsigned int from, unsigned int to)
	const
{
	// Build best path using Floyd-Warshall's table
	std::vector<unsigned int> P;
	if (_paths.at(from).at(to) == _paths.size())
		return P;
	while (from != to) {
		from = _paths.at(from).at(to);
		P.push_back(from);
	}
	return P;
}

unsigned int Graph::size(void) const
{
	return _costs_rewards.size();
}

unsigned int Graph::start(void) const
{
	return _start;
}

void Graph::add_edge(unsigned int from, unsigned int to, double cost,
		     unsigned int reward)
{
	_costs_rewards.at(from).emplace(std::make_pair(to, Edge(cost, reward)));
}

Edge Graph::edge(unsigned int from, unsigned int to) const
{
	// If arc exists, return a copy (we don't want users modifying the
	// graph)
	if (_costs_rewards.at(from).count(to))
		return _costs_rewards.at(from).at(to);

	// If arc does not exist, return a dummy one
	return Edge(INFINITY, 0);
}

double Graph::min_cost(unsigned int from, unsigned int to) const
{
	return _min_costs.at(from).at(to);
}

unsigned int Graph::max_reward(unsigned int from, unsigned int to) const
{
	return _max_rewards.at(from).at(to);
}

void Graph::analyze(bool generate_mst) {
	// Generate FLoyd-Warshall table
	make_floyd_warshall();

	// Generate MST
	if (generate_mst)
		make_mst();
}

std::vector<unsigned int> &Graph::blacklist(void)
{
	return _blacklist;
}

std::vector<unsigned int> Graph::preorder(std::vector<double> const &P,
					  std::vector<bool> const &V)
{
	// Prepare a vector to store the route
	std::vector<unsigned int> R;

	// Ask MST for preorder route using priorities given and skipping some
	// nodes
	_MST.preorder(R, P, V);

	// Return to origin
	R.push_back(_start);

	return R;
}
