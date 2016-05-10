SpaceId Exec(char *name);

void Create_Syscall(unsigned int vaddr, int len);
int Read_Syscall(unsigned int vaddr, int len, int id);
void Write_Syscall(unsigned int vaddr, int len, int id);
void Close_Syscall(int fd);
static void Exit_syscall();
/* Close the file, we're done reading and writing to it. */
void Close(OpenFileId id);
