#ifndef PROJECT_TEXTPREPROCESS_HPP
#define PROJECT_TEXTPREPROCESS_HPP

#ifndef UNORDERED_SET
#define UNORDERED_SET
#include <unordered_set>
#endif

#ifndef FILESYSTEM
#define FILESYSTEM
#include <filesystem>
#endif

#ifndef IOSTREAM
#define IOSTREAM
#include <iostream>
#endif

#ifndef FSTREAM
#define FSTREAM
#include <fstream>
#endif

#ifndef VECTOR
#define VECTOR
#include <vector>
#endif

#include "FileProcess.hpp"

namespace TextProcess {
    using strVec = std::vector<std::string>;
    using phraseVector = std::vector< strVec >;

    // Input: path to file with stop_chars, Output: stop chars loaded into a set
    std::unordered_set<char> load_stop_chars(const std::string& chars_file_path);

    // Input: path to file with stop_words, Output: stop words loaded into a set
    std::unordered_set<std::string> load_stop_words(const std::string& words_file_path);

    std::vector<std::vector<std::string>> parse_text_phrases(std::istream& in_stream, const std::unordered_set<char>& stop_chars, const std::unordered_set<std::string>& stop_words);

    // Function that splits a text into sentences
    std::vector<std::string> parse_text_sentences(std::istream& in_stream, const std::unordered_set<char>& sent_end_chars);

    // split a string (sentence) into a vector of words, removing stop_chars, stop_words
    strVec process_sentence(const std::string& sentence_in, const std::unordered_set<char>& stop_chars,
                           const std::unordered_set<std::string>& stop_words);

    // split each sentence into a vector of words, remove stop_words, stop_chars
    phraseVector process_sentences(const strVec& sentences, const std::unordered_set<char>& stop_chars,
                                   const std::unordered_set<std::string>& stop_words);

    void output_to_stream(std::ostream& out_stream, const std::vector< std::vector<std::string> >& str_matrix);

    void output_to_stream(std::ostream& out_stream, const std::vector< std::string>& str_vec);
}


#endif //PROJECT_TEXTPREPROCESS_HPP
