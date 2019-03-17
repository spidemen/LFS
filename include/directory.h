#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <flash.h>
#include  "log.h"
#include  "file.h"

#define IFILE 0

typedef string FileName;

vector<pair<FileName,inum>> tables;

int inodeSize=144;   // default vaule

int currentinum=1;

#endif