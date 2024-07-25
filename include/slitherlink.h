#pragma once
#include <iostream>

#include "base-lib.h"

namespace algorithms {
	namespace slitherlink {
		class Algorithm : public baselib::PuzzleAlgorithm {
		public:
			Algorithm();
			~Algorithm();
			std::string getName() const override;
			void printFormat(std::ostream& o) override;
		protected:
			void processFile(std::istream& input) override;
		};
	}
}