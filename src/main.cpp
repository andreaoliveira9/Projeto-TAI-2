#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include "MetaClass.hpp"
#include <cctype>

// Estrutura para armazenar os resultados (identificador e NRC) de cada sequência
struct SequenceResult {
    std::string id;
    std::string seq;
    double nrc;
};

// Função para remover espaços e quebras de linha do fim da string
void trim(std::string &s) {
    while(!s.empty() && std::isspace(s.back()))
        s.pop_back();
}

int main(int argc, char* argv[]){
    if(argc < 9) {
        std::cerr << "Uso: " << argv[0] << " -d <db_file> -m <model> -a <alpha> -t <top>" << std::endl;
        return 1;
    }
    
    std::string db_filename;
    std::string model_filename;
    double a;
    int top;
    
    // Processa os argumentos da linha de comando
    for(int i = 1; i < argc; i++){
        std::string arg = argv[i];
        if(arg == "-d" && i+1 < argc) {
            db_filename = argv[++i];
        } else if(arg == "-m" && i+1 < argc) {
            model_filename = argv[++i];
        } else if(arg == "-a" && i+1 < argc) {
            a = std::atof(argv[++i]);
        } else if(arg == "-t" && i+1 < argc) {
            top = std::atoi(argv[++i]);
        }
    }
    
    // Carrega o modelo usando a classe MetaClass
    MetaClass model;
    if(!model.loadModel(model_filename)){
        std::cerr << "Erro a carregar o modelo" << std::endl;
        return 1;
    }
    
    // Abre o ficheiro da base de dados (db.txt) e processa cada sequência
    std::ifstream dbFile(db_filename);
    if(!dbFile) {
        std::cerr << "Erro ao abrir o ficheiro da base de dados: " << db_filename << std::endl;
        return 1;
    }
    
    std::vector<SequenceResult> results;
    std::string line;
    std::string current_id;
    std::string current_seq;
    
    while(std::getline(dbFile, line)) {
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
    std::sort(results.begin(), results.end(), [](const SequenceResult &a, const SequenceResult &b){
        return a.nrc < b.nrc;
    });
    
    std::cout << "Top " << top << " sequências por NRC (menor é melhor):" << std::endl;
    int printed = std::min(static_cast<int>(results.size()), top);
    for (int i = 0; i < printed; i++){
        std::cout << i+1 << ". " << results[i].id << " - NRC: " << results[i].nrc << std::endl;
    }
    
    return 0;
}