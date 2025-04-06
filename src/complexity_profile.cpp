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
    cout << "Usage: " << progName << " -meta <meta_file> -db <db_file> -id <sequence_id> -k <context_size> -a <smoothing_parameter>" << endl;
    cout << "Example: " << progName << "-meta txt_files/meta.txt -db txt_files/db.txt -k 10 -a 0.01 -id 'NC_005831.2 Human Coronavirus NL63, complete genome'" << endl;
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

int train_markov_model(ContextMap& model, const string& text, int k) {
    for (size_t i = 0; i + k < text.size(); ++i) {
        string context = text.substr(i, k);
        char next = text[i + k];
        model[context][next]++;
    }
    unordered_set<char> alphabet;
    for (const auto& [ctx, transitions] : model) {
        for (const auto& [symbol, _] : transitions) {
            alphabet.insert(symbol);
        }
    }
    return alphabet.size();
}

double calculate_probability(const ContextMap& model, const string& context, char next, double alpha, int alphabet_size) {
    if (model.count(context) == 0) {
        return 1.0 / alphabet_size; 
    }

    const auto& next_counts = model.at(context);
    double total = 0.0;
    for (auto& p : next_counts) {
        total += p.second;
    }

    double count = next_counts.count(next) ? next_counts.at(next) : 0;
    double vocab = alphabet_size; 
    return (count + alpha) / (total + alpha * vocab);
}

void generate_complexity_profile(const string& sequence, const ContextMap& model, int k, double alpha, const string& output_csv, int alphabet_size) {
    ofstream out(output_csv);
    out << "position,complexity\n";

    for (size_t i = k; i < sequence.size(); ++i) {
        string context = sequence.substr(i - k, k);
        char next = sequence[i];
        double p = calculate_probability(model, context, next, alpha, alphabet_size);
        double complexity = -log2(p);
        out << i << "," << complexity << "\n";
    }

    cout << "Gráfico gerado em: " << output_csv << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 11) {
        printUsage(argv[0]);
        return 1;
    }

    string meta_file, db_file, id;
    int k = 0;
    double alpha = 0.0;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-meta") {
            meta_file = argv[++i];
        } else if (arg == "-db") {
            db_file = argv[++i];
        } else if (arg == "-id") {
            id = argv[++i];
        } else if (arg == "-k") {
            k = stoi(argv[++i]);
        } else if (arg == "-a") {
            alpha = stod(argv[++i]);
        } else {
            printUsage(argv[0]);
            return 1;
        }
    }

    string meta_seq = read_meta_sequence(meta_file);

    ContextMap model;
    int alphabet_size = train_markov_model(model, meta_seq, k);

    ifstream db(db_file);
    string seq = read_fasta_sequence(db, id);

    generate_complexity_profile(seq, model, k, alpha, "analysis/perfil_complexidade_" + to_string(k) + "_" + to_string(alpha) + "_" + id + ".csv", alphabet_size);
    return 0;
}