#pragma once
#include <iostream>
#include <vector>
#include <set>

#include "base-lib.h"

using namespace baselib;

/*
The algorithm for solving the slitherlink puzzle uses coloring of the fields.
If the colors are opposite (for example 2 and -2) then a line runs between these fields.
The surroundings have color 1 so the algorithm aims to paint the entire board with colors 1 and -1.

In the loop, as long as there are changes, the algorithm goes through all the squares that
contain numbers and checks if it can apply certain rules to them.

For example, if the number 3 is present and two neighboring squares are painted the same color - say 4 - then
it is known that the square with the number will get the same color 4 and the other neighbors are
painted the opposite color to each other. If they do not have a color, they will be colored with the
next free color (5 and -5 if it is free). If one of them already has a color or both have a different
color then the ColorBoard class tries to ensure that the coloring is satisfied and ensures the integrity
of the whole board by performing a merge colors.
*/

namespace algorithms {
	namespace slitherlink {
		/**
		* The class responsible for coloring. It holds the update flag and is responsible for integrity.
		*/
		class ColorBoard : public Board<int> {
		protected:
			int colorMargin;
			int nextColor;
		public:
			bool updateFlag;
		public:
			ColorBoard(int width, int height);
			/**
			* Colors the fields with the given indexes the same color.
			* If the fields do not contain colors (value 0) a new color will be assigned,
			* if the fields contain more than 1 color already entered then the mergeColors method will be called.
			* 
			* @param indexes - list of indexes of fields to paint.
			* @param oposite - optional list of indexes of fields to paint in the opposite color.
			*/
			void colorSame(std::vector<Index> indexes, std::vector<Index> oposite);

			/**
			* Unifies the given colors and their opposites into one lowest on the list.
			* Example: If the numbers 1 and -2 are given, all -2 numbers will become 1 and all 2 numbers will become -1
			*/
			int mergeColors(const std::set<int>& colors);
			/**
			* Reduces colors to the smallest possible values
			*/
			void normalizeColors();

			int& operator [](const Index& idx);
			const int& operator [](const Index& idx) const;
			
			int* operator [](const int& x);
			const int* operator [](const int& x) const;

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

			/**
			* Finds constant patterns once at the beginning of the algorithm. (for example, two 3's next to each other)
			*/
			void findPatterns();
			void mainLoop();

			/**
			* For each number, it checks its own and his neighbors' colors, trying to apply the rules.
			*/
			void stepCountNeighbours();
			void stepCheckCrosses();
			void stepExpandAreas();
			
		public:
			friend std::ostream& operator<<(std::ostream&, const Algorithm&);
		};
		
		std::ostream& operator<<(std::ostream&, const Algorithm&);

		struct BFSAreaResult {
			int color;
			bool connectOutside;
			std::set<Index> area;
			std::set<Index> connections;
		};

		class BFSAreaVerifier {
		private:
			Board<bool> visited;
			const ColorBoard& colors;

		public:
			BFSAreaVerifier(const ColorBoard& colors);

		private:
			BFSAreaResult bfs(Index start);

		public:
			std::vector<BFSAreaResult> findAll();
		};
	}
}