#ifndef DEFINES_HEADER
#define DEFINES_HEADER

#define MAX 4096
#define MAX_WD 4096

// a file is in source but not in backup, so needs to be added to backup
#define FILE_IN_SOURCE 1   
// a directory is in source but not in backup, so needs to be added to backup  
#define DIR_IN_SOURCE  2
// a file is in backup but not in source, so needs to be removed from backup
#define FILE_IN_BACKUP 3
// a directory is in backup but not in source, so needs to be removed from backup
#define DIR_IN_BACKUP  4
// a file is in both, so needs to be checked for changes
#define FILE_IN_BOTH   5
// a directory is in both, so do nothing 
#define DIR_IN_BOTH    6

#endif
