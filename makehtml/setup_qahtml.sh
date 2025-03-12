#! /bin/bash

if [[ ! "$0" != "$BASH_SOURCE" ]]
then
    echo "setup_qahtml.sh needs to be sourced"
    exit
fi
if [[ -z "$QA_MAIN" ]]
then
    echo "QA_MAIN environment variable not set, exiting"
    exit -1
fi

[[ -z "$QA_HTMLDIR" ]] && export QA_HTMLDIR=/sphenix/WWW/subsystem/QAHtmlTest
if [ -f ${OPT_SPHENIX}/bin/setup_local.sh ]
then
  source ${OPT_SPHENIX}/bin/setup_local.sh ${QA_MAIN} >& /dev/null
else
  echo no ${OPT_SPHENIX}/bin/setup_local.sh
  exit -1
fi

