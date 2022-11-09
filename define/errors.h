#ifndef NITCBASE_ERRORS_H
#define NITCBASE_ERRORS_H

// Successful
#define SUCCESS 0
// Error: Command Failed
#define FAILURE -1
// Error: Out of bound
#define E_OUTOFBOUND -2
// Error: Free slot
#define E_FREESLOT -3
// Error: No index
#define E_NOINDEX -4
// Error: Insufficient space in Disk
#define E_DISKFULL -5
// Error: Invalid block
#define E_INVALIDBLOCK -6
// Error: Relation does not exist
#define E_RELNOTEXIST -7
// Error: Relation already exists
#define E_RELEXIST -8
// Error: Attribute does not exist
#define E_ATTRNOTEXIST -9
// Error: Attribute already exists
#define E_ATTREXIST -10
// Error: Cache is full
#define E_CACHEFULL -11
// Error: Relation is not open
#define E_RELNOTOPEN -12
// Error: Mismatch in number of attributes
#define E_NATTRMISMATCH -13
// Error: Duplicate attributes found
#define E_DUPLICATEATTR -14
// Error: Relation is open
#define E_RELOPEN -15
// Error: Mismatch in attribute type
#define E_ATTRTYPEMISMATCH -16
// Error: Invalid index or argument
#define E_INVALID -17
// Error: Maximum number of relations already present
#define E_MAXRELATIONS -18
// Error: Maximum number of attributes allowed for a relation is 125
#define E_MAXATTRS -19
// Error: Operation not permitted
#define E_NOTPERMITTED -20
// Error: Search for requested record unsuccessful
#define E_NOTFOUND -21
// Error: Block not found in buffer
#define E_BLOCKNOTINBUFFER -22
// Due to insufficient disk space, index blocks have been released from the disk
#define E_INDEX_BLOCKS_RELEASED -23

// 'temp' errors
// Error: Cannot create relation named 'temp' as it is used for internal purposes
#define E_CREATETEMP -24
// Error: Cannot create a target relation named 'temp' as it is used for internal purposes
#define E_TARGETNAMETEMP -25
// Error: Cannot rename a relation to 'temp'
#define E_RENAMETOTEMP -26

#endif  // NITCBASE_ERRORS_H