#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

struct Record{
    char Name[100];
    char Place[50];
    char Number[50];
    char Mode[50];
};
struct ExpeditionData{
    int RecordNumber;
    int Limit;
    char PlaceName[50];
    int pipefd[2]; //pipefd[0] olvasásra //pipe[1] írásra
};
struct MessageData{
    int Limit;
    char *PlaceName;
};
struct msg_t
{
    long mtype;
    struct MessageData data;
};

void printMenu(const int WhichMenu) //Prints a menu (1 - main menu, 2 - ModifyMenu)
{
    if(WhichMenu == 1)
    {
        fprintf(stderr,"__________MAIN MENU__________\n");
        fprintf(stderr,"a - Add Line\n");
        fprintf(stderr,"m - Modify Line\n");
        fprintf(stderr,"r - Remove Line\n");
        fprintf(stderr,"l - List by Place\n");
        fprintf(stderr,"q - Quit Program\n");
        fprintf(stderr,"(d - Display Current File)\n");
        fprintf(stderr,"_____________________________\n");
        fprintf(stderr,"Enter an option. \n");
    }
    else if(WhichMenu == 2)
    {
        fprintf(stderr,"__________MODIFY MENU__________\n");
        fprintf(stderr,"n - Name\n");
        fprintf(stderr,"p - Place\n");
        fprintf(stderr,"t - Tel. Number\n");
        fprintf(stderr,"m - Means of Travel\n");
        fprintf(stderr,"_______________________________\n");
        fprintf(stderr,"Enter an option. \n");
    }
}
char readSelection() //Reads in the menu selection returns the chosen character
{
    char buffer[15];
    scanf(" %15[^\n]s", &buffer);
    char MenuSelection;
    if(strlen(buffer)>1)
    {
    MenuSelection = 'x';
    }
    else
    {
        MenuSelection = buffer[0];
    }
    return MenuSelection;
}
void ReadFile(const int argNumOfRecords, struct Record *argTempRecords) //Reads the file and puts in the the array given as the arument
{
    FILE *Fileptr = fopen("Output02.txt", "r");
    if(Fileptr != NULL)
    {
        //Read the file
        for (int i = 0; i < argNumOfRecords; i++)
        {
            fscanf(Fileptr, "%[^_]", &argTempRecords[i].Name);
            fseek(Fileptr, 1, SEEK_CUR);
            fscanf(Fileptr, "%[^_]", &argTempRecords[i].Place);
            fseek(Fileptr, 1, SEEK_CUR);
            fscanf(Fileptr, "%[^_]", &argTempRecords[i].Number);
            fseek(Fileptr, 1, SEEK_CUR);
            fscanf(Fileptr, "%[^\n]", &argTempRecords[i].Mode);
            fseek(Fileptr, 1, SEEK_CUR);
        }
    }
    fclose(Fileptr);
}
void WriteFile(char *OverWriteString)
{
    FILE *Fileptr = fopen("Output02.txt", "w");
    fclose(Fileptr);// Force a new file;
    Fileptr = fopen("Output02.txt", "w");
    fputs(OverWriteString, Fileptr);
    fclose(Fileptr);
}
int AddLine(const int argNumOfRecords, int *argCurrentNums) //Appends a line to Output2.txt and increases the right limitnumber. Returns the new recordnumber.
{
    struct Record AddLineRecord;
    //Name
    fprintf(stderr,"Enter a Name:\n");
    scanf(" %100[^\n]s", &AddLineRecord.Name);
    scanf("%*[^\n]%*c");
    //Place
    char Places[5][50] = {"Bali", "Mali", "Cook szigetek", "Bahamák", "Izland"};
    int bIsValid = 0;
    do{
        fprintf(stderr,"Enter a Place:\n");
        scanf(" %15[^\n]s", &AddLineRecord.Place);
        scanf("%*[^\n]%*c");
        for (int i=0; i<5;i++)
        {
            if(strcmp(Places[i],AddLineRecord.Place) == 0)
            {
                bIsValid = 1;
            }
        }
        if(bIsValid == 0)
        {
            fprintf(stderr,"Invalid Place!\nValid Places are: Mali, Bali, Cook szigetek, Bahamák, Izland\n");
        }
    }while(bIsValid == 0);
    //Tel. number
    printf("Enter a Telephone Number:\n");
    scanf(" %11[^\n]s", &AddLineRecord.Number);
    scanf("%*[^\n]%*c");
    //Mode of travel
    char Modes[3][50] = {"repülő", "hajó", "autóbusz"};
    bIsValid = 0;
    do{
        fprintf(stderr,"Enter a means of transport:\n");
        scanf(" %15[^\n]s", &AddLineRecord.Mode);
        for (int i=0; i<3;i++)
        {
            if(strcmp(Modes[i],AddLineRecord.Mode) == 0)
            {
                bIsValid = 1;
            }
        }
        if(bIsValid == 0)
        {
            fprintf(stderr,"Invalid means of transport!\nValid means of transport are: repülő, hajó, autóbusz \n");
        }
    }while(bIsValid == 0);
    //Increasing the right current limit
    if(strcmp(AddLineRecord.Place, "Bali") == 0)
    {
        *(argCurrentNums) += 1;
    }
    else if(strcmp(AddLineRecord.Place, "Mali") == 0)
    {
        *(argCurrentNums+1) += 1;
    }
    else if(strcmp(AddLineRecord.Place, "Cook szigetek") == 0)
    {
        *(argCurrentNums+2) += 1;
    }
    else if(strcmp(AddLineRecord.Place, "Bahamák")== 0)
    {
        *(argCurrentNums+3) += 1;
    }
    else if(strcmp(AddLineRecord.Place, "Izland") == 0)
    {
        *(argCurrentNums+4) += 1;
    }
    //Formatting the record for writing
    strcpy(AddLineRecord.Name, strcat(AddLineRecord.Name,"_")); //Name_
    strcpy(AddLineRecord.Place, strcat(AddLineRecord.Place,"_")); //Place_
    strcpy(AddLineRecord.Number, strcat(AddLineRecord.Number,"_")); //Number_
    strcpy(AddLineRecord.Mode, strcat(AddLineRecord.Mode, "\n")); //Mode\n
    //Creating a string to append to file
    char AppendString[200];
    strcpy(AppendString, AddLineRecord.Name);
    strcpy(AppendString, strcat(AppendString,AddLineRecord.Place));
    strcpy(AppendString, strcat(AppendString,AddLineRecord.Number));
    strcpy(AppendString, strcat(AppendString,AddLineRecord.Mode));
    //Append the string to the file
    FILE *Fileptr = fopen("Output02.txt", "a");
    if (Fileptr != NULL)
    {
        fputs(AppendString, Fileptr);
        fclose(Fileptr);
    }
    else
    {
        printf("File does not exist!");
    }
    return argNumOfRecords+1;
}
void ModifyFile(const int argNumOfRecords) //Allows for the modification of current records via names
{
    struct Record Records[argNumOfRecords];
    ReadFile(argNumOfRecords, Records);
    //Ask which name to modify
    fprintf(stderr,"Which record would you like to modify?\nEnter a name:\n");
    char ChosenName[100];
    scanf(" %100[^\n]s", &ChosenName);
    scanf("%*[^\n]%*c");
    //
    int bChosenNameIsValid = 0;
    int ChosenRecordIndex;

    int cv = 0;
    do
    {
        if(strcmp(Records[cv].Name, ChosenName) == 0)
        {
            bChosenNameIsValid = 1;
            ChosenRecordIndex = cv;
            cv = -1;
        }
        else{
            cv++;
        }
    } while (cv != argNumOfRecords && cv != -1);
    if(bChosenNameIsValid)
    {
        int bCanContinue = 0;
        do
        {
            printMenu(2);
            char SelectedMenu = readSelection();
            switch (SelectedMenu)
            {
            case 'n':
                bCanContinue = 1;
                fprintf(stderr,"Enter the new Name:\n");
                scanf(" %100[^\n]s", &Records[ChosenRecordIndex].Name);
                scanf("%*[^\n]%*c");
                break;
            case 'p':
                bCanContinue = 1;
                fprintf(stderr,"Enter the new Place:\n");
                char Places[5][50] = {"Bali", "Mali", "Cook szigetek", "Bahamák", "Izland"};
                int bIsValid = 0;
                do{
                    
                    scanf(" %15[^\n]s", &Records[ChosenRecordIndex].Place);
                    scanf("%*[^\n]%*c");
                    for (int i=0; i<5;i++)
                    {
                        if(strcmp(Places[i],Records[ChosenRecordIndex].Place) == 0)
                        {
                            bIsValid = 1;
                        }
                    }
                    if(bIsValid == 0)
                    {
                        fprintf(stderr,"Invalid place Enter a correct place!\nValid Places are: Mali, Bali, Cook szigetek, Bahamák, Izland\n");
                    }
                }while(bIsValid == 0);
                break;
            case 't':
                bCanContinue = 1;
                fprintf(stderr,"Enter the new Number:\n");
                scanf(" %11[^\n]s", &Records[ChosenRecordIndex].Number);
                scanf("%*[^\n]%*c");
                break;
            case 'm':
                bCanContinue = 1;
                fprintf(stderr,"Enter the new means of travel:\n");
                char Modes[3][50] = {"repülő", "hajó", "autóbusz"};
                bIsValid = 0;
                do{
                    scanf(" %15[^\n]s", &Records[ChosenRecordIndex].Mode);
                    for (int i=0; i<3;i++)
                    {
                        if(strcmp(Modes[i],Records[ChosenRecordIndex].Mode) == 0)
                        {
                            bIsValid = 1;
                        }
                    }
                    if(bIsValid == 0)
                    {
                        fprintf(stderr,"Invalid means of transport Enter a correct one!\nValid means of transport are: repülő, hajó, autóbusz \n");
                    }
                }while(bIsValid == 0);
                break;
            default:
                fprintf(stderr,"Select a valid option (n,p,t,m)\n");
                break;
            }
        } while (bCanContinue == 0);
        //Creating a new string for the file
        char ModifiedString[argNumOfRecords*250]; //NumberOfRecords * Maximum length of a record
        strcpy(ModifiedString, "Start");
        for(int i = 0; i<argNumOfRecords;i++)
        {
            char FormattedRecord[250];
            strcpy(FormattedRecord, strcat(Records[i].Name,"_"));
            strcat(FormattedRecord, strcat(Records[i].Place,"_"));
            strcat(FormattedRecord, strcat(Records[i].Number,"_"));
            strcat(FormattedRecord, strcat(Records[i].Mode,"\n"));
            if(strcmp(ModifiedString,"Start")==0)
            {
                strcpy(ModifiedString, FormattedRecord);
            }
            else
            {
                strcat(ModifiedString, FormattedRecord);
            }
        }
        //Overwriting the file
        WriteFile(ModifiedString);
    }
    else
    {
        fprintf(stderr,"No such name found!\n");
    }
    
}
int RemoveRecordByName(const int argNumOfRecords,char *argRecordName) //Removes the first record with the name given as a parameter
{
    struct Record Records[argNumOfRecords];
    ReadFile(argNumOfRecords, Records);


    int bChosenNameIsValid = 0;
    int ChosenRecordIndex;

    int cv = 0;
    do
    {
        if(strcmp(Records[cv].Name, argRecordName) == 0)
        {
            bChosenNameIsValid = 1;
            ChosenRecordIndex = cv;
            cv = -1;
        }
        else
        {
            cv++;
        }
        
    } while (cv != argNumOfRecords && cv != -1);
    if(bChosenNameIsValid == 1)
    {
        char RemovedString [argNumOfRecords*250];
        if(argNumOfRecords != 1)
        {
            strcpy(RemovedString, "Start");
        }
        for(int i = 0; i<argNumOfRecords;i++)
        {
            if(i != ChosenRecordIndex)
            {
                char FormattedRecord[250];
                strcpy(FormattedRecord, strcat(Records[i].Name,"_"));
                strcat(FormattedRecord, strcat(Records[i].Place,"_"));
                strcat(FormattedRecord, strcat(Records[i].Number,"_"));
                strcat(FormattedRecord, strcat(Records[i].Mode,"\n"));
                if(strcmp(RemovedString,"Start")==0 || argNumOfRecords == 1)
                {
                    strcpy(RemovedString, FormattedRecord);
                }
                else
                {
                    strcat(RemovedString, FormattedRecord);
                }
            }
        }
        //Overwriting the file
        WriteFile(RemovedString);
        return argNumOfRecords-1;
    }
    else
    {
        fprintf(stderr,"No such name in the records\n");
        return argNumOfRecords;
    }
    
}
void Display(const int argNumOfRecords, const int bDisplayByName)
{
    struct Record Records[argNumOfRecords];
    ReadFile(argNumOfRecords, Records);
    char ChosenPlace[50];
    if(bDisplayByName == 1)
    {
        char Places[5][50] = {"Bali", "Mali", "Cook szigetek", "Bahamák", "Izland"};
        int bIsValid = 0;
        do{
            
            scanf(" %15[^\n]s", &ChosenPlace);
            scanf("%*[^\n]%*c");
            for (int i=0; i<5;i++)
            {
                if(strcmp(Places[i],ChosenPlace) == 0)
                {
                    bIsValid = 1;
                }
            }
            if(bIsValid == 0)
            {
                fprintf(stderr,"Invalid place Enter a correct place!\nValid Places are: Mali, Bali, Cook szigetek, Bahamák, Izland\n");
            }
        }while(bIsValid == 0);
    }
    fprintf(stderr,"__________CURRENT RECORDS__________\n");
        for (int i = 0; i < argNumOfRecords; i++)
        {
            if(bDisplayByName == 0 || (bDisplayByName == 1 && strcmp(ChosenPlace, Records[i].Place) == 0))
            {
                fprintf(stderr,"Name: %s, Place: %s, Number: %s, Mode: %s\n",Records[i].Name,Records[i].Place,Records[i].Number,Records[i].Mode);
            }
        }
    fprintf(stderr,"___________________________________\n");
}
int CompareLimits(int *Limitsptr, int *Currentptr)
{
    for(int i=0;i<5;i++)
    {
        if(*(Limitsptr+i) == *(Currentptr+i))
        {
            return i;
        }
    }
    return -1;
}
void GiveNamesThroughPipe(int sig, siginfo_t *si, void *ucontext)
{
    struct ExpeditionData *ExpData= si->si_value.sival_ptr;
    //Creating the NamesString
    struct Record Records[ExpData->RecordNumber];
    ReadFile(ExpData->RecordNumber,Records);
    char NamesString[ExpData->Limit*100];
    int cv=0;
    int ExpLimit=0;
    do
    {
        if(strcmp(Records[cv].Place,ExpData->PlaceName)==0)
        {
            if(ExpLimit == 0)
            {
                strcpy(NamesString,strcat(Records[cv].Name,"_"));
            }
            else if(ExpLimit == ExpData->Limit-1)
            {
                strcat(NamesString, Records[cv].Name);
            }
            else
            {
                strcat(NamesString, strcat(Records[cv].Name,"_"));
            }
            ExpLimit++;
        }
        cv++;
    } while (ExpLimit != ExpData->Limit);
    //NamesString is Created
    //fprintf(stderr,"NAMESSTRING IS : %s\n",NamesString);   

    //PIPEING
    close(ExpData->pipefd[0]); //Close reading end
    write(ExpData->pipefd[1],NamesString,strlen(NamesString)+1);
    close(ExpData->pipefd[1]); //Close writing end
}
void Sighandler(int sig)
{
    key_t Key = ftok(".", 1);
    int MsgID = msgget(Key,0666 | IPC_CREAT);
    if(MsgID != -1)
    {
        struct msg_t ReceivedData;
        msgrcv(MsgID,&ReceivedData, sizeof(ReceivedData.data), 0,0);
        fprintf(stderr,"RECEIVED: %i AND %s\n",ReceivedData.data.Limit, ReceivedData.data.PlaceName);
        msgctl(MsgID,IPC_RMID, NULL);
    }
    else
    {
        fprintf(stderr,"Error while receiving message\n");
    }
    
}
int main()
{
    //Initializing
    int Limits[5] = {2,4,3,4,5}; //Bali[0] __ Mali[1] __ Cook szigetek[2] __ Bahamák[3] __ Izland[4]
    int CurrentNums[5] = {0,0,0,0,0};
    int NumOfRecords = 0;
    //Establish handler
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = GiveNamesThroughPipe;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);
    //Establish other handler
    signal(SIGUSR2, Sighandler);
    //Forcing new file
    FILE *Fileptr = fopen("Output02.txt", "w");
    fclose(Fileptr);
    //Main Cycle
    while (1)
    {
        
        int FilledIndex = CompareLimits(Limits, CurrentNums);
        if(FilledIndex != -1) //TALÁLT LIMITET
        {
            CurrentNums[FilledIndex] = 0;
            //GATHERING DATA
            struct ExpeditionData ExpData_t;
            ExpData_t.RecordNumber = NumOfRecords;
            ExpData_t.Limit = Limits[FilledIndex];
            switch (FilledIndex)
            {
            case 0:
                strcpy(ExpData_t.PlaceName,"Bali");
                break;
            case 1:
                strcpy(ExpData_t.PlaceName,"Mali");
                break;
            case 2:
                strcpy(ExpData_t.PlaceName,"Cook szigetek");
                break;
            case 3:
                strcpy(ExpData_t.PlaceName,"Bahamák");
                break;
            case 4:
                strcpy(ExpData_t.PlaceName,"Izland");
                break;
            }
            pipe(ExpData_t.pipefd);
            //Packing the Data
            struct ExpeditionData *ExpData = &ExpData_t;
            union sigval sv;
            sv.sival_int = FilledIndex;
            sv.sival_ptr = ExpData;
            NumOfRecords = NumOfRecords - Limits[FilledIndex];
            if(fork() == 0)
            {
                //In Child
                sigqueue(getppid(), SIGUSR1, sv);
                //PIPEING
                close(ExpData_t.pipefd[1]); // Close writing end of Pipe
                char NamesString[ExpData_t.Limit*100];
                read(ExpData_t.pipefd[0],NamesString, ExpData_t.Limit*100);
                close(ExpData_t.pipefd[0]); // Close reading end of Pipe
                char *NamesFromPipe[ExpData_t.Limit];
                char *token;
                token = strtok(NamesString, "_");
                int cv = 0;
                do
                {
                    NamesFromPipe[cv] = token;
                    token = strtok(NULL, "_"); 
                    cv++;
                } while (token != NULL);
                for(int i=0;i<ExpData_t.Limit;i++)
                {
                    ExpData_t.RecordNumber = RemoveRecordByName(ExpData_t.RecordNumber,NamesFromPipe[i]);
                }

                key_t Key = ftok(".", 1);
                int MsgID = msgget(Key,0666 | IPC_CREAT);
                if(MsgID != -1)
                {   
                    struct msg_t msg;
                    msg.mtype = 1;
                    msg.data.PlaceName = ExpData_t.PlaceName;
                    msg.data.Limit = ExpData_t.Limit;
                    kill(getppid(), SIGUSR2);
                    msgsnd(MsgID, &msg, sizeof(msg.data),0);
                }
                else
                {
                    fprintf(stderr,"Error while messagign\n");
                }
                exit(0);
            }
            else
            {
                //wait(NULL);
            }
        }
        else
        {
            printMenu(1);
        }
        char SelectedMenu = readSelection();
        if(SelectedMenu != '\0')
        {
            switch (SelectedMenu)
            {
            case 'q':
                exit(0);
                break;
            case 'a':
                NumOfRecords = AddLine(NumOfRecords,CurrentNums);
                break;
            case 'm':
                if(NumOfRecords != 0)
                {
                    Display(NumOfRecords, 0);
                    ModifyFile(NumOfRecords);
                }
                else
                {
                    fprintf(stderr,"No record to modify\n");
                }
                break;
            case 'r':
                if(NumOfRecords != 0)
                {
                    Display(NumOfRecords, 0);
                    fprintf(stderr,"Which record would you like to remove?\nEnter a name:\n");
                    char ChosenName[100];
                    scanf(" %100[^\n]s", &ChosenName);
                    scanf("%*[^\n]%*c");
                    NumOfRecords = RemoveRecordByName(NumOfRecords,ChosenName);
                }
                else
                {
                    fprintf(stderr,"No recrod to remove\n");
                }
                
                break;
            case 'd':
                Display(NumOfRecords, 0);
                break;
            case 'l':
                if(NumOfRecords != 0)
                {
                    Display(NumOfRecords, 1);
                }
                else
                {
                    fprintf(stderr, "There are no saved records\n");
                }
            default:
                fprintf(stderr,"Select a valid option (a,m,r,d,l,q)\n");
                break;
            }
        }
        
        
    }
}