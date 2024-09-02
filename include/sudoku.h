#pragma once
#include <iostream>

#include "base-lib.h"

using namespace baselib;

/*

Smart backtracking algorithm

*/

namespace algorithms {
	namespace sudoku {

		struct Memory {
			int row;
			int col;
			int area;
			int mask;
		};

		class Algorithm : public PuzzleAlgorithm {
		protected:
			// Numbers in table from 0 to 8. -1 if empty
			int tab[9][9];
			// Are values in original state
			bool taken[9][9];

			// Masks that says which numbers are allow to enter in specific area
			// Number l is allowed to place if 1 << l is 1. Therefor numbers are from 0 to 8 not from 1 to 9. Full mask is 0x1ff
			// Mask in field is row & col & area
			int rows[9];
			int cols[9];
			int area[3][3];
		public:
			Algorithm();
			~Algorithm();
			std::string getName() override;
			void printFormat(std::ostream& o) override;
		protected:
			void processFile(std::ifstream& inFile, std::ofstream& outFile, const std::string& fileName) override;
			void cleanUp() override;
			void prepare(std::ifstream& inFile);

			inline void nextFreeField(int& i) const;
			inline void backToPreviousField(int& i) const;

			bool mainLoop();

		public:
			friend std::ostream& operator<<(std::ostream&, const Algorithm&);
		};

		std::ostream& operator<<(std::ostream&, const Algorithm&);

	};
}
