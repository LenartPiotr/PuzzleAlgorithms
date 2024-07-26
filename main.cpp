#include <iostream>

#include <cxxopts.hpp>
#include <base-lib.h>
#include <slitherlink.h>

using namespace std;
using namespace cxxopts;

int main(int argc, char* argv[]) {
	baselib::PuzzleAlgorithm* algorithm = NULL;

	Options options("PuzzleAlgorithm", "Algorithms and generators for popular puzzles");
	options.add_options()
		("a,algorithm", "Select puzzle algorithm", value<string>())
		("l,list", "List possible puzzle algorithms")
		("i,input", "Input directory - default = ./samples/<alg_name>/in/", value<string>())
		("o,output", "Output directory - default = ./samples/<alg_name>/out/", value<string>())
		("format", "Print selected algorithm file format (only with -a)")
		("h,help", "Print help");
	ParseResult result;
	try {
		result = options.parse(argc, argv);
	}
	catch (exception& e) {
		cout << options.help();
		return 0;
	}

	if (result.count("l") > 0) {
		cout << string("Algorithms:\n")
			+ " slitherlink\n";
		return 0;
	}
	if (result.count("h") > 0) {
		cout << options.help();
		return 0;
	}
	if (result.count("a") == 1) {
		string value = result["a"].as<string>();
		if (value == "slitherlink") algorithm = new algorithms::slitherlink::Algorithm();
		else {
			cout << "Use --list to show all possible algorithms" << endl;
			return 0;
		}
	}
	if (result.count("i") == 1) {
		if (algorithm) algorithm->setInput(result["i"].as<string>());
	}
	if (result.count("o") == 1) {
		if (algorithm) algorithm->setOutput(result["o"].as<string>());
	}
	if (result.count("f") > 0) {
		if (algorithm) {
			algorithm->printFormat(cout);
			delete algorithm;
		}
		return 0;
	}

	try {
		if (algorithm) {
			algorithm->runAlgorithm();
		}
		else {
			cout << options.help();
			return 0;
		}
	}
	catch (exception& e) {
		cout << "Exception\n";
		cout << e.what();
	}
	if (algorithm) delete algorithm;
	return 0;
}