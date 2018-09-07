#!/bin/bash

ln -f credex-all redex-all
pushd redex
tar czf ../credex.tar.gz ../redex-all redex.py pyredex/*.py
cat selfextract.sh ../credex.tar.gz > ../credex
chmod +x ../credex
popd
rm redex-all
