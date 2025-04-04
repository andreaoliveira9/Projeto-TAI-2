#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include "MetaClass.hpp"
#include <cctype>

using namespace std;

// Estrutura para armazenar os resultados (identificador e NRC) de cada sequência
struct SequenceResult {
    string id;
    string seq;
    double nrc;
};

// Função para remover espaços e quebras de linha do fim da string
void trim(string &s) {
    while(!s.empty() && isspace(s.back()))
        s.pop_back();
}

int main(int argc, char* argv[]){
    if(argc < 9) {
        cerr << "Uso: " << argv[0] << " -i <db_file> -m <model> -a <alpha> -t <top>" << endl;
        return 1;
    }
    
    string db_filename;
    string model_filename;
    double a;
    int top;
    
    // Processa os argumentos da linha de comando
    for(int i = 1; i < argc; i++){
        string arg = argv[i];
        if(arg == "-db" && i+1 < argc) {
            db_filename = argv[++i];
        } else if(arg == "-m" && i+1 < argc) {
            model_filename = argv[++i];
        } else if(arg == "-a" && i+1 < argc) {
            a = atof(argv[++i]);
        } else if(arg == "-t" && i+1 < argc) {
            top = atoi(argv[++i]);
        } else {
            cerr << "Argumento inválido: " << arg << endl;
            return 1;
        }
    }
    
    // Carrega o modelo usando a classe MetaClass
    MetaClass model;
    if(!model.loadModel(model_filename)){
        cerr << "Erro a carregar o modelo" << endl;
        return 1;
    }
    
    // Abre o ficheiro da base de dados (db.txt) e processa cada sequência
    ifstream dbFile(db_filename);
    if(!dbFile) {
        cerr << "Erro ao abrir o ficheiro da base de dados: " << db_filename << endl;
        return 1;
    }
    
    vector<SequenceResult> results;
    string line;
    string current_id;
    string current_seq;
    
    while(getline(dbFile, line)) {
        trim(line);
        if(line.empty()) continue;
        if(line[0] == '@'){
            if(!current_id.empty() && !current_seq.empty()){
                SequenceResult res;
                res.id = current_id;
                res.seq = current_seq;
                res.nrc = model.computeNRC(current_seq, a);
                results.push_back(res);
            }
            current_id = line.substr(1); // remove o '@'
            current_seq.clear();
        } else {
            current_seq += line;
        }
    }
    // Guarda a última sequência
    if(!current_id.empty() && !current_seq.empty()){
        SequenceResult res;
        res.id = current_id;
        res.seq = current_seq;
        res.nrc = model.computeNRC(current_seq, a);
        results.push_back(res);
    }
    dbFile.close();
    
    // Ordena os resultados por NRC (ordem crescente: menor NRC indica maior similaridade)
    sort(results.begin(), results.end(), [](const SequenceResult &a, const SequenceResult &b){
        return a.nrc < b.nrc;
    });
    
    cout << "Top " << top << " sequências por NRC (menor é melhor):" << endl;
    int printed = min(static_cast<int>(results.size()), top);
    for (int i = 0; i < printed; i++){
        cout << i+1 << ". " << results[i].id << " - NRC: " << results[i].nrc << endl;
    }
    
    return 0;
}