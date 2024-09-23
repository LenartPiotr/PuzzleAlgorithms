#pragma once
#include <iostream>
#include <vector>

#include "base-lib.h"

using namespace baselib;

namespace algorithms {
	namespace signpost {
		
		struct Node {
			int order;
			int nextNode;
			int prevNode;
			std::vector<int> next;
			std::vector<int> prev;
		};

		struct Range {
			int startOrder;
			int startNode;
			int endOrder;
			int endNode;
			inline int length() const { return endOrder - startOrder + 1; }
		};

		class Algorithm : public PuzzleAlgorithm {
		protected:
			bool updateFlag;
			int startIndex;
			int endIndex;
			int width;
			int height;
			std::vector<Node> nodes;
			std::vector<Range> ranges;
		public:
			Algorithm();
			~Algorithm();
			std::string getName() override;
			void printFormat(std::ostream& o) override;
		protected:
			void prepare(std::ifstream& in);
			void processFile(std::ifstream& inFile, std::ofstream& outFile) override;
			void cleanUp() override;

			void mainLoop();

			void disconnect(int prev, int next);
			void connect(int prev, int next);

			void prepareRanges();

			void simplifyGraph();
			void connectRanges();
			void searchPath(std::vector<std::vector<int>>& solutions, std::vector<int>& currentSolution, std::vector<bool>& visited, int node, int order, int targetNode, int targetOrder);
			void findPaths();

			void printNodes(std::ostream& os);
			void printRanges(std::ostream& os);

		public:
			friend std::ostream& operator<<(std::ostream&, const Algorithm&);
		};

		std::ostream& operator<<(std::ostream&, const Algorithm&);

	};
}