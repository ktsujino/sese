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
  sese::SearchEngine search_engine(ifs_lexicon, ifs_index, ifs_documents);
  while (true) {
    std::string keywords;
    std::getline(std::cin, keywords);
    std::cout << keywords << std::endl;
    std::vector<sese::Document> documents = search_engine.query(keywords);
    for (int i = 0; i < documents.size() && i < 20; i++) {
      sese::Document document = documents[i];
      std::cout << document.document_id << "\t" << document.title << "\t" << document.url << std::endl;
    }
  }
  return 0;
}
