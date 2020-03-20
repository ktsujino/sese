#include <fstream>
#include <vector>

#include "document.h"
#include "index.h"
#include "lexicon.h"
#include "query.h"
#include "ranker.h"

int main(int argc, char* argv[])
{
  sese::DocumentReader document_reader;
  sese::IndexBuilder index_builder;
  for (int i = 2; i < argc; i++) {
    std::ifstream ifs(argv[i]);
    std::vector<sese::Document> documents = document_reader.readXML(ifs);
    for (const sese::Document &document : documents) {
      index_builder.addDocument(document);
    }
    std::cout << "Finished processing " << argv[i] << std::endl;
  }
  sese::DocumentStore document_store = document_reader.getDocumentStore();
  sese::Index index = index_builder.getIndex();
  sese::Lexicon lexicon = index_builder.getLexicon();
  std::ofstream ofs_lexicon(std::string(argv[1]) + "/lexicon.txt");
  std::ofstream ofs_index(std::string(argv[1]) + "/index.txt");
  std::ofstream ofs_document_store(std::string(argv[1]) + "/documents.txt");
  lexicon.save(ofs_lexicon);
  index.save(ofs_index);
  document_store.save(ofs_document_store);
}
