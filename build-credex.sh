#!/bin/bash

pushd redex
tar czf ../credex.tar.gz ../credex-all redex.py pyredex/*.py
cat selfextract.sh ../credex.tar.gz > ../credex
chmod +x ../credex
