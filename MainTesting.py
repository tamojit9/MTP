import re
import os

def chooseRelations(rf, labelsMap, sp = '	') :
	relations = []
	rff = open(rf)
	c = 0
	for line in rff :
		if(c > 10) :
			break
		[le, rs, re] = line.split(sp)
		le = changeMID(le).strip('\n').strip(' ')
		re = changeMID(re).strip('\n').strip(' ')
		if(le not in labelsMap or re not in labelsMap) : continue
		relations.append(line)
		c += 1
	return relations

def findOnlyExistingMIDS(fileName, labelsMap) :
	readFileId = open(fileName)
	print len(labelsMap.keys())
	mids = {}
	for line in readFileId :
		line = line.strip('\n')
		if(re.search('/\w/', line) != None) :
			midPos = line.find('/')
			mid = changeMID(line[midPos:])
			if(mid in labelsMap and mid not in mids) :
				mids[mid] = labelsMap[mid]
	print len(mids.keys())
	return mids

def extractRelations(relation) :
	return relation.split('/')

def parseAndExtractRelations(relations, relationMap, entityMap, sp = '	') :
	for triplet in relations :
		[le, rs, re] = triplet.split(sp)
		le = changeMID(le)
		re = changeMID(re)
		for r in extractRelations(rs) :
			relationMap[r] = (le, re)
			entityMap[le] = r
			entityMap[re] = r
	return (relationMap, entityMap)

def createLabelsMap(lf, labelsMap) :
	lff = open(lf)
	for line in lff :
		line = line.strip('@en')
		for labels in line.split('@en') :
			parts = labels.split(' "')
			if(len(parts) < 2) : continue
			labelsMap[parts[0].strip('\n').strip(' ')] = parts[1].strip('"').strip(' ')
	return labelsMap

def changeMID(e) :
	e = e.split('/')
	e = e[1] + "." + e[2]
	return e

def writeTheEntities(entityMap, ef) :
	eff = open(ef, 'w')
	for e in entityMap.keys() :
		if('\n' not in e) :
			eff.write(e + '\n')
		else :
			eff.write(e)
	eff.close()
def printTheLabels(entityMap, labelsMap) :
	for e in entityMap.keys() :
		print e,
		print labelsMap[e.strip('\n')]
def filterCorpus(cf) :
	l = ""
	lines = []
	for line in open(cf) :
		if('.' in line) :
			parts = line.split('.')
			lines.append(l+parts[0])
			if(len(parts) > 1) :
				l = parts[1]	
		else :
			l += line
	return lines
def write(lines) :
	wt = open('finalCorpus', 'w')
	[wt.write(line) for line in lines]
	wt.close()
def main() :
	relations = []
	relationMap = {}
	entityMap = {}
	labelsMap = {}
	labelsMap = createLabelsMap('Final_FB15K_LABELS.txt', labelsMap)
	labelsMap = findOnlyExistingMIDS("nyt-train_COPY", labelsMap)
	relations = chooseRelations('freebase_mtr100_mte100-train.txt', labelsMap)
	relationMap, entityMap = parseAndExtractRelations(relations, relationMap, entityMap)
	writeTheEntities(entityMap, 'final_mids_cleaned.txt')
	printTheLabels(entityMap , labelsMap)
	import removeSlash
	removeSlash.main()
	print "finally filtering the corpus"
	lines = filterCorpus('corpuswop')
	print "writting the final corpus"
	write(lines)
if __name__ == '__main__' :
	main()
