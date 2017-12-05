SHELL := /bin/bash
TEMPLATE = "https://raw.githubusercontent.com/tajmone/pandoc-goodies/2e29e9cd54bb9113c0b4fb474a07b8fcf6ee287b/templates/html5/github/GitHub.html5"

report/report.html: README.md Makefile report/GitHub.html5
	mkdir -p report && pandoc README.md --template report/GitHub.html5 --self-contained --toc --toc-depth 2 -r markdown+yaml_metadata_block -t html -s -o report/report.html
report/GitHub.html5: Makefile
	mkdir -p report && rm -f report/GitHub.html5 && wget $(TEMPLATE) -O report/GitHub.html5
clean:
	rm -rf report
