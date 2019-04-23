#include "../define/constants.h"
#include "../define/id.h"

typedef struct RelCatEntry{
	char rel_name[ATTR_SIZE];
	int num_attr;
	int num_rec;
	int first_blk;
	int num_slots_blk;
}RelCatEntry ;

typedef struct AttrCatEntry{
	char rel_name[ATTR_SIZE];
	char attr_name[ATTR_SIZE];
	int attr_type;
	bool primary_flag;
	int root_block;
	int offset;
}AttrCatEntry ;

typedef struct AttributeCache{
	AttrCatEntry attrcat_entry;
	bool dirty;
	recId rec_id;
	IndexId sid; /*IMPORTANT: used for storing state information of any search.
									 which will be used for getting next hit. */
	struct AttributeCache *next;
} AttributeCache;

typedef struct OpenRelTableEntry{
	RelCatEntry relcat_entry;
	struct AttributeCache *attr_list_head;
	bool free, dirty;
	recId rec_id, prev_recid;
}OpenRelTableEntry;


class OpenRelTable{
private:
	static OpenRelTableEntry rel_table[MAXOPEN];
	static relId getFreeOpenRelTableEntry();
	
public:	
	static relId OpenRel(char rel_name[ATTR_SIZE]);
	static int CloseRel(relId rel_id);
	static relId getRelId(char rel_name[ATTR_SIZE]);
	static int getRelCatEntry(relId rel_id, RelCatEntry *relcat_buf);
	static int setRelCatEntry(relId rel_id, RelCatEntry *relcat_buf);
	static recId getPrevRecId(relId rel_id); /*CHANGE INTERFACE HERE*/
	static int setPrevRecId(relId rel_id, recId rid);
	static int getAttrCatEntry(relId rel_id, char attr_name[ATTR_SIZE], AttrCatEntry *attrcat_buf);
	static int getAttrCatEntry(relId rel_id, int attr_offset, AttrCatEntry *attrcat_buf);
	static int setAttrCatEntry(relId rel_id, char attr_name[ATTR_SIZE], AttrCatEntry *attrcat_buf);
	static int setAttrCatEntry(relId rel_id, int attr_offset, AttrCatEntry *attrcat_buf);
	static IndexId getPrevIndexId(relId rel_id, char attr_name[ATTR_SIZE]);
	static IndexId getPrevIndexId(relId rel_id, int attr_offset);
	static int setPrevIndexId(relId rel_id, char attr_name[ATTR_SIZE], IndexId sid);
	static int setPrevIndexId(relId rel_id, int attr_offset, IndexId sid);
};

/*TODO:
Write constructor for OpenRelTable class. (destructor as well).
*/

