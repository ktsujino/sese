#include <iostream>
#include <unicode/rbbi.h>
#include <unicode/ustream.h>

#include "tokenizer.h"

int main() {
  sese::Tokenizer tokenizer(Locale::getRoot());
  while (true) {
    std::string s;
    std::getline(std::cin, s);
    std::vector<UnicodeString> words = tokenizer.tokenize(s);
    for (const auto &word : words) {
      std::cout << word << std::endl;
    }
    std::cout << std::endl;
  }
}
