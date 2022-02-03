//
// Created by Gokul Sreekumar on 31/10/21.
//

#ifndef FRONTEND_INTERFACE_H
#define FRONTEND_INTERFACE_H
class Disk {
public:
    Disk();
    ~Disk();
    static int readBlock(unsigned char *block, int blockNum);
    static int writeBlock(unsigned char *block, int blockNum);
};
#endif //FRONTEND_INTERFACE_H
