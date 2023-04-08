#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <vector>
#include <map>


//'`~!@#$%^&*()-=+[{]}\\|;:\'",.<>/?'

template<typename T>
concept FileStream = std::same_as<T, std::ifstream> || std::same_as<T, std::ofstream>;

class WordScore {
    using type = unsigned int;
    type freq = 0;
    type deg = 0;
public:
    void incr_dif() { deg++ }

    void incr_same() {freq++; deg++; }

    double score() {
        return static_cast<double>(deg) / freq;
    }

};

//class RAKE {
//    std::istream* isp_;
//
//};

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
    // Close all files if open
    close_files(file_in, file_out);
    return 0;
}
