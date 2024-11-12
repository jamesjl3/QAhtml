#!/usr/bin/env python

import os
import pyodbc
import subprocess
import glob
import time
import argparse

track_hist_types = ["HIST_CALOQA", "HIST_JETS"]
runtypes = ["_run2pp","_run2auau"]


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
    query = "SELECT runnumber, dataset FROM datasets WHERE dsttype='{}'  GROUP BY runnumber,dataset;".format(dsttype)
    cursor.execute(query)
    runnumbers = {(row.runnumber, row.dataset) for row in cursor.fetchall()}
    
    return runnumbers

def getPaths(cursor, run, dataset, type, runtype):
    dsttype = type + runtype
    query = "SELECT files.full_file_path FROM files,datasets WHERE datasets.runnumber={} AND datasets.dataset='{}' AND datasets.dsttype='{}' AND files.lfn=datasets.filename".format(run,dataset,dsttype)
    cursor.execute(query)
    filepaths = {(row.full_file_path) for row in cursor.fetchall()}
    return filepaths

def getDbTag(filename):
     return filename[filename.find("2024p")+5:filename.find("2024p")+8]

def main():   
    import time
    conn = pyodbc.connect("DSN=FileCatalog_read;UID=phnxrc;READONLY=True")
    cursor = conn.cursor()
    aggDirectory = "/sphenix/data/data02/sphnxpro/QAhtml/aggregated/"
    for runtype in runtypes:
        for histtype in track_hist_types:
            runs_dbtags = get_unique_run_dataset_pairs(cursor, histtype, runtype)

            for run, dbtag in runs_dbtags:

                filepaths = getPaths(cursor, run, dbtag, histtype, runtype)
                filepathWildcard = aggDirectory + histtype + "*" + dbtag + "*" + str(run) + "*"

                filepath = glob.glob(filepathWildcard)
                path = ""
                aggFileTime = 0
                if len(filepath) > 0:
                    path = filepath[0]
                    aggFileTime = os.path.getmtime(path)

                #need to figure out the latest db tag
                latestdbtag= ""
                ana = ""
                latestdbtagInt=0
                for newpath in filepaths:
                    thistag = getDbTag(newpath)
                    ana = newpath[newpath.find("ana")+3:newpath.find("ana")+6]
                    if int(thistag) > latestdbtagInt:
                        latestdbtag=thistag
                        latestdbtagInt = int(thistag)

                reagg=False
                if len(path) == 0:
                    reagg=True
                newFileTime = 0
                if reagg == False:
                    for newpath in filepaths:
                        if newpath.find(latestdbtag) == -1:
                            continue
                        if os.path.getmtime(newpath) > newFileTime:
                            newFileTime = os.path.getmtime(newpath)
                        if os.path.getmtime(newpath) > aggFileTime:
                            reagg = True
                            break
                if args.verbose == True:
                    print("Agg file " + path + "  time is " + str(aggFileTime))
                    print("latest new file time is " + str(newFileTime))
                if reagg == False:
                    continue
                filestoadd = []
                nfiles = 0
                if len(path) == 0:
                    path = (aggDirectory + histtype + runtype + "_" + dbtag + "-{:08d}-9000.root").format(run)

                if args.verbose == True:
                    print("agg file path is " + path)
                command = ["hadd","-ff",path]
                for newpath in filepaths:
                    # make sure the file has the same db tag
                    if newpath.find(latestdbtag) == -1: 
                       continue
                    command.append(str(newpath))
                    nfiles+=1

                # wait for at least 10 files
                if nfiles < 10:
                    continue
                if args.verbose:
                    print("executing command")
                    print(command)
                if not args.test:
                    subprocess.run(command)

    conn.close()
if __name__ == "__main__":
    main()
