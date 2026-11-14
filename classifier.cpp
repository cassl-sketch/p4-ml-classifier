#include <iostream>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include "csvstream.hpp"
using namespace std;

class Classifier {
private:
    int num_posts;
    map<string, int> contains_w;
    map<string, int> num_posts_c;
    map<pair<string, string>, int> contains_cw;

    double log_prior(const string &label) const {
        double pc = log(static_cast<double>(num_posts_c.at(label))/num_posts);
        return pc;
    }

    double log_likelihood(const string &label, const string &word) const {
        double pwc = 0;
        
        if(contains_cw.find({label, word}) != contains_cw.end()){
            pwc = log(static_cast<double>(contains_cw.at({label, word}))/
            num_posts_c.at(label));
        } else if(contains_w.find(word) == contains_w.end()){
            pwc = log(1.0/num_posts);
        } else {
            pwc = log(static_cast<double>(contains_w.at(word))/num_posts);
        }

        return pwc;
    }

public:
    Classifier()
    : num_posts(0) {}

    int get_num_words() const{
        return contains_w.size();
    }

    int get_num_posts() const{
        return num_posts;
    }

    // for 1 post
    void train(const string &label, const set<string> &words) {
        ++ num_posts;
        ++ num_posts_c[label];
        
        for(const string &word : words){
            ++ contains_w[word];
            ++ contains_cw[{label, word}];
        }
    }

    // for 1 post
    double lps_score(const string &label, const set<string> &words) const{
        double lps = log_prior(label);

        for(const string & word : words){
            lps += log_likelihood(label, word);
        }

        return lps;
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

    void print_classes() const {
        cout << "classes:" << endl;
        for(const auto &entry : num_posts_c){
            cout << "  " << entry.first << ", " << entry.second 
            << " examples, log-prior = " << log_prior(entry.first) << endl;
        }
    }

    void print_parameters() const {
        cout << "classifier parameters:" << endl;
        for(const auto &entry : contains_cw){
            cout << "  " << entry.first.first << ":" << entry.first.second
            << ", " << "count = " << entry.second << ", log-likelihood = "
            <<  log_likelihood(entry.first.first, entry.first.second) << endl;
        }
    }
};

// EFFECTS: Return a set of unique whitespace delimited words
set<string> unique_words(const string &str) {
  istringstream source(str);
  set<string> words;
  string word;
  while (source >> word) {
    words.insert(word);
  }
  return words;
}

bool file1_check(Classifier &classifier, const string &filename1, int mode){
    try {
        csvstream csvin(filename1);
        map<string, string> row;

        if(mode == 1){
            cout << "training data:" << endl;
        }

        while(csvin >> row){
            string tag = row["tag"];
            string content = row["content"];
            set<string> words = unique_words(content);

            classifier.train(tag, words);
            if(mode == 1){
                cout << "  label = " << tag << ", content = " << content << endl;
            }
        } return 0;
    } catch(const csvstream_exception &e) {
        cout << "Error opening file: " << filename1 << endl;
        return 1;
    }
}

void print_training(const Classifier & classifier){
    cout << "vocabulary size = " << classifier.get_num_words() << endl; 
    cout << endl;
    classifier.print_classes();
    classifier.print_parameters();
    cout << endl;
}

bool file2_check(const Classifier &classifier, const string &filename2, int mode){
    int correct = 0;
    int num_test_posts = 0;
    try {
        csvstream csvin(filename2);
        map<string, string> row;

        cout << endl;
        cout << "test data:" << endl;
        while(csvin >> row){
            string tag = row["tag"];
            string content = row["content"];
            set<string> words = unique_words(content);

            string prediction = classifier.predict_label(words);
            double score = classifier.lps_score(prediction, words);

            cout << "  correct = " << tag << ", predicted = " << prediction
            << ", log-probability score = " << score << endl;

            cout << "  content = " << content << endl;
            cout << endl;

            if(prediction == tag){
                ++ correct;
            }

            ++ num_test_posts;
        } 

        cout << "performance: " << correct << " / " << num_test_posts
        << " posts predicted correctly" << "\n";
        return 0;
    } catch(const csvstream_exception &e) {
        cout << "Error opening file: " << filename2 << endl;
        return 1;
    }
}

int main(int argc, char** argv) {
    cout.precision(3);
    Classifier classifier;
    int mode = 1; // train only
    string filename2 = "";
    // check for invalid command line input
    if(argc != 2 && argc != 3){
        cout << "Usage: classifier.exe TRAIN_FILE [TEST_FILE]" << endl;
        return 1;
    } 
    
    if(argc == 3){
        filename2 = argv[2];
        mode = 2; // test mode
    }
    
    string filename1 = argv[1];
    if (file1_check(classifier, filename1, mode) == 1){
        return 1;
    }

    cout << "trained on " << classifier.get_num_posts() << " examples" << endl;
    
    if(mode == 1){
        print_training(classifier);
    }

    if(mode == 2){
        if(file2_check(classifier, filename2, mode) == 1){
            return 1;
        }
    }
}
