#!/usr/bin/env python

import os
import pyodbc
import subprocess
import glob
import time
import argparse
import hashlib

NROCS = [6,8,24]

hist_types = ["HIST_DST_STREAMING_EVENT_MVTX", "HIST_DST_STREAMING_EVENT_INTT","HIST_DST_STREAMING_EVENT_TPC"]

runtypes = ["_run3auau"]
aggDirectory = "/sphenix/data/data02/sphnxpro/QAhtml/aggregated/"
    

parser = argparse.ArgumentParser(description="Aggregate the QA histogram files produced for each DST segment of a run into a single QA histogram file per run.")
parser.add_argument("-v","--verbose",help="add additional printing", action="store_true")
parser.add_argument("-t","--test",help="run a verbose test without actually aggregating", action="store_true")
args = parser.parse_args()
if args.test and not args.verbose:
    args.verbose = True
print("Verbose is " + str(args.verbose))
print("Test is " + str(args.test))

def get_unique_run_dataset_pairs(cursor, type, runtype):
    dsttype = type + runtype
    query = "SELECT runnumber, dataset FROM datasets WHERE dsttype='{}' GROUP BY runnumber, dataset;".format(dsttype)
   
    cursor.execute(query)
    runnumbers = {(row.runnumber, row.dataset) for row in cursor.fetchall()}
    return runnumbers


def get_aggregated_file(cursor, dsttype, runnumber):
    query = "SELECT full_file_path FROM files WHERE lfn in (select filename from datasets files where dsttype='{}' and segment=9999 and runnumber='{}')".format(dsttype,runnumber)
    #if args.verbose:
        #print(query)
    cursor.execute(query)
    returnfile = ""
    # there is only ever one return for this query
    for row in cursor.fetchall():
        returnfile = row[0]
    return returnfile
  


def main():
    FCRead = pyodbc.connect("DSN=FileCatalog_read;UID=phnxrc;READONLY=True")
    FCReadCursor = FCRead.cursor()
    FCWrite = pyodbc.connect("DSN=FileCatalog;UID=phnxrc")
    FCWriteCursor = FCWrite.cursor()
    for runtype in runtypes:
        for subsystemID in range(3):
            nrocs = NROCS[subsystemID]
            hist = hist_types[subsystemID]
            # just use the 0th one to get the run/db range once per subsystem instead of for each ROC
            dummyhisttype = hist+"0"
            if hist.find("TPC") != -1:
                dummyhisttype = hist+"00"
            for run, dbtag in get_unique_run_dataset_pairs(FCReadCursor, dummyhisttype, runtype): 
                filesToAdd = []
                for ROC in range(nrocs):
                    histtype = hist+str(ROC)
                    if hist.find("TPC") != -1:
                        histtype = hist+"{:02d}".format(ROC)
                    thisfile = get_aggregated_file(FCReadCursor, histtype, run)
                    if len(thisfile) > 0:
                        filesToAdd.append(thisfile)
                if args.verbose :
                    print(filesToAdd)
                if len(filesToAdd) == 0:
                    # nothing to add, move on
                    continue
                print(filesToAdd[0])
                tags = (filesToAdd[0]).split(os.sep)
                index = tags.index(runtype[1:])
                collisiontag = tags[index]
                beamtag = tags[index+1]
                anadbtag = dbtag
                dsttypetag = hist
                rundirtag = tags[index+4]
                # make an analogous path to the production DST in sphenix/data
                completeAggDir = aggDirectory + collisiontag + "/" + beamtag + "/" + anadbtag + "/" + dsttypetag + "/" + rundirtag + "/"
                if args.verbose == True:
                    print("aggregated directory")
                    print(completeAggDir)
                
                #check for a similar file in this dir
                filepathWildcard = completeAggDir + hist + "*" + dbtag + "*" + str(run) + "*"
                if not os.path.isdir(completeAggDir):
                    if args.verbose == True:
                        print("making a new aggregated dir")
                    if not args.test == True:
                        os.makedirs(completeAggDir, exist_ok=True)

                filepath = glob.glob(filepathWildcard)
                
                path = ""
                aggFileTime = 0
                if len(filepath) > 0:
                    path = filepath[0]
                    aggFileTime = os.path.getmtime(path)
                
                reagg=False
                if len(path) == 0:
                    reagg=True
                newFileTime = 0
                for rocpath in filesToAdd:
                    if os.path.getmtime(rocpath) > newFileTime:
                        newFileTime = os.path.getmtime(rocpath)
                    if os.path.getmtime(rocpath) > aggFileTime:
                        reagg = True
                        break
                    
                if args.verbose == True:
                    print("Agg file " + path + "  time is " + str(aggFileTime))
                    print("latest new file time is " + str(newFileTime))
                
                if reagg == False:
                    continue

                lfn = hist + runtype + "_" + dbtag + "-{:08d}-9999.root".format(run)
                
                path = completeAggDir + lfn
                if args.verbose == True:
                    print ("lfn is " + lfn)
                    print("agg file path is " + path)
                command = ["hadd","-ff",path]
            
                for rocpath in filesToAdd:
                    command.append(str(rocpath))
                if args.verbose:
                    print("executing command for "+str(len(filesToAdd)) + " files")
                    print(command)

                if not args.test:
                    subprocess.run(command)
                    
                    #insert into filecatalog
                    size= int( os.path.getsize(path) )
                    file_hash=None
                    with open( path, "rb") as f:
                        file_hash = hashlib.md5()
                        chunk = f.read(8192)
                        while chunk:
                            file_hash.update(chunk)
                            chunk = f.read(8192)
                    md5 = file_hash.hexdigest()
                    
                    insertquery="""
                    insert into files (lfn,full_host_name,full_file_path,time,size,md5) 
                    values ('{}','gpfs','{}','now',{},'{}')
                    on conflict
                    on constraint files_pkey
                    do update set 
                    time=EXCLUDED.time,
                    size=EXCLUDED.size,
                    md5=EXCLUDED.md5
                    ;
                    """.format(lfn,path,size,md5)
                    if args.verbose :
                        print(insertquery)
                    FCWritecursor.execute(insertquery)
                    FCWritecursor.commit()

                    insertquery="""
                    insert into datasets (filename,runnumber,segment,size,dataset,dsttype)
                    values ('{}','{}',9999,'{}','{}','{}')
                    on conflict
                    on constraint datasets_pkey
                    do update set
                    runnumber=EXCLUDED.runnumber,
                    segment=EXCLUDED.segment,
                    size=EXCLUDED.size,
                    dsttype=EXCLUDED.dsttype,
                    events=EXCLUDED.events
                    ;
                    """.format(lfn,run,size,dbtag,hist)
                    if args.verbose :
                        print(insertquery)
                    FCWritecursor.execute(insertquery)
                    
                    FCWritecursor.commit()

                    

    FCWrite.close()
    FCRead.close()
    
if __name__ == "__main__":
    main()

    
