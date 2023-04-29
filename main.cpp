#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <utility>
#include <cmath>

#include "FileProcess.hpp"
#include "TextPreprocess.hpp"
#include "Rake.hpp"


constexpr char PATH_SEP = std::filesystem::path::preferred_separator;
const std::string STOP_CHARS_PATH = "resources" + std::string(1, PATH_SEP) + "stopchars.txt";
const std::string STOP_WORDS_PATH = "resources" + std::string(1, PATH_SEP) + "stopwords.txt";

const std::unordered_set<char> sent_end_chars = {'.', '!', '?', ';'};

struct TextRank_Node {
    double score;
    size_t sent_index;
    std::vector< std::pair<size_t, double> > edges;
};

class TextRank {
    using strVec = std::vector<std::string>;
    using phraseVector = std::vector< strVec >;

    std::vector<TextRank_Node> graph_;
    phraseVector tokenized_sentences_;

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

    void normalize_graph_edges() {
        for (auto& node: graph_) {
            double sum = 0;
            for (auto& edge_pair : node.edges) {
                sum += edge_pair.second;
            }
            for (auto& edge_pair : node.edges) {
                edge_pair.second = edge_pair.second / sum;
            }
        }
    }

    void construct_graph() {
        size_t size = tokenized_sentences_.size();
        graph_.reserve(size);
        double init_score = (double)1 / (double)size;
        for (size_t i = 0; i < size; i++) {
            graph_.emplace_back();
            graph_[i].sent_index = i;
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
        normalize_graph_edges();
    }

public:
    TextRank(phraseVector& tokenized_sentences) {
        tokenized_sentences_ = std::move(tokenized_sentences);
        construct_graph();
    }

};


int main(int argc, char* argv[]) {
    std::ifstream file_in;
    std::ofstream file_out;

    // command line args handling
    try {
        switch (argc) {
            case 1: {
                // read from and output to command line
                std::cout << "reading, writing to from cin" << std::endl;
                break;
            }
            case 2: {
                // read from file, output to command line
                std::string file_in_name = argv[1];
                file_in = FileProcess::open_file<std::ifstream>(file_in_name, std::ios_base::in);
                std::cout << file_in.get() << std::endl;
                break;
            }
            case 3: {
                // read from and output to files
                std::string file_in_name = argv[1];
                std::string file_out_name = argv[2];
                file_in = FileProcess::open_file<std::ifstream>(file_in_name, std::ios_base::in);
                file_out = FileProcess::open_file<std::ofstream>(file_out_name, std::ios_base::out);
                file_out << file_in.get() << std::endl;
                break;
            }
            default:
                std::cerr << "Usage : ./a.out [file_in] [file_out]" << std::endl;

        }
    }
    catch (std::ios_base::failure& e) {
        std::cerr << e.what() << std::endl;
        FileProcess::close_files(file_in, file_out);
        exit(1);
    }




    auto stop_chars = TextProcess::load_stop_chars(STOP_CHARS_PATH);
    auto stop_words = TextProcess::load_stop_words(STOP_WORDS_PATH);
//    auto phrases = TextProcess::read_text_phrases(file_in, stop_chars, stop_words);

    auto sentences = TextProcess::split_into_sentences(file_in, sent_end_chars);
    auto processed_sentences = TextProcess::process_sentences(sentences, stop_chars, stop_words);
    TextRank tk(processed_sentences);

//    RAKE rk(std::move(phrases));
//    auto key_phrases = rk.get_key_phrases(1);
//    TextProcess::custom_print(key_phrases);

    // Close all files if open
    FileProcess::close_files(file_in, file_out);
    return 0;
}



