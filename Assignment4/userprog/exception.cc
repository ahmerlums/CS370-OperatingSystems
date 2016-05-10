// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include <stdio.h>
#include <iostream>
using namespace std;
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

int copyin(unsigned int vaddr, int len, char *buf) {

   bool result;
   int n=0;
   int *temp = new int;

   while ( n >= 0 && n < len) {
   result = machine->ReadMem( vaddr, 1, temp );

   buf[n++] = *temp;

   if ( !result ) {
   return -1;
   }
  
   vaddr++;
   }

   delete temp;
   return n;
}


int copyout(unsigned int vaddr, int len, char *buf) {
    // Copy len bytes to the current thread's virtual address vaddr.
    // Return the number of bytes so written, or -1 if an error
    // occors.  Errors can generally mean a bad virtual address was
    // passed in.
    bool result;
    int n=0;      // The number of bytes copied in

    while ( n >= 0 && n < len) {
      // Note that we check every byte's address
      result = machine->WriteMem( vaddr, 1, (int)(buf[n++]) );

      if ( !result ) {
  //translation failed
  return -1;
      }

      vaddr++;
    }

    return n;
}

void Do_This(int which)
{
    currentThread -> space -> InitRegisters();     // set the initial register values
    currentThread -> space -> RestoreState();      // load page table register

    machine->Run();         // jump to the user progam
    ASSERT(FALSE);          // machine->Run never returns;
}

SpaceId Exec(char *name) {
    OpenFile *executable = fileSystem->Open(name);
    AddrSpace *space;

    if (executable == NULL) {
        printf("Unable to open file %s\n", name);
        return -1;
    }

    space = new AddrSpace(executable);    

    Thread *t = new Thread("New Thread", TRUE);

    t -> space = space;
    t->Fork(Do_This, 1);
    cout<<"Opend..and done\n";
    return t -> getID();
}


int Join(SpaceId id) {
    ListElement* temp =  (scheduler -> getList() -> getFirst());
    Thread* thread_temp;

    while (temp) {
        thread_temp = (Thread*) temp -> item;
        if (thread_temp -> getID() == (int) id) {
            thread_temp -> Join();
            return id;
        }

        temp = temp -> next;
    }
}

void Exit_Syscall(int status)
{
     
    currentThread->setExitStatus(status);
    if(currentThread->getParent())
      if (currentThread->getParent()->getStatus() == BLOCKED)
          scheduler->ReadyToRun(currentThread->getParent());

    currentThread->Finish();
}

void Create(unsigned int vaddr) {
  char* filename;
  copyin(vaddr,6,filename);
  fileSystem->Create(filename,0);
}

// -------------------------------------------------------------------------

OpenFileId Open(unsigned int vaddr) {
     // Open the file with the name in the user buffer pointed to by
     // vaddr. The file name is at most MAXFILENAME chars long. If
     // the file is opened successfully, it is put in the address
     // space's file table and an id returned that can find the file
     // later. If there are any errors, -1 is returned.
    int len = 6;
     char *buf = new char[len+1];
     OpenFile *f; // The new open file
     int id; // The openfile id

     if (!buf) {
     printf("%s","Can't allocate kernel buffer in Open\n");
     return -1;
     }

     if( copyin(vaddr,len,buf) == -1 ) {
     printf("%s","Bad pointer passed to Open\n");
     delete[] buf;
     return -1;
     }

     buf[len]='\0';

     f = fileSystem->Open(buf);
     delete[] buf;

     if ( f ) {
         id = currentThread->space->fileTable.Put(f);
         return id;
       }
     else
     return -1;
} 


// ------------------------------------------------------------------------------
void Write(unsigned int vaddr, int len, OpenFileId id) {
    char *buf;    // Kernel buffer for output
    OpenFile *f;  // Open file for output
    buf = new char[len];
    copyin(vaddr,len,buf);
    
    if ( (f = (OpenFile *) currentThread->space->fileTable.Get(id)) ) {
          f->Write(buf, len);
      } else {
        printf("%s","Bad OpenFileId passed to Write\n");
        len = -1;
    }  
}

// ------------------------------------------------------------------------------
int Read(unsigned int vaddr, int len, int id) {
    // Write the buffer to the given disk file.  If ConsoleOutput is
    // the fileID, data goes to the synchronized console instead.  If
    // a Write arrives for the synchronized Console, and no such
    // console exists, create one.    We reuse len as the number of bytes
    // read, which is an unnessecary savings of space.
    char *buf;    // Kernel buffer for input
    OpenFile *f;  // Open file for output

    if ( id == ConsoleOutput) return -1;
    
    if ( !(buf = new char[len]) ) {
  printf("%s","Error allocating kernel buffer in Read\n");
  return -1;
    }

    if ( id == ConsoleInput) {
      //Reading from the keyboard
      scanf("%s", buf);

      if ( copyout(vaddr, len, buf) == -1 ) {
  printf("%s","Bad pointer passed to Read: data not copied\n");
      }
    } else {
  if ( (f = (OpenFile *) currentThread->space->fileTable.Get(id)) ) {
      len = f->Read(buf, len);
      if ( len > 0 ) {
          //Read something from the file. Put into user's address space
            if ( copyout(vaddr, len, buf) == -1 ) {
        printf("%s","Bad pointer passed to Read: data not copied\n");
    }
      }
  } else {
      printf("%s","Bad OpenFileId passed to Read\n");
      len = -1;
  }
    }
    cout << buf <<endl;
    delete[] buf;

    return len;
}

void Close_Syscall(int fd) {
    // Close the file associated with id fd.  No error reporting.
    OpenFile *f = (OpenFile *) currentThread->space->fileTable.Remove(fd);

    if ( f ) {
      delete f;
    } else {
      printf("%s","Tried to close an unopen file\n");
    }
}


void ExceptionHandler(ExceptionType which) {
    int type = machine->ReadRegister(2); // Which syscall?
    int rv=0;   // the return value from a syscall

    if ( which == SyscallException) {//
        switch (type) {
            case SC_Halt:

            DEBUG('a', "Halt syscall - initiated by user.\n");
            cout<<"Halted by user successfull\n";
            interrupt->Halt();
                    break;

          case SC_Create:
          cout<<"Entered create\n";
            DEBUG('a', "Create syscall - initiated by user\n");
            Create(machine->ReadRegister(4));
            cout<<"Created file\n";
            break;
           case SC_Open:
           DEBUG('a', "Create syscall - initiated by user\n");
            rv=Open(machine->ReadRegister(4));
            cout<<"Opened - file\n";
            break;
          case SC_Write:
            DEBUG('a', "Write syscall.\n");
            cout<<"Write syscall\n";
            Write(machine->ReadRegister(4),
                    machine->ReadRegister(5),
                    machine->ReadRegister(6));
            break;
          case SC_Read:
            DEBUG('a', "Read syscall.\n");
            rv = Read(machine->ReadRegister(4),
                    machine->ReadRegister(5),
                    machine->ReadRegister(6));
          break;
           case SC_Close:
            DEBUG('a', "Close syscall.\n");
            Close_Syscall(machine->ReadRegister(4));  
          break;

            case SC_Exit:
            DEBUG('a',"Exit Normally - initiated by user\n");
            cout<<"Exiting normally  \n";
            Exit_Syscall(machine->ReadRegister(4));
            break;
          case SC_Exec:
            DEBUG('a',"Exec system call - initiated by user\n");
            cout<<"Exec called\n";
            break;
          case SC_Join:
            // implement join here
            break;
             case SC_Fork:

          break;

          case SC_Yield:
            currentThread->Yield();

          break;
          default:
            DEBUG('a', "Unknown syscall - shutting down.\n");


	}

      machine->WriteRegister(2,rv);
      machine->WriteRegister(PrevPCReg,machine->ReadRegister(PCReg));
      machine->WriteRegister(PCReg,machine->ReadRegister(NextPCReg));
      machine->WriteRegister(NextPCReg,machine->ReadRegister(PCReg)+4);
      return;
    
    } else {
      cout<<"Unexpected user mode exception - which:"<<which<<"  type:"<< type<<endl;
      interrupt->Halt();
    }

}

