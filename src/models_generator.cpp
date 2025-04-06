#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

void printUsage(const string& progName) {
    cout << "Usage: " << progName << " -meta <meta_file> -k <context_size>" << endl;
    cout << "Example: " << progName << "-meta txt_files/meta.txt -k 13" << endl;
}

// Converte um caractere (A, C, G, T) para índice (0 a 3)
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

// Calcula 4^k
unsigned long power4(int k) {
    unsigned long res = 1;
    for (int i = 0; i < k; i++)
        res *= 4;
    return res;
}

// Lê o conteúdo completo de um arquivo
string readFile(const string& filename) {
    ifstream file(filename);
    if (!file)
        throw runtime_error("Erro ao abrir o arquivo " + filename);
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Extrai da string apenas os caracteres A, C, G, T (em maiúsculo)
string extractSequence(const string& content) {
    string sequence;
    for (char c : content) {
        char uc = toupper(c);
        if (uc == 'A' || uc == 'C' || uc == 'G' || uc == 'T')
            sequence.push_back(uc);
    }
    return sequence;
}

// Calcula as contagens dos contextos utilizando a técnica de janela deslizante
vector<int> countContexts(const string& sequence, int k) {
    if (sequence.size() < static_cast<size_t>(k + 1))
        throw runtime_error("Sequência demasiado curta para o valor de k fornecido.");

    unsigned long numContexts = power4(k);
    // Vetor de contagens: cada contexto (numContexts) com 4 possíveis símbolos seguintes
    vector<int> counts(numContexts * 4, 0);

    // Calcula o índice do primeiro contexto (janela de tamanho k)
    unsigned long context = 0;
    for (int j = 0; j < k; j++) {
        int idx = charToIndex(sequence[j]);
        if (idx < 0)
            throw runtime_error("Caractere inválido encontrado na sequência.");
        context = context * 4 + idx;
    }
    // Conta o símbolo que segue o primeiro contexto
    int sym = charToIndex(sequence[k]);
    if (sym >= 0)
        counts[context * 4 + sym]++;

    // Pre-calcula 4^(k-1) para atualizar a janela deslizante
    unsigned long highest = power4(k - 1);

    // Atualiza os contextos de forma deslizante e incrementa as contagens
    for (size_t i = 1; i <= sequence.size() - k - 1; i++) {
        int new_digit = charToIndex(sequence[i + k - 1]);
        context = (context % highest) * 4 + new_digit;
        int next_sym = charToIndex(sequence[i + k]);
        if (next_sym >= 0)
            counts[context * 4 + next_sym]++;
    }

    return counts;
}

// Grava o modelo (valor de k e as contagens) em um arquivo binário
void writeModel(const string& modelFilename, int k, const vector<int>& counts) {
    // Cria o diretório "models" de forma portável
    fs::create_directories("models");

    ofstream outFile(modelFilename, ios::binary);
    if (!outFile)
        throw runtime_error("Erro ao abrir " + modelFilename + " para escrita");

    outFile.write(reinterpret_cast<const char*>(&k), sizeof(int));
    outFile.write(reinterpret_cast<const char*>(counts.data()), counts.size() * sizeof(int));
    outFile.close();
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        printUsage(argv[0]);
        return 1;
    }

    string metaFilename;
    int k = 0;

    // Processa os argumentos da linha de comando
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-meta" && i + 1 < argc) {
            metaFilename = argv[++i];
        } else if (arg == "-k" && i + 1 < argc) {
            try {
                k = stoi(argv[++i]);
            } catch (const invalid_argument& e) {
                cerr << "Valor inválido para k: " << argv[i] << endl;
                return 1;
            }
        } else {
            cerr << "Argumento inválido: " << arg << endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    if (k <= 0) {
        cerr << "O valor de k deve ser um inteiro positivo." << endl;
        return 1;
    }
    if (metaFilename.empty()) {
        cerr << "Nome do arquivo meta não fornecido." << endl;
        return 1;
    }

    try {
        // Lê o conteúdo do arquivo e extrai a sequência
        string content = readFile(metaFilename);
        string sequence = extractSequence(content);

        // Calcula as contagens dos contextos
        vector<int> counts = countContexts(sequence, k);

        // Define o nome do arquivo do modelo e grava o modelo
        string modelFilename = "models/k" + to_string(k) + ".bin";
        writeModel(modelFilename, k, counts);

        cout << "Modelo gerado e guardado em " << modelFilename << endl;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}