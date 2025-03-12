#!/bin/bash
[[ -e aggrunningtrack ]] && exit 0
echo $$ > aggrunningtrack
source ./setup_all.sh
python3 TrackerDBAgg.py >& /sphenix/u/sphnxpro/qahtml/QAhtml/makehtml/runaggtrack.log
rm aggrunningtrack
# no echos - this will force an email from cron
#echo "Finished"
