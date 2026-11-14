#include <iostream>
#include <cmath>
#include <map>
#include <set>
#include <string>
using namespace std;

class Classifier {
private:
    double num_posts;
    double num_words;
    map<string, double> contains_w;
    map<string, double> num_posts_c;
    map<pair<string, string>, double> contains_cw;

    double log_prior(const string &label) const {
        double pc = log(num_posts_c.at(label)/num_posts);
        return pc;
    }

    double log_likelihood(const string &label, const string &word) const {
        double pwc = 0;
        
        if(contains_cw.find({label, word}) != contains_cw.end()){
            pwc = log(contains_cw.at({label, word})/num_posts_c.at(label));
        } else if(contains_w.find(word) == contains_w.end()){
                pwc = log(1/num_posts);
        } else {
            pwc = log(contains_w.at(word)/num_posts);
        
        }

        return pwc;
    }

public:

};


int main() {
  
}
