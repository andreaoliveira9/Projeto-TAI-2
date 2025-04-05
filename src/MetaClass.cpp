#include "MetaClass.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cctype>

using namespace std;

MetaClass::MetaClass() : k(0) {}

int MetaClass::charToIndex(char c) const {
    c = toupper(c);
    switch(c) {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        default: return -1;
    }
}

unsigned long MetaClass::power4(int k) const {
    unsigned long res = 1;
    for (int i = 0; i < k; i++)
        res *= 4;
    return res;
}

bool MetaClass::loadModel(const string &filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        cerr << "Erro ao abrir o ficheiro do modelo: " << filename << endl;
        return false;
    }
    inFile.read(reinterpret_cast<char*>(&k), sizeof(int));
    if(!inFile) {
        cerr << "Erro a ler k do ficheiro do modelo" << endl;
        return false;
    }
    unsigned long numContexts = power4(k);
    counts.resize(numContexts * 4);
    inFile.read(reinterpret_cast<char*>(counts.data()), counts.size() * sizeof(int));
    if(!inFile) {
        cerr << "Erro a ler as contagens do modelo" << endl;
        return false;
    }
    inFile.close();
    return true;
}

double MetaClass::compressSequence(const string &seq, double a, int alphabetSize) const {
    int n = seq.size();
    if (n == 0) {
        return 0.0;
    }
    
    double cost = 0.0;
    
    int initialSymbols = min(n, k);
    cost += initialSymbols * log2(alphabetSize);
    
    for (int i = k; i < n; i++) {
        unsigned long index = 0;
        bool validContext = true;
        for (int j = i - k; j < i; j++) {
            int idx = charToIndex(seq[j]);
            if (idx < 0 || idx >= alphabetSize) {
                validContext = false;
                break;
            }
            index = index * alphabetSize + idx;
        }
        int sym = charToIndex(seq[i]);
        if (sym < 0 || sym >= alphabetSize) {
            validContext = false;
        }
        if (!validContext) {
            cost += log2(alphabetSize);
            continue;
        }
        int countSymbol = counts[index * alphabetSize + sym];
        int sumContext = 0;
        for (int s = 0; s < alphabetSize; s++) {
            sumContext += counts[index * alphabetSize + s];
        }
        double prob = (countSymbol + a) / (sumContext + a * alphabetSize);
        cost += -log2(prob);
    }
    return cost;
}

double MetaClass::computeNRC(const string &seq, double a) const {
    int n = seq.size();
    if(n == 0) return 0.0;
    int alphabetSize = counts.size() / power4(k);
    double cost = compressSequence(seq, a, alphabetSize);
    return cost / (log2(alphabetSize) * n);
}

void MetaClass::setCounts(const vector<int> &counts) {
    this->counts = counts;
}

void MetaClass::setK(int k) {
    this->k = k;
}