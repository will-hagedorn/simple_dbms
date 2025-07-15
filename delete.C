///////////////////////////////////////////////////////////////////////////////
// Author: Will Hagedorn - 9084091942
// Author: Aman Jain - 9084372169
// Author: Ainesh Mohan - 9084256412
// ----------------------------------------------------------------------------
// Description: Query Operators - delete for a database system.
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

const Status QU_Delete(const string &relation,
					   const string &attrName,
					   const Operator op,
					   const Datatype type,
					   const char *attrValue)
{

	Status status;

	HeapFileScan heapfs = HeapFileScan(relation, status);

	if (status != OK)
	{

		return status;
	}

	// Check for unconditional search
	if (attrName == "")
	{

		heapfs.startScan(0, 0, type, NULL, op);
	}
	// If there is an attribute name, we go to else
	else
	{

		// casting attrValue to a char* to use as a filter

		char *filter;
		int intVal;
		float floatVal;
		if (type == STRING)
		{
			filter = (char *)attrValue;
		}
		else if (type == INTEGER)
		{
			intVal = atoi(attrValue);
			filter = (char *)&intVal;
		}
		else if (type == FLOAT)
		{
			floatVal = atof(attrValue);
			filter = (char *)&floatVal;
		}

		// getting the attribute description from catalog
		AttrDesc attrDesc;

		status = attrCat->getInfo(relation, attrName, attrDesc);

		// Check if status is OK
		if (status != OK)
		{
			return status;
		}

		// scanning based on the attribute description we got before
		heapfs.startScan(attrDesc.attrOffset, attrDesc.attrLen, type, filter, op);
	}

	RID rid;

	while ((status = heapfs.scanNext(rid)) == OK)
	{

		heapfs.deleteRecord();
	}

	return OK;
}