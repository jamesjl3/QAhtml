#!/bin/bash
[[ -e aggrunningbco ]] && exit 0
echo $$ > aggrunningbco
source ./setup_all.sh
python3 BcoDBAgg.py >& /sphenix/u/sphnxpro/qahtml/QAhtml/makehtml/runaggbco.log
rm aggrunningbco
# no echos - this will force an email from cron
#echo "Finished"
