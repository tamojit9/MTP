import re
import os

mid_fileID = {"" : []}
locationMap = {"":""}
to = open("corpus", 'w')

def makeMIDDictionary(fileName) :
	global mid_fileID
	readMIDs = open(fileName)
	for mid in readMIDs :
		mid_fileID[mid[mid.find('.')+1:].strip()] = []

def fillDictionary(fileName) :
	global mid_fileID
	readFileId = open(fileName)
	remDup = {"":True}
	fileId = ""
	for line in readFileId :
		line = line.strip('\n')
		if(re.search('/\w/', line) != None) :
			midPos = line.rfind('/')
			mid = line[midPos+1:]
			if(mid in mid_fileID and fileId not in remDup) :
				remDup[fileId] = True
				mid_fileID[mid].append(fileId)
		elif('\t' in line) :
			fileId = line[:line.find('\t')]

def mapLocations(ffrom) :
	global locationMap
	for line in open(ffrom) :
		pos = line.rfind('/')
		xmlFile = line[pos+1:]
		if(len(xmlFile) > 0) :
			locationMap[xmlFile.split(".")[0]] = "./"+line.strip('\n')

def extractText(file) :
	global text
	start = False
	for line in open(file) :
		if('<p>' in line) :
			start = True
		if(start) :
			to.write(line)
		if('</p>' in line) :
			start = False

def extractTextFromAllFiles() :
	print "loading ",
	for key in mid_fileID.keys() :
		for file in mid_fileID[key] :
			extractText(locationMap[file])
		print "#",
	print
def removeTag(tag, fromF) :
	ff = open(fromF)
	tf = open(fromF+"wo"+tag, 'w')
	ot = "<" + tag + ">"
	ct = "</" + tag + ">"
	for line in ff :
		if(ot in line) :
			tf.write(line[3:]+"\n")
		elif(ct in line) :
			tf.write(line[:line.find(ct)]+"\n")
	tf.close()
if __name__ == '__main__' :
	fileLocation = "corpus"
	if(os.path.isfile(fileLocation)) :
		makeMIDDictionary("final_mids_cleaned.txt")
		print "MID mapping Done"
		print "number of MIDS = " + str(len(mid_fileID))                                                             
		fillDictionary("nyt-train_COPY")
		print("found the file ids corresponding to the previous MIDS")
		mapLocations('file.tbl')
		print "Mapping of locations done"
		print "extracting text"
		extractTextFromAllFiles()
		to.close()
		print "done extraction"
	else :
		removTag('p', fileLocation)
	
