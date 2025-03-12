#!/bin/bash
[[ -e htmlrunning ]] && exit 0
echo $$ > htmlrunning
source ./setup_all.sh
Xvfb :2 -nolisten tcp &
export DISPLAY=unix:2
for i in /sphenix/data/data02/sphnxpro/QAhtml/aggregated/HIST_CALO_*.root; do
root.exe -q TestDrawingJetQA.C\(\"${i}\"\); done
kill $!
rm htmlrunning
