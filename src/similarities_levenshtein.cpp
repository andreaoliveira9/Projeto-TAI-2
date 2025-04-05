#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cctype>
#include <algorithm>

using namespace std;

struct Sequence {
  string id;
  string seq;
};

void trim(string &s) {
  while (!s.empty() && isspace(s.back()))
    s.pop_back();
}

int levenshteinDistance(const string &s1, const string &s2) {
  const size_t len1 = s1.size(), len2 = s2.size();
  if (len1 == 0) return len2;
  if (len2 == 0) return len1;

  vector<int> prev(len2 + 1), curr(len2 + 1);

  for (size_t j = 0; j <= len2; ++j)
    prev[j] = j;

  for (size_t i = 1; i <= len1; ++i) {
    curr[0] = i;
    for (size_t j = 1; j <= len2; ++j) {
      int cost = (toupper(s1[i - 1]) == toupper(s2[j - 1])) ? 0 : 1;
      curr[j] = min({
        prev[j] + 1,
        curr[j - 1] + 1,
        prev[j - 1] + cost
      });
    }
    swap(prev, curr);
  }

  return prev[len2];
}

int main(int argc, char *argv[]) {
  string dbFile, id1, id2;

  for (int i = 1; i < argc; ++i) {
    string arg = argv[i];
    if (arg == "-db" && i + 1 < argc) {
      dbFile = argv[++i];
    } else if (arg == "-id1" && i + 1 < argc) {
      id1 = argv[++i];
    } else if (arg == "-id2" && i + 1 < argc) {
      id2 = argv[++i];
    }
  }

  if (dbFile.empty() || id1.empty() || id2.empty()) {
    cerr << "Uso: " << argv[0] << " --db <ficheiro> --id1 <ID1> --id2 <ID2>" << endl;
    return 1;
  }

  ifstream infile(dbFile);
  if (!infile) {
    cerr << "Erro ao abrir o ficheiro de dados: " << dbFile << endl;
    return 1;
  }

  vector<Sequence> sequences;
  string line;
  string current_id;
  string current_seq;

  while (getline(infile, line)) {
    trim(line);
    if (line.empty()) continue;
    if (line[0] == '@') {
      if (!current_id.empty() && !current_seq.empty()) {
        sequences.push_back({current_id, current_seq});
      }
      current_id = line.substr(1);
      current_seq.clear();
    } else {
      current_seq += line;
    }
  }

  if (!current_id.empty() && !current_seq.empty()) {
    sequences.push_back({current_id, current_seq});
  }

  infile.close();

  int n = sequences.size();
  if (n == 0) {
    cerr << "Nenhuma sequência encontrada no ficheiro." << endl;
    return 1;
  }

  Sequence *seq1 = nullptr, *seq2 = nullptr;

  for (auto &seq : sequences) {
    if (seq.id == id1) seq1 = &seq;
    if (seq.id == id2) seq2 = &seq;
  }

  if (!seq1 || !seq2) {
    cerr << "Erro: Um ou ambos os IDs não foram encontrados." << endl;
    return 1;
  }

  int dist = levenshteinDistance(seq1->seq, seq2->seq);
  double similarity = 1.0 - (double)dist / max(seq1->seq.size(), seq2->seq.size());
  cout << "Similaridade: " << similarity << endl;

  return 0;
}