# RAKE_TextRank
An implementation of **Rapid Automatic Keyword Extraction (RAKE)** and **TextRank** algorithms for summarization of text.

## Building and Usage
The ***Resources*** folder contains files ***stopchars.txt*** and ***stopwords.txt***.
These files contain the stop phrases and stop characters which are used by the program to parse the input text into tokens.
They can be modified according to the specifics of the text.
The paths to these files are stored in constants in ***main.cpp*** however as an extension, the program could have been modified
to get the paths to these files from ***environment variables***.
Furthermore, ***sentence_end_chars*** are defined directly inside ***main.cpp*** and could be modified if needed as well.
### How to compile?
**Option 1:**
```console
g++ -std=c++20 src/main.cpp src/Rake.cpp src/TextPreprocess.cpp src/TextRank.cpp -lboost_program_options
```
**Option 2:**
*CMakeFile.txt* is included and could be used to build the project.
### Usage
```console
./program <--rake | --text-rank> [--input-file file_name] [--output-file file_name] [--lenght n | --percent d]
```
- ``` <--rake | --text-rank> ```: summarize using RAKE or TextRank
- Default input-file : ```std::cin```
- Default output-file : ```std::cout```
- ```console [--lenght n | --percent d] ``` : Chose the length of the summary by number of keywords / sentences or by percentage of the whole text
## Description
### Rapid Automatic Keyword Extraction (RAKE)
RAKE is a keyword extraction algorithm that is based on splitting the intput text into phrases, scoring each word based on its occurance frequency as well as its co-occurance with other words. Phrase scores are calculated by summing the scores of the words in the phrases. Phrase score corresponds to its importance.
RAKE is a simple and efficient algorithm which can be used to get the key phrases in a large text quickly although it does not always produce the best results.
### TextRank
TextRank is an algorithm that is based on Google's PageRank algorithm for scoring web pages by importance. It splits the text into sentences and builds a graph where sentences are the nodes and edges correspond to similarity between sentences. There are various formulas for calculating the similarity between sentences. The formula that I used: 
