#include <base-lib.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>

using namespace baselib;
namespace fs = std::filesystem;

PuzzleAlgorithm::PuzzleAlgorithm() : input("./samples/<alg_name>/in"), output("./samples/<alg_name>/out") { }
PuzzleAlgorithm::~PuzzleAlgorithm() { }
void PuzzleAlgorithm::setInput(std::string value)
{
	input = value;
}

void PuzzleAlgorithm::setOutput(std::string value)
{
	output = value;
}

std::string PuzzleAlgorithm::getName()
{
	return std::string();
}

void PuzzleAlgorithm::printFormat(std::ostream& o) { }

void PuzzleAlgorithm::runAlgorithm(bool measureTime)
{
    std::string toReplace = "<alg_name>";
    std::string inputDir = input;
    std::size_t pos = inputDir.find(toReplace);
    if (pos != std::string::npos) inputDir.replace(pos, toReplace.length(), getName());
    std::string outputDir = output;
    pos = outputDir.find(toReplace);
    if (pos != std::string::npos) outputDir.replace(pos, toReplace.length(), getName());

    if (!fs::exists(inputDir)) {
        fs::create_directories(inputDir);
    }
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (entry.is_regular_file()) {
            std::string inputFilePath = entry.path().string();
            std::string fileName = entry.path().filename().string();
            std::string outputFilePath = outputDir + "/" + fileName;

            std::ifstream inFile(inputFilePath);
            if (!inFile) {
                std::cerr << "Failed to open input file: " << inputFilePath << std::endl;
                continue;
            }

            std::ofstream outFile(outputFilePath);
            if (!outFile) {
                std::cerr << "Failed to open output file: " << outputFilePath << std::endl;
                if (inFile.is_open()) inFile.close();
                continue;
            }

            try {
                std::cout << "File: " << fileName << std::endl;

                auto start_time = std::chrono::high_resolution_clock::now();
                processFile(inFile, outFile);
                auto end_time = std::chrono::high_resolution_clock::now();

                if (measureTime) {
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                    auto milliseconds = duration.count();

                    auto seconds = milliseconds / 1000;
                    milliseconds %= 1000;

                    std::cout << "Duration: " << seconds << ":" << milliseconds << std::endl;
                }
            }
            catch (const WrongFileFormatException& e) {
                std::cerr << "Wrong file format: " << fileName << std::endl;
            }
            catch (const NoSolutionException& e) {
                std::cerr << "No solution: " << fileName << std::endl;
            }
            catch (const std::exception & e) {
                std::cerr << "Failed process file: " << fileName << std::endl;
                std::cerr << e.what() << std::endl;
            }
            cleanUp();

            inFile.close();
            outFile.close();
        }
    }

}
