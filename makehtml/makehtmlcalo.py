#!/usr/bin/python3

import argparse
import glob
import os
import os.path
import subprocess
import pyodbc

parser = argparse.ArgumentParser(description="Create the HTML files with QA histograms files for all events in a run.")
parser.add_argument("-v","--verbose",help="add additional printing", action="store_true")
parser.add_argument("-t","--test",help="run a verbose test without actually creating the HTML", action="store_true")
args = parser.parse_args()
if args.test and not args.verbose:
    args.verbose = True
    
runtypes = ["_run3auau"]
subsys = { "calo" : ["HIST_CALO","CaloQA","draw_calo.C"], "globalQA" : ["HIST_CALO","GlobalQA","draw_GlobalQA.C"], "jets" : ["HIST_JETS","JetsQA","draw_jet.C"] }

qapath = os.environ.get("QA_HTMLDIR")+"/physics"


def get_aggregated_files(cursor, dsttype):
    query = "SELECT full_file_path FROM files WHERE lfn in (select filename from datasets files where dsttype='{}' and segment=9999)".format(dsttype)
    if args.verbose :
        print(query)
    cursor.execute(query)
    return {(row.full_file_path) for row in cursor.fetchall()}

def get_file(cursor, dsttype, runnumber):
    query = "SELECT full_file_path FROM files WHERE lfn in (select filename from datasets files where dsttype='{}' and segment=9999 and runnumber='{}')".format(dsttype,runnumber)
    if args.verbose:
        print(query)
    cursor.execute(query)
    return list({(row.full_file_path) for row in cursor.fetchall()})
  

def getBuildDbTag(type, filename):
    parts = filename.split("_")
    index = parts.index(type[1:])
    if args.verbose == True:
        print("db tag is " + parts[index+2])
    return parts[index+2]

def main():
    conn = pyodbc.connect("DSN=FileCatalog_read;UID=phnxrc;READONLY=True")
    cursor = conn.cursor()
    for runtype in runtypes:
        for s in subsys:
            full_paths = get_aggregated_files(cursor, subsys[s][0])

            subsysAggRuns = {}
            subsysAggRunsDbtag = {}
            for aggfile in full_paths:
                runnumber = int(aggfile.split("/")[-1].split("-")[1])
                dbtag = getBuildDbTag(runtype, aggfile.split("/")[-1])
                if runnumber in subsysAggRuns:
                    #only take the highest db tag, as that is what we end up plotting
                    if dbtag > subsysAggRunsDbtag[runnumber]:
                        subsysAggRuns[runnumber] = os.path.getmtime(aggfile)
                        subsysAggRunsDbtag[runnumber] = dbtag
                    else:
                        continue
                else:
                    subsysAggRuns[runnumber] = os.path.getmtime(aggfile)
                    subsysAggRunsDbtag[runnumber] = dbtag
            if args.verbose:
                print(subsys[s][0]+" all aggregated runs: ")
                print(subsysAggRuns)


        #check all current qa files for a subsystem in the QA_HTMLDIR  and get the latest modify time for each run
        #this asssums the path is in $QA_HTMLDIR/physics/(run range directory)/runnumber/QAfiles
        qaFilesModified = {}
        for d in next(os.walk(qapath))[1] :
            for rundir in next(os.walk(qapath+"/"+d))[1] :

                runnum = int(rundir.split("/")[-1])
                if runnum < 57000:
                    continue
                qafiles = glob.glob(qapath+"/"+d+"/"+rundir+"/"+subsys[s][1]+"*")
                maxmodtime = 0
                for f in qafiles :
                    modtime = os.path.getmtime(f)
                    if modtime > maxmodtime :
                        maxmodtime = modtime
                qaFilesModified[runnum] = maxmodtime
        if args.verbose :
            print(s + " modification time of all QA files:")
            print(qaFilesModified)

        #now run the HTML piece for all of those runs that don't exist or for those that have been modified
        updatedRuns = []
        for run in subsysAggRuns:
            if (not run in qaFilesModified) or (qaFilesModified[run] < subsysAggRuns[run]) :
                aggFile=get_file(cursor, subsys[s][0], run)
                if len(aggFile) == 0:
                    print("There is no aggregated histos file for run " + str(run))
                    print("Doing nothing.")
                    continue
                if len(aggFile) == 1 : 
                    macro = "/sphenix/u/sphnxpro/qahtml/QAhtml/subsystems/"+s+"/macros/"+subsys[s][2]+"(\""+aggFile[0]+"\")"
                    cmd = ["root.exe","-q",macro]
                    if args.verbose :
                        print(cmd)
                    if not args.test :
                        subprocess.run(cmd)
                        updatedRuns.append(run)
                else :
                    dbtagToDraw = "001"
                    fileToDraw = ""
                    # find the file with the most recent db tag
                    for file in aggFile:
                        # find the db string
                        filename = file.split("/")[-1]
                        dbtag = getBuildDbTag(runtype, filename)
                        if(int(dbtag.split("p")[1]) > int(dbtagToDraw)) :
                            fileToDraw = file
                            dbtagToDraw = dbtag
                    #Draw that one
                    macro = "/sphenix/u/sphnxpro/qahtml/QAhtml/subsystems/"+s+"/macros/"+subsys[s][2]+"(\""+fileToDraw+"\")"
                    cmd = ["root.exe","-q",macro]
                    if args.verbose :
                        print(cmd)
                    if not args.test :
                        subprocess.run(cmd)
                        updatedRuns.append(run)


                    print("There are multiple matching files.")
                    print(aggFile)
                    print("Drawing file with latest db tag: " + fileToDraw)
        if args.verbose :
            print("run numbers updated")
            print(updatedRuns)

if __name__ == "__main__":
    main()
