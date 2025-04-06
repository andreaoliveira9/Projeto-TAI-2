#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <vector>
#include <cstdlib>

using namespace std;

void printUsage(const string& progName) {
    cout << "Usage: " << progName << " -meta <source_file> -db <input_file> -id -k <order> -a <smoothing_parameter>" << endl;
    cout << "Example: " << progName << " -meta meta.txt -db db.txt -id NewMutation1 -k 3 -a 0.1" << endl;
}

void saveModelBinary(const unordered_map<string, unordered_map<char, int>>& contextCounts, const string& filename) {
    ofstream out(filename, ios::binary);
    if (!out) {
        cerr << "Erro: Não foi possível abrir o ficheiro " << filename << " para escrita." << endl;
        return;
    }
    
    size_t numContexts = contextCounts.size();
    out.write(reinterpret_cast<const char*>(&numContexts), sizeof(numContexts));
    
    for (const auto& [context, freqMap] : contextCounts) {
        size_t contextLength = context.size();
        out.write(reinterpret_cast<const char*>(&contextLength), sizeof(contextLength));
        out.write(context.data(), contextLength);
        
        size_t mapSize = freqMap.size();
        out.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
        
        for (const auto& [symbol, count] : freqMap) {
            out.write(reinterpret_cast<const char*>(&symbol), sizeof(symbol));
            out.write(reinterpret_cast<const char*>(&count), sizeof(count));
        }
    }
    
    out.close();
}


double calculateAverageInformation(const unordered_map<string, unordered_map<char, int>>& contextCounts, const string& text, int k, double alpha) {
    unordered_set<char> alphabet(text.begin(), text.end());
    int alphabetSize = alphabet.size();
    
    double totalInfo = 0.0;
    int countSymbols = text.size() - k;
    
    for (size_t i = k; i < text.size(); i++) {
        string context = text.substr(i - k, k);
        char symbol = text[i];
        
        auto it = contextCounts.find(context);
        if (it == contextCounts.end()) continue;
        
        int totalCount = 0;
        int symbolCount = 0;
        // Safely get the count for symbol (defaulting to 0 if not found)
        auto freqIt = it->second.find(symbol);
        if (freqIt != it->second.end()) {
            symbolCount = freqIt->second;
        }
        
        for (const auto& [_, count] : it->second) {
            totalCount += count;
        }
        
        double probability = ( symbolCount + alpha) / (totalCount + alpha * alphabetSize);
        double logProb = -log2(probability);

        totalInfo += logProb;
    
        // Output per-symbol probability for plotting
        cout << i << " " << logProb << " " << symbol << endl;
    }
    
    return totalInfo / countSymbols;
}

int main(int argc, char* argv[]) {
    // Expecting exactly 9 arguments: program name, -s, source_file, -i, input_file, -k, order, -a, smoothing_parameter
    if (argc != 9) {
        printUsage(argv[0]);
        return 1;
    }
    
    string sourceFile;
    string inputFile;
    int k = 0;
    double alpha = 0.0;
    
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-s" && i + 1 < argc) {
            sourceFile = argv[++i];
        } else if (arg == "-i" && i + 1 < argc) {
            inputFile = argv[++i];
        } else if (arg == "-k" && i + 1 < argc) {
            k = stoi(argv[++i]);
        } else if (arg == "-a" && i + 1 < argc) {
            alpha = stod(argv[++i]);
        } else {
            cerr << "Unknown or incomplete parameter: " << arg << endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Read the source file to build the model
    ifstream sourceStream(sourceFile);
    if (!sourceStream) {
        cerr << "Erro: Não foi possível abrir o ficheiro " << sourceFile << endl;
        return 1;
    }

    stringstream sourceBuffer;
    string line;
    while (getline(sourceStream, line)) {
        // Optionally, remove extra whitespace (if needed)
        // line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        // For now, we simply append the line (getline removes the '\n')
        sourceBuffer << line;
    }
    std::string sourceText = sourceBuffer.str();
    sourceStream.close();
    
    if (sourceText.size() <= static_cast<size_t>(k)) {
        cerr << "Erro: O comprimento do texto do source_file é menor ou igual à ordem k." << endl;
        return 1;
    }
    
    // Build context model from sourceText
    unordered_map<string, unordered_map<char, int>> contextCounts;
    for (size_t i = k; i < sourceText.size(); i++) {
        string context = sourceText.substr(i - k, k);
        char symbol = sourceText[i];
        contextCounts[context][symbol]++;
    }

    // Save model binary
    // saveModelBinary(contextCounts, "model.bin");
    // cout << "Model saved to model.bin" << endl;

    // Read the input file for analysis
    ifstream inputStream(inputFile);
    if (!inputStream) {
        cerr << "Erro: Não foi possível abrir o ficheiro " << inputFile << endl;
        return 1;
    }

    std::stringstream inputBuffer;
    while (std::getline(inputStream, line)) {
        // Append each line (newlines removed by getline)
        inputBuffer << line;
    }
    std::string inputText = inputBuffer.str();
    inputStream.close();
    
    if (inputText.size() <= static_cast<size_t>(k)) {
        cerr << "Erro: O comprimento do texto do input_file é menor ou igual à ordem k." << endl;
        return 1;
    }

    double averageInfo = calculateAverageInformation(contextCounts, inputText, k, alpha);
    // cout << "Average Information Content: " << averageInfo << " bits per symbol" << endl;
    
    return 0;
}