#ifndef PROJECT_TEXTRANK_HPP
#define PROJECT_TEXTRANK_HPP

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

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

public:
    TextRank(strVec& sentences, phraseVector& tokenized_sentences) {
        tokenized_sentences_ = std::move(tokenized_sentences);
        sentences_ = std::move(sentences);
        construct_graph();
    }

    strVec get_summary(double percent = (double)1 / 3) {
        // TODO throw exception in percent is not in [0,1]
        size_t len = graph_.size() * percent;
        return get_summary_priv(len);
    }

    strVec get_summary(size_t len) {
        // TODO issue a warning if len is greater than the number of sentences
        len = std::min(len, sentences_.size());
        return get_summary_priv(len);
    }

private:

    static bool doublesEqual(double a, double b, double epsilon) {
        return std::abs(a - b) < epsilon;
    }

    static double similarity(strVec& sent_1, strVec& sent_2) {
        double top = 0;
        for (auto& word : sent_1) {
            if (std::find(sent_2.begin(), sent_2.end(), word) != sent_2.end()) {
                top += 1;
            }
        }
        double bottom = log(sent_1.size()) + log(sent_2.size());
        return top / bottom;
    }

    void set_norm_constants() {
        for (TextRank_Node& node: graph_) {
            double sum = 0;
            for (auto& edge_pair : node.edges) {
                sum += edge_pair.second;
            }
            node.norm_constant = sum;
        }
    }

    void construct_graph() {
        size_t size = tokenized_sentences_.size();
        graph_.reserve(size);
        double init_score = (double)1 / (double)size;
        for (size_t i = 0; i < size; i++) {
            graph_.emplace_back();
            graph_[i].sent_index = i;
            graph_[i].norm_constant = 1;
            graph_[i].score = init_score;
        }

        for (size_t i = 0; i < size; i++) {
            for (size_t j = i + 1; j < size; j++) {
                double sim_i_j = similarity(tokenized_sentences_[i], tokenized_sentences_[j]);
                if (!doublesEqual(sim_i_j, 0, 1e-9)) {
                    graph_[i].edges.emplace_back(j, sim_i_j);
                    graph_[j].edges.emplace_back(i, sim_i_j);
                }
            }
        }
        set_norm_constants();
    }

    double iteration(double d) {
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

    static bool custom_comp (const TextRank_Node& a, const TextRank_Node& b) {
        if (a.score != b.score) {
            return a.score > b.score;
        }
        return a.sent_index < b.sent_index;
    }

    void iterate() {
        double change = std::numeric_limits<double>::max();
        while (change > 0.001) {
            change = iteration(0.85);
        }
    }

    strVec get_summary_priv(size_t len) {
        iterate();
        std::sort(graph_.begin(), graph_.end(), custom_comp);   // sort graph_ by scores
        std::vector<size_t> summary_sents_i;
        summary_sents_i.reserve(len);
        for (size_t i = 0; i < len; i++) {
            summary_sents_i.push_back(graph_[i].sent_index);
        }
        std::sort(summary_sents_i.begin(), summary_sents_i.end());

        strVec summary;
        summary.reserve(len);
        for (auto i: summary_sents_i) {
            summary.push_back(sentences_[i]);
        }
        return summary;
    }
};

#endif //PROJECT_TEXTRANK_HPP
