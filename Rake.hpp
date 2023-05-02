#ifndef PROJECT_RAKE_HPP
#define PROJECT_RAKE_HPP

#ifndef IOSTREAM
#define IOSTREAM
#include <iostream>
#endif

#ifndef FSTREAM
#define FSTREAM
#include <fstream>
#endif

#ifndef STRING
#define STRING
#include <string>
#endif

#ifndef VECTOR
#define VECTOR
#include <vector>
#endif

#ifndef UNORDERED_SET
#define UNORDERED_SET
#include <unordered_set>
#endif

#ifndef MAP
#define MAP
#include <map>
#endif

#ifndef SET
#define SET
#include <set>
#endif

#ifndef FUNCTIONAL
#define FUNCTIONAL
#include <functional>
#endif


// Class that handles word scores
class Rake_WordScore {
    using type = unsigned int;
    type freq = 0;
    type deg = 0;
public:
    void incr_deg(size_t x) { deg += x; }

    void incr_same() {freq++; }

    [[nodiscard]] double score() const{
        return static_cast<double>(deg) / freq;
    }

    bool operator<(const Rake_WordScore& other) const{
        return this->score() < other.score();
    }
};


class RAKE {
    using strVec = std::vector<std::string>;
    using phraseVector = std::vector< strVec >;

    std::map<std::string, Rake_WordScore> word_scores_;
    std::vector< std::pair<strVec, double> > phrases_with_scores_;
    bool calculated = false;

public:
    // Constructor accepting phrases by l-value reference, and copying them
    // Original phrases left untouched
    explicit RAKE(phraseVector& phrases);

    // Constructor accepting phrases by r-value reference, moving them
    // Original phrases moved
    explicit RAKE(phraseVector&& phrases);

    // Returns a percentages, provided by the user, of all the phrases
    phraseVector get_key_phrases(double percent = static_cast<double>(1) / 3);

    // Returns the top len_i phrases
    phraseVector get_key_phrases(int len_i);

private:
    phraseVector get_key_phrases_priv(size_t num);

    void set_scores();

    // my comparator function for std::pair< phrase, score >
    // First compares by scores in decreasing order
    // If scores are the same, compares strVec lexicographically
    static bool custom_comp(const std::pair<strVec, double>&  a, const std::pair<strVec, double>& b);

    // remove duplicates from phrases_with_scores
    void rem_duplicates_sort();
};

#endif //PROJECT_RAKE_HPP
