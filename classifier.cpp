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

    // for 1 post
    double lps_score(const string &label, const set<string> &words) const{
        double lps = log_prior(label);

        for(const string & word : words){
            lps += log_likelihood(label, word);
        }

        return lps;
    }


public:
    Classifier()
    : num_posts(0), num_words(0) {};

    // getters
    double get_num_words(){
        return num_words;
    }

    double get_num_posts() const{
        return num_posts;
    }

    // const map<string, double> get_contains_w() const{
    //     return const &contains_w;
    // }

    // map<string, double> get_num_posts_c(){
    //     return num_posts_c;
    // }

    // map<pair<string, string>, double> get_contains_cw(){
    //     return contains_cw;
    // }

    // helper functions

    // for 1 post
    void train(const string &label, const set<string> &words) {
        num_posts ++;
        num_posts_c[label] ++;
        
        for(const string &word : words){
            contains_w[word] ++;
            contains_cw[{label, word}] ++;
        }
    }

    string predict_label(const set<string> &words) const {
        string best_label = num_posts_c.begin() -> first;
        double best_score = lps_score(best_label, words);
        double score = 0;
        for(const auto &entry : num_posts_c){
            score = lps_score(entry.first, words);
            if(score > best_score){
                best_score = score;
                best_label = entry.first;
            }
        }
        return best_label;
    }
};


int main() {
  
}
