SHELL := /bin/bash

report.html: ../README.md Makefile GitHub.html5
	pandoc ../README.md --template GitHub.html5 --self-contained --toc --toc-depth 2 -r markdown+yaml_metadata_block -t html -s -o report.html
GitHub.html5: Makefile
	rm -f GitHub.html5 && wget https://raw.githubusercontent.com/tajmone/pandoc-goodies/master/templates/html5/github/GitHub.html5
