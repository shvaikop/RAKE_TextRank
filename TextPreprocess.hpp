#ifndef PROJECT_TEXTPREPROCESS_HPP
#define PROJECT_TEXTPREPROCESS_HPP

//#include <iostream>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <vector>

#include "FileProcess.hpp"

namespace TextProcess {
    // Input: path to file with stop_chars, Output: stop chars loaded into a set
    std::unordered_set<char> load_stop_chars(const std::string& chars_file_path) {
        std::unordered_set<char> stop_chars;
        std::ifstream stop_chars_file = FileProcess::open_file<std::ifstream>(chars_file_path, std::ios_base::in);
        char c;
        while (stop_chars_file.get(c)) {
            stop_chars.insert(c);
        }
        return stop_chars;
    }

    // Input: path to file with stop_words, Output: stop words loaded into a set
    std::unordered_set<std::string> load_stop_words(const std::string& words_file_path) {
        std::unordered_set<std::string> stop_words;
        std::ifstream stop_words_file = FileProcess::open_file<std::ifstream>(words_file_path, std::ios_base::in);
        std::string word;
        char c;
        while(stop_words_file.get(c)) {
            if (std::isspace(c)) {
                if (!word.empty()) {
                    stop_words.insert(std::move(word));
                    word.clear();
                }
            }
            else {
                word += c;
            }
        }
        return stop_words;
    }

    std::vector<std::vector<std::string>> read_text(std::istream& in_stream, const std::unordered_set<char>& stop_chars, const std::unordered_set<std::string>& stop_words) {
        std::vector<std::vector<std::string>> phrases;
        std::vector<std::string> phrase;
        std::string word;
        char c;
        while (in_stream.get(c)) {
            if (!stop_chars.contains(c) && !std::isspace(c)) {
                word += std::tolower(c);
                continue;
            }
                // c is white space or stop_char
            else if (word.empty()) {
                continue;
            }
                // word is not empty
            else if (std::isspace(c) and !stop_words.contains(word)) {
                phrase.push_back(std::move(word));
                word.clear();
                continue;
            }
                // either c is a stop_char or word is a stop_word
            else if (stop_words.contains(word)){
                if (!phrase.empty()){
                    phrases.push_back(std::move(phrase));
                    phrase.clear();
                }
                word.clear();
                continue;
            }
                // c must be a stop_char
            else if (stop_chars.contains(c)) {
                phrase.push_back(std::move(word));
                if (!phrase.empty()){
                    phrases.push_back(std::move(phrase));
                    phrase.clear();
                }
                word.clear();
            }

            else {
                // TODO throw exception
            }
        }

        if (!word.empty() && !stop_words.contains(word)) {
            phrase.push_back(std::move(word));
        }

        if (!phrase.empty()) {
            phrases.push_back(std::move(phrase));
        }
        return phrases;
    }

    void custom_print(const std::vector< std::vector<std::string> >& inpt) {
        for (const auto& phrase: inpt) {
            for (const auto& word: phrase) {
                std::cout << word << " ";
            }
            std::cout << std::endl;
        }
    }
}


#endif //PROJECT_TEXTPREPROCESS_HPP
