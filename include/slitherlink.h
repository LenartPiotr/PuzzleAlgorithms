#pragma once
#include <iostream>
#include <vector>
#include <set>

#include "base-lib.h"

using namespace baselib;

namespace algorithms {
	namespace slitherlink {
		class ColorBoard : public Board<int> {
		protected:
			int colorMargin;
			int nextColor;
		public:
			bool updateFlag;
		public:
			ColorBoard(int width, int height);
			void colorSame(std::vector<Index> indexes, std::vector<Index> oposite);
			int mergeColors(const std::set<int>& colors);
			void normalizeColors();

			int& operator [](const Index& idx);
			const int& operator [](const Index& idx) const;

			friend std::ostream& operator<<(std::ostream&, const ColorBoard&);
		};

		std::ostream& operator<<(std::ostream&, const ColorBoard&);
		
		class Algorithm : public baselib::PuzzleAlgorithm {
		protected:
			ColorBoard* colorBoard;
			Board<int>* numbers;
			std::vector<Index> updateNumbers;
		public:
			Algorithm();
			~Algorithm();
			std::string getName() override;
			void printFormat(std::ostream& o) override;
		protected:
			void processFile(std::ifstream& inFile, std::ofstream& outFile, const std::string& fileName) override;
			void cleanUp() override;

			void findPatterns();
			void mainLoop();

			void stepCountNeighbours();
			void stepCheckCrosses();
		};
	}
}