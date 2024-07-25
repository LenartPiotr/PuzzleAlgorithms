#pragma once
#include <iostream>

namespace baselib{
	class PuzzleAlgorithm {
	private:
		std::string input;
		std::string output;
	public:
		PuzzleAlgorithm();
		virtual ~PuzzleAlgorithm();
		void setInput(std::string value);
		void setOutput(std::string value);
		virtual std::string getName() const;
		virtual void printFormat(std::ostream& o);
		void runAlgorithm();
	protected:
		virtual void processFile(std::istream& o) = 0;
	};
}