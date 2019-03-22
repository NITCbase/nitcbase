/* insert an index into b-tree */
#include "blockBuffer.h"
#include <cstring>


int compareAttr(union Attribute val1,union Attribute val2,int type ){
    if(type==INT){
        if(val1.ival==val2.ival){
            return 0;
        }else if(val1.ival<val2.ival){
            return 1;
        }else{
            return 2;
        }
    }else if(type==STRING){
        if(strcmp(val1.strval,val2.strval)==0){
            return 0;
        }else if(strcmp(val1.strval,val2.strval)<0){
            return 1;
        }else{
            return 2;
        }
    }
}

int bplus_insert(int root_block,struct Index indexval, int type_attr){
    if(root_block==-1){
        return FAILURE;
    }

    class IndBuffer *head=getIndBlock(root_block);
    struct HeadInfo header=head->getheader();

    if(header.block_type==IND_INTERNAL){
        //change head to internal ind pointer type
        
        int nEntries=header.num_entries;
        int child_head;

        struct InternalEntry elem=head->getInternalEntry(0);
        int compare_val=compareAttr(indexval.attrval,elem.attrval,type_attr);
        if(compare_val<=1){
            child_head=elem.lchild;
        }else{
            int iter;
            for(iter=1;iter<nEntries;iter++){
                elem=head->getInternalEntry(iter);
                int compare_val1=compareAttr(indexval.attrval,elem.attrval,type_attr);
                if(compare_val1<=1){
                    child_head=elem.lchild;
                    break;
                }

            }
            if(iter==nEntries){
                child_head=elem.rchild;
            }
        }
        bplus_insert(child_head,indexval,type_attr);

    }else{
        //change head to ind_leaf
        int nEntries=header.num_entries;
        bool success = false;
        if(nEntries!=63){
            struct Index elem;
            int iter;
            for(iter=0;iter<nEntries;iter++){
                elem=root_block->getIndexval(iter);
                int compare_val=compareAttr(indexval.attrval,elem.attrval,type_attr);
                if(compare_val<=1){
                    break;
                }
            }
            int ind=iter;
            for(iter=nEntries;iter>ind;iter--){
                struct Index elem;
                elem=head->getIndexval(iter-1);
                head->setIndexval(elem,iter);
            }

            head->setIndexval(indexval,ind);
            header.num_entries=nEntries+1;
            head->setheader(header);
            success=true;
        }else{
            int par_block=header.pblock;
            struct Index attrs[nEntries+1];
            int iter;

            struct Index elem;
            for(iter=0;iter<nEntries;iter++){
                elem=head->getIndexval(iter);
                int compare_val=compareAttr(indexval.attrval,elem.attrval,type_attr);
                if(compare_val<=1){
                    break;
                }
                attrs[iter]=head->getIndexval(iter);
            }
            int ind=iter;
            for(iter=nEntries;iter>ind;iter--){
                attrs[iter]=attrs[iter-1];
            }

            class IndLeaf* new_blk=buffer->getFreeIndInternal();
            struct HeadInfo newblkheader=new_blk->getheader();

            for(iter=0;iter<32;iter++){
                head->setattrval(attrs[iter],iter);
                new_blk->setattrval(attrs[32+iter],iter);
            }
            union Attribute new_attrval;
            new_attrval=attrs[31].attrval;
            header.num_entries=32;
            newblkheader.num_entries=32;
            newblkheader.pblock=par_block;
            head->setheader(header);
            new_blk->setheader(newblkheader);
            int new_blocknum=new_blk->getblocknum();
            int child_block=head->getblocknum();

            while(success==false){
                if(par_block!=-1){
                    class IndBuffer *parblk=getIndBlock(par_block);
                    struct HeadInfo parheader=parblk->getheader();
                    
                    struct InternalEntry internal_entries[parheader.num_entries+1];
                    int iter;
                    int tar_ind=parheader.num_entries;

                    for(iter=0;iter<parheader.num_entries;iter++){
                        internal_entries[iter]=parblk->getInternalEntry(iter);
                        if(internal_entries[iter].lchild==child_block){
                            tar_ind=iter;
                        }
                    }
                    for(iter=parheader.num_entries;iter>tar_ind;iter++){
                        internal_entries[iter]=internal_entries[iter-1];
                    }
                    internal_entries[tar_ind].attrval=new_attrval;
                    internal_entries[tar_ind].lchild=child_block;
                    internal_entries[tar_ind].rchild=new_blocknum;

                    if(tar_ind!=parheader.num_entries){
                        internal_entries[tar_ind+1].lchild=internal_entries[tar_ind].rchild;
                    }

                    if(parheader.num_entries!=100){
                        for(iter=0;iter<=parheader.num_entries;iter++){
                            parblk->setInternalEntry(internal_entries[iter],iter);
                        }
                        parheader.num_entries=parheader.num_entries+1;
                        parblk->setheader(parheader);
                        newblkheader.pblock=par_block;  //might have to change this line to load new_blocknum
                        new_blk->setheader(newblkheader);
                        success=true;
                        //close newblock,childblock,parblock
                    }else{
                        //close childblk,newblk
                        //getnewfreeinternalblk
                        //split internalentries array in to parblk and new_internalblk
                        class IndInternal* new_blk=buffer->getFreeIndInternal();
                        newblkheader=new_blk->getheader();

                        for(iter=0;iter<50;iter++){
                            parblk->setattrval(internal_entries[iter],iter);
                            new_blk->setattrval(internal_entries[51+iter],iter);
                        }
                        parheader.num_entries=50;
                        newblkheader.num_entries=50;
                        newblkheader.pblock=parheader.pblock;
                        parblk->setheader(parheader);
                        new_blk->setheader(newblkheader);
                        //set the pblock of all childs appropriately
                        child_block=par_block;
                        par_block=parheader.pblock;
                        new_attrval=(parblk->getInternalEntry(50)).attrval;
                        //close parblk
                        
                    }


                }else{
                    //get new internal block 
                    //add lblock=root,rblock=#new_block..only one single entry in head;
                    //update head of respective cache element
                    //update par of both rblock and lblock
                    success=true;
                }
            }

        }

        
    }
    // close head

}