#!/bin/bash
[[ -e aggrunningcalo ]] && exit 0
echo $$ > aggrunningcalo
source ./setup_all.sh
python3 CaloDBAgg.py >& /sphenix/u/sphnxpro/qahtml/QAhtml/makehtml/runaggcalos.log

rm aggrunningcalo
# no echos - this will force an email from cron
#echo "Finished"
