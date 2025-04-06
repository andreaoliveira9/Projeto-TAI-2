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

using ContextMap = unordered_map<string, unordered_map<char, int>>;

void printUsage(const string& progName) {
    cout << "Usage: " << progName << " -meta <source_file> -db <input_file> -id -k <order> -a <smoothing_parameter>" << endl;
    cout << "Example: " << progName << " -meta meta.txt -db db.txt -id NewMutation1 -k 3 -a 0.1" << endl;
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

string read_fasta_sequence(ifstream& file, const string& target_id) {
    string line, sequence, current_id;
    bool found = false;

    while (getline(file, line)) {
        if (line[0] == '@') {
            if (line.substr(1).find(target_id) != string::npos) {
                found = true;
                sequence.clear();
                current_id = line.substr(1);
            } else {
                found = false;
            }
        } else if (found) {
            sequence += line;
        }
    }

    if (sequence.empty()) {
        cerr << "ID \"" << target_id << "\" não encontrado.\n";
        exit(1);
    }

    return sequence;
}

string read_meta_sequence(const string& filename) {
    ifstream file(filename);
    string line, sequence;
    while (getline(file, line)) {
        sequence += line;
    }
    return sequence;
}

void train_markov_model(ContextMap& model, const string& text, int k) {
    for (size_t i = 0; i + k < text.size(); ++i) {
        string context = text.substr(i, k);
        char next = text[i + k];
        model[context][next]++;
    }
}

double calculate_probability(const ContextMap& model, const string& context, char next, double alpha) {
    if (model.count(context) == 0) {
        return 1.0 / 4.0;  // uniform prob if unseen context
    }

    const auto& next_counts = model.at(context);
    double total = 0.0;
    for (auto& p : next_counts) {
        total += p.second;
    }

    double count = next_counts.count(next) ? next_counts.at(next) : 0;
    double vocab = 4.0; // A, C, G, T
    return (count + alpha) / (total + alpha * vocab);
}

void generate_complexity_profile(const string& sequence, const ContextMap& model, int k, double alpha, const string& output_csv) {
    ofstream out(output_csv);
    out << "position,complexity\n";

    for (size_t i = k; i < sequence.size(); ++i) {
        string context = sequence.substr(i - k, k);
        char next = sequence[i];
        double p = calculate_probability(model, context, next, alpha);
        double complexity = -log2(p);
        out << i << "," << complexity << "\n";
    }

    cout << "Gráfico gerado em: " << output_csv << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 6) {
        cerr << "Uso: ./perfil_complexidade <meta.txt> <db.txt> <ordem_k> <alfa> <ID>\n";
        return 1;
    }

    string meta_file = argv[1];
    string db_file = argv[2];
    int k = stoi(argv[3]);
    double alpha = stod(argv[4]);
    string id = argv[5];

    string meta_seq = read_meta_sequence(meta_file);

    ContextMap model;
    train_markov_model(model, meta_seq, k);

    ifstream db(db_file);
    string seq = read_fasta_sequence(db, id);

    generate_complexity_profile(seq, model, k, alpha, "analysis/perfil_complexidade_" + to_string(k) + "_" + to_string(alpha) + "_" + id + ".csv");
    return 0;
}