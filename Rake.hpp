#ifndef PROJECT_RAKE_HPP
#define PROJECT_RAKE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <set>
#include <functional>

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
    explicit RAKE(phraseVector& phrases) {
        for (auto& phrase : phrases) {
            phrases_with_scores_.emplace_back(phrase, 0);
        }
    }

    // Constructor accepting phrases by r-value reference, moving them
    // Original phrases moved
    explicit RAKE(phraseVector&& phrases) {
        for (auto&& phrase : phrases) {
            phrases_with_scores_.emplace_back(std::move(phrase), 0);
        }
    }

    // Returns a percentages, provided by the user, of all the phrases
    phraseVector get_key_phrases(double percent = static_cast<double>(1) / 3) {
        if (percent < 0 || percent > 1) {
            throw std::runtime_error("Error: Percentage of phrases included in the summary should be between 0 and 1!");
        }
        size_t num = static_cast<size_t>(static_cast<double>(phrases_with_scores_.size()) * percent);
        return get_key_phrases_priv(num);
    }

    // Returns the top len_i phrases
    phraseVector get_key_phrases(int len_i) {
        if (len_i < 0) {
            throw std::runtime_error("Error: Length of summary cannot be negative!");
        }
        size_t len = static_cast<size_t>(len_i);
        if (len > phrases_with_scores_.size()) {
            std::cerr << "Warning: Number of phrases requested in the summary is greater than the total number of phrases" << std::endl;
        }
        len = std::min(len, phrases_with_scores_.size());
        return get_key_phrases_priv(len);
    }

private:
    phraseVector get_key_phrases_priv(size_t num) {
        if (!calculated) {
            set_scores();
            rem_duplicates_sort();
            calculated = true;
        }

        phraseVector result;
        for (size_t i = 0; i < num; i++){
            result.push_back(phrases_with_scores_[i].first);
        }
        return result;
    }

    void set_scores() {
        // score each word
        for (const auto& pair_phrase_score : phrases_with_scores_) {
            size_t phrase_len = pair_phrase_score.first.size();
            for (const auto& word : pair_phrase_score.first) {
                word_scores_[word].incr_same();
                word_scores_[word].incr_deg(phrase_len);
            }
        }
        // sum up scores for each sentence
        for (auto& pair_phrase_score : phrases_with_scores_) {
            double phrase_score = 0;
            for (auto& word : pair_phrase_score.first) {
                phrase_score += word_scores_[word].score();
            }
            pair_phrase_score.second = phrase_score;
        }
    }

    // my comparator function for std::pair< phrase, score >
    // First compares by scores in decreasing order
    // If scores are the same, compares strVec lexicographically
    static bool custom_comp(const std::pair<strVec, double>&  a, const std::pair<strVec, double>& b) {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return a.first < b.first;
    }

    // remove duplicates from phrases_with_scores
    void rem_duplicates_sort() {
        // set to keep elements
        std::set< std::pair<strVec, double>,
        std::function<bool(std::pair<strVec, double>, std::pair<strVec, double>)> > phr_score_set(custom_comp);
        // move elements from vector to set
        std::move(phrases_with_scores_.begin(),
                  phrases_with_scores_.end(),
                  std::inserter(phr_score_set, phr_score_set.begin()));

        phrases_with_scores_.clear();
        phrases_with_scores_.reserve(phr_score_set.size());
        // move elements back from set to vector
        std::move(phr_score_set.begin(),
                  phr_score_set.end(),
                  std::back_inserter(phrases_with_scores_));
    }
};

#endif //PROJECT_RAKE_HPP
