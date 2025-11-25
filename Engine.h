#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>   
#include <vector>     
#include "BST.h"      
#include "Record.h"
//add header files as needed

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

        if(lastIndex.find(recIn.last) == nullptr){
            vector<int> v = {index};
            lastIndex.insert(recIn.last, v);
        }else{
            vector<int>* v = lastIndex.find(recIn.last);
            v->push_back(index);
        }

        return index;
    }

    // Deletes a record logically (marks as deleted and updates indexes)
    // Returns true if deletion succeeded.
    bool deleteById(int id) {

        if(idIndex.find(id) != nullptr){

            int index = *idIndex.find(id);

            idIndex.erase(id);

            heap[index].deleted = true;

            return true;
        }

        return false;


        //     vector<int>* v = lastIndex.find(heap[index].last);

        //     auto it = std::find(v->begin(), v->end(), index); //Used AI to help with iterator implimentation
        //     if (it != v->end()) {
        //         v->erase(it);
        //     }

        //     return true;
        // }
    }

    // Finds a record by student ID.
    // Returns a pointer to the record, or nullptr if not found.
    // Outputs the number of comparisons made in the search.
    const Record *findById(int id, int &cmpOut) {

        Record* out = nullptr;

        idIndex.resetMetrics();

        if(idIndex.find(id) != nullptr){
            idIndex.resetMetrics();

            int index = *idIndex.find(id);

            out = &heap[index];
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
                out.push_back(&heap[rid]);
            }
        );

        cmpOut = idIndex.comparisons;

        return out;
    }

    // Returns all records whose last name begins with a given prefix.
    // Case-insensitive using lowercase comparison.
    vector<const Record *> prefixByLast(const string &prefix, int &cmpOut) {
        //TODO
        vector<const Record *> out;

        int *ptr = &cmpOut;

        for(int i = 0; i < heap.size(); i++){
            if(prefixMatch(heap[i].last, prefix, *ptr) && !heap[i].deleted){
                out.push_back(&heap[i]);
            }
        }

        // lastIndex.find(.compare(0, prefix.size(), prefix) == 0);

        return out;
    }

    //Used Git AI to help create helper function
    private: bool prefixMatch(const string &last, const string prefix, int &cmpOut) {
        if (last.size() < prefix.size()) return false;
        for (size_t i = 0; i < prefix.size(); ++i) {
            cmpOut++;
            if (tolower(last[i]) != tolower(prefix[i])) return false;
        }
        return true;
    }
};

#endif
