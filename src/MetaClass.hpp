#ifndef METACLASS_HPP
#define METACLASS_HPP

#include <string>
#include <vector>

using namespace std;

class MetaClass {
public:
    int k;                      // Ordem do modelo
    vector<int> counts;    // Vetor de contagens (tamanho: 4^(k) * 4)

    MetaClass();
    
    // Carrega o modelo a partir de um ficheiro binário
    bool loadModel(const string &filename);
    
    // Calcula o custo de compressão (em bits) para a sequência dada, usando suavização com parâmetro a
    double compressSequence(const string &seq, double a, int alphabetSize) const;
    
    // Calcula o Normalized Relative Compression (NRC) para a sequência: NRC = custo / (2 * |seq|)
    double computeNRC(const string &seq, double a) const;
    
private:
    int charToIndex(char c) const;
    unsigned long power4(int k) const;
};

#endif