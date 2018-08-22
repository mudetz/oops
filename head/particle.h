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

#ifndef __particle_h__
#define __particle_h__

#include <vector>
#include "graph.h"

class Particle {
private:
	Graph &_graph;

	unsigned int _times_no_improve;
	double _cost;
	unsigned int _reward;
	double _best_cost;
	unsigned int _best_reward;

	std::vector<double> _priorities;
	std::vector<bool> _visiting;

	std::vector<double> _priorities_speed;
	std::vector<double> _visiting_speed;

	std::vector<double> _best_priorities;
	std::vector<bool> _best_visiting;

	std::vector<unsigned int>_make_route(std::vector<double> const &, std::vector<bool> const &) const;

	static double _penalty;
	static double _Cmin;
	static double _Cmax;
	static bool _use_mst;
public:
	Particle(Graph &);
	Particle &operator=(Particle const &);

	void randomize(void);
	void eval(void);
	void update_speed(Particle &, double, double);
	void update_position(void);


	double cost(void) const;
	double best_cost(void) const;
	unsigned int reward(void) const;
	unsigned int best_reward(void) const;
	std::vector<unsigned int> route(void) const;
	std::vector<unsigned int> best_route(void) const;

	static void penalty(double);
	static void Cmin(double);
	static void Cmax(double);
	static void use_mst(bool);
	static Particle best(std::vector<Particle> &);
};

#endif

