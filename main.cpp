#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <vector>
#include <map>
#include <set>
#include <filesystem>
#include <cctype>

constexpr char PATH_SEP = std::filesystem::path::preferred_separator;
const std::string STOP_CHARS_PATH = "resources" + std::string(1, PATH_SEP) + "stopchars.txt";
const std::string STOP_WORDS_PATH = "resources" + std::string(1, PATH_SEP) + "stopwords.txt";

template<typename T>
concept FileStream = std::same_as<T, std::ifstream> || std::same_as<T, std::ofstream>;

// Closes the files provided as arguments (arbitrary many)
template<FileStream... Files>
void close_files(Files&... files) {
    ( (files.is_open() ? files.close() : void()), ... );
}

// Open file, check if open, return file stream
template <FileStream Stream>
Stream open_file(const std::string& file_name, std::ios_base::openmode open_mode) {
    Stream file(file_name, open_mode);
    if (!file.is_open()) {
        throw std::ios_base::failure("Error opening file " + file_name);
    }
    return file;
}

class WordScore {
    using type = unsigned int;
    type freq = 0;
    type deg = 0;
public:
    void incr_dif() { deg++; }

    void incr_same() {freq++; deg++; }

    double score() {
        return static_cast<double>(deg) / freq;
    }
};

class RAKE {
    std::istream& in_stream_;
    std::ostream& out_stream_;
    std::map<std::string, WordScore> word_scores_;
    std::vector<std::vector<std::string>> phrases_;
    std::set<char> stop_chars;
    std::set<std::string> stop_words;

public:
    // constuctor that sets the in_stream and the out_stream
    explicit RAKE(std::istream& in_stream = std::cin, std::ostream& out_stream = std::cout) : in_stream_(in_stream), out_stream_(out_stream) {
    }

    void load_stop_chars() {
        std::ifstream stop_chars_file = open_file<std::ifstream>(STOP_CHARS_PATH, std::ios_base::in);
        char c;
        while (stop_chars_file.get(c)) {
            stop_chars.insert(c);
        }
    }

    void load_stop_words() {
        std::ifstream stop_words_file = open_file<std::ifstream>(STOP_WORDS_PATH, std::ios_base::in);
        std::string word;
        char c;
        while(stop_words_file.get(c)) {
            if (std::isspace(c)) {
                if (!word.empty()) {
                    stop_words.insert(std::move(word));
                    word.clear();
                }
                // if word is empty - do nothing
            }
            else {
                word += c;
            }
        }
    }

    // TEST function
    void print_stop_chars() {
        for (auto elem : stop_chars) {
            std::cout << elem << "  ";
        }
        out_stream_ << std::endl;
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
                file_in = open_file<std::ifstream>(file_in_name, std::ios_base::in);
                std::cout << file_in.get() << std::endl;
                break;
            }
            case 3: {
                // read from and output to files
                std::string file_in_name = argv[1];
                std::string file_out_name = argv[2];
                file_in = open_file<std::ifstream>(file_in_name, std::ios_base::in);
                file_out = open_file<std::ofstream>(file_out_name, std::ios_base::out);
                file_out << file_in.get() << std::endl;
                break;
            }
            default:
                std::cerr << "Usage : ./a.out [file_in] [file_out]" << std::endl;

        }
    }
    catch (std::ios_base::failure& e) {
        std::cerr << e.what() << std::endl;
        close_files(file_in, file_out);
        exit(1);
    }

    RAKE rk;
    rk.load_stop_chars();
    rk.load_stop_words();
    rk.print_stop_chars();

    // Close all files if open
    close_files(file_in, file_out);
    return 0;
}
