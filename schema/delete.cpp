// delete a relation (specified by relname)
#include "../define/constants.h"
#include "../cache/cache.h"

int deleterel(char relname[ATTR_SIZE]){
    int relid=OpenRelTable::getRelId(relname);

    if(relid!=E_NOTOPEN){
        return E_RELOPEN; // target relation is opened
    }

    int ba_delete_rel=ba_delete(relname);
    
    return ba_delete_rel;
}