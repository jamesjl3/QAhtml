#!/bin/bash
[[ -e htmlrunningtrk ]] && exit 0
echo $$ > htmlrunningtrk
source ./setup_all.sh
Xvfb :2 -nolisten tcp &
export DISPLAY=unix:2
python3 makehtmltrackers.py >& /sphenix/u/sphnxpro/qahtml/QAhtml/makehtml/makehtmltrack.log
kill $!
rm htmlrunningtrk
# no echos - this will force an email from cron
#echo "Finished"
