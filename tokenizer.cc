#include "tokenizer.h"

#include <vector>

#include <unicode/rbbi.h>
#include <unicode/ustream.h>

namespace sese {

Tokenizer::Tokenizer(icu::Locale locale) {
  UErrorCode status = U_ZERO_ERROR;
  bi_ = BreakIterator::createWordInstance(locale, status);
}

Tokenizer::~Tokenizer() {
  delete bi_;
}

std::vector<UnicodeString> Tokenizer::tokenize(std::string utf8str, bool enable_normalize) {
  UnicodeString u_str = UnicodeString::fromUTF8(utf8str);
  std::vector<UnicodeString> words;
  
  bi_->setText(u_str);
  int32_t pos = bi_->first();
  while (pos != BreakIterator::DONE) {
    int32_t next_pos = bi_->next();
    if (next_pos > pos) {
      words.emplace_back(u_str.tempSubString(pos, next_pos - pos));
    }
    pos = next_pos;
  }
  if (enable_normalize) {
    words = normalize(words);
  }
  return words;  
}

std::vector<UnicodeString> Tokenizer::normalize(std::vector<UnicodeString> tokens) {
  std::vector<UnicodeString> normalized;
  for (UnicodeString token : tokens) {
    token = token.trim().toLower();

    // remove trailing "es" or "s": naive stemming for English plural form
    if (token.endsWith(UnicodeString::fromUTF8("es"))) {
      token = token.tempSubString(0, token.length() - 2);
    } else if (token.endsWith(UnicodeString::fromUTF8("s"))) {
      token = token.tempSubString(0, token.length() - 1);
    }

    if (!token.isEmpty()) {
      normalized.emplace_back(token);
    }
  }
  return normalized;
}

} // namespace sese

