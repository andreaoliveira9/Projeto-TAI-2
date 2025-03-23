#include "MetaClass.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <cctype>

MetaClass::MetaClass() : k(0) {}

int MetaClass::charToIndex(char c) const {
    c = std::toupper(c);
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

bool MetaClass::loadModel(const std::string &filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Erro ao abrir o ficheiro do modelo: " << filename << std::endl;
        return false;
    }
    inFile.read(reinterpret_cast<char*>(&k), sizeof(int));
    if(!inFile) {
        std::cerr << "Erro a ler k do ficheiro do modelo" << std::endl;
        return false;
    }
    unsigned long numContexts = power4(k);
    counts.resize(numContexts * 4);
    inFile.read(reinterpret_cast<char*>(counts.data()), counts.size() * sizeof(int));
    if(!inFile) {
        std::cerr << "Erro a ler as contagens do modelo" << std::endl;
        return false;
    }
    inFile.close();
    return true;
}

double MetaClass::compressSequence(const std::string &seq, double a) const {
    int n = seq.size();
    if(n < k) return 0.0;
    double cost = 0.0;
    // Para os primeiros k símbolos, assume-se codificação uniforme (2 bits cada)
    cost += k * 2.0;
    
    for (int i = k; i < n; i++) {
        unsigned long index = 0;
        bool valid = true;
        for (int j = i - k; j < i; j++) {
            int idx = charToIndex(seq[j]);
            if(idx < 0) { valid = false; break; }
            index = index * 4 + idx;
        }
        if(!valid) continue;
        int sym = charToIndex(seq[i]);
        if(sym < 0) continue;
        int count = counts[index * 4 + sym];
        int sum = 0;
        for (int s = 0; s < 4; s++) {
            sum += counts[index * 4 + s];
        }
        double prob = (count + a) / (sum + 4 * a);
        cost += -log(prob) / log(2.0);
    }
    return cost;
}

double MetaClass::computeNRC(const std::string &seq, double a) const {
    int n = seq.size();
    if(n == 0) return 0.0;
    double cost = compressSequence(seq, a);
    return cost / (2.0 * n);
}