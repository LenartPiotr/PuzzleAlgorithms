#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <functional>

/**
* The baselib namespace provides classes with algorithms for use in many problems
*/
namespace baselib{

	class AlgorithmException : public std::exception { };
	class WrongFileFormatException : public AlgorithmException { };
	class NoSolutionException : public AlgorithmException { };

	/**
	* Base class for algorithms solving specific problems
	*/
	class PuzzleAlgorithm {
	private:
		std::string input;
		std::string output;
	public:
		PuzzleAlgorithm();
		virtual ~PuzzleAlgorithm();

		/**
		* Sets the path to the input directory.
		* The substring "<alg_name>" will be replaced in the runAlgorithm() method by the algorithm name
		*/
		void setInput(std::string value);
		/**
		* Sets the path to the output directory.
		* The substring "<alg_name>" will be replaced in the runAlgorithm() method by the algorithm name
		*/
		void setOutput(std::string value);

		/**
		* Run the algorithm for each file in the input folder
		*/
		void runAlgorithm();

		virtual std::string getName();
		/**
		* Prints the format for the user that the input file should have to be handled by the algorithm
		*/
		virtual void printFormat(std::ostream& o);
	protected:
		/**
		* The concrete method reads the problem from a file. It processes it, then writes the result to the file. The body of the function should be overridden
		* 
		* @param inFile
		*	stream to input file with problem to read
		* @param outFile
		*	stream to file for saving results
		* @param fileName
		*	name of current process file
		*/
		virtual void processFile(std::ifstream& inFile, std::ofstream& outFile, const std::string& fileName) = 0;
		/**
		* The function is always called after processFile.
		* It should be used to free variables before running the algorithm for the next file.
		*/
		virtual void cleanUp() = 0;
	};

	/**
	* Represents the x and y coordinates in a two-dimensional array
	*/
	class Index {
	public:
		int x;
		int y;
		Index();
		Index(const Index& other);
		Index(int x, int y);
		/**
		* Returns the coordinates of the neighboring indices
		* @param sides Whether to return neighbors located on the sides (top, bottom, left, right).
		* @param corners Whether to return neighbors located on the corners.
		* @return A vector of neighboring coordinates.
		*/
		std::vector<Index> neighbours(bool sides, bool corners) const;
		inline Index copy() { return Index(x, y); }

		inline Index operator+(const Index& o) const { return Index(x + o.x, y + o.y); }
		inline Index operator-(const Index& o) const { return Index(x - o.x, y - o.y); }
		inline bool operator==(const Index& o) const { return x == o.x && y == o.y; }
		inline bool operator!=(const Index& o) const { return x != o.x || y != o.y; }
		inline bool operator<(const Index& o) const { return y < o.y || (y == o.y && x < o.x); }

		friend std::ostream& operator<<(std::ostream&, const Index&);
	};

	std::ostream& operator<<(std::ostream&, const Index&);

	/**
	* Represents a board of dimensions width x height that can hold elements of type T.
	* @tparam T The type of elements stored on the board.
	*/
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
		/**
		* Processes each element of the board by calling the given function on its coordinates and value.
		* @param func Function called for each element of the board. Arguments are the coordinates and value of the element.
		*/
		void forEach(std::function <void(Index, T)> func)
		{
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					func(Index(x, y), tab[x][y]);
				}
			}
		}
		/**
		* Checks if the given index is within the board's scope.
		* @param index The index coordinates.
		* @return true if the index is within the board's scope, false otherwise.
		*/
		inline bool inRange(const Index& index) const { return index.x >= 0 && index.x < width && index.y >= 0 && index.y < height; }

		/**
		* Indexing operator (element access) for the board.
		* @param idx Coordinate index.
		* @return The element at the given index.
		* @throws std::out_of_range When the index is outside the board's range.
		*/
		T& operator [](const Index& idx)
		{
			if (inRange(idx)) {
				return tab[idx.x][idx.y];
			}
			throw std::out_of_range("Index out of range");
		}
		/**
		* Indexing operator (element access) for the board.
		* @param idx Coordinate index.
		* @return The element at the given index.
		* @throws std::out_of_range When the index is outside the board's range.
		*/
		const T& operator [](const Index& idx) const
		{
			if (inRange(idx)) {
				return tab[idx.x][idx.y];
			}
			throw std::out_of_range("Index out of range");
		}

		/**
		* Indexing operator without using the Index object.
		* The inline method does not check if the index is in the range of the array. For safety, use the [] operator with Index.
		* @param x The x coordinate.
		* @return A pointer to a row in the table.
		*/
		inline T* operator [](const int& x)
		{
			return tab[x];
		}
		/**
		* Indexing operator without using the Index object.
		* The inline method does not check if the index is in the range of the array. For safety, use the [] operator with Index.
		* @param x The x coordinate.
		* @return A pointer to a row in the table.
		*/
		inline const T* operator [](const int& x) const
		{
			return tab[x];
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

	/**
	* Represents a coordinate transformation on the board with the possibility of rotation and mirroring.
	*/
	class IndexTransform {
	private:
		int areaWidth;
		int areaHeight;
	public:
		bool flipX;
		bool flipY;
		int rotation; /// Number of 90 degree turns to the right. The value should be 0, 1, 2 or 3.

		IndexTransform(int width, int height, bool flipX = false, bool flipY = false, int rotation = 0);
		
		/**
		* Applies the transformation to the given index.
		* @param idx The index to transform.
		* @return The new index after applying the transformation.
		*/
		Index transform(const Index& idx) const;

		/**
		* Calls transform method
		*/
		Index operator*(const Index& idx);
	};

	/**
	* Calls IndexTransform transform method
	*/
	Index operator *(const Index& idx, const IndexTransform& transform);
}