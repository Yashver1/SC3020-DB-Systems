### Design

Qns:

Disk Implementation of storing records
- parsing of schema
- fixed or variable length records
- dynamic block size?
- system calls to use to write to disks
- classes to use
- traversing of block
- structure of each record

Implement Page Cacheing not priorirt

B+Tree Implementation on disk
- iterative or bulk loading
- order of insertion
- number of keys in each node
- traversing of tree
- linear scan

\

data structures We can use

SchemaTable
key to Schemas

Block Data
- maintains block data and some header information liek Blk size and also whether Index block 
- offset fields
- constructor takes in fostream and constructs block data

Record Sruct
header
    timestamp
    length
    schema point

fields
(maybe does operator overload of a ostream for easy print)

Index Struct
single bit to indicate leaf index
pointer/offset from start of index file or data file if singel bit is true
key



RecordView
schema key 
holds Record struct
holds a block data 
works as a view on the block byte data and traverses based on record dat

IndexView
schema key
holds IndexStruct
holds a Index block data
works as a view ont he block byte data and traversess the Index Node .

DiskManager handle these internal functionality

GuiManager handle the interface (cli) to call disk manaer gunctions



ALL OFfsets based on distance from start of file


Actions

First start with bulk loading
1.create binary records and then store primary key in heap maybe for later. 
while creating records if total size of current binary records just below block size, make one block and store in file
2. after done all blocks start on b+tree index. we have sorted primary keys then we can start bulk loading
(pack leaf node to full? since no insertions) and conduct bulk loading
3. after btree created can store in another file


On actions
1.normal query we just fetch the first index block then create block data and therefore indexview.
scan indexview until we can get the right pointer offset and fetch second index block baesd on index
do this continuosly until data block found (known via isLeaf node) and then fetch from offset from data start

2. on range query we uise linera scan. Do same thing until we reach first leaf node and then continously scan through leaf nodes til upper boound.

For statistics
 
the number of index nodes the process (can keep a global staticcount on creation and descruction)
accesses; 

the number of data blocks the process accesses (gloval coutn also on each fetch query)

theaverage of “FG3_PCT_home” of the records that are returned (maintian running average)

the
running time of the retrieval process; (just use time stamp limb before and after fetching starst)

the number of data blocks that
would be accessed by a brute-force linear scan method (i.e., it scans
the data blocks one by one) and its running time (for comparison)


