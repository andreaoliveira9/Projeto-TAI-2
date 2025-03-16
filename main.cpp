#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include <cmath>
#include <algorithm>
#include <cctype>

using namespace std;

// Função para mapear um caractere (A, C, G, T) para índice 0..3.
int charToIndex(char c) {
    c = toupper(c);
    if (c == 'A') return 0;
    if (c == 'C') return 1;
    if (c == 'G') return 2;
    if (c == 'T') return 3;
    return -1; // caractere inválido
}

// Estrutura para armazenar cada sequência do banco de dados
struct Sequence {
    string id;
    string seq;
};

int main(int argc, char* argv[]) {
    // Parâmetros padrão
    string sampleFile, dbFile;
    int k = 10;
    double a = 0.1;
    int topCount = 20;

    // Processa argumentos da linha de comando
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-s" && i + 1 < argc) {
            sampleFile = argv[++i];
        } else if (arg == "-d" && i + 1 < argc) {
            dbFile = argv[++i];
        } else if (arg == "-k" && i + 1 < argc) {
            k = stoi(argv[++i]);
        } else if (arg == "-a" && i + 1 < argc) {
            a = stod(argv[++i]);
        } else if (arg == "-t" && i + 1 < argc) {
            topCount = stoi(argv[++i]);
        } else {
            cerr << "Argumento desconhecido: " << arg << endl;
            return 1;
        }
    }

    if (sampleFile.empty() || dbFile.empty()) {
        cerr << "Uso: MetaClass -s sample.txt -d db.txt -k <ordem> -a <alpha> -t <top>" << endl;
        return 1;
    }

    // Lê o arquivo de amostra (meta.txt)
    ifstream sFile(sampleFile);
    if (!sFile) {
        cerr << "Erro ao abrir o arquivo de amostra: " << sampleFile << endl;
        return 1;
    }
    stringstream ss;
    ss << sFile.rdbuf();
    string sample = ss.str();
    sFile.close();

    // Remove eventuais quebras de linha (caso a sequência esteja dividida)
    sample.erase(remove(sample.begin(), sample.end(), '\n'), sample.end());
    sample.erase(remove(sample.begin(), sample.end(), '\r'), sample.end());

    // Constrói o modelo de contexto finito a partir do sample.
    // Para cada substring de tamanho k (contexto) contabiliza as ocorrências de cada símbolo seguinte.
    unordered_map<string, array<int, 4>> model;
    unordered_map<string, int> contextCount;
    for (size_t i = 0; i + k < sample.size(); i++) {
        string context = sample.substr(i, k);
        char nextChar = sample[i + k];
        int idx = charToIndex(nextChar);
        if (idx < 0) continue; // ignora símbolos inválidos
        model[context][idx]++;
        contextCount[context]++;
    }

    // Lê o arquivo do banco de dados (db.txt)
    // O arquivo possui linhas iniciadas com '@' para identificar uma nova sequência.
    ifstream dbStream(dbFile);
    if (!dbStream) {
        cerr << "Erro ao abrir o arquivo do banco de dados: " << dbFile << endl;
        return 1;
    }
    vector<Sequence> sequences;
    Sequence current;
    string line;
    while(getline(dbStream, line)) {
        if (line.empty()) continue;
        if (line[0] == '@') {
            if (!current.id.empty()) {
                sequences.push_back(current);
            }
            current.id = line.substr(1); // remove o '@'
            current.seq = "";
        } else {
            // Remove quebras de linha e espaços e concatena a sequência
            line.erase(remove(line.begin(), line.end(), '\n'), line.end());
            current.seq += line;
        }
    }
    if (!current.id.empty()) {
        sequences.push_back(current);
    }
    dbStream.close();

    // Para cada sequência do banco de dados, calcula o custo em bits usando o modelo treinado.
    // Para os primeiros k símbolos, assume-se uma codificação uniforme (2 bits/símbolo).
    vector<pair<string, double>> results; // par: (identificador, valor NRC)
    for (const auto &seqObj : sequences) {
        const string &seq = seqObj.seq;
        int n = seq.size();
        if(n == 0) continue; // ignora sequências vazias
        double cost = 0.0;
        // Custo para os primeiros k símbolos (ou todos, se n < k)
        int initial = min(n, k);
        cost += initial * 2.0; // log2(4)=2 bits por símbolo

        // Para cada símbolo a partir da posição k, calcula o custo baseado no contexto
        for (size_t i = k; i < seq.size(); i++) {
            string context = seq.substr(i - k, k);
            char c = seq[i];
            int idx = charToIndex(c);
            if (idx < 0) {
                // Se o símbolo não for A, C, G ou T, assume o pior caso (2 bits)
                cost += 2.0;
                continue;
            }
            int countContext = 0;
            if (contextCount.find(context) != contextCount.end())
                countContext = contextCount[context];
            int countSymbol = 0;
            if (model.find(context) != model.end())
                countSymbol = model[context][idx];
            // Probabilidade com suavização: (contagem + a) / (total + 4*a)
            double prob = (countSymbol + a) / (countContext + 4 * a);
            cost += -log2(prob);
        }
        // Calcula o NRC: custo total normalizado pelo custo de codificar uniformemente (2 bits por símbolo)
        double nrc = cost / (2 * n);
        results.push_back({seqObj.id, nrc});
    }

    // Ordena os resultados pelo valor NRC (menor valor indica maior similaridade)
    sort(results.begin(), results.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
        return a.second < b.second;
    });

    // Exibe os top resultados
    cout << "Top " << topCount << " sequências por NRC:" << endl;
    for (int i = 0; i < topCount && i < (int)results.size(); i++) {
        cout << results[i].first << " : " << results[i].second << endl;
    }

    return 0;
}