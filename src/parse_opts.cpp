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

#include <iostream>
#include "parse_opts.h"

po::variables_map VM;

bool parse_opts(int const argc, char const **argv)
{
	po::options_description desc("Available options");
	desc.add_options()
		("help", po::bool_switch()->default_value(false),
			"Produce a help message")
		("verbose", po::bool_switch()->default_value(false),
			"Run verbosely")
		("random", po::bool_switch()->default_value(false),
			"Randomly move each particle")
		("mst", po::bool_switch()->default_value(false),
			"Use a MST of G as a guide for route building")
		("swarm-size", po::value<unsigned int>()->default_value(100),
			"Set amount of particles")
		("max-velocity", po::value<double>()->default_value(0, "0"),
			"Set particle maximum velocity. "
			"0 means there is no velocity limit.")
		("social-factor",
			po::value<double>()->default_value(0.7, "0.7"),
			"Set particle social factor")
		("cognitive-factor",
			po::value<double>()->default_value(0.3, "0.3"),
			"Set particle coginitive factor")
		("max-cycles",
			po::value<unsigned int>()->default_value(100, "100"),
			"Set max program iterations")
		("optima",
			po::value<unsigned int>()->default_value(0),
			"Tell the program to stop at certain optima. 0 means do not stop.")
		("threads",
			po::value<unsigned int>()->default_value(0),
			"Threads to use. 0 means hardware determined.");



	po::store(po::parse_command_line(argc, argv, desc), VM);
	po::notify(VM);

	if (VM.at("help").as<bool>()) {
		std::cout << desc << '\n';
		return true;
	}

	return false;
}

void verbose_opts(void)
{
	std::cerr << "Active Options: \n"
		  << "\t--verbose\t\t\t1"
		  << "\n\t--random\t\t\t"
			<< VM.at("random").as<bool>()
		  << "\n\t--mst\t\t\t\t"
			<< VM.at("mst").as<bool>()
		  << "\n\t--swarm-size\t\t\t"
			<< VM.at("swarm-size").as<unsigned int>()
		  << "\n\t--max-cycles\t\t\t"
			<< VM.at("max-cycles").as<unsigned int>()
		  << "\n\t--social-factor\t\t\t"
			<< VM.at("social-factor").as<double>()
		  << "\n\t--cognitive-factor\t\t"
			<< VM.at("cognitive-factor").as<double>()
		  << "\n\t--optima\t\t\t"
			<< VM.at("optima").as<unsigned int>()
		  << "\n\t--threads\t\t\t"
			<< VM.at("threads").as<unsigned int>()
		  << "\n\n";

}
