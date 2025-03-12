#!/bin/bash
[[ -e htmlrunningcalo ]] && exit 0
echo $$ > htmlrunningcalo
source ./setup_all.sh
Xvfb :4 -nolisten tcp &
export DISPLAY=unix:4
python3 makehtmlcalo.py >& /sphenix/user/sphnxpro/qahtml/QAhtml/makehtml/makehtmlcalo.log
kill $!
rm htmlrunningcalo
rm -f deadHotTowers*HIST_CALO*.root # -f: no error if no files exist
# no echos - this will force an email from cron
#echo "Finished"
