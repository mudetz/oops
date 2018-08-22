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
#include <ctime>
#include <deque>
#include <numeric>
#include <random>
#include <unordered_set>
#include <utility>
#include "edge.h"
#include "overloads.h"
#include "particle.h"

double Particle::_penalty;
double Particle::_Cmin;
double Particle::_Cmax;
bool Particle::_use_mst;

Particle::Particle(Graph &G)
	: _graph(G)
	, _times_no_improve(0)
	, _cost(INFINITY)
	, _reward(0)
	, _best_cost(NAN)
	, _best_reward(0)
	, _priorities(G.size(), NAN)
	, _visiting(G.size(), true)
	, _priorities_speed(G.size(), NAN)
	, _visiting_speed(G.size(), NAN)
	, _best_priorities(G.size(), NAN)
	, _best_visiting(G.size(), true)
{
	assert(G.size() > 0);
}

Particle &Particle::operator=(Particle const &other)
{
	if (this == &other)
		return *this;

	_graph = other._graph;

	_cost = other._cost;
	_best_cost = other._best_cost;

	_reward = other._reward;
	_best_reward = other._best_reward;

	_priorities = other._priorities;
	_visiting = other._visiting;

	_priorities_speed = other._priorities_speed;
	_visiting_speed = other._visiting_speed;

	_best_priorities = other._best_priorities;
	_best_visiting = other._best_visiting;

	return *this;
}

void Particle::randomize(void)
{
	// RNGs
	std::default_random_engine re(std::time(NULL));
	std::uniform_real_distribution<double> double_dist(-5, 5);
	std::uniform_int_distribution<short> short_dist(0, 1);

	auto real = [&re, &double_dist]() {
		return double_dist(re);
	};
	auto integer = [&re, &short_dist]() {
		return short_dist(re);
	};

	// Random position
	std::generate(_priorities.begin(), _priorities.end(), real);
	std::generate(_visiting.begin(), _visiting.end(), integer);

	// Random speed
	std::generate(_priorities_speed.begin(), _priorities_speed.end(), real);
	std::generate(_visiting_speed.begin(), _visiting_speed.end(), real);
}

void Particle::eval(void)
{
	// Reset cost & reward
	_cost = 0;
	_reward = 0;

	// Get the represented route
	std::vector<unsigned int> R = std::move(route());

	// Evaluate the route, ignorw double-used arcs rewards
	bool double_use = false;
	std::unordered_set<std::pair<unsigned int, unsigned int>, pair_hash> used;
	for (size_t i = 0; i < R.size() - 1; i++) {
		_cost += _graph.edge(R.at(i), R.at(i + 1)).cost();
		if (!used.count(std::minmax(R.at(i), R.at(i + 1)))) {
			_reward += _graph.edge(R.at(i), R.at(i + 1)).reward();
			used.emplace(std::minmax(R.at(i), R.at(i + 1)));
		} else {
			double_use = true;
		}
	}

	// Penalize constraint violation
	if (_cost < _Cmin || _cost > _Cmax)
		_cost += 1 * _penalty;
	if (double_use)
		_cost += 3 * _penalty;

	// Check if it is better than the local best
	if (std::isnan(_best_cost) || (_reward > _best_reward && _cost <= _Cmax)) {
		_best_cost = _cost;
		_best_reward = _reward;
		_best_priorities = _priorities;
		_best_visiting = _visiting;
	} else {
		_times_no_improve++;
	}

	if (_times_no_improve == 6) {
		_times_no_improve = 0;
		randomize();
	}

}

void Particle::update_speed(Particle &best, double sf, double cf)
{
	// Priorities speed
	_priorities_speed += cf * (_best_priorities - _priorities);
	_priorities_speed += sf * (best._best_priorities - _priorities);

	// Visiting speed
	_visiting_speed += cf * std::vector<double>(_best_visiting.begin(), _best_visiting.end());
	_visiting_speed -= cf * std::vector<double>(_visiting.begin(), _visiting.end());
	_visiting_speed += sf * std::vector<double>(best._best_visiting.begin(), best._best_visiting.end());
	_visiting_speed -= sf * std::vector<double>(_visiting.begin(), _visiting.end());
}

void Particle::update_position(void)
{
	// Update position with speed
	_priorities += _priorities_speed;

	// Use sigma function to update each coordinate of _visiting
	// stochastically
	std::random_device r;
	std::default_random_engine e(r());
	std::uniform_real_distribution<double> rand(0, 1);
	for (size_t i = 0; i < _visiting.size(); i++)
		if (rand(e) < 1 / (1 + exp(- _visiting_speed.at(i))))
			_visiting.at(i) = true;
		else
			_visiting.at(i) = false;
	for (auto &blacklisted : _graph.blacklist())
		_visiting.at(blacklisted) = false;
}

double Particle::cost(void) const
{
	return _cost;
}

double Particle::best_cost(void) const
{
	return _best_cost;
}

unsigned int Particle::reward(void) const
{
	return _reward;
}

unsigned int Particle::best_reward(void) const
{
	return _best_reward;
}

std::vector<unsigned int> Particle::_make_route(std::vector<double> const &pri, std::vector<bool> const &vis) const
{
	// Route vector starting at start
	std::vector<unsigned int> R;
	R.reserve(_graph.size());
	R.push_back(_graph.start());

	// Comparison function to sort cities in order
	auto cmp = [&pri](auto const a, auto const b) {
		return pri.at(a) < pri.at(b);
	};

	// Vector of vertices to visit in order
	std::deque<unsigned int> V;
	if (!_use_mst) {
		for (size_t i = 0; i < pri.size(); i++)
			if (vis.at(i) && i != _graph.start())
				V.push_back(i);
		std::sort(V.begin(), V.end(), cmp);
	} else {
		std::vector<unsigned int> Preorder = _graph.preorder(pri, vis);
		for (auto &p : Preorder)
			V.push_back(p);
	}

	// Visit cities, add them in the best available position while route
	// cost is less than Cmin
	double cost = 0;
	unsigned int max_tries = 3 * V.size();
	unsigned int tries = 0;
	while (!V.empty() && tries < max_tries && cost < (_Cmin + _Cmax) / 2) {
		unsigned int new_vertex = V.front(); V.pop_front();
		double candidate_cost = INFINITY;
		// Try to insert before than pos 1
		size_t candidate_position = 0;

		// Try inserting in between
		for (size_t i = 1; i < R.size(); i++) {
			double e1 = _graph.edge(R.at(i - 1), new_vertex).cost();
			double e2 = _graph.edge(new_vertex, R.at(i)).cost();
			if (cost + e1 + e2 < candidate_cost && cost + e1 + e2 < _Cmax) {
				candidate_cost = cost + e1 + e2;
				candidate_position = i;
			}
		}
		// Try inserting in the end
		{
			double e = _graph.edge(R.back(), new_vertex).cost();
			if (cost + e < candidate_cost && cost + e < _Cmax) {
				candidate_cost = cost + e;
				candidate_position = R.size();
			}
		}

		// We tried
		tries += 1;

		// Maybe no position was good, try again later :/
		if (candidate_position == 0) {
			V.push_back(new_vertex);
			continue;
		}

		// Do the insert
		R.insert(R.begin() + candidate_position, new_vertex);
		cost = candidate_cost;
	};

	// Repair path until the end. Use Floyd Warshall's.
	std::vector<unsigned int> path = _graph.best_path(R.back(), R.front());
	R.insert(R.end(), path.begin(), path.end());

	return R;
}

std::vector<unsigned int> Particle::route(void) const
{
	// Reference priority and visiting
	auto &pri = _priorities;
	auto &vis = _visiting;

	return std::move(_make_route(pri, vis));
}

std::vector<unsigned int> Particle::best_route(void) const
{
	// Reference priority and visiting
	auto &pri = _best_priorities;
	auto &vis = _best_visiting;

	return std::move(_make_route(pri, vis));
}

void Particle::penalty(double p)
{
	_penalty = p;
}

void Particle::Cmin(double cm)
{
	_Cmin = cm;
}

void Particle::Cmax(double cm)
{
	_Cmax = cm;
}

void Particle::use_mst(bool use_it)
{
	_use_mst = use_it;
}

Particle Particle::best(std::vector<Particle> &S)
{
	// Filter particles (indices)
	std::vector<unsigned int> C;
	C.reserve(S.size());

	for (unsigned int i = 0; i < S.size(); i++)
		if (S.at(i).best_cost() > _Cmin && S.at(i).best_cost() < _Cmax)
			C.push_back(i);

	// If no particle is good, return such with min cost (try to repair by
	// cost first, then by reward, otherwise we will get only undfeasible
	// solutions)
	auto cmp1 = [](auto const &a, auto const &b) {
		return a.best_cost() < b.best_cost();
	};
	if (C.empty())
		return *std::min_element(S.begin(), S.end(), cmp1);

	// If there are feasible solutions, sort them by reward
	auto cmp2 = [&S](unsigned int const &a, unsigned int const &b) {
		return S.at(a).best_reward() > S.at(b).best_reward();
	};
	std::stable_sort(C.begin(), C.end(), cmp2);

	// Return the most rewarding feasible particle
	return S.at(C.front());
}
