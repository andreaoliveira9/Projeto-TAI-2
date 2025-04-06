#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include "MetaClass.hpp"
#include <cctype>
#include <cmath> 

using namespace std;

struct SequenceResult {
    string id;
    string seq;
    double nrc;
};

int charToIndex(char c) {
    c = toupper(c);
    switch(c) {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        default:   return -1;
    }
}

unsigned long power4(int k) {
    unsigned long res = 1;
    for (int i = 0; i < k; i++)
        res *= 4;
    return res;
}

void trim(string &s) {
    while(!s.empty() && isspace(s.back()))
        s.pop_back();
}

vector<int> countContexts(const string& sequence, int k) {
    if (sequence.size() < static_cast<size_t>(k + 1))
        throw runtime_error("Sequência demasiado curta para o valor de k fornecido.");

    unsigned long numContexts = power4(k);

  vector<int> counts(numContexts * 4, 0);

    unsigned long context = 0;
    for (int j = 0; j < k; j++) {
        int idx = charToIndex(sequence[j]);
        if (idx < 0)
            throw runtime_error("Caractere inválido encontrado na sequência.");
        context = context * 4 + idx;
    }

    int sym = charToIndex(sequence[k]);
    if (sym >= 0)
      counts[context * 4 + sym]++;

    unsigned long highest = power4(k - 1);

    for (size_t i = 1; i <= sequence.size() - k - 1; i++) {
        int new_digit = charToIndex(sequence[i + k - 1]);
        context = (context % highest) * 4 + new_digit;
        int next_sym = charToIndex(sequence[i + k]);
        if (next_sym >= 0)
            counts[context * 4 + next_sym]++;
    }

    return counts;
}

int main(int argc, char* argv[]){
    if(argc < 9) {
        cerr << "Uso: " << argv[0] << " -i <db_file> -m <model> -a <alpha> -t <top>" << endl;
        return 1;
    }
    
    string db_filename, id1, id2;
    int k;
    double a;
    
    for(int i = 1; i < argc; i++){
        string arg = argv[i];
        if(arg == "-db" && i+1 < argc) {
            db_filename = argv[++i];
        } else if(arg == "-a" && i+1 < argc) {
            a = atof(argv[++i]);
        } else if(arg == "-k" && i+1 < argc) {
            k = atoi(argv[++i]);
        } else if(arg == "-id1" && i+1 < argc) {
            id1 = argv[++i];
        } else if(arg == "-id2" && i+1 < argc) {
            id2 = argv[++i];
        } else {
            cerr << "Argumento inválido: " << arg << endl;
            return 1;
        }
    }
    
    ifstream dbFile(db_filename);
    if(!dbFile) {
        cerr << "Erro ao abrir o ficheiro da base de dados: " << db_filename << endl;
        return 1;
    }

    bool found1 = false, found2 = false;
    bool reading1 = false, reading2 = false;
    string line;
    string seq1, seq2;

    while (getline(dbFile, line)) {
        trim(line);
        if (line.empty()) continue;

        if (line == "@" + id1) {
            reading1 = true;
            reading2 = false;
            continue;
        } else if (line == "@" + id2) {
            reading2 = true;
            reading1 = false;
            continue;
        } else if (line[0] == '@') {
            reading1 = false;
            reading2 = false;
            continue;
        }

        if (reading1) {
            seq1 += line;
            found1 = true;
        } else if (reading2) {
            seq2 += line;
            found2 = true;
        }
    }

    if(!found1 || !found2) {
        cerr << "Identificadores não encontrados na base de dados." << endl;
        return 1;
    }
    
    vector<int> counts1 = countContexts(seq1, k);
    vector<int> counts2 = countContexts(seq2, k);

    MetaClass model1, model2;
    model1.setCounts(counts1);
    model1.setK(k);
    model2.setCounts(counts2);
    model2.setK(k);

    double nrc12 = model1.computeNRC(seq2, a);
    double nrc21 = model2.computeNRC(seq1, a);
    double meanNRC = (nrc12 + nrc21) / 2.0;

    double similarity = exp(-meanNRC);
    cout << "NRC (id1 → id2): " << nrc12 << endl;
    cout << "NRC (id2 → id1): " << nrc21 << endl;
    cout << "NRC médio: " << meanNRC << endl;
    cout << "Similaridade (exp(-NRC)): " << similarity << endl;
    
    return 0;
}