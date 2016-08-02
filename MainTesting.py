def chooseRelations(rf) :
	relations = []
	rff = open(rf)
	c = 0
	for line in rff :
		if(c > 10) :
			break
		relations.append(line)
		c += 1
	return relations
def parseAndExtractRelations(relations, relationMap, entityMap, sp = '	') :
	for triplet in relations :
		[le, r, re] = triplet.split(sp)
		relationMap[r] = (le, re)
		entityMap[le] = r
		entityMap[re] = r
	return (relationMap, entityMap)
def createLabelsMap(lf, labelsMap) :
	lff = open(lf)
	for line in lff :
		for labels in line.split('@en') :
			parts = labels.split(' "')
			if(len(parts) < 2) : continue
			labelsMap[parts[0]] = parts[1].rstrip('"')
	return labelsMap
def writeTheEntities(entitityMap, ef) :
	eff = open(ef, 'w')
	for e in entityMap.keys() :
		e = e.split('/')
		e = e[1] + "." + e[2]
		if('\n' not in e) :
			eff.write(e + '\n')
		else :
			eff.write(e)
	eff.close()
if __name__ == '__main__' :
	relations = []
	relationMap = {}
	entityMap = {}
	labelsMap = {}
	relations = chooseRelations('freebase_mtr100_mte100-train.txt')
	relationMap, entityMap = parseAndExtractRelations(relations, relationMap, entityMap)
	labelsMap = createLabelsMap('Final_FB15K_LABELS.txt', labelsMap)
	writeTheEntities(entityMap, 'final_mids_cleaned.txt')
	#print relationMap['/location/country/form_of_government']