## CS8.401 Software Programming for Performance

# In-memory Key-Value Storage Software in C++

## Setup and Running : 

* Execute the following commands :

```bash
    git clone https://github.com/mallika2011/In-Memory-Key-Value-Storage.git
    cd In-Memory-Key-Value-Storage
    cd codes
    g++ benchmark_correct.cpp
```

* Directory structure :
  * codes
    * benchmark_correct.cpp
    * kvStore.cpp

* **kvStore.cpp** contains the code and implementation of the data structures and all the functionality of each of the following 5 API calls that this key-value pair database supports:
  
  * get(key,value)     : Gets the value corresponding to the particular key
  * put(key,value)     : Insert/ overwrites the key-value pair
  * del(key)           : Removes the key-value pair corresponding to that key from the databse
  * get(N, key, value) : Gets the Nth key-value pair (all data is lexicographically stored with respect to the keys)
  * del(N)             : Removes the Nthe key-value pair from the database

* **benchmark_correct.cpp** is a standard benchmark written to test the functionality of the kvStore.
  * It first makes 100000 put() calls to the database
  * Next a total of 1000000 random APIs are called and tested simultaneously.
  * Once this is done, multithreading with the API calls is also tested. 
  * Correspondingly, outputs are genrated if the funcitonality is correct or incorrect.

## Key-Value Store: An Introduction

A key-value store is a simple database that uses an associative array as the fundamental data model where
each key is associated with one and only one value in a collection.

They provide a way to store, retrieve and update data using simple get, put and delete commands; the path to
retrieve data is a direct request to the object in memory. The simplicity of this model makes a key-value store
fast, easy to use, scalable, portable and flexible.

## Analysis Of Data Structure

### B Trees

B trees are not very efficient, when compared to other balanced trees, when they reside in RAM. Inserting or
deleting elements involve moving many keys/values around. A B-tree has a chance to be efficient in RAM, if it
has a very low branching factor, so that a node fits a cache line. This may lead to cache miss
minimization.Leaf and non-leaf nodes are of different size, this complicates storage.

### Hash table

Hash tables become quite inefficient when there are many collisions. While extremely uneven hash
distributions are extremely unlikely to arise by chance, a malicious adversary with knowledge of the hash
function may be able to supply information to a hash that creates worst-case behavior by causing excessive
collisions, resulting in very poor performance. In critical applications, universal hashing can be used; a data
structure with better worst-case guarantees may be preferable.

Operations on a hash table take constant time on average.Thus hash tables are not effective when the
number of entries is very small. It is slow due to synchronization.

### Hash tables


## Primary Data Structure: Trie

A trie is a multiway tree structure that stores sets of strings by successively partitioning them. Tries have two
properties that cannot be easily imposed on data structures based on binary search.

First, strings are clustered by shared prefix. Second, there is a great reduction in the number of string
comparisons—an important characteristic, as a major obstacle to efficiency is excessive numbers of string
comparisons. Tries can be rapidly traversed and offer good worst-case performance, without the overhead of
balancing.

## Radix Trie

In computer science, a radix trie is a data structure that represents a space-optimized trie in which each node
that is the only child is merged with its parent. This is done in order to save space.

In a trie, each node contains a character that is a part of the word. A radix trie tactfully breaks strings into
common and uncommon strings and stores these substrings in the trie nodes.

In a large data set, after inserting the strings one can find common prefixes of length 4 to 10 characters in
general, even if take the worst cade of 10 common character, the remainder of the string has a max length of
54 characters. Ordinarily in a regular trie we would require 54 nodes to store this. In a compressed trie this
string can be accommodated in a single node thus saving a large amount of space.

The result is that the number of children of every internal node is at most the radix r of the radix trie, where r
is a positive integer and a power x of 2, having x ≥ 1.

In our key-value systems, the radix or base is the number of unique characters used to generate a key. Since
a key can be made of both uppercase and lower case characters we can have a maximum of 26 + 26

children emerging from a parent node. However to ensure that each character is correctly mapped to its
ASCII value we allocate a maximum space of 58 bytes for each trie node.

Unlike regular tries, edges can be labeled with sequences of elements as well as single elements. This
makes radix tries much more efficient for large sets and for sets of strings that share long prefixes.

## Common Operations in Compressed Trie

Compressed Trie supports the following main operations, all of which are O(k), where k is the maximum
length of all strings in the set:

### Lookup:

Determines if a string is in the set. This operation is identical to tries except that some edges consume
multiple characters.

### Insert:

Add a string to the tree. We search the tree until we can make no further progress. At this point there are five
possible cases,

>Case 1 

Key to be inserted doesn't exist and had no common prefix in the database:
In this case the key is simply inserted via a pointer from the root node.

>Case 2

Key to be inserted already exits.

Here the trie traversed to reach the last node of the key and the value is overwritten. While traversing the trie
for this key the descendants are incremented due to a condition in the search function.

To correct this appropriate node descendant values are decremented.

>Case 3

The key to be inserted is a super string of a node present in the database

Here, the common prefix is left untouched and the remaining part is attached is node to this common prefix.

>Case 4

The key to be inserted is a substring of a node in the database.

Here, the existing is broken into the common prefix and the remaining part which is attached as child to the
common prefix node.

The descendants of the new and existing nodes are updated appropriately.

>Case 5

The key to be inserted has a common prefix with an existing node in the trie.

Here, the uncommon part of the existing node is broken into a new node and the key is inserted as a whole.
The uncommon part of the key is then broken into a new node and then attached to the common prefix. Now
the existing uncommon node is also attached to the common prefix node.

All splitting steps ensure that no node has more children than there are possible string characters.

### Delete:

Delete a string from the trie. First, we traverse the trie until we find the key to be deleted. We then recursively
backtrack the key path and remove the necessary nodes wherever possible. This is done in O(k) where k is
the length of the key.

## Lexicographical Storage

Thr array present in each node is of size 58 where the index corresponds to the alphabets in ASCII notation.
This takes care of the sorting of keys.

## Handling Multithreading

In order to handle multithreading, pthread_mutex_locks are used. Whenever a particular funcitonality is being executed, the other two are prevented from accessing the database. For eg: if the put() API is called, then the get() and del() APIs are prevented from meddling with the database. This is achieved by having a total of 4 locks.

```c++
  pthread_mutex_init(&del_lock, NULL);
  pthread_mutex_init(&put_lock, NULL);
  pthread_mutex_init(&get_lock, NULL);
  pthread_mutex_init(&db_lock, NULL);
```

The del_lock, put_lock, get_lock are for the respective API calls, while the db_lock is used to lock the database while testing takes place as well.

## API Overview

```
Supported API Description
get(key) Returns the value of the key
put(key, value) Adds key-value, and overwrites the existing value
delete(key) Delete the key from the data-structure
get(int n) Returns the value of lexographically nth smaller key
delete(int n) Delete the nth key-value pair
```
## Performance Analysis

For further performance analysis and to view the extended scope of the project please view the Report.pdf file.

## Contributors :

* **[Mallika Subramanian](https://github.com/mallika2011)**

* **[Jyoti Sunkara](https://github.com/JyotiSunkara)**

* **[Ainsley Dsouza](https://github.com/Ainsley2000)**

* **[Naren Akash](https://github.com/narenakash)**


## References

```
1. The analysis of hybrid trie structures, Clément (Julien), Flajolet (Philippe), and Vallée (Brigitte).
2. Redesigning the String Hash Table, Burst Trie, and BST to Exploit Cache, Nikolas Askitis, and Justin
Zobel
3. Emory CS323 Data Structures and Algorithms: Compressed(Patricia) Tries
```

