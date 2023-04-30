#ifndef PROJECT_FILEPROCESS_HPP
#define PROJECT_FILEPROCESS_HPP

#ifndef IOSTREAM
#define IOSTREAM
#include <iostream>
#endif

#ifndef FSTREAM
#define FSTREAM
#include <fstream>
#endif

#ifndef CONCEPTS
#define CONCEPTS
#include <concepts>
#endif

namespace FileProcess {
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
}

#endif //PROJECT_FILEPROCESS_HPP
