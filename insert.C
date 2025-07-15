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
 *  OK on success
 *  an error code otherwise
 */

const Status QU_Insert(const string &relation,
                       const int attrCnt,
                       const attrInfo attrList[])
{
    Status status;                  // status var
    int numAttributesRel;           // var to keep track of num of attributes in relation
    AttrDesc *attributeDescriptors; // descriptors for all attributes of the relation
    int lenAttributes;              // total length of attributes

    status = attrCat->getRelInfo(relation, numAttributesRel, attributeDescriptors);
    if (status != OK)
    {
        return status;
    }
    if (numAttributesRel != attrCnt)
    {
        return OK;
    }

    // get attribute offset vals
    int attributeOffsetVals[attrCnt];
    lenAttributes = 0;
    for (int i = 0; i < attrCnt; i++)
    {
        attributeOffsetVals[i] = attributeDescriptors[i].attrOffset;
        lenAttributes += attributeDescriptors[i].attrLen;
    }
    // create InsertFileScan object on relation catalog table
    InsertFileScan insertRelation(relation, status);
    if (status != OK)
    {
        return status;
    }
    // create dummy record to store data
    Record dummyRecord;
    RID output_rec_id;

    // find length for dummy_record struct
    dummyRecord.length = lenAttributes;

    char dummyRecordData[lenAttributes];
    dummyRecord.data = (void *)dummyRecordData;

    // match attributes in relation catalog to attributes in the attribute catalog
    // The outer for loop will iterate through each the attributes in the relation catalog
    for (int relationCatalogIndex = 0; relationCatalogIndex < numAttributesRel; relationCatalogIndex++)
    {
        // the inner loop will iterate through each of the attributes in the record to be inserted
        for (int toBeInsertedCatalogIndex = 0; toBeInsertedCatalogIndex < attrCnt; toBeInsertedCatalogIndex++)
        {
            // check to see if the attributes are the same by checking if the names are the same
            if (strcmp(attributeDescriptors[relationCatalogIndex].attrName, attrList[toBeInsertedCatalogIndex].attrName) == 0)
            {

                int numBytesForMemcpy = attributeDescriptors[relationCatalogIndex].attrLen;
                char *destination_for_memcpy = (char *)dummyRecord.data + attributeOffsetVals[relationCatalogIndex];

                // if attribute is STRING
                if (attributeDescriptors[relationCatalogIndex].attrType == STRING)
                {
                    void *string_value_pointer = attrList[toBeInsertedCatalogIndex].attrValue;
                    memcpy(destination_for_memcpy, string_value_pointer, numBytesForMemcpy);
                }
                // if attribute is INTEGER
                if (attributeDescriptors[relationCatalogIndex].attrType == INTEGER)
                {
                    int int_value = atoi((char *)attrList[toBeInsertedCatalogIndex].attrValue);
                    memcpy(destination_for_memcpy, &int_value, numBytesForMemcpy);
                }
                // if attribute is FLOAT
                if (attributeDescriptors[relationCatalogIndex].attrType == FLOAT)
                {
                    float float_value = atof((char *)attrList[toBeInsertedCatalogIndex].attrValue);
                    memcpy(destination_for_memcpy, &float_value, numBytesForMemcpy);
                }
            }
        }
    }

    insertRelation.insertRecord(dummyRecord, output_rec_id);

    // part 6
    return OK;
}

