"""
Parse a JSON file and check for irregularities.
"""
import json


def Check_InUseInodes_Without_DirEntries(myjson):
    print "Checking inodes without directories..."
    missingDirs = []
    for i in range(len(myjson)):
        #if ("path" not in myjson[i]) or (len(myjson[i]["path"]) == "0"):
        #if (len(myjson[i]["path"]) == 0) and (myjson[i]["in_use"] == "1"):
        if myjson[i]["in_use"] == "0":
            missingDirs.append(myjson[i])
    #print(missingDirs)
    print "Number of in-use inodes without directories:", len(missingDirs), "== 1"
    for i in range(len(missingDirs)):
        print "-- inode", missingDirs[i]["inum"]
    return

def Check_DirEntries_ReferTo_UnusedInodes(myjson):
    print "Checking directory entries referring to un-used Inodes..."
    unUsedInodes = []
    for i in range(len(myjson)):
        if (myjson[i]["type"] == "1"): #if it's a dictionary
            inodes = myjson[i]["data"]
            for j in range(len(inodes)):
                if (inodes[j]["in_use"] == "0"):
                    unUsedInodes.append(myjson[i]["data"][j])
    #print(unUsedInodes)
    print "Number of in-use inodes without directories:", len(unUsedInodes), "== 0"
    for i in range(len(unUsedInodes)):
        print "-- inode", unUsedInodes[i]["inum"]
    return

def Check_SegmentSummaryInfo():
    return


def parseJsonFile(a,b):
    filename = "memory.json"
    f = open(filename, 'r')

    myjsonString = f.read()

    myjson = json.loads(myjsonString) #error

    Check_InUseInodes_Without_DirEntries(myjson)
    Check_DirEntries_ReferTo_UnusedInodes(myjson)

    return myjson

def main():
    a = 1
    b = 2
    parseJsonFile(a,b)
    return

if __name__ == '__main__':
    main()


