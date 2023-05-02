#ifndef PROJECT_TEXTRANK_HPP
#define PROJECT_TEXTRANK_HPP

#ifndef IOSTREAM
#define IOSTREAM
#include <iostream>
#endif

#ifndef STRING
#define STRING
#include <string>
#endif

#ifndef VECTOR
#define VECTOR
#include <vector>
#endif

#ifndef CMATH
#define CMATH
#include <cmath>
#endif

#ifndef ALGORITHM
#define ALGORITHM
#include <algorithm>
#endif



struct TextRank_Node {
    double score;
    double norm_constant;
    size_t sent_index;
    std::vector< std::pair<size_t, double> > edges;
};

class TextRank {
    using strVec = std::vector<std::string>;
    using phraseVector = std::vector< strVec >;

    std::vector<TextRank_Node> graph_;
    phraseVector tokenized_sentences_;
    strVec sentences_;
    bool calculated = false;

public:
    // Constructor that exploits forwarding references
    // Enables the user to provide either an r-value or an l-value for both parameters
    // Avoids multiples constructor overloads
    template <typename StrVec, typename PhraseVector>
    TextRank(StrVec&& sentences, PhraseVector&& tokenized_sentences) {
        tokenized_sentences_ = std::forward<PhraseVector>(tokenized_sentences);
        sentences_ = std::forward<StrVec>(sentences);
        construct_graph();
    }

    // Returns summary of length calculated by percentage of the overall length of the text
    strVec get_summary(double percent = static_cast<double>(1) / 3);

    // Returns summary with specified number of sentences
    // Get the parameter as int in case the user inputs a negative number
    // Converts it to size_t after necessary checks
    strVec get_summary(int len_i);

private:
    // Equality for doubles, epsilon defines precision required
    static bool doublesEqual(double a, double b, double epsilon = 1e-9);

    // Calculates the similarity between 2 sentences
    static double similarity(strVec& sent_1, strVec& sent_2);

    // Calculates the normalization constants for each node
    // Normalization Constant = Sum of the weights of all outgoing edges
    void set_norm_constants();

    void construct_graph();

    // A single iteration of the algorithm that incrementally updates node scores
    // Returns the total change in scores during this iteration
    double iteration(double d);

    // Comparison function for Nodes to be used by std::set and std::sort
    // First compares scores, then indices
    static bool custom_comp (const TextRank_Node& a, const TextRank_Node& b);

    // Iterates until scores reach equilibrium
    void iterate();

    // Returns summary of specified number of sentences
    strVec get_summary_priv(size_t len);
};

#endif //PROJECT_TEXTRANK_HPP
