#ifndef NITCBASE_BLOCKACCESS_H
#define NITCBASE_BLOCKACCESS_H

#include <iostream>
#include <cstring>

#include "../Buffer/BlockBuffer.h"
#include "../define/id.h"


class BlockAccess {
public:

	static int search(int relId, Attribute *record, char *attrName, Attribute attrVal, int op, int flagValidAttrName);

	static int insert(int relId, union Attribute *record);

	static int renameRelation(char *oldName, char *newName);

	static int renameAttribute(char *relName, char *oldName, char *newName);

	static int deleteRelation(char *relName);

	static RecId linearSearch(int relId, char *attrName, Attribute attrVal, int op);

};

#endif //NITCBASE_BLOCKACCESS_H