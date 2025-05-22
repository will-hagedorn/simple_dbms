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
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Delete(const string & relation, 
		       const string & attrName, 
		       const Operator op,
		       const Datatype type, 
		       const char *attrValue)
{

	Status status;
	AttrDesc attrDesc;

	// Open the table
    HeapFileScan rel(relation, status);
    if (status != OK) { 
		return status; 
	}

	// Check if WHERE condition given
	if (attrName.length() != 0) {
		// Get attr info
		status = attrCat->getInfo(relation, attrName, attrDesc);
		if (status != OK) {
			return status;
		}

		int tmpInt;
		float tmpFloat;
		// Check attr type
		const char *filter;
		if (type == STRING) {
			filter = attrValue;
		} else if (type == INTEGER) {
			tmpInt = atoi(attrValue);
			filter = (char*)&tmpInt;
		} else if (type == FLOAT) {
			tmpFloat = atof(attrValue);
			filter = (char*)&tmpFloat;
		}

		// Start scan
		status = rel.startScan(attrDesc.attrOffset, attrDesc.attrLen, type, filter, op);
		if (status != OK) {
			return status;
		}
	} else { // No condition given
		Datatype type = STRING;
		// Start scan
		status = rel.startScan(0, 0, type, NULL, EQ);
		if (status != OK) {
			return status;
		}
	}

	// Scan for tuples to delete
	RID rid;
	while(rel.scanNext(rid) == OK) {
		// Found match so delete
		status = rel.deleteRecord();
		if (status != OK) {
			return status;
		}
	}

	// If made it this far
	return OK;
}