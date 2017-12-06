SHELL := /bin/bash
TEMPLATE = "https://raw.githubusercontent.com/tajmone/pandoc-goodies/2e29e9cd54bb9113c0b4fb474a07b8fcf6ee287b/templates/html5/github/GitHub.html5"
.PHONY: clean

report/report.html: README.md report/GitHub.html5 renderings/frame.png renderings/drag.png renderings/wheel.png report/generated | report
	pandoc README.md --template report/GitHub.html5 --resource-path=assets:renderings --self-contained --toc --toc-depth 3 -r markdown+yaml_metadata_block+footnotes+pipe_tables -t html -s -o report/report.html
report/GitHub.html5: | report
	rm -f report/GitHub.html5 && wget $(TEMPLATE) -O report/GitHub.html5
clean:
	rm -rf report
	rm -rf jng55_zd53_jgc232.zip
	rm -rf renderings
	rm -rf generated
renderings/frame.png: cad/frame.scad cad/main.scad | renderings
	openscad --render --imgsize 1600,1200 -o renderings/frame.png cad/frame.scad
renderings/drag.png: cad/drag.scad cad/main.scad | renderings
	openscad --render --imgsize 1600,1200 -o renderings/drag.png cad/drag.scad
renderings/wheel.png: cad/wheel.scad cad/main.scad | renderings
	openscad --render --imgsize 1600,1200 --camera 0,0,0,40,0,25,240 -o renderings/wheel.png cad/wheel.scad
report/generated: report/GitHub.html5 ble.X/$(wildcard *.c) ble.X/$(wildcard *.h)
	./generate_source_pages.sh
report:
	mkdir -p $@
renderings:
	mkdir -p $@
zip: jng55_zd53_jgc232.zip
jng55_zd53_jgc232.zip: report/report.html
	rm -rf jng55_zd53_jgc232
	mkdir jng55_zd53_jgc232
	cp -r report/* jng55_zd53_jgc232/
	zip -r jng55_zd53_jgc232.zip jng55_zd53_jgc232/
	rm -rf jng55_zd53_jgc232
