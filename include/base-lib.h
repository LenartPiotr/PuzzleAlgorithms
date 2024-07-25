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
		void runAlgorithm();

		virtual std::string getName() const;
		virtual void printFormat(std::ostream& o);
	protected:
		virtual void processFile(std::istream& o) = 0;
	};

	class Index {
	public:
		int x;
		int y;
		Index();
		Index(int x, int y);
		Index* neighbours();
		inline Index copy() { return Index(x, y); }

		inline Index operator+(const Index& o) const { return Index(x + o.x, y + o.y); }
		inline Index operator-(const Index& o) const { return Index(x - o.x, y - o.y); }
		inline bool operator==(const Index& o) const { return x == o.x && y == o.y; }
		inline bool operator!=(const Index& o) const { return x != o.x || y != o.y; }

		friend std::ostream& operator<<(std::ostream&, const Index&);
	};

	std::ostream& operator<<(std::ostream&, const Index&);
}