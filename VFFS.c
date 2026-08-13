/////////////////////////////////////////////////////////////////////
//
//  Header Files Inclusion
//
/////////////////////////////////////////////////////////////////////

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdbool.h>
#include<sys/stat.h>

/////////////////////////////////////////////////////////////////////
//
//  User Defined Macros
//
/////////////////////////////////////////////////////////////////////

#define MAXINODE 5
#define MAXFILESIZE 50
#define MAXOPENFILES 5

#define READ 1
#define WRITE 2
#define EXECUTE 4

#define START 0
#define CURRENT 1
#define END 2

#define EXECUTE_SUCCESS 0

#define REGULARFILE 1
#define SPECIALFILE 2

/////////////////////////////////////////////////////////////////////
//
//  User Defined Macros for error handling
//
/////////////////////////////////////////////////////////////////////

#define ERR_INVALID_PARAMTER -1

#define ERR_NO_INODES -2

#define ERR_FILE_ALREADY_EXIST -3
#define ERR_FILE_NOT_EXIST -4

#define ERR_PERMISSION_DENIED -5

#define ERR_INSUFFICIENT_SPACE -6
#define ERR_INSUFFICIENT_DATA -7

#define ERR_MAX_FILES_OPEN -8

/////////////////////////////////////////////////////////////////////
//
//  Structure Name :        BootBlock
//  Description    :        It holds the information to     
//                          Boot the Operating System
//
/////////////////////////////////////////////////////////////////////

struct BootBlock
{
    char Information[100];
};

/////////////////////////////////////////////////////////////////////
//
//  Structure Name :        SuperBlock
//  Description    :        It holds the information of
//                          complete File System
//
/////////////////////////////////////////////////////////////////////

struct SuperBlock
{
    int TotalInodes;
    int FreeInodes;
};

/////////////////////////////////////////////////////////////////////
//
//  Structure Name :        Inode
//  Description    :        It holds information of file
// 
/////////////////////////////////////////////////////////////////////

#pragma pack(1)
struct Inode
{
    char FileName[20];
    int InodeNumber;
    int FileSize;
    int ActualFileSize;
    int FileType;
    int ReferenceCount;
    int Permission;
    char *Buffer;
    struct Inode *next;
};

typedef struct Inode INODE;
typedef struct Inode* PINODE;
typedef struct Inode** PPINODE;

/////////////////////////////////////////////////////////////////////
//
//  Structure Name :        FileTable
//  Description    :        It holds information of opened
//                          files
//
/////////////////////////////////////////////////////////////////////

#pragma pack(1)
struct FileTable
{
    int ReadOffset;
    int WriteOffset;
    int Mode;
    PINODE ptrinode;
};

typedef struct FileTable FILETABLE;
typedef struct FileTable* PFILETABLE;

/////////////////////////////////////////////////////////////////////
//
//  Structure Name :        UAREA
//  Description    :        It holds information of Process
//                      
/////////////////////////////////////////////////////////////////////

struct UAREA
{
    char ProcessName[20];
    PFILETABLE UFDT[MAXOPENFILES];
};

/////////////////////////////////////////////////////////////////////
//
//  Global variables used in the projectss
//
/////////////////////////////////////////////////////////////////////

struct BootBlock bootobj;
struct SuperBlock superobj;
struct UAREA uareaobj;

PINODE head = NULL;

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     InitialiseUAREA
//  Description :       It is used to intialise UAREA
//  Author :            Rushikesh Sanjay Puri
//  Date :              31/07/2026
//
/////////////////////////////////////////////////////////////////////

void InitialiseUAREA()
{
    int i = 0;
    strcpy(uareaobj.ProcessName, "myexe");

    for (i = 0; i < MAXOPENFILES; i++)
    {
        uareaobj.UFDT[i] = NULL;
    }

    printf(" VFSS : UAREA gets initialised successfully.\n");

}

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     InitialiseSuperBlock
//  Description :       It is used to intialise super block
//  Author :            Rushikesh Sanjay Puri
//  Date :              31/07/2026
//
/////////////////////////////////////////////////////////////////////

void InitialiseSuperBlock()
{
    superobj.TotalInodes = MAXINODE;
    superobj.FreeInodes = MAXINODE;

    printf(" VFSS : Super Block gets initialised successfully.\n");
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     CreateDILB()
//  Description :       It is used to create linked-list of inodes
//  Author :            Rushikesh Sanjay Puri
//  Date :              31/07/2026
//
/////////////////////////////////////////////////////////////////////

void CreateDILB()
{
    PINODE temp = NULL;
    PINODE newn = NULL;

    int i = 0;
    temp = head;

    for(i = 1; i <= MAXINODE; i++)
    {
        newn = (PINODE)malloc(sizeof(INODE));

        newn->InodeNumber = i;
        strcpy(newn->FileName, "\0");
        newn -> FileSize = 0;
        newn->ActualFileSize = 0;
        newn->FileType = 0;
        newn->ReferenceCount = 0;
        newn->Permission = 0;
        newn->Buffer = NULL;

        if(temp == NULL)
        {
            head = newn;
            temp = newn;
        }
        else
        {
            temp -> next = newn;
            temp = temp -> next;
        }

    }

    printf(" VFSS : DILB gets created successfully.\n");
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     StartAuxilaryDataInitialisation()
//  Description :       It is used to call all such functions 
//                      which are used to initialised auxillary data
//  Author :            Rushikesh Sanjay Puri
//  Date :              31/07/2026
//
/////////////////////////////////////////////////////////////////////

void StartAuxilaryDataInitialisation()
{
    strcpy(bootobj.Information, "Booting process of VFSS is completed");
    
    printf("%s\n",bootobj.Information);
    
    InitialiseUAREA();
    
    InitialiseSuperBlock();
    
    CreateDILB();
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     DisplayHelp()
//  Description :       It is used to display help to run the 
//                      user of project
//  Author :            Rushikesh Sanjay Puri
//  Date :              01/08/2026
//
/////////////////////////////////////////////////////////////////////

void DisplayHelp()
{
    printf("\n=================================================================\n");
    printf("==========================  VFSS Help Page ======================\n");
    printf("=================================================================\n");

    printf("man   : It is used to display the manual page.\n");
    printf("clear : It is used to clear the terminal screen.\n");
    printf("open  : It is used to open the regular file.\n");
    printf("close : It is used to close the regular file.\n");
    printf("creat : It is used to create the new regular file.\n");
    printf("write : It is used to write the data into the file.\n");
    printf("read  : It is used to read the data from file.\n");
    printf("stat  : It is used to display statistical information of file.\n");
    printf("unlink : It is used to delete the file.\n");
    printf("exit   : It is used to terminate  VFSS.\n");

    printf("==================================================================\n");

}

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     ManPageDisplay()
//  Description :       It is used to display the manual page of a
//                      specific command.
//  Input :             Name of Command
//  Author :            Rushikesh Sanjay Puri
//  Date :              01/08/2026
//
/////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

void Header(char Command[])
{
    printf("\n\033[1;36m");
    printf("=============================================================\n");
    printf("                 VIRTUAL FILE SYSTEM MANUAL\n");
    printf("=============================================================\n");
    printf("\033[0m");

    printf("\033[1;33mCommand : %s\033[0m\n\n", Command);
}

void Footer()
{
    printf("\n\033[1;36m=============================================================\033[0m\n");
}

void ManPageDisplay(char Name[])
{
    if(strcmp(Name,"exit")==0)
    {
        Header("exit");

        printf("DESCRIPTION\n");
        printf("    Terminates the Virtual File System shell.\n\n");

        printf("USAGE\n");
        printf("    exit\n\n");

        printf("EXAMPLE\n");
        printf("    VFS :> exit\n\n");

        printf("NOTE\n");
        printf("    Make sure to save important data before exiting.\n");

        Footer();
    }

    else if(strcmp(Name,"ls")==0)
    {
        Header("ls");

        printf("DESCRIPTION\n");
        printf("    Lists all existing files in the Virtual File System.\n\n");

        printf("USAGE\n");
        printf("    ls\n\n");

        printf("EXAMPLE\n");
        printf("    VFS :> ls\n\n");

        printf("OUTPUT\n");
        printf("    demo.txt\n");
        printf("    project.c\n");
        printf("    report.pdf\n");

        Footer();
    }

    else if(strcmp(Name,"clear")==0)
    {
        Header("clear");

        printf("DESCRIPTION\n");
        printf("    Clears the terminal screen.\n\n");

        printf("USAGE\n");
        printf("    clear\n\n");

        printf("EXAMPLE\n");
        printf("    VFS :> clear\n");

        Footer();
    }

    else if(strcmp(Name,"creat")==0 || strcmp(Name,"create")==0)
    {
        Header("create");

        printf("DESCRIPTION\n");
        printf("    Creates a new file in the Virtual File System.\n\n");

        printf("USAGE\n");
        printf("    create <FileName> <Permission>\n\n");

        printf("PARAMETERS\n");
        printf("    FileName   : Name of the file.\n");
        printf("    Permission :\n");
        printf("        1 -> Read\n");
        printf("        2 -> Write\n");
        printf("        3 -> Read + Write\n\n");

        printf("EXAMPLE\n");
        printf("    VFS :> create Demo.txt 3\n\n");

        printf("RESULT\n");
        printf("    File 'Demo.txt' created successfully.\n");

        Footer();
    }

    else if(strcmp(Name,"unlink")==0)
    {
        Header("unlink");

        printf("DESCRIPTION\n");
        printf("    Deletes an existing file permanently.\n\n");

        printf("USAGE\n");
        printf("    unlink <FileName>\n\n");

        printf("EXAMPLE\n");
        printf("    VFS :> unlink Demo.txt\n\n");

        printf("WARNING\n");
        printf("    Deleted files cannot be recovered.\n");

        Footer();
    }

    else if(strcmp(Name,"stat")==0)
    {
        Header("stat");

        printf("DESCRIPTION\n");
        printf("    Displays detailed information about a file.\n\n");

        printf("USAGE\n");
        printf("    stat <FileName>\n\n");

        printf("EXAMPLE\n");
        printf("    VFS :> stat Demo.txt\n\n");

        printf("OUTPUT INFORMATION\n");
        printf("    • File Name\n");
        printf("    • File Size\n");
        printf("    • Inode Number\n");
        printf("    • Link Count\n");
        printf("    • File Permission\n");
        printf("    • File Type\n");

        Footer();
    }

    else
    {
        printf("\n");
        printf("\033[1;31m");
        printf("-------------------------------------------------------------\n");
        printf(" ERROR : No manual entry for '%s'\n", Name);
        printf("-------------------------------------------------------------\n");
        printf("\033[0m");

        printf("Available Commands:\n");
        printf("   exit\n");
        printf("   ls\n");
        printf("   clear\n");
        printf("   create\n");
        printf("   unlink\n");
        printf("   stat\n\n");

        printf("Type : man <command>\n");
        printf("Example : man create\n");
    }
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     IsFileExist()
//  Description :       It is used to check whether the file 
//                      is present or not. 
//  Input :             Name of file 
//  Output :            True if Present / False if not present
//  Author :            Rushikesh Sanjay Puri
//  Date :              01/08/2026
//
/////////////////////////////////////////////////////////////////////

bool IsFileExist(   
                    char name[]         // Name of file
                )
{
    PINODE temp = head;
    bool bFlag = false;

    while(temp != NULL)
    {
        if(strcmp(temp -> FileName,name) == 0)
        {
            bFlag = true;
            break;
        }

        temp = temp -> next;
    }

    return bFlag;
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     CreateFile()
//  Description :       It is used to create new file. 
//  Input :             Name of file & Permission
//  Output :            FileDescriptor (fd)
//  Author :            Rushikesh Sanjay Puri
//  Date :              01/08/2026
//
/////////////////////////////////////////////////////////////////////

int CreateFile (
                    char name[],            // Name of file
                    int Permission          // File permission
               )
{
    PINODE temp = head;

    int i = 0;

    if(superobj.FreeInodes == 0)
    {
        return ERR_NO_INODES;
    }

    // if permission value is wrong
    // Permission = 1 -> Read
    // Permission = 2 -> Write
    // Permission = 3 -> Read + Write

    // filter
    if(Permission < 1 || Permission > 3)
    {
        return ERR_INVALID_PARAMTER;
    }
    
    if(IsFileExist(name) == true)
    {
        return ERR_FILE_ALREADY_EXIST;
    }

    // Search for empty inode
    while (temp != NULL)
    {
        if(temp -> FileType == 0)
        {
            break;
        }
        
        temp = temp -> next;
    }
    
    // rare case
    if(temp == NULL)
    {
        return ERR_NO_INODES;
    }

    // Search Empty UFDT entry
    
    // Reserve first 3 FD's
    for(i = 3; i < MAXOPENFILES; i++)
    {
        if(uareaobj.UFDT[i] == NULL)
        {
            break;
        }
    }

    if(i == MAXOPENFILES)
    {
        return ERR_MAX_FILES_OPEN;
    }

    // Allocate memory for filetable
    uareaobj.UFDT[i] = (PFILETABLE)malloc(sizeof(FILETABLE));

    // Intialie File Table
    uareaobj.UFDT[i] -> ReadOffset = 0;
    uareaobj.UFDT[i] -> WriteOffset = 0;
    uareaobj.UFDT[i] -> Mode = Permission;

    // Connect File table with Inode/IIT
    uareaobj.UFDT[i] -> ptrinode = temp;

    // Intialise all members of inode
    strcpy(uareaobj.UFDT[i] -> ptrinode -> FileName,name); 

    uareaobj.UFDT[i] -> ptrinode -> FileSize = MAXFILESIZE;

    uareaobj.UFDT[i] -> ptrinode -> ActualFileSize = 0;

    uareaobj.UFDT[i] -> ptrinode -> FileType = REGULARFILE;

    uareaobj.UFDT[i] -> ptrinode -> ReferenceCount = 1;

    uareaobj.UFDT[i] -> ptrinode -> Permission = Permission;

    // Allocate memory for files data (Data Block)

    uareaobj.UFDT[i] -> ptrinode -> Buffer = (char*)malloc(MAXFILESIZE);

    superobj.FreeInodes--;

    return i;
 
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     LsFile()
//  Description :       It is used to Display names of all files
//  Input :             None
//  Output :            None
//  Author :            Rushikesh Sanjay Puri
//  Date :              01/08/2026
//
/////////////////////////////////////////////////////////////////////

void LsFile()
{
    PINODE temp = head;

    printf("\n=================================================================\n");
    printf("===============  VFSS Files Information ===============\n");
    printf("=================================================================\n");

    while(temp != NULL)
    {
        if(temp -> FileType != 0)
        {
            printf("%s\n",temp ->FileName);
        }

        temp = temp -> next;
    }
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     LsFile_All()
//  Description :       Its used to Display all details of all files
//  Input :             None
//  Output :            None
//  Author :            Rushikesh Sanjay Puri
//  Date :              02/08/2026
//
/////////////////////////////////////////////////////////////////////

void LsFile_All()
{
    PINODE temp = head;

    printf("\n=================================================================\n");
    printf("===============  VFSS Files Information ===============\n");
    printf("=================================================================\n");

    while(temp != NULL)
    {
        if(temp -> FileType != 0)
        {
            printf("%s %d %d\n",temp ->FileName,temp -> InodeNumber,temp -> ActualFileSize);
        }

        temp = temp -> next;
    }
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     stat_file()
//  Description :       Its used to Display all details of specific 
//                      files
//  Input :             File name
//  Output :            Exit status of function
//  Author :            Rushikesh Sanjay Puri
//  Date :              02/08/2026
//
/////////////////////////////////////////////////////////////////////

int stat_file (
                char name[]
              )
{
    PINODE temp = NULL;
    int Permission = 0;
    int Type = 0;

    if(IsFileExist(name) == false)
    {
        return ERR_FILE_NOT_EXIST;
    }

    temp = head;

    while(temp != NULL)
    {
        if(strcmp(temp -> FileName,name) == 0)
        {
            printf("\n=================================================================\n");
            printf("================= Statistical Information of File ===============\n");
            printf("=================================================================\n");

            printf("File name : %s\n",temp -> FileName);
            
            printf("Inode number : %d\n",temp -> InodeNumber);
            
            printf("File size : %d\n",temp -> FileSize);

            printf("Actual file size : %d\n",temp -> ActualFileSize);

            printf("Reference count : %d\n",temp -> ReferenceCount);

            Permission = temp -> Permission;

            if(Permission == READ)
            {
                printf("File permission : READ ONLY\n");
            }
            else if(Permission == WRITE)
            {
                printf("File permission : WRITE ONLY\n");
            }
            else if(Permission == READ + WRITE)
            {
                printf("File permission : READ + WRITE \n");
            }

            Type = temp -> FileType;

            if(Type == REGULARFILE)
            {
                printf("File type : Regular File\n");
            }
            else if(Type == SPECIALFILE)
            {
                printf("File type : Special File\n");
            }

            printf("=================================================================\n");
            
            break;
        }

        temp = temp -> next;
    }

    return EXECUTE_SUCCESS;
}

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     unlink_file()
//  Description :       Its used to Delete the specific file
//  Input :             File name
//  Output :            Exit status of function
//  Author :            Rushikesh Sanjay Puri
//  Date :              02/08/2026
//
/////////////////////////////////////////////////////////////////////

int unlink_file (
                    char name[] // Name of file
                )
{
    int i = 0;

    if(IsFileExist(name) == false)
    {
        return ERR_FILE_NOT_EXIST;
    }

    // Travel the ufdt

    for(i = 0; i < MAXOPENFILES; i++)
    {
        if(uareaobj.UFDT[i] != NULL)
        {
            if(strcmp(uareaobj.UFDT[i] -> ptrinode -> FileName, name) == 0)
            {
                // Deallocate memory of Buffer
                free(uareaobj.UFDT[i] -> ptrinode -> Buffer);
                uareaobj.UFDT[i] -> ptrinode -> Buffer = NULL;

                strcpy(uareaobj.UFDT[i] -> ptrinode -> FileName, "\0");
                
                uareaobj.UFDT[i] -> ptrinode -> FileSize = 0;
                
                uareaobj.UFDT[i] -> ptrinode -> ActualFileSize = 0;
                
                uareaobj.UFDT[i] -> ptrinode -> FileType = 0;
                
                uareaobj.UFDT[i] -> ptrinode -> ReferenceCount = 0;
                
                uareaobj.UFDT[i] -> ptrinode -> Permission = 0;
                
                // Deallocate memory of file Table
                free(uareaobj.UFDT[i]);

                uareaobj.UFDT[i] = NULL;

                superobj.FreeInodes++;

                break;  //IMPORTANT
            }
        }//End of if
    
    }// End of for loop

    return EXECUTE_SUCCESS;
    
}// End of unlink_file()

/////////////////////////////////////////////////////////////////////
//
//  Function Name :     write_file()
//  Description :       Its used to write the data into 
//                      specific file
//  Input :             File Descriptor
//                      Data we want to write
//                      Size of data
//  Output :            Number of bytes successfully written
//  Author :            Rushikesh Sanjay Puri
//  Date :              02/08/2026
//
/////////////////////////////////////////////////////////////////////

int write_file  (
                    int fd,
                    char *Data,
                    int size
                )
{
    int offset = 0;

    printf("File Descriptor : %d\n",fd);
    printf("Data we want to write : %s\n",Data);
    printf("Size of Data : %d\n",size);

    // if fd is invalid
    if(fd < 0 || fd > MAXOPENFILES)
    {
        return ERR_INVALID_PARAMTER;
    }

    // if writing permission is not there
    if(uareaobj.UFDT[fd]->ptrinode->Permission < WRITE)
    {
        return ERR_PERMISSION_DENIED;
    }

    // Check the space is there or not
    if ((MAXFILESIZE - uareaobj.UFDT[fd]->WriteOffset) < size) 
    { 
        return ERR_INSUFFICIENT_SPACE; 
    }

    if(uareaobj.UFDT[fd] == NULL)
    {
        return ERR_FILE_NOT_EXIST;
    }

    //offset = uareaobj.UFDT[fd]-> ptrinode -> Buffer + uareaobj.UFDT[fd] -> WriteOffset;

    // Actual writing data
    strncpy(uareaobj.UFDT[fd]-> ptrinode -> Buffer + uareaobj.UFDT[fd] -> WriteOffset,Data,size);

    // update the write offset
    uareaobj.UFDT[fd] -> WriteOffset = uareaobj.UFDT[fd] -> WriteOffset + size;

    // update acutal file size
    uareaobj.UFDT[fd]->ptrinode->ActualFileSize = uareaobj.UFDT[fd]->ptrinode->ActualFileSize + size;

    return size;
}

//////////////////////////////////////////////////////////////////////
//  Function Name :     save_file()
//
//  Description :       Its used to save the data of
//                      specific VFSS file into
//                      physical file on disk
//
//  Input :             File Descriptor
//
//  Output :            None
//
//  Author :            Rushikesh Sanjay Puri
//
//  Date :              02/08/2026
//
//////////////////////////////////////////////////////////////////////

void save_file(int VFSSfd)
{
    char *Buffer = NULL;
    char FilePath [100] = {'\0'};
    int Size = 0;
    int Backupfd = 0;
    int Ret = 0;

    Buffer = uareaobj.UFDT[VFSSfd] -> ptrinode -> Buffer;
    Size = uareaobj.UFDT[VFSSfd] -> ptrinode -> ActualFileSize;
    sprintf(FilePath,"Backup/%s",uareaobj.UFDT[VFSSfd] -> ptrinode -> FileName);

    Backupfd = open(FilePath,O_WRONLY | O_CREAT | O_TRUNC,0777);

    if(Backupfd == -1)
    {
        printf("Unable to create backup file\n");
        return;
    }

    Ret = write(Backupfd,Buffer,Size);

    if(Ret != Size)
    {
        printf("Unable to write file\n");
        close(Backupfd);
        return;
    }

    close(Backupfd);
}


/////////////////////////////////////////////////////////////////////
//
//  Function Name :     read_file()
//  Description :       Its used to read the data into 
//                      specific file
//  Input :             File Descriptor
//                      address of empty Buffer
//                      Size of data
//  Output :            Number of bytes successfully read
//  Author :            Rushikesh Sanjay Puri
//  Date :              02/08/2026
//
/////////////////////////////////////////////////////////////////////

int read_file(
                int fd,
                char *data,
                int size
            )
{
    // Invalid fd
    if(fd < 0 || fd >= MAXOPENFILES)
    {
        return ERR_INVALID_PARAMTER;
    }

    if(size <= 0)
    {
        return ERR_INVALID_PARAMTER;
    }

    // filter for fd
    if(uareaobj.UFDT[fd] == NULL)
    {
        return ERR_FILE_NOT_EXIST;
    }

    // filter for permission
    if(uareaobj.UFDT[fd] -> ptrinode->Permission < READ)
    {
        return ERR_PERMISSION_DENIED;
    }

    // Insufficent data
    if((uareaobj.UFDT[fd]->ptrinode->ActualFileSize -uareaobj.UFDT[fd]->ReadOffset) < size)
    {
        return ERR_INSUFFICIENT_DATA;
    }   

    // Read the data
    strncpy(data,uareaobj.UFDT[fd] -> ptrinode -> Buffer + uareaobj.UFDT[fd] -> ReadOffset,size);
    {

        uareaobj.UFDT[fd] -> ReadOffset = uareaobj.UFDT[fd] -> ReadOffset + size;

        return size;
    }

}

/////////////////////////////////////////////////////////////////////
//
//  Entry Point function of the VFSS project
//
/////////////////////////////////////////////////////////////////////

int main()
{   
    mkdir("Backup",0777);

    // for storing Input the command
    char str[80] = {'\0'};
    
    // for tokenizing the command
    char Command[5][20] ={{'\0'}};

    // Data for write System call
    char InputBuffer [MAXFILESIZE] = {'\0'};
    
    int iRet = 0;
    int iCount = 0;
    int size = 0;

    char *EmptyBuffer = NULL;

    StartAuxilaryDataInitialisation();

    printf("\n=================================================================\n");
    printf("======= Virtual File System Simulator started successfully ======\n");
    printf("=================================================================\n");

    // Infinite listening shell
    while(1)
    {
        fflush(stdin);

        strcpy(str,"");

        printf("\n VFSS :> ");
        fgets(str,sizeof(str),stdin);

        iCount = sscanf(str,"%s %s %s %s %s ",Command[0],Command[1],Command[2],Command[3],Command[4]);

        fflush(stdin);

        if(iCount == 1)
        {
            // VFSS :> exit
            if(strcmp(Command[0],"exit") == 0)
            {
                printf("Thankyou for using  VFSS.\n");
                printf("Deallocating all resources of  VFSS.\n");
                
                break;
            }
            // VFSS :> help
            else if(strcmp(Command[0],"help") == 0)
            {   
                DisplayHelp();
            }
            // VFSS :> clear
            else if(strcmp(Command[0],"clear") == 0)
            {
                #ifdef _WIN32
                    system("cls");
                #else
                    system("clear");  
                #endif
            }
            // VFSS :> ls
            else if(strcmp(Command[0],"ls") == 0)
            {
               LsFile();
            }
            else
            {
                printf("command not found\n");
                printf("Please refer help option to get more information\n");
                printf("Please refer manual page of command using man\n");
            }
        }
        else if(iCount == 2)
        {
            // VFSS :> man open
            if(strcmp(Command[0],"man") == 0)
            {
                ManPageDisplay(Command[1]);
            }
            //  VFSS :> ls -a
            else if((strcmp(Command[0],"ls") == 0) && (strcmp(Command[1],"-a") == 0))
            {
                LsFile_All();
            }
            //  VFSS :> stat Ganesh.txt
            else if(strcmp(Command[0],"stat") == 0) 
            {
                iRet = stat_file(Command[1]);

                if(iRet == ERR_FILE_NOT_EXIST)
                {
                    printf("Error : File not exist.\n");
                }
            }
            //  VFSS :> unlink Ganesh.txt
            else if(strcmp(Command[0],"unlink") == 0) 
            {
                iRet = unlink_file(Command[1]);

                if(iRet == ERR_FILE_NOT_EXIST)
                {
                    printf("Error : File not exist.\n");
                }
            }
            //  VFSS :> write 1(fd)
            else if(strcmp(Command[0],"write") == 0) 
            {
                printf("Enter the data that you want to write into the file.\n");
                fgets(InputBuffer,MAXFILESIZE,stdin);

                size = strlen(InputBuffer);

                iRet = write_file(atoi(Command[1]), InputBuffer,size -1);

                if(iRet == ERR_INVALID_PARAMTER)
                {
                    printf("Error : Invalid Parameter.\n");
                    printf("Please refer help option to get more information\n");
                    printf("Please refer manual page of command using man\n");

                }
                else if(iRet == ERR_FILE_NOT_EXIST)
                {
                    printf("Error : File not exist.\n");
                }
                else if(iRet == ERR_PERMISSION_DENIED)
                {
                    printf("Error : There is no permission to write the data.\n");
                }
                else if(iRet == ERR_INSUFFICIENT_SPACE)
                {
                    printf("Error : There is no space to write the data.\n");
                }
                else if(iRet == ERR_FILE_NOT_EXIST)
                {
                    printf("Error : File not exis.\n");
                }
                else
                {
                    printf("%d bytes gets successfully written into the file.\n",iRet);
                    save_file(atoi(Command[1]));
                }
            }
            else
            {
                printf("command not found\n");
                printf("Please refer help option to get more information\n");
                printf("Please refer manual page of command using man\n");
            }
        }
        else if(iCount == 3)
        {
            // VFSS :> creat Ganesh.txt 3
            if(strcmp(Command[0], "creat") == 0)
            {
                iRet = CreateFile(Command[1],atoi(Command[2]));

                if(iRet == ERR_NO_INODES)
                {
                    printf("Error : Unable to create new file.\n");
                    printf("Because there is no free inode.\n");
                }
                else if(iRet == ERR_INVALID_PARAMTER)
                {
                    printf("Error : Unable to create new file.\n");
                    printf("Because parameters of command are Invalid.\n");
                    printf("Please use man page to get actual parameter.\n");
                }
                else if(iRet == ERR_FILE_ALREADY_EXIST)
                {
                    printf("Error : Unable to create new file.\n");
                    printf("Because file name is already present .\n");
                    printf("Please use ls command to check names of all files.\n");
                }
                else if(iRet == ERR_MAX_FILES_OPEN)
                {
                    printf("Error : Unable to create new file.\n");
                    printf("Because the UFDT full.\n");
                    printf("Please close some opened file.\n");
                }
                else
                {
                    printf("File successfully created with FD : %d\n",iRet);
                }

            }
            
            // VFSS :> read fd 10
            else if(strcmp(Command[0], "read") == 0)
            {
                EmptyBuffer = (char *)malloc(atoi(Command[2]));

                iRet = read_file(atoi(Command[1]),EmptyBuffer, atoi(Command[2]));

                if(iRet == ERR_INVALID_PARAMTER)
                {
                    printf("Error : Invalid Parameter.\n");
                    printf("Please refer help option to get more information\n");
                    printf("Please refer manual page of command using man\n");

                }
                else if(iRet == ERR_INSUFFICIENT_DATA)
                {
                    printf("Error : Insuffiecent data\n");
                }
                else if(iRet == ERR_PERMISSION_DENIED)
                {
                    printf("Error : permission not found\n");
                }
                else if(iRet == ERR_FILE_NOT_EXIST)
                {
                    printf("Error : file not found\n");
                }
                else
                {
                    printf("Read operation is successfull : \n");
                    printf("Data from file is : %s",EmptyBuffer);
                }
            }
            else
            {
                printf("command not found\n");
                printf("Please refer help option to get more information\n");
                printf("Please refer manual page of command using man\n");
            }
        }
        else if(iCount == 4)
        {
            
        }
        else
        {
            printf("command not found\n");
            printf("Please refer help option to get more information\n");
            printf("Please refer manual page of command using man\n");
        }

    }   // End of while

    return 0;
}   // End of main