#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>

#include <unicode/rbbi.h>
#include <unicode/ustream.h>

#include <gtest/gtest.h>

#include "document.h"
#include "index.h"
#include "lexicon.h"
#include "query.h"
#include "ranker.h"
#include "tokenizer.h"

namespace sese {

std::vector<UnicodeString> usVector(std::initializer_list<std::string> l) {
  std::vector<UnicodeString> us;
  std::for_each(l.begin(), l.end(),
		[&us](const std::string &word){us.push_back(UnicodeString::fromUTF8(word));});
  return us;
}

template <class T>
void assertVectorEqual(const std::vector<T> &expected,
		       const std::vector<T> &actual) {
  EXPECT_EQ(expected.size(), actual.size());
  for (int i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], actual[i]);
  }
}

TEST(Tokenizer, not_normalized) {
  Tokenizer tokenizer;
  std::vector<UnicodeString> tokenized = tokenizer.tokenize("I love NY!", false);
  std::vector<UnicodeString> expected = usVector({"I", " ", "love", " ", "NY", "!"});
  assertVectorEqual(expected, tokenized);
}

TEST(Tokenizer, normalized) {
  Tokenizer tokenizer;
  std::vector<UnicodeString> tokenized = tokenizer.tokenize("She loves NY guys!", true);
  std::vector<UnicodeString> expected = usVector({"she", "lov", "ny", "guy", "!"});
  assertVectorEqual(expected, tokenized);
}

TEST(Lexicon, conversion) {
  Lexicon lexicon;
  const UnicodeString foo = UnicodeString::fromUTF8("foo");
  const UnicodeString bar = UnicodeString::fromUTF8("bar");
  const UnicodeString baz = UnicodeString::fromUTF8("baz");

  lexicon.setEntry(foo, WordID(0), 1);
  lexicon.setEntry(bar, WordID(1), 2);
  EXPECT_EQ(0, lexicon.token2id(foo));
  EXPECT_EQ(1, lexicon.token2id(bar));
  EXPECT_EQ(Lexicon::outOfVocabularyId(), lexicon.token2id(baz));
  EXPECT_EQ(foo, lexicon.id2token(0));
  EXPECT_EQ(bar, lexicon.id2token(1));
  EXPECT_EQ(Lexicon::outOfVocabularyToken(), lexicon.id2token(2));
  EXPECT_EQ(1, lexicon.token2DocumentFrequency(foo));
  EXPECT_EQ(2, lexicon.token2DocumentFrequency(bar));
  EXPECT_EQ(0, lexicon.token2DocumentFrequency(baz));

  std::vector<UnicodeString> tokens({foo, bar, baz});
  std::vector<WordID> expected_ids({0, 1, Lexicon::outOfVocabularyId()});
  std::vector<UnicodeString> expected_backtranslated_tokens({foo, bar, Lexicon::outOfVocabularyToken()});

  std::vector<WordID> actual_ids = lexicon.tokens2ids(tokens);
  assertVectorEqual(expected_ids, actual_ids);
  std::vector<UnicodeString> actual_backtranslated_tokens = lexicon.ids2tokens(actual_ids);
  assertVectorEqual(expected_backtranslated_tokens, actual_backtranslated_tokens);
}

TEST(Lexicon, save_and_load) {
  LexiconBuilder lexicon_builder;
  std::vector<UnicodeString> corpus0 = usVector({"foo", "bar"});
    std::vector<UnicodeString> corpus1 = usVector({"foo", "foo"});
  lexicon_builder.registerTokens(corpus0);
  lexicon_builder.registerTokens(corpus1);
  Lexicon generated_lexicon = lexicon_builder.getLexicon();

  std::string lexicon_file_name = "/tmp/test_lexicon_save_and_load.lexicon";
  std::ofstream ofs(lexicon_file_name);
  generated_lexicon.save(ofs);
  ofs.close();

  std::ifstream ifs(lexicon_file_name);
  Lexicon loaded_lexicon(ifs);
  
  std::vector<UnicodeString> query = usVector({"foo", "bar", "baz"});
  std::vector<WordID> expected_ids_lexicon({0, 1, Lexicon::outOfVocabularyId()});
  std::vector<UnicodeString> expected_backtranslated_tokens({query[0], query[1], Lexicon::outOfVocabularyToken()});

  std::vector<WordID> actual_ids_lexicon = loaded_lexicon.tokens2ids(query);
  assertVectorEqual(expected_ids_lexicon, actual_ids_lexicon);
  std::vector<UnicodeString> actual_backtranslated_tokens = loaded_lexicon.ids2tokens(actual_ids_lexicon);
  assertVectorEqual(expected_backtranslated_tokens, actual_backtranslated_tokens);

  EXPECT_EQ(2, loaded_lexicon.token2DocumentFrequency(UnicodeString::fromUTF8("foo")));
  EXPECT_EQ(1, loaded_lexicon.token2DocumentFrequency(UnicodeString::fromUTF8("bar")));
  EXPECT_EQ(0, loaded_lexicon.token2DocumentFrequency(UnicodeString::fromUTF8("baz")));
  EXPECT_EQ(2, loaded_lexicon.id2DocumentFrequency(WordID(0)));
  EXPECT_EQ(1, loaded_lexicon.id2DocumentFrequency(WordID(1)));
  EXPECT_EQ(0, loaded_lexicon.id2DocumentFrequency(WordID(2)));
}
  
TEST(LexiconBuilder, read_and_get_lexicon) {
  LexiconBuilder lexicon_builder;
  std::vector<UnicodeString> corpus0 = usVector({"foo", "bar"});
  std::vector<WordID> expected_ids_builder0({0, 1});
  std::vector<UnicodeString> corpus1 = usVector({"foo", "foo"});
  std::vector<WordID> expected_ids_builder1({0, 0});

  std::vector<WordID> actual_ids_builder0 = lexicon_builder.registerTokens(corpus0);
  assertVectorEqual(expected_ids_builder0, actual_ids_builder0);
  std::vector<WordID> actual_ids_builder1 = lexicon_builder.registerTokens(corpus1);
  assertVectorEqual(expected_ids_builder1, actual_ids_builder1);

  Lexicon lexicon = lexicon_builder.getLexicon();
  std::vector<UnicodeString> query = usVector({"foo", "bar", "baz"});
  std::vector<WordID> expected_ids_lexicon({0, 1, Lexicon::outOfVocabularyId()});
  std::vector<UnicodeString> expected_backtranslated_tokens({query[0], query[1], Lexicon::outOfVocabularyToken()});

  std::vector<WordID> actual_ids_lexicon = lexicon.tokens2ids(query);
  assertVectorEqual(expected_ids_lexicon, actual_ids_lexicon);
  std::vector<UnicodeString> actual_backtranslated_tokens = lexicon.ids2tokens(actual_ids_lexicon);
  assertVectorEqual(expected_backtranslated_tokens, actual_backtranslated_tokens);

  EXPECT_EQ(2, lexicon.token2DocumentFrequency(UnicodeString::fromUTF8("foo")));
  EXPECT_EQ(1, lexicon.token2DocumentFrequency(UnicodeString::fromUTF8("bar")));
  EXPECT_EQ(0, lexicon.token2DocumentFrequency(UnicodeString::fromUTF8("baz")));
  EXPECT_EQ(2, lexicon.id2DocumentFrequency(WordID(0)));
  EXPECT_EQ(1, lexicon.id2DocumentFrequency(WordID(1)));
  EXPECT_EQ(0, lexicon.id2DocumentFrequency(WordID(2)));
}

void queryTest(const Index &index,
	       const QueryProcessor &query_processor,
	       const std::string &keywords,
	       std::vector<MatchInfo> expected_result) {
  QueryInfo query_info = query_processor.processQuery(keywords);
  std::vector<MatchInfo> actual_result = index.query(query_info);
  EXPECT_EQ(expected_result.size(), actual_result.size());
  for (int pos = 0; pos < expected_result.size(); pos++) {
    EXPECT_EQ(expected_result[pos].document_id, actual_result[pos].document_id);
    EXPECT_EQ(expected_result[pos].document_length, actual_result[pos].document_length);
    assertVectorEqual(expected_result[pos].term_frequency, actual_result[pos].term_frequency);
  }
}

TEST(Index, query) {
  Document document0("url0", "foo", DocumentID(0), std::vector<std::string>{"foo foo bar"});
  Document document1("url1", "foo", DocumentID(1), std::vector<std::string>{"baz", "bun"});

  std::vector<Document> documents{document0, document1};

  IndexBuilder index_builder(documents);
  Index index = index_builder.getIndex();
  Lexicon lexicon = index_builder.getLexicon();
  QueryProcessor query_processor(lexicon);

  std::string query0 = "foo";
  std::vector<MatchInfo> expected_result0{MatchInfo{0, 4, std::vector<int>{3}},
					  MatchInfo{1, 3, std::vector<int>{1}}};
  queryTest(index, query_processor, query0, expected_result0);

  std::string query1 = "foo bar";
  std::vector<MatchInfo> expected_result1{MatchInfo{0, 4, std::vector<int>{3, 1}}};
  queryTest(index, query_processor, query1, expected_result1);

  std::string query2 = "baz";
  std::vector<MatchInfo> expected_result2{MatchInfo{1, 3, std::vector<int>{1}}};
  queryTest(index, query_processor, query2, expected_result2);

  std::string query3 = "foo baz bun";
  std::vector<MatchInfo> expected_result3{MatchInfo{1, 3, std::vector<int>{1, 1, 1}}};
  queryTest(index, query_processor, query3, expected_result3);

  std::string query4 = "foo bar baz";
  std::vector<MatchInfo> expected_result4;
  queryTest(index, query_processor, query4, expected_result4);

  std::string query5 = "xxx";
  std::vector<MatchInfo> expected_result5;
  queryTest(index, query_processor, query5, expected_result5);
}

TEST(Index, save_and_load) {
  Document document0("url0", "foo", DocumentID(0), std::vector<std::string>{"foo foo bar"});
  Document document1("url1", "foo", DocumentID(1),std::vector<std::string>{"baz", "bun"});

  std::vector<Document> documents{document0, document1};
  IndexBuilder index_builder(documents, icu::Locale::getRoot());
  Index generated_index = index_builder.getIndex();
  Tokenizer tokenizer;
  Lexicon generated_lexicon = index_builder.getLexicon();

  std::string index_file_name = "/tmp/test_index_save_and_load.index";
  std::ofstream ofs_index(index_file_name);
  generated_index.save(ofs_index);
  ofs_index.close();
  std::string lexicon_file_name = "/tmp/test_index_save_and_load.lexicon";
  std::ofstream ofs_lexicon(lexicon_file_name);
  generated_lexicon.save(ofs_lexicon);
  ofs_lexicon.close();

  std::ifstream ifs_index(index_file_name);
  Index loaded_index(ifs_index);
  std::ifstream ifs_lexicon(lexicon_file_name);
  QueryProcessor query_processor(ifs_lexicon);

  std::string query0 = "foo";
  std::vector<MatchInfo> expected_result0{MatchInfo{0, 4, std::vector<int>{3}},
					  MatchInfo{1, 3, std::vector<int>{1}}};
  queryTest(loaded_index, query_processor, query0, expected_result0);

  std::string query1 = "foo bar";
  std::vector<MatchInfo> expected_result1{MatchInfo{0, 4, std::vector<int>{3, 1}}};
  queryTest(loaded_index, query_processor, query1, expected_result1);

  std::string query2 = "baz";
  std::vector<MatchInfo> expected_result2{MatchInfo{1, 3, std::vector<int>{1}}};
  queryTest(loaded_index, query_processor, query2, expected_result2);

  std::string query3 = "foo baz bun";
  std::vector<MatchInfo> expected_result3{MatchInfo{1, 3, std::vector<int>{1, 1, 1}}};
  queryTest(loaded_index, query_processor, query3, expected_result3);

  std::string query4 = "foo bar baz";
  std::vector<MatchInfo> expected_result4;
  queryTest(loaded_index, query_processor, query4, expected_result4);

  std::string query5 = "xxx";
  std::vector<MatchInfo> expected_result5;
  queryTest(loaded_index, query_processor, query5, expected_result5);
}

TEST(DocumentReader, read_stream) {
  std::string input_string =
    "<doc id=\"1\" url=\"url0\" title=\"title0\">\n"
    "body0\n"
    "</doc>\n"
    "<doc id=\"2\" url=\"url1\" title=\"title1\">\n"
    "body1-1\n"
    "body1-2\n"
    "</doc>\n";

  std::stringstream ss(input_string);
  DocumentReader document_reader;
  std::vector<Document> documents = document_reader.readXML(ss);

  EXPECT_EQ(2, documents.size());
  EXPECT_EQ(DocumentID(0), documents[0].document_id);
  EXPECT_EQ("url0", documents[0].url);
  EXPECT_EQ("title0", documents[0].title);
  assertVectorEqual(std::vector<std::string>{"body0"}, documents[0].body);
  EXPECT_EQ(DocumentID(1), documents[1].document_id);
  EXPECT_EQ("url1", documents[1].url);
  EXPECT_EQ("title1", documents[1].title);
  assertVectorEqual(std::vector<std::string>{"body1-1", "body1-2"}, documents[1].body);

  DocumentStore document_store = document_reader.getDocumentStore();
  Document document0 = document_store.getDocument(DocumentID(0));
  EXPECT_EQ("url0", document0.url);
  EXPECT_EQ("title0", document0.title);
  assertVectorEqual(std::vector<std::string>{}, document0.body);
}

TEST(Ranker, rank) {
  std::vector<MatchInfo> documents{MatchInfo{DocumentID(0),
					     4,
					     std::vector<int>{2,1}},
				   MatchInfo{DocumentID(1),
					     6,
					     std::vector<int>{1,3}}};
  QueryInfo query_info{"foo bar",
		       std::vector<UnicodeString>{UnicodeString::fromUTF8("foo"),
						  UnicodeString::fromUTF8("bar")},
		       std::vector<WordID>{WordID(0), WordID(1)},
		       std::vector<int>{2,4}};
  Ranker ranker;
  std::vector<std::pair<MatchInfo, RankScore>> ranked = ranker.rank(documents, query_info);
  ASSERT_FLOAT_EQ(0.9016844005556022, ranked[0].second); // (2.0 / 4) / log(2) + (1.0 / 4) / log(4) = 0.9016844005556022
  ASSERT_FLOAT_EQ(0.6011229337037347, ranked[1].second); // (1.0 / 6) / log(2) + (3.0 / 6) / log(4) = 0.6011229337037347
  EXPECT_EQ(DocumentID(0), ranked[0].first.document_id);
  EXPECT_EQ(DocumentID(1), ranked[1].first.document_id);
}

} // namespace sese

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
