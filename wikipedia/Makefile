all: text

jawiki-20200220-pages-articles.xml.bz2:
	curl https://dumps.wikimedia.org/jawiki/20200220/jawiki-20200220-pages-articles.xml.bz2

wikiextractor:
	git clone https://github.com/attardi/wikiextractor.git

text: wikiextractor jawiki-20200220-pages-articles.xml.bz2
	wikiextractor/WikiExtractor.py jawiki-20200220-pages-articles.xml.bz2

clean:
	rm -rf text *~ wikiextractor
