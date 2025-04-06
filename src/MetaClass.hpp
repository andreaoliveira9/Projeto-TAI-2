#ifndef METACLASS_HPP
#define METACLASS_HPP

#include <string>
#include <vector>

using namespace std;

class MetaClass {
public:
    int k;                     
    vector<int> counts;    

    MetaClass();
    
    bool loadModel(const string &filename);
    
    double compressSequence(const string &seq, double a, int alphabetSize) const;
    
    double computeNRC(const string &seq, double a) const;

    void setCounts(const vector<int> &counts);

    void setK(int k);
    
private:
    int charToIndex(char c) const;
    unsigned long power4(int k) const;
};

#endif