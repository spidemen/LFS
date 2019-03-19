"""
Parse a JSON file and check for irregularities.
"""
import json
"""{
    ".ifile": {
        "inum": "0",
        "logAddress": {
            "blockNo": "0",
            "segmentNo": "0"
        }
    },
    "firstfile": {
        "inum": "1",
        "in_use": "1"
    },
    "secondfile": {
        "inum": "1",
        "in_use": "1"
    }
}"""


def Check_DirEntries_ReferTo_UnusedInodes():
	"""Iterate through the JSON and find the directories
	If the directory has entries, check those inodes"""
	
	return

def Check_InUseInodes_Without_DirEntries():

	return




def parseJsonFile(a,b):
	filename = "memory.json"
	f = open(filename, 'r')

	myjsonString = f.read()

	#myjson = json.loads(myjsonString) #error
	
	Check_InUseInodes_Without_DirEntries()
	Check_DirEntries_ReferTo_UnusedInodes()

	return myjson



