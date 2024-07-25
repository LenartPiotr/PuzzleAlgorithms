#include <iostream>

#include "include/cxxopts.hpp"
#include "include/base-lib.h"

using namespace std;
using namespace cxxopts;

int main(int argc, char* argv[]) {
	Options options("PuzzleAlgorithm", "Algorithms and generators for popular puzzles");
	options.add_options()
		("a,algorithm", "Select puzzle algorithm", value<string>())
		("l,list", "List possible puzzle algorithms")
		("i,input", "Input directory", value<string>(), "Default ./samples/<puzzle algorithm name>/in/")
		("o,output", "Output directory", value<string>(), "Default ./samples/<puzzle algorithm name>/out/")
		("format", "Print selected algorithm file format");
	auto result = options.parse(argc, argv);
	cout << options.help();
}