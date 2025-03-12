#!/bin/bash
[[ -e aggrunningrocbco ]] && exit 0
echo $$ > aggrunningrocbco
source ./setup_all.sh
python3 BcoDBROCAgg.py >& /sphenix/u/sphnxpro/qahtml/QAhtml/makehtml/runaggrocbco.log
rm aggrunningrocbco
# no echos - this will force an email from cron
#echo "Finished"
