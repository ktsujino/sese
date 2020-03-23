all: text

jawiki-latest-pages-articles.xml.bz2:
	wget https://dumps.wikimedia.org/jawiki/latest/jawiki-latest-pages-articles.xml.bz2

wikiextractor:
	git clone https://github.com/attardi/wikiextractor.git

text: wikiextractor jawiki-latest-pages-articles.xml.bz2
	wikiextractor/WikiExtractor.py jawiki-latest-pages-articles.xml.bz2

clean:
	rm -rf text *~ wikiextractor jawiki-latest-pages-articles.xml.bz2
