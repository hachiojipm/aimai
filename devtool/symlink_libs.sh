#!/bin/bash

set -eu

REPO_ROOT="$(cd ./"$(git rev-parse --show-cdup)" || exit; pwd)"

SI4703_LINK="Si4703"
SI4713_LINK="Si4713"
SSD1306_LINK="SSD1306_LINK"

(cd "${REPO_ROOT}/lib" && rm -rf "$SI4703_LINK" && ln -s ../Si4703_FM_Tuner_Evaluation_Board/Libraries/Arduino "$SI4703_LINK")
(cd "${REPO_ROOT}/lib" && rm -rf "$SI4713_LINK" && ln -s ../Adafruit-Si4713-Library "$SI4713_LINK")
(cd "${REPO_ROOT}/lib" && rm -rf "$SSD1306_LINK" && ln -s ../Adafruit_SSD1306 "$SSD1306_LINK")

