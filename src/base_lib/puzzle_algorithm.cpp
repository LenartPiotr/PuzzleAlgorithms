#include <base-lib.h>

using namespace baselib;

PuzzleAlgorithm::PuzzleAlgorithm()
{
	input = "./samples/" + getName() + "/in/";
}
PuzzleAlgorithm::~PuzzleAlgorithm() { }
void PuzzleAlgorithm::setInput(std::string value)
{
	input = value;
}

void PuzzleAlgorithm::setOutput(std::string value)
{
	output = value;
}

std::string PuzzleAlgorithm::getName() const
{
	return std::string();
}

void PuzzleAlgorithm::printFormat(std::ostream& o) { }

void PuzzleAlgorithm::runAlgorithm()
{
	// TODO
}
