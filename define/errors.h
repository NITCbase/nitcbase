#ifndef B18_CODE_ERRORS_H
#define B18_CODE_ERRORS_H

#define SUCCESS 0
#define FAILURE -1
#define E_OUTOFBOUND -2
#define E_FREESLOT -3
#define E_NOINDEX -4
#define E_DISKFULL -5
#define E_INVALIDBLOCK -6
#define E_RELNOTEXIST -7
#define E_RELEXIST -8
#define E_ATTRNOTEXIST -9
#define E_ATTREXIST -10
#define E_CACHEFULL -11
#define E_NOTOPEN -12
#define E_RELNOTOPEN -13
#define E_NATTRMISMATCH -14
#define E_DUPLICATEATTR -15
#define E_RELOPEN -16
#define E_ATTRTYPEMISMATCH -17
#define E_INVALID -18
#define E_MAXRELATIONS -19
#define E_MAXATTRS -20

// 'temp' errors
#define E_CREATETEMP -21
#define E_TARGETNAMETEMP -22
#define E_RENAMETOTEMP -23

#endif //B18_CODE_ERRORS_H