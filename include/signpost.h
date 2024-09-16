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
			int start;
			int end;
			inline int length() const { return end - start + 1; }
		};

		class Algorithm : public PuzzleAlgorithm {
		protected:
			int startIndex;
			int endIndex;
			std::vector<Node> nodes;
		public:
			Algorithm();
			~Algorithm();
			std::string getName() override;
			void printFormat(std::ostream& o) override;
		protected:
			void prepare(std::ifstream& in);
			void processFile(std::ifstream& inFile, std::ofstream& outFile) override;
			void cleanUp() override;

			void disconnect(int prev, int next);
			void connect(int prev, int next);

			void simplifyGraph();
			void findPaths();

			void printNodes(std::ostream& os);

		public:
			friend std::ostream& operator<<(std::ostream&, const Algorithm&);
		};

		std::ostream& operator<<(std::ostream&, const Algorithm&);

	};
}