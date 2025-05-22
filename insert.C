///////////////////////////////////////////////////////////////////////////////
// Author: Will Hagedorn - 9084091942
// Author: Aman Jain - 9084372169
// Author: Ainesh Mohan - 9084256412
// ----------------------------------------------------------------------------
// Description: Query Operators - insert for a database system.
///////////////////////////////////////////////////////////////////////////////

#include "catalog.h"
#include "query.h"


/*
 * Inserts a record into the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation, 
	const int attrCnt, 
	const attrInfo attrList[])
{

	Status status;
	int resCnt = 0;
	AttrDesc *attrDesc;

	status = attrCat->getRelInfo(relation, resCnt, attrDesc);
	if (status != OK) {
		return OK;
	}

	// Open table to insert into
    InsertFileScan resultRel(relation, status);
    if (status != OK) { 
		return status; 
	}

	// Init recLen
	int recLen = 0;
	for (int i = 0; i < resCnt; i++) {
		recLen += attrDesc[i].attrLen;
	}

	char outputData[recLen];
    Record rec;
    rec.data = (void *) outputData;
    rec.length = recLen;

	//int outputOffset = 0;
	// Iterate through both lists of attrs and compare
	for (int i = 0; i < resCnt; i++) {
		for (int j = 0; j < attrCnt; j++) {
			// Check for match
			if (strcmp(attrList[j].attrName, attrDesc[i].attrName) == 0) {
				// Check type of attr
				void *filter;
				Datatype type;
				if (attrList[j].attrType == STRING) {
					type = STRING;
					filter = (void*)(attrList[j].attrValue);
				} else if (attrList[j].attrType == INTEGER) {
					type = INTEGER;
					int temp = atoi((char*)attrList[j].attrValue);
					filter = &temp;
				} else if (attrList[j].attrType == FLOAT) {
					type = FLOAT;
					float temp = atof((char*)attrList[j].attrValue);
					filter = &temp;
				}

				memcpy((char *)rec.data + attrDesc[i].attrOffset, filter, attrDesc[i].attrLen);
				break;
			}
		}
	}

	// Perform insert
	RID rid;
	status = resultRel.insertRecord(rec, rid);
	if (status != OK) {
		return status;
	}

	// If made it this far
	return OK;
}

