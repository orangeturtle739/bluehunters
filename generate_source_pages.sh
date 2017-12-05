#! /bin/bash

mkdir -p generated

for f in ble.X/*.c; do
  TARGET_MD="generated/$(basename $f).md"
  TARGET_HTML="report/$(basename $f).html"
  rm -f $TARGET_MD
  rm -f $TARGET_HTML
  echo "---" >> $TARGET_MD
  echo "title: $(basename $f)" >> $TARGET_MD
  echo "---" >> $TARGET_MD
  echo >> $TARGET_MD
  echo '```c' >> $TARGET_MD
  cat $f >> $TARGET_MD
  echo >> $TARGET_MD
  echo '```' >> $TARGET_MD
  pandoc $TARGET_MD --template report/GitHub.html5 --self-contained --toc --toc-depth 3 -r markdown+yaml_metadata_block -t html -s -o $TARGET_HTML
done
