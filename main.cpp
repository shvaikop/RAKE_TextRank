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



//#include <boost/program_options.hpp>


#include "FileProcess.hpp"
#include "TextPreprocess.hpp"
#include "Rake.hpp"
#include "TextRank.hpp"


constexpr char PATH_SEP = std::filesystem::path::preferred_separator;
const std::string STOP_CHARS_PATH = "resources" + std::string(1, PATH_SEP) + "stopchars.txt";
const std::string STOP_WORDS_PATH = "resources" + std::string(1, PATH_SEP) + "stopwords.txt";

const std::unordered_set<char> sent_end_chars = {'.', '!', '?', ';'};




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

    auto sentences = TextProcess::parse_text_sentences(file_in, sent_end_chars);
    auto processed_sentences = TextProcess::process_sentences(sentences, stop_chars, stop_words);
    TextRank tk(std::move(sentences), processed_sentences);
    auto summary = tk.get_summary(0.55);

//    RAKE rk(std::move(phrases));
//    auto key_phrases = rk.get_key_phrases(1);
//    TextProcess::custom_print(key_phrases);

    // Close all files if open
    FileProcess::close_files(file_in, file_out);
    return 0;
}



