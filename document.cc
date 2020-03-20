#include "document.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <unicode/unistr.h>

#include "util.h"

namespace sese {

DocumentStore::DocumentStore(std::istream &ist) {
  load(ist);
}

void DocumentStore::save(std::ostream &ost) const {
  ost << id2document_.size() << std::endl;
  for (const std::pair<DocumentID, Document> &entry : id2document_) {
    ost << entry.second.document_id << "\t"
	<< entry.second.url << "\t"
	<< entry.second.title << "\t"
	<< entry.second.body.size() << std::endl;
    for (const std::string &line : entry.second.body) {
      ost << line << std::endl;
    }
  }
}

Document DocumentStore::getDocument(const DocumentID &document_id) const {
  const auto &it = id2document_.find(document_id);
  if (it != id2document_.end()) {
    return it->second;
  }else {
    return Document("", "", kNonExistentDocumentID, std::vector<std::string>());
  }
}

int DocumentStore::size() {
  return id2document_.size();
}

void DocumentStore::load(std::istream &ist) {
  std::string line;
  std::cout << "DocumentStore::load" << std::endl;
  id2document_.clear();
  int num_documents;
  ist >> num_documents;
  std::getline(ist, line);
  std::cout << num_documents << std::endl;
  for (int i = 0; i < num_documents; i++) {
    std::vector<std::string> body;
    std::getline(ist, line);
    
    std::stringstream ss(line);
    std::vector<std::string> fields = split(line, '\t');
    DocumentID document_id = std::atoi(fields[0].c_str());
    std::string url = fields[1];
    std::string title = fields[2];
    int body_size = std::atoi(fields[3].c_str());
    for (int bl = 0; bl < body_size; bl++) {
      std::getline(ist, line);
      body.push_back(line);
    }
    Document document(url, title, document_id, body);
    id2document_[document_id] = document;
  }
}

void DocumentStore::addDocument(Document document) {
  id2document_[document.document_id] = document;
  if (document.document_id % 10000 == 0) {
    std::cout << "Read " << document.document_id << " documents" << std::endl;
  }
}

std::vector<Document> DocumentReader::readXML(std::istream &ist) {
  std::vector<Document> documents;
  // dirty reader, should use proper XML parser
  std::string line;
  std::string url, title;
  std::vector<std::string> body;
  while (std::getline(ist, line)) {
    if (line.find("<doc id=") == 0) {
      // start line
      std::vector<std::string> elems = split(line, '"');
      url = elems[3];
      title = elems[5];
      body.clear();
    } else if (line.find("</doc>") == 0) {
      // end line
      DocumentID document_id(document_store_.id2document_.size());
      Document document(url,
			title,
			document_id,
			body);
      documents.push_back(document);
      document.body.clear();
      document_store_.addDocument(document);
    } else {
      // in body
      body.push_back(line);
    }
  }
  return documents;
}

DocumentStore &&DocumentReader::getDocumentStore() {
  return std::move(document_store_);
}


} // namespace sese {
