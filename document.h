#ifndef _SESE_DOCUMENT_H_
#define _SESE_DOCUMENT_H_

#include <iostream>
#include <vector>
#include <map>

#include <gtest/gtest.h>
#include <unicode/unistr.h>

namespace sese {

using DocumentID = int; // for future adjustment of bit width
constexpr DocumentID kNonExistentDocumentID = -1; // guaranteed to be smaller than any existing document ID

struct Document {
public:
  Document(std::string url, std::string title,
	   DocumentID document_id, std::vector<std::string> body) :
    url(url), title(title), document_id(document_id), body(body) {};
  Document() {};
    
  DocumentID document_id;
  std::string url;
  std::string title;
  std::vector<std::string> body;
};

class DocumentStore {
public:
  DocumentStore(std::istream &ist);
  void save(std::ostream &ost);  
  Document getDocument(const DocumentID &document_id);
  int size();

private:
  DocumentStore() {};
  void load(std::istream &ist);
  void addDocument(Document document);
  std::map<DocumentID, Document> id2document_;

  friend class DocumentReader;
  FRIEND_TEST(Index, query);
  FRIEND_TEST(Index, save_and_load);
};

class DocumentReader {
public:
  std::vector<Document> readXML(std::istream &ist);
  DocumentStore getDocumentStore();

private:
  DocumentStore document_store_;
};

} // namespace sese

#endif // #ifndef _SESE_DOCUMENT_H_
