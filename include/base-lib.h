#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <functional>

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

		virtual std::string getName();
		virtual void printFormat(std::ostream& o);
	protected:
		virtual void processFile(std::ifstream& inFile, std::ofstream& outFile, const std::string& fileName) = 0;
		virtual void cleanUp() = 0;
	};

	class Index {
	public:
		int x;
		int y;
		Index();
		Index(int x, int y);
		std::vector<Index> neighbours(bool, bool);
		inline Index copy() { return Index(x, y); }

		inline Index operator+(const Index& o) const { return Index(x + o.x, y + o.y); }
		inline Index operator-(const Index& o) const { return Index(x - o.x, y - o.y); }
		inline bool operator==(const Index& o) const { return x == o.x && y == o.y; }
		inline bool operator!=(const Index& o) const { return x != o.x || y != o.y; }

		friend std::ostream& operator<<(std::ostream&, const Index&);
	};

	std::ostream& operator<<(std::ostream&, const Index&);

	template <typename T>
	class Board {
	private:
		int width;
		int height;
	protected:
		T** tab;
	public:
		Board(int width, int height) : width(width), height(height)
		{
			tab = new T*[width];
			for (int x = 0; x < width; x++) {
				tab[x] = new T[height];
			}
		}
		~Board()
		{
			for (int x = 0; x < width; x++) {
				delete[] tab[x];
			}
			delete[] tab;
		}
		inline int getWidth() const { return width; }
		inline int getHeight() const { return height; }
		void forEach(std::function <void(Index, T)> func)
		{
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					func(Index(x, y), tab[x][y]);
				}
			}
		}
		inline bool inRange(const Index& index) const { return index.x >= 0 && index.x < width && index.y >= 0 && index.y < height; }

		T& operator [](const Index& idx)
		{
			if (inRange(idx)) {
				return tab[idx.x][idx.y];
			}
			throw std::out_of_range("Index out of range");
		}
		const T& operator [](const Index& idx) const
		{
			if (inRange(idx)) {
				return tab[idx.x][idx.y];
			}
			throw std::out_of_range("Index out of range");
		}

		template <typename T2>
		friend std::ostream& operator<<(std::ostream&, const Board<T2>&);
	};

	template <typename T>
	std::ostream& operator<<(std::ostream& os, const Board<T>& board)
	{
		for (int y = 0; y < board.height; y++) {
			for (int x = 0; x < board.width; x++) {
				os << board.tab[x][y] << " ";
			}
			os << std::endl;
		}
		return os;
	}

	class IndexTransform {
	private:
		Index hook;
		int areaWidth;
		int areaHeight;
	public:
		bool flipX;
		bool flipY;
		int rotation;

		IndexTransform(const Index& hook, int width, int height);
		Index operator*(const Index& idx);
	};
}