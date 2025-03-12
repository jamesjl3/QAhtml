#!/bin/bash

source  /opt/sphenix/core/bin/sphenix_setup.sh -n new
export QA_MAIN=/sphenix/u/sphnxpro/qahtml/install
export QA_HTMLDIR=/sphenix/WWW/subsystem/QAHtmlTest
#export QA_REAL_HTML=1
source setup_qahtml.sh
