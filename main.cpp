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

#ifndef FILESYSTEM
#define FILESYSTEM
#include <filesystem>
#endif

#ifndef UNORDERED_SET
#define UNORDERED_SET
#include <unordered_set>
#endif

#ifndef VARIANT
#define VARIANT
#include <variant>
#endif

#include <boost/program_options.hpp>


#include "FileProcess.hpp"
#include "TextPreprocess.hpp"
#include "Rake.hpp"
#include "TextRank.hpp"


constexpr char PATH_SEP = std::filesystem::path::preferred_separator;
const std::string STOP_CHARS_PATH = "resources" + std::string(1, PATH_SEP) + "stopchars.txt";
const std::string STOP_WORDS_PATH = "resources" + std::string(1, PATH_SEP) + "stopwords.txt";

const std::unordered_set<char> sent_end_chars = {'.', '!', '?', ';'};

enum Length_Mode {DEFAULT, LENGTH, PERCENT};

void validate_program_options(boost::program_options::variables_map& vm) {
    if (!vm.count("rake") && !vm.count("text-rank")) {
        std::cerr << "Error: no action provided, provide either <rake> or <text-rank>!" << std::endl;
        exit(2);
    }

    // rake and text-rank are mutually exclusive
    if (vm.count("rake") && vm.count("text-rank")) {
        std::cerr << "Error: options <rake> and <text-rank> are mutually exclusive, choose one!" << std::endl;
        exit(2);
    }

    // length and percent are mutually exclusive
    if (vm.count("length") && vm.count("percent")) {
        std::cerr << "Error: options <length> and <percent> are mutually exclusive, choose one!" << std::endl;
        exit(2);
    }
}

std::vector< std::vector<std::string> > perform_rake(std::vector< std::vector<std::string> >&& phrases,
                                                     Length_Mode length_mode,
                                                     std::variant<std::monostate, double, int> length_val) {
    RAKE rk(std::move(phrases));
    std::vector< std::vector<std::string> > key_phrases;
    if (length_mode == LENGTH) {
        key_phrases = rk.get_key_phrases(std::get<int>(length_val));
    }
    else if (length_mode == PERCENT) {
        key_phrases = rk.get_key_phrases(std::get<double>(length_val));
    }
    else {
        key_phrases = rk.get_key_phrases();
    }
    return key_phrases;
}

std::vector<std::string> perform_textrank(std::vector<std::string>&& sentences,
                                                         std::vector<std::vector<std::string>>&& processed_sentences,
                                                         Length_Mode length_mode,
                                                         std::variant<std::monostate, double, int> length_val) {
    TextRank tk(std::move(sentences), std::move(processed_sentences));
    std::vector< std::string> summary;
    if (length_mode == LENGTH) {
        summary = tk.get_summary(std::get<int>(length_val));
    }
    else if (length_mode == PERCENT) {
        summary = tk.get_summary(std::get<double>(length_val));
    }
    else {
        summary = tk.get_summary();
    }
    return summary;
}


int main(int argc, char* argv[]) {
    std::string input_file, output_file;
    Length_Mode length_mode;
    std::variant<std::monostate, double, int> length_val;

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("input_file", boost::program_options::value<std::string>(&input_file), "set input file")
            ("output_file", boost::program_options::value<std::string>(&output_file), "set output file")
            ("rake", "produce key phrases using RAKE")
            ("text-rank", "produce a summary using TextRank")
            ("length", boost::program_options::value<int>(), "number of lexical units included in the summary")
            ("percent", boost::program_options::value<double>(), "length of the summary as a percentage of the length of the original text");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    validate_program_options(vm);

    // store length or percent into length_val variant
    // leave as std::monostate if both length and percent were not provided
    if (vm.count("length")) {
        length_val = vm["length"].as<int>();
    }
    else if (vm.count("percent")) {
        length_val = vm["percent"].as<double>();
    }

    // Figure out how the length of the summary will be determined
    if (vm.count("length")) {
        length_mode = LENGTH;
    }
    else if (vm.count("percent")) {
        length_mode = PERCENT;
    }
    else {
        length_mode = DEFAULT;
    }


    // Establishing input stream
    // cin or file
    std::istream* input_stream_ptr;
    std::ifstream input_file_stream;
    if (input_file.empty()) {
        input_stream_ptr = &std::cin;
    }
    else {
        input_file_stream = FileProcess::open_file<std::ifstream>(input_file, std::ios_base::in);
        input_stream_ptr = &input_file_stream;
    }
    std::istream& input_stream = *input_stream_ptr; // input stream to be used

    // Establishing output stream
    // cout or file
    std::ostream* output_stream_ptr;
    std::ofstream output_file_stream;
    if (output_file.empty()) {
        output_stream_ptr = &std::cout;
    }
    else {
        output_file_stream = FileProcess::open_file<std::ofstream>(output_file, std::ios_base::out);
        output_stream_ptr = &output_file_stream;
    }
    std::ostream& output_stream = *output_stream_ptr;   // output stream to be used

    // Load stop_chars and stop_words
    auto stop_chars = TextProcess::load_stop_chars(STOP_CHARS_PATH);
    auto stop_words = TextProcess::load_stop_words(STOP_WORDS_PATH);

    if (vm.count("rake")) {
        auto phrases = TextProcess::parse_text_phrases(input_stream, stop_chars, stop_words);
        TextProcess::output_to_stream(output_stream, perform_rake(std::move(phrases), length_mode, length_val));
    }

    else if (vm.count("text-rank")) {
        auto sentences = TextProcess::parse_text_sentences(input_stream, sent_end_chars);
        auto processed_sentences = TextProcess::process_sentences(sentences, stop_chars, stop_words);
        TextProcess::output_to_stream(output_stream, perform_textrank(std::move(sentences), std::move(processed_sentences),
                                                                      length_mode, length_val));
    }

    // Close all files if open
    FileProcess::close_files(input_file_stream, output_file_stream);

    return 0;
}



