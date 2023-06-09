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

#include "TextRank.hpp"

using strVec = std::vector<std::string>;
using phraseVector = std::vector< strVec >;


// Returns summary of length calculated by percentage of the overall length of the text
strVec TextRank::get_summary(double percent) {
    if (percent < 0 || percent > 1) {
        throw std::runtime_error("Error: Percentage of sentences included in the summary should be between 0 and 1!");
    }
    size_t len = graph_.size() * percent;
    return get_summary_priv(len);
}

// Returns summary with specified number of sentences
// Get the parameter as int in case the user inputs a negative number
// Converts it to size_t after necessary checks
strVec TextRank::get_summary(int len_i) {
    if (len_i < 0) {
        throw std::runtime_error("Error: Length of summary cannot be negative!");
    }
    size_t len = static_cast<size_t>(len_i);
    if (len > sentences_.size()) {
        std::cerr << "Warning: Length of summary requested is longer than the text." << std::endl;
    }
    len = std::min(len, sentences_.size());
    return get_summary_priv(len);
}

// Equality for doubles, epsilon defines precision required
bool TextRank::doublesEqual(double a, double b, double epsilon) {
    return std::abs(a - b) < epsilon;
}

// Calculates the similarity between 2 sentences
double TextRank::similarity(TextRank::strVec &sent_1, TextRank::strVec &sent_2) {
    double top = 0;
    for (auto& word : sent_1) {
        if (std::find(sent_2.begin(), sent_2.end(), word) != sent_2.end()) {
            top += 1;
        }
    }
    double bottom = log(sent_1.size()) + log(sent_2.size());
    return top / bottom;
}

// Calculates the normalization constants for each node
// Normalization Constant = Sum of the weights of all outgoing edges
void TextRank::set_norm_constants() {
    for (TextRank_Node& node: graph_) {
        double sum = 0;
        for (auto& edge_pair : node.edges) {
            sum += edge_pair.second;
        }
        node.norm_constant = sum;
    }
}

void TextRank::construct_graph() {
    size_t size = tokenized_sentences_.size();
    graph_.reserve(size);
    double init_score = static_cast<double>(1) / static_cast<double>(size);
    for (size_t i = 0; i < size; i++) {
        graph_.emplace_back();
        graph_[i].sent_index = i;
        graph_[i].norm_constant = 1;    // default normalization constant
        graph_[i].score = init_score;
    }

    for (size_t i = 0; i < size; i++) {
        for (size_t j = i + 1; j < size; j++) {
            double sim_i_j = similarity(tokenized_sentences_[i], tokenized_sentences_[j]);
            if (!doublesEqual(sim_i_j, 0)) {
                graph_[i].edges.emplace_back(j, sim_i_j);
                graph_[j].edges.emplace_back(i, sim_i_j);
            }
        }
    }
    set_norm_constants();
}

// A single iteration of the algorithm that incrementally updates node scores
// Returns the total change in scores during this iteration
double TextRank::iteration(double d) {
    double change = 0;
    for (TextRank_Node& node : graph_) {
        double old_score = node.score;
        double new_score = 0;
        for (const auto& edge_pair : node.edges) {
            size_t edge_from = edge_pair.first;
            double w = edge_pair.second / graph_[edge_from].norm_constant;
            new_score += w * graph_[edge_from].score;
        }
        new_score *= d;
        new_score += 1 - d;
        node.score = new_score;
        change += std::abs(new_score - old_score);
    }
    return change;
}

// Comparison function for Nodes to be used by std::set and std::sort
// First compares scores, then indices
bool TextRank::custom_comp(const TextRank_Node &a, const TextRank_Node &b) {
    if (a.score != b.score) {
        return a.score > b.score;
    }
    return a.sent_index < b.sent_index;
}

// Iterates until scores reach equilibrium
void TextRank::iterate() {
    double change = std::numeric_limits<double>::max();
    while (change > 0.001) {
        change = iteration(0.85);
    }
}

// Returns summary of specified number of sentences
strVec TextRank::get_summary_priv(size_t len) {
    if (!calculated){
        iterate();
        std::sort(graph_.begin(), graph_.end(), custom_comp);   // sort graph_ by scores
        calculated = true;
    }

    std::vector<size_t> summary_sents_i;    // sentence indices
    summary_sents_i.reserve(len);
    for (size_t i = 0; i < len; i++) {      // store indices of sentences that should be in the summary
        summary_sents_i.push_back(graph_[i].sent_index);
    }
    // sort the indices so that sentences in the summary appear in order
    std::sort(summary_sents_i.begin(), summary_sents_i.end());

    strVec summary;
    summary.reserve(len);
    for (auto i: summary_sents_i) {
        summary.push_back(sentences_[i]);
    }
    return summary;
}

