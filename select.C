///////////////////////////////////////////////////////////////////////////////
// Author: Will Hagedorn - 9084091942
// Author: Aman Jain - 9084372169
// Author: Ainesh Mohan - 9084256412
// ----------------------------------------------------------------------------
// Description: Query Operators - select for a database system.
///////////////////////////////////////////////////////////////////////////////


#include "catalog.h"
#include "query.h"


// forward declaration
const Status ScanSelect(const string & result,
			const int projCnt,
			const AttrDesc projNames[],
			const AttrDesc *attrDesc,
			const Operator op,
			const char *filter,
			const int reclen);

/*
//  * Selects records from the specified relation.
//  *
//  * Returns:
//  * 	OK on success
//  * 	an error code otherwise
//  */

const Status QU_Select(const string & result,
        const int projCnt,
        const attrInfo projNames[],
        const attrInfo *attr,
        const Operator op,
        const char *attrValue)
{
    Status status;
    AttrDesc attrDesc;
    AttrDesc* attrDescArr;
    int reclen;
    Operator myOp;
    const char* filter;

    reclen = 0;
    // Array of attrDesc to hold projection descriptions
    attrDescArr = new AttrDesc[projCnt];
    // Get total length of the proj rec
    for (int i = 0; i < projCnt; i++) {
        Status status = attrCat->getInfo(projNames[i].relName, projNames[i].attrName, attrDescArr[i]);

        reclen += attrDescArr[i].attrLen;
        if (status != OK) { 
			return status; 
		}
    }

    if (attr != NULL) {
        status = attrCat->getInfo(string(attr->relName), string(attr->attrName), attrDesc);

        int tmpInt;
        float tmpFloat;
        // Convert data type
        if (attr->attrType == INTEGER) { 
			tmpInt = atoi(attrValue);
            filter = (char*)&tmpInt;
		} else if (attr->attrType == FLOAT) { 
			tmpFloat = atof(attrValue);
            filter = (char*)&tmpFloat;
		} else if (attr->attrType == STRING) { 
            filter = attrValue;
        }
        myOp = op;
    } else {
        // Use defaults for no cond
        strcpy(attrDesc.relName, projNames[0].relName);
        strcpy(attrDesc.attrName, projNames[0].attrName);

        attrDesc.attrOffset = 0;
        attrDesc.attrLen = 0;

        attrDesc.attrType = STRING;
        filter = NULL;
        myOp = EQ;
    }
    // Call ScanSelect to build the table
    status = ScanSelect(result, projCnt, attrDescArr, &attrDesc, myOp, filter, reclen);
    if (status != OK) { 
		return status; 
	}

    return OK;
}


const Status ScanSelect(const string & result,
        const int projCnt,
        const AttrDesc projNames[],
        const AttrDesc *attrDesc,
        const Operator op,
        const char *filter,
        const int reclen)
{
    Status status;
    int resCnt = 0;

    // Open the result table
    InsertFileScan scan(result, status);
    if (status != OK) { 
		return status; 
	}

    // Init ptr location of size reclen
    char outputData[reclen];
    Record outputRec; // Record to be copied to later
    outputRec.data = (void *) outputData;
    outputRec.length = reclen;

    // Start scan
    HeapFileScan rel(attrDesc->relName, status);
    if (status != OK) { 
		return status; 
	}

    status = rel.startScan(attrDesc->attrOffset, attrDesc->attrLen, (Datatype) attrDesc->attrType, filter, op);
    if (status != OK) { 
		return status; 
	}

    // Scan through table
    RID rid;
    Record rec;
    while (rel.scanNext(rid) == OK) {
        status = rel.getRecord(rec);
        if (status != OK) { 
			return status; 
		}

        // Match found, copy data into the output record
        int outputOffset = 0;
        for (int i = 0; i < projCnt; i++) {
            memcpy(outputData + outputOffset, (char *)rec.data + projNames[i].attrOffset, projNames[i].attrLen);
            outputOffset += projNames[i].attrLen;
		}

        // Add new rec to the output relation
        RID outRID;
        status = scan.insertRecord(outputRec, outRID);
        if (status != OK) { 
			return status; 
		}
        resCnt++;
    }
    
	// Made it this far
    return OK;
}
