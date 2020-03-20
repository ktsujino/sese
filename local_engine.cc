#include <fstream>
#include <iostream>
#include <vector>

#include "document.h"
#include "search_engine.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: "
	      << argv[0]
	      << " <index dir> "
	      << std::endl;
    return -1;
  }
  std::ifstream ifs_lexicon(std::string(argv[1]) + "/lexicon.txt");
  std::ifstream ifs_index(std::string(argv[1]) + "/index.txt");
  std::ifstream ifs_documents(std::string(argv[1]) + "/documents.txt");
  std::cout << "Loading index file..." << std::endl;
  sese::SearchEngine search_engine(ifs_lexicon, ifs_index, ifs_documents);
  std::cout << "Ready." << std::endl;  
  while (true) {
    std::string keywords;
    std::getline(std::cin, keywords);
    std::cout << keywords << std::endl;
    std::vector<sese::Document> documents = search_engine.query(keywords);
    for (const sese::Document &document : documents) {
      std::cout << document.document_id << "\t" << document.title << "\t" << document.url << std::endl;
    }
  }
  return 0;
}
