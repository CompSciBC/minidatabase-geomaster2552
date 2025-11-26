#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>   
#include <vector>     
#include "BST.h"      
#include "Record.h"
//add header files as needed

/* 
* Emmett Oliver
* Assignment 2
* CS300
* Fall 2025
*/

using namespace std;

// Converts a string to lowercase (used for case-insensitive searches)
static inline string toLower(string s) {
    for (char &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// ================== Index Engine ==================
// Acts like a small "database engine" that manages records and two BST indexes:
// 1) idIndex: maps student_id → record index (unique key)
// 2) lastIndex: maps lowercase(last_name) → list of record indices (non-unique key)
struct Engine {
    vector<Record> heap;                  // the main data store (simulates a heap file)
    BST<int, int> idIndex;                // index by student ID
    BST<string, vector<int>> lastIndex;   // index by last name (can have duplicates)

    // Inserts a new record and updates both indexes.
    // Returns the record ID (RID) in the heap.
    int insertRecord(const Record &recIn) {
        
        int index = heap.size();

        heap.push_back(recIn);

        idIndex.insert(recIn.id, index);

        string last = toLower(recIn.last);

        if(lastIndex.find(last) == nullptr){
            vector<int> v = {index}; // base case for lastIndex: no preexisting last name
            lastIndex.insert(last, v);
        }else{
            vector<int>* v = lastIndex.find(last); // preexisting last name, add new index
            v->push_back(index);
        }

        return index;
    }

    // Deletes a record logically (marks as deleted and updates indexes)
    // Returns true if deletion succeeded.
    bool deleteById(int id) {

        if(idIndex.find(id) != nullptr){

            int index = *idIndex.find(id);

            heap[index].deleted = true; // logically deletes record in heap
            // when retrieving from heap or lastIndex must check if index is flagged as deleted
            idIndex.erase(id); // erases from idIndex
 
            return true;
        }

        return false;
    }

    // Finds a record by student ID.
    // Returns a pointer to the record, or nullptr if not found.
    // Outputs the number of comparisons made in the search.
    const Record *findById(int id, int &cmpOut) {

        Record* out = nullptr; // base case: not found

        idIndex.resetMetrics();

        int* index = idIndex.find(id); // find index by id in idIndex

        if(index != nullptr && !heap[*index].deleted){
            out = &heap[*index]; // updates index to out if found
        }

        cmpOut = idIndex.comparisons;

        return out;
    }

    // Returns all records with ID in the range [lo, hi].
    // Also reports the number of key comparisons performed.
    vector<const Record *> rangeById(int lo, int hi, int &cmpOut) {

        idIndex.resetMetrics();

        vector<const Record *> out;

        idIndex.rangeApply(lo, hi, [&](const int &k, int &rid) {
            if (rid >= 0 && rid < (int)heap.size() && !heap[rid].deleted)
                out.push_back(&heap[rid]); // Lambda function pushes record to out when in the range applied
            }
        );

        cmpOut = idIndex.comparisons;

        return out;
    }

    // Returns all records whose last name begins with a given prefix.
    // Case-insensitive using lowercase comparison.
    vector<const Record *> prefixByLast(const string &prefix, int &cmpOut) {
        
        vector<const Record *> out;

        string pre = toLower(prefix); // last name must be logically greater than prefix
        string preGT = pre + "{"; // last name must be logically less than prefix + character greater than z

        lastIndex.resetMetrics();

        lastIndex.rangeApply(pre, preGT, [&](const string &k, vector<int> &v) {
                for(int i = 0; i < v.size(); i++){  // Lambda function iterates through indices
                    if(!heap[v[i]].deleted){        // checks if not deleted
                        out.push_back(&heap[v[i]]); // pushes to out 
                    }
                }
            }
        );

        cmpOut = lastIndex.comparisons;

        return out;
    }
};

#endif
