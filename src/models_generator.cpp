#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <cstdlib>

using namespace std;

// Converte um carácter (A, C, G, T) para índice (0 a 3)
int charToIndex(char c) {
    c = toupper(c);
    switch(c) {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        default: return -1;
    }
}

// Calcula 4^k
unsigned long power4(int k) {
    unsigned long res = 1;
    for (int i = 0; i < k; i++) res *= 4;
    return res;
}

int main(int argc, char* argv[]) {
    if(argc < 5) {
        cerr << "Uso: " << argv[0] << " -i <meta_file> -k <k>" << endl;
        return 1;
    }

    string metaFilename;
    int k;

    for(int i = 1; i < argc; i++){
        string arg = argv[i];
        if(arg == "-i" && i+1 < argc) {
            metaFilename = argv[++i];
        } else if(arg == "-k" && i+1 < argc) {
            k = atoi(argv[++i]);
        } else {
            cerr << "Argumento inválido: " << arg << endl;
            return 1;
        }
    }
    
    // Lê todo o ficheiro meta
    ifstream metaFile(metaFilename);
    if(!metaFile) {
        cerr << "Erro ao abrir o ficheiro " << metaFilename << endl;
        return 1;
    }
    stringstream buffer;
    buffer << metaFile.rdbuf();
    string content = buffer.str();
    
    // Extrai apenas os caracteres A, C, G, T (convertendo para maiúsculas)
    string sequence;
    for (char c : content) {
        char uc = toupper(c);
        if(uc=='A' || uc=='C' || uc=='G' || uc=='T')
            sequence.push_back(uc);
    }
    
    if(sequence.size() < static_cast<size_t>(k+1)) {
        cerr << "Sequência demasiado curta para o k fornecido" << endl;
        return 1;
    }
    
    unsigned long numContexts = power4(k);
    // Cria um vetor de contagens (tamanho = numContexts * 4), inicializado a zero
    vector<int> counts(numContexts * 4, 0);
    
    // Para cada posição da sequência, extrai o contexto de tamanho k e atualiza a contagem do símbolo seguinte
    for(size_t i = 0; i <= sequence.size() - k - 1; i++) {
        unsigned long index = 0;
        bool valid = true;
        for (int j = 0; j < k; j++){
            int idx = charToIndex(sequence[i+j]);
            if(idx < 0) { valid = false; break; }
            index = index * 4 + idx;
        }
        if(!valid) continue;
        int sym = charToIndex(sequence[i+k]);
        if(sym < 0) continue;
        counts[index * 4 + sym]++;
    }
    
    // Grava o modelo no ficheiro binário "model.bin"
    system("mkdir -p models");
    string modelFilename = "./models/k" + to_string(k) + ".bin";
    ofstream outFile(modelFilename, ios::binary);
    if(!outFile) {
        cerr << "Erro ao abrir model.bin para escrita" << endl;
        return 1;
    }
    outFile.write(reinterpret_cast<const char*>(&k), sizeof(int));
    outFile.write(reinterpret_cast<const char*>(counts.data()), counts.size() * sizeof(int));
    outFile.close();
    
    cout << "Modelo gerado e guardado em " + modelFilename << endl;
    return 0;
}