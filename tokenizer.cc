#include "tokenizer.h"

#include <vector>

#include <unicode/brkiter.h>
#include <unicode/rbbi.h>
#include <unicode/ustream.h>

namespace sese {

Tokenizer::Tokenizer(icu::Locale locale) {
  UErrorCode status = U_ZERO_ERROR;
  bi_ = icu::BreakIterator::createWordInstance(locale, status);
}

Tokenizer::~Tokenizer() {
  delete bi_;
}

std::vector<icu::UnicodeString> Tokenizer::tokenize(std::string utf8str, bool enable_normalize) const {
  icu::UnicodeString u_str = icu::UnicodeString::fromUTF8(utf8str);
  std::vector<icu::UnicodeString> words;
  
  bi_->setText(u_str);
  int32_t pos = bi_->first();
  while (pos != icu::BreakIterator::DONE) {
    int32_t next_pos = bi_->next();
    if (next_pos > pos) {
      icu::UnicodeString word = u_str.tempSubString(pos, next_pos - pos);
      words.push_back(word);
    }
    pos = next_pos;
  }
  if (enable_normalize) {
    words = normalize(words);
  }
  return words;  
}

std::vector<icu::UnicodeString> Tokenizer::normalize(std::vector<icu::UnicodeString> tokens) const {
  std::vector<icu::UnicodeString> normalized;
  for (icu::UnicodeString token : tokens) {
    token = token.trim().toLower();

    // remove trailing "es" or "s": naive stemming for English plural form
    if (token.endsWith(icu::UnicodeString::fromUTF8("es"))) {
      token = token.tempSubString(0, token.length() - 2);
    } else if (token.endsWith(icu::UnicodeString::fromUTF8("s"))) {
      token = token.tempSubString(0, token.length() - 1);
    }

    if (!token.isEmpty()) {
      normalized.emplace_back(token);
    }
  }
  return normalized;
}

} // namespace sese

