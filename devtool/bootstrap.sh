#!/bin/bash

set -uex

pythonz install 3.7.4 --configure="--with-openssl=$(brew --prefix openssl)"

ENVRC_FILE='.envrc'
ENVRC_PYTHON_INST="layout python \$(pythonz locate 3.7.4)"

if ! grep "$ENVRC_PYTHON_INST" "$ENVRC_FILE"; then
  echo "$ENVRC_PYTHON_INST" >> .envrc
  direnv allow .
fi

pip install platformio

