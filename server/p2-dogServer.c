/*
    - Oscar Julian Tinjaca Reyes        - ojtinjacar@unal.edu.co
    - Guiselle Tatiana Zambrano Penagos - gtzambranop@unal.edu.co
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>



#define SIZETABLE 15803
#define PORT 3535
#define STDINFD  0
#define SIZEDOGTYPE sizeof(dogType)
#define SIZENODE sizeof(node)
#define BACKLOG 32

// structs

typedef struct dogType dogType;

struct dogType {
    int nextPosition;

    char name[32];
    char type[32];
    int age;
    char race[16];
    int height;
    float weight;
    char sex;
    char clinicHistory[100]; 
};

struct clientPackage
{
    int count;
    int tmpClientfd;
    struct sockaddr_in tmpClient;
};

//global variables

pthread_t thread[BACKLOG];
int clientArray[BACKLOG];

char nameDataBase[] = "dataBase.dat";
char nameBinFile[] = "dataDogs.dat";
char nameBinTable[] = "hashTable.dat";
char nameAuxBinFile[] = "auxDataDogs.dat";
char nameClinicHistory[] = "clinicHistory.txt";
char nameDocServer[] = "serverDogs.log";

//functions

long int getBytesFile(char nameFile[]);
void checkFiles();
long int getCodeName(char name[]);
int functionHash(long codeName);
void addInFile(void* newDog, int headList, int signal);
void addInTable(void* newDog);
void* consultAnimal(int numAnimal);
void* modifyStruct(void* animal, int positionInFile);
void deleteBinTable(int posInTable, int newHeadList, int posDelete);
void findAnimalToDelete(int positionDelete);
int searchAnimal(char petName[]);
int consultAnimalSearched(char petName[], int numberToConsult);
void generateRandomAnimal(int numAnimals);
void clientMenu(void* tempClient);
void registerOperation(char ip[], char action[], char array[]);


long int getBytesFile(char nameFile[])
{
    FILE* fl;
    long int bytes;

    fl = fopen(nameFile, "rb");

    if (fl == NULL)
    {
        perror("Error fopen");
        exit(-1);
    }

    fseek(fl, 0, SEEK_END);
    bytes = ftell(fl);
    fclose(fl);

    return bytes;
}

void checkFiles()
{
    FILE* fl;

    int number  = 0,
        size    = 0,
        r       = 0,
        q       = 0,
        i       = 0;

    system("clear");

    size = getBytesFile(nameDocServer);

    if (size > 0)
    {
        size = getBytesFile(nameBinFile) / SIZEDOGTYPE;
        if (size > 0)
        {
            printf("Hay %i animales registrados, ¿desea restaurarlos?\n 1. Si. \n 2. No.\n", size);
            scanf(" %i", &number);
            if (number == 2)
            {
                fl = fopen(nameBinTable, "wb"); 
                if (fl == NULL)
                {
                    perror("Error fopen");
                    exit(-1);
                }

                r = -1;
                for (i = 0; i < SIZETABLE; ++i)
                {
                    q = fwrite(&r, sizeof(r), 1, fl);
                    if (q < 0)
                    {
                        perror("Error fwrite 1 in function checkFiles.");
                        exit(-1);
                    }
                }
                fclose(fl);

                fl = fopen(nameBinFile, "wb"); 
                if (fl == NULL)
                {
                    perror("Error fopen");
                    exit(-1);
                }
                fclose(fl);

                fl = fopen(nameDocServer, "w"); 
                if (fl == NULL)
                {
                    perror("Error fopen");
                    exit(-1);
                }
                fclose(fl);
            }
        }else
        {
            printf("No hay animales registrados, sin embargo, existe el historial de operaciones \nreaizadas. ¿Desea restaurarlo?\n 1. Si.\n 2. No.\n");
            scanf(" %i", &number);
            if (number == 2)
            {
                fl = fopen(nameDocServer, "w"); 
                if (fl == NULL)
                {
                    perror("Error fopen");
                    exit(-1);
                }
                fclose(fl);
            }
                 
        }
    }else
    {
        fl = fopen(nameBinTable, "wb"); 
        if (fl == NULL)
        {
            perror("Error fopen");
            exit(-1);
        }

        r = -1;
        for (i = 0; i < SIZETABLE; ++i)
        {
            q = fwrite(&r, sizeof(r), 1, fl);
            if (q < 0)
            {
                perror("Error fwrite 2 in function checkFiles.");
                exit(-1);
            }
        }
        fclose(fl);

        fl = fopen(nameBinFile, "wb"); 
        if (fl == NULL)
        {
            perror("Error fopen");
            exit(-1);
        }
        fclose(fl);

        fl = fopen(nameDocServer, "w"); 
        if (fl == NULL)
        {
            perror("Error fopen");
            exit(-1);
        }
        fclose(fl);
    }
    system("clear");
}

long int getCodeName(char name[])
{
    long d      = 0;
    long ascii  = 0;

    for (int j = 0; j < strlen(name); j++) {

        ascii = name[j];

        d = d * 21 + ascii * 5;
       
    }
    return ((d >= 0) ? d : -d);
}

int functionHash(long codeName)
{
    return codeName % SIZETABLE;
}

void addInFile(void* newDog, int headList, int signal)
{
    FILE* fl;
    int numAnimals  = 0,
        nextNode    = 0,
        endList     = 0,
        q           = 0;

    dogType *tempDog;
    dogType *auxDog;

    tempDog = (dogType*)newDog;    
    numAnimals = getBytesFile(nameBinFile) / SIZEDOGTYPE;
    endList = -1;

    if (signal == 0)
    {
        auxDog = malloc(SIZEDOGTYPE);
        if (auxDog == NULL)
        {
            perror("malloc in function addInFile");
            exit(-1);
        }

        fl = fopen(nameBinFile, "rb+");
        if (fl == NULL) {
            perror("Error fopen 2 in function addInFile");
            exit(-1);
        }

        nextNode = headList;
        do
        {
            q = fseek(fl, SIZEDOGTYPE * nextNode, SEEK_SET);
            if (q < 0)
            {
                perror("Error fseek 1 in function addInFile");
                exit(-1);
            }

            q = fread(auxDog, SIZEDOGTYPE, 1, fl);
            if (q < 0)
            {
                perror("Error fread 2 in function addInFile");
                exit(-1);
            }
            endList = auxDog->nextPosition;

            if (endList < 0)
            {
                auxDog->nextPosition = numAnimals;

                q = fseek(fl, SIZEDOGTYPE * nextNode, SEEK_SET);
                if (q < 0)
                {
                    perror("Error fseek 4 in function addInFile");
                    exit(-1);
                }

                q = fwrite(auxDog, SIZEDOGTYPE, 1, fl);
                if (q < 0)
                {
                    perror("Error fread 3 in function addInFile");
                    exit(-1);
                }
            }else
            {
                nextNode = endList;
            }
        }while(endList > 0);

        fclose(fl);
        free(auxDog);
    }

    fl = fopen(nameBinFile, "ab");
    if (fl == NULL) {
        perror("Error fopen 3 in function addInFile");
        exit(-1);
    }

    q = fwrite(tempDog, SIZEDOGTYPE, 1, fl); 
    if (q == 0) {
        perror("Error fwrite 2 in function addInFile");
        exit(-1);
    }

    fclose(fl);
    free(tempDog);
}

void addInTable(void* newDog)
{
    FILE* fl;

    int posInTable  = 0,
        signal      = 0, 
        headList    = 0,
        nextNode    = 0,
        endList     = 0,
        numAnimals  = 0,
        q           = 0;
    long codeName   = 0;
    char tempName[32] = "";

    dogType *tempDog;

    tempDog = (dogType*) newDog;

    strcpy(tempName, tempDog->name);
    codeName = getCodeName(tempName);
    posInTable = functionHash(codeName);

    numAnimals = getBytesFile(nameBinFile) / SIZEDOGTYPE;

    fl = fopen(nameBinTable, "rb+");
    if (fl == NULL) {
        perror("Error fopen 1 in function addInFile");
        exit(-1);
    }

    q = fseek(fl, sizeof(int) * posInTable, SEEK_SET);
    if (q < 0)
    {
        perror("Error fseek 1 in function addInTable");
        exit(-1);
    }

    q = fread(&headList, sizeof(headList), 1, fl);
    if (q < 0)
    {
        perror("Error fread 1 in function addInTable");
        exit(-1);
    }

    if (headList < 0)
    {
        headList = numAnimals;

        q = fseek(fl, sizeof(headList) * posInTable, SEEK_SET);
        if (q < 0)
        {
            perror("Error fseek 2 in function addInTable");
            exit(-1);
        }

        q = fwrite(&headList, sizeof(headList), 1, fl);
        if (q < 0)
        {
            perror("Error fwrite 1 in function addInTable");
            exit(-1);
        }
        signal = 1; //there is no head
    }

    fclose(fl);
    addInFile(tempDog, headList, signal);
}

void* consultAnimal(int numAnimal)
{
    FILE* fl;

    int b = 0;

    dogType* tempDog;
 
    tempDog = malloc(SIZEDOGTYPE);
    if (tempDog == NULL)
    {
        perror("Error malloc in function consultAnimal");
        exit(-1);
    }

    fl = fopen(nameBinFile, "rb");
    if (fl == NULL)
    {
        perror("Error fopen in function consultAnimal");
        exit(-1);
    }

    b = fseek(fl, numAnimal * SIZEDOGTYPE, SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek in function consultAnimal");
        exit(-1);
    }

    b = fread(tempDog, SIZEDOGTYPE, 1, fl);
    if (b == 0)
    {
        perror("Error fread in function consultAnimal");
        exit(-1);
    }

    fclose(fl);
    return tempDog;
}

void* modifyStruct(void* animal, int positionInFile)
{
    FILE* fl;

    int r = 0;
    dogType* tempDog;
    dogType* newDog;

    newDog = (dogType*)animal;
    
    fl = fopen(nameBinFile,"rb+");
    if (fl == NULL)
    {
        perror("Error fopen 1 in function modifyStruct");
        exit(-1);
    }

    r = fseek(fl, (SIZEDOGTYPE * positionInFile), SEEK_SET);
    if (r < 0)
    {
        perror("Error fseek 2 in funcion modifyStruct");
    }

    r = fwrite(newDog, SIZEDOGTYPE, 1, fl);
    if (r < 0)
    {
        perror("Error fwrite in funcion modifyStruct");
    }

    fclose(fl);
    
    free(tempDog);
    return newDog;
}

void deleteBinTable(int posInTable, int newHeadList, int posDelete)
{
    FILE* fl;   
    int tempHeadList    = 0,
        auxHeadList     = 0,
        i               = 0,
        b               = 0;  
    
    fl = fopen(nameBinTable, "rb");
    if (fl == NULL)
    {
        perror("Error fopen 1 in function deleteBinTable");
        exit(-1);
    }

    b = fseek(fl, posInTable * sizeof(int), SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek 1 in function deleteBinTable");
        exit(-1);
    }

    b = fread(&tempHeadList, sizeof(int), 1, fl);
    if (b < 0)
    {
        perror("Error fread 1 in function deleteBinTable");
        exit(-1);
    }

    fclose(fl);

    if (tempHeadList == posDelete)
    {
        fl = fopen(nameBinTable, "rb+");
        if (fl == NULL)
        {
            perror("Error fopen 1 in function deleteBinTable");
            exit(-1);
        }

        for (i = 0; i < SIZETABLE; ++i)
        {
            if (i == posInTable)
            {                
                b = fwrite(&newHeadList, sizeof(int), 1, fl);
                if (b <0)
                {
                    perror("Error write 1 in function deleteBinTable");
                    exit(-1);
                }
            }else
            {
                auxHeadList = 0;
                b = fread(&auxHeadList, sizeof(int), 1, fl);
                if (b < 0)
                {
                    perror("Error fread 2 in function deleteBinTable");
                    exit(-1);
                }

                if (auxHeadList > posDelete)
                {
                    b = fseek(fl, i * sizeof(int), SEEK_SET);
                    if (b < 0)
                    {
                        perror("Error fseek 2 in function deleteBinTable");
                        exit(-1);
                    }
                    auxHeadList --;

                    b = fwrite(&auxHeadList, sizeof(int), 1, fl);
                    if (b <0)
                    {
                        perror("Error write 2 in function deleteBinTable");
                        exit(-1);
                    }

                }
            }
        }
    }

    fclose(fl);
}

void findAnimalToDelete(int positionDelete)
{
    FILE* fl;
    FILE* auxfl;

    int tempNextPosition    = 0,
        newNextPosition     = 0,
        numberAnimals       = 0,
        newHeadList         = 0,
        posInTable          = 0,
        count               = 0,
        b                   = 0;
    long codeName           = 0;
    char tempName[32] = "";
    char command[40] = "rm -r dataDogs.dat";
    dogType* animalTemp;



    numberAnimals = getBytesFile(nameBinFile) / SIZEDOGTYPE;

    fl = fopen(nameBinFile, "rb");
    if (fl == NULL)
    {
        perror("Error fopen 1 in function findAnimalToDelete");
        exit(-1);
    }    

    auxfl = fopen(nameAuxBinFile,  "wb+");
    if (auxfl == NULL)
    {
        perror("Error fopen 2 in function findAnimalToDelete");
        exit(-1);
    }

    animalTemp = malloc(SIZEDOGTYPE);
    if(animalTemp == NULL){
        perror("Error malloc in function findAnimalToDelete");
        exit(-1);
    }

    b = fseek(fl, positionDelete * SIZEDOGTYPE, SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek 1 in function findAnimalToDelete");
        exit(-1);
    }

    b = fread(animalTemp, SIZEDOGTYPE, 1, fl);
    if (b == 0)
    {
        perror("Error fread 1 in function findAnimalToDelete");
        exit(-1);
    }

    strcpy(tempName, animalTemp->name);
    codeName = getCodeName(tempName);
    posInTable = functionHash(codeName);

    newHeadList = animalTemp->nextPosition;

    if (newHeadList > 0)
    {
        newHeadList --;
    }

    deleteBinTable(posInTable, newHeadList, positionDelete);

    b = fseek(fl, 0, SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek 2 in function findAnimalToDelete");
        exit(-1);
    }

    while(count < numberAnimals)
    {
        if (count == positionDelete)
        {
            b = fread(animalTemp, SIZEDOGTYPE, 1, fl);
            if (b == 0)
            {
                perror("Error fread 2 in function findAnimalToDelete");
                exit(-1);
            }
            count ++;
        }

        if (count < numberAnimals)
        {
            b = fread(animalTemp, SIZEDOGTYPE, 1, fl);
            if (b == 0)
            {
                perror("Error fread 3 in function findAnimalToDelete");
                exit(-1);
            }

            tempNextPosition = animalTemp->nextPosition;
            newNextPosition = tempNextPosition;

            if (tempNextPosition >= positionDelete)
            {
                if (tempNextPosition == positionDelete)
                {
                    newNextPosition = newHeadList;
                }else
                {
                    newNextPosition = tempNextPosition - 1; 
                }
            }
            
            animalTemp->nextPosition = newNextPosition;

            b = fwrite(animalTemp, SIZEDOGTYPE, 1, auxfl); 
            if (b == 0)
            {
                perror("Error fwrite in function findAnimalToDelete");
                exit(-1);
            }   
            count ++;
        }       
    }

    b = system(command);
    if (b < 0){
        perror("Error system in function findAnimalToDelete");
        exit(-1);
    }
    
    if (rename(nameAuxBinFile, nameBinFile) != 0)
    {
        perror("Error rename in function findAnimalToDelete");
        exit(-1);
    }
    fclose(fl);
    fclose(auxfl);
    free(animalTemp);
}

int searchAnimal(char petName[])
{
    FILE* fl;
    long codeName   = 0;
    int posInTable  = 0,
        numAnimals  = 0,
        posInFile   = 0,
        nextPos     = 0,
        b           = 0;
    dogType* tempDog;

    codeName = getCodeName(petName);
    posInTable = functionHash(codeName);

    tempDog = malloc(SIZEDOGTYPE);
    if (tempDog == NULL)
    {
        perror("Error malloc in function searchAnimal");
        exit(-1);
    }

    fl = fopen(nameBinTable, "rb");
    if (fl == NULL)
    {
        perror("Error fopen 1 in function searchAnimal");
        exit(-1);
    }

    b = fseek(fl, posInTable * sizeof(int), SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek 1 in function searchAnimal");
        exit(-1);
    }

    b = fread(&posInFile, sizeof(int), 1, fl);
    if (b < 0)
    {
        perror("Error fread 1 in function searchAnimal");
        exit(-1);
    }
    fclose(fl);

    if (posInFile >= 0)
    {
        fl = fopen(nameBinFile, "rb");
        if (fl == NULL)
        {
            perror("Error fopen 2 in function searchAnimal");
            exit(-1);
        } 

        nextPos = posInFile;

        while(nextPos >= 0)
        {
            b = fseek(fl, nextPos * SIZEDOGTYPE, SEEK_SET);
            if (b < 0)
            {
                perror("Error fseek 1 in function searchAnimal");
                exit(-1);
            }

            b = fread(tempDog, SIZEDOGTYPE, 1, fl);
            if (b < 0)
            {
                perror("Error fread in function searchAnimal");
                exit(-1);
            }

            if (strcmp(tempDog->name, petName) == 0)
            {
                numAnimals ++;
            }
            nextPos = tempDog->nextPosition;
        }
        fclose(fl);
    }

    free(tempDog);
    return numAnimals;
}

int consultAnimalSearched(char petName[], int numberToConsult)
{
    FILE* fl;

    long codeName   = 0;
    int posInFile   = 0,
        posInTable  = 0,
        nextPos     = 0,
        count       = 0,
        b           = 0;

    dogType* tempDog;

    codeName = getCodeName(petName);
    posInTable = functionHash(codeName);

    tempDog = malloc(SIZEDOGTYPE);
    if (tempDog == NULL)
    {
        perror("Error malloc in function consultAnimalSearched");
        exit(-1);
    }

    fl = fopen(nameBinTable, "rb");
    if (fl == NULL)
    {
        perror("Error fopen 1 in function consultAnimalSearched");
        exit(-1);
    }

    b = fseek(fl, posInTable * sizeof(int), SEEK_SET);
    if (b < 0)
    {
        perror("Error fseek 1 in function consultAnimalSearched");
        exit(-1);
    }

    b = fread(&posInFile, sizeof(int), 1, fl);
    if (b < 0)
    {
        perror("Error fread 1 in function consultAnimalSearched");
        exit(-1);
    }
    fclose(fl);

    fl = fopen(nameBinFile, "rb");
    if (fl == NULL)
    {
        perror("Error fopen 2 in function consultAnimalSearched");
        exit(-1);
    } 

    nextPos = posInFile;

    while(count < numberToConsult)
    {
        b = fseek(fl, nextPos * SIZEDOGTYPE, SEEK_SET);
        if (b < 0)
        {
            perror("Error fseek 2 in function consultAnimalSearched");
            exit(-1);
        }

        b = fread(tempDog, SIZEDOGTYPE, 1, fl);
        if (b < 0)
        {
            perror("Error fread 2 in function consultAnimalSearched");
            exit(-1);
        }

        if (strcmp(tempDog->name, petName) == 0)
        {
            count ++;
        }

        posInFile = nextPos;
        nextPos = tempDog->nextPosition;
    }
    fclose(fl);

    free(tempDog);
    consultAnimal(posInFile);
    return posInFile;
}

void generateRandomAnimal(int numAnimals)
{
    FILE* fl;

    int count           = 0,
        rdmName         = 0,
        rdmType         = 0,
        rdmAge          = 0,
        rdmRace         = 0,
        rdmHeight       = 0,
        rdmSex          = 0,
        i               = 0,    
        r               = 0;
    float rdmWeight     = 0;
    char buffer[32];
    char tempClinicH[100] = "";
    char character = 'M';

    dogType* tempDog;

    srand(time(NULL));
    srand48(time(NULL));


    for (i = 0; i < numAnimals; ++i)
    {
        tempDog = malloc(SIZEDOGTYPE);
        if (tempDog == NULL)
        {
            perror("Error malloc in function generateRandomAnimal");
            exit(-1);
        }

        strcpy(tempClinicH, "");

        fl = fopen(nameDataBase, "rb");
        if (fl == NULL)
        {
            perror("Error fopen gnerateRandomAnimal");
            exit(-1);
        }

        rdmName = rand()%(21021 + 1);
        rdmType = rand()%(21230 - 21022 + 1) + 21022;
        rdmAge = rand()%(20 - 1 + 1) + 1;
        rdmRace = rand()%(21230 - 21022 + 1) + 21022;
        rdmHeight = rand()%(100 -10 + 1) + 10;
        rdmWeight = drand48()*(30.0 + 1.0 - 1.0) + 1.0 ;
        rdmSex = rand()%(1 + 1);

        r = fseek(fl, rdmName*sizeof(buffer), SEEK_SET);
        if (r < 0)
        {
            perror("Error fseek 1 in function generateRandomAnimal");
            exit(-1);
        }

        r = fread(buffer, sizeof(char), sizeof(buffer), fl);
        if (r == 0)
        {
            perror("Error fread generateRandom rdmName");
            exit(-1);
        }
        for (int i = 0; buffer[i] != '\0'; ++i)
        {
            buffer[i] = tolower(buffer[i]);
        }
        strcpy(tempDog -> name, buffer);

        r = fseek(fl, rdmType*sizeof(buffer), SEEK_SET);
        if (r < 0)
        {
            perror("Error fseek 1 in function generateRandomAnimal");
            exit(-1);
        }

        r = fread(buffer, sizeof(char), sizeof(buffer), fl);
        if (r == 0)
        {
            perror("Error fread generateRandom rdmType");
            exit(-1);
        }
        strcpy(tempDog -> type, buffer);

        r = fseek(fl, rdmRace*sizeof(buffer), SEEK_SET);
        if (r < 0)
        {
            perror("Error fseek 1 in function generateRandomAnimal");
            exit(-1);
        }

        r = fread(buffer, sizeof(char), 16, fl);
        if (r == 0)
        {
            perror("Error fread generateRandom rdmRace");
            exit(-1);
        }
        strcpy(tempDog -> race, buffer);

        if (rdmSex == 0)
        {
            character = 'H';
        }

        strcpy(tempDog -> clinicHistory, tempClinicH);
        tempDog -> nextPosition = -1;

        tempDog -> sex = character;    
        tempDog -> age = rdmAge;
        tempDog -> height = rdmHeight;
        tempDog -> weight = rdmWeight;

        fclose(fl); 
        addInTable(tempDog);  
        //borrar
        printf("%i\n", i+1);
    }    
    printf("\n");
}

void registerOperation(char ip[], char action[], char array[])
{
    FILE *fl;

    time_t t;
    struct tm* ltime;

    char date[32];

    fl = fopen(nameDocServer, "a");
    if (fl == NULL)
    {
        perror("Error fopen");
        exit(-1);
    }

    t = time(0);
    ltime = localtime(&t);
    strftime(date, sizeof(date), "[%Y/%m/%d %H:%M:%S]", ltime);

    if (strcmp(action, "generar aleatorios") == 0)
    {
        fprintf(fl,"%sCliente[%s][%s][Cantidad %s]\n", date, ip, action, array);
    }else
    {
        fprintf(fl,"%sCliente[%s][%s][Registro %s]\n", date, ip, action, array);
    }

    fclose(fl);
}

void clientMenu(void* tempClient)
{
    int animalSelected  = 0, 
        countClient     = 0,   
        clientfd        = 0,
        posInFile       = 0,
        option          = 0,
        sizeFileDogs    = 0,
        numAnimal       = 0,
        r               = 0;

    time_t t;

    char message[64];
    char animalName[32];
    char ip[10];
    char action[20];
    char array[32];
        
    dogType* animal;
    dogType* tempAnimal;
    struct clientPackage* auxClient;
    struct sockaddr_in client;

    auxClient = (struct clientPackage*) tempClient;

    countClient = auxClient->count;
    client = auxClient->tmpClient;
    clientfd = auxClient->tmpClientfd;

    strcpy(ip, inet_ntoa(client.sin_addr));

    animal= malloc(SIZEDOGTYPE);
    if(animal == NULL)
    {
        perror("Error malloc in function clientMenu");
        exit(-1);
    }

    r = send(clientfd, &countClient, sizeof(countClient), 0);
    if (r < 0)
    {
        perror("Error send 2 in function clientMenu");
        exit(-1);
    }

    do{         
        r = recv(clientfd, &option, sizeof(option), 0);
        if (r < 0)
        {
            perror("Error recv 1 in function clientMenu");
            exit(-1);
        }

        switch (option)
        {
            case 1:

                strcpy(action, "insercion");

                tempAnimal = malloc(SIZEDOGTYPE);
                if (tempAnimal == NULL)
                {
                    perror("Error malloc 2 in function clientMenu");
                    exit(-1);
                }

                r = recv(clientfd, tempAnimal, SIZEDOGTYPE, 0);
                if (r < 0)
                {
                    perror("Error recv 2 in function clientMenu");
                } 

                animal = (dogType*)tempAnimal;

                strcpy(array, animal->name);

                addInTable(animal);

                strcpy(message, "El animal ha sido registrado correctamente");
                r = send(clientfd, message, sizeof(message), 0);
                if (r < 0)
                {
                    perror("Error send 2 in function clientMenu");
                    exit(-1);
                }

                registerOperation(ip, action, array);

                break;
            
            case 2:

                strcpy(action, "lectura");

                sizeFileDogs = getBytesFile(nameBinFile)/SIZEDOGTYPE;

                r = send(clientfd, &sizeFileDogs, sizeof(sizeFileDogs), 0);
                if (r < 0)
                {
                    perror("Error send 3 in function clientMenu");
                    exit(-1);
                }
                if (sizeFileDogs > 0)
                {
                    r = recv(clientfd, &numAnimal, sizeof(numAnimal), 0);
                    if (r < 0)
                    {
                        perror("Error recv 3 in function clientMenu");
                    }

                    if (numAnimal > 0)
                    {
                        numAnimal --;
                        animal = consultAnimal(numAnimal);

                        strcpy(array, animal->name);

                        r = send(clientfd, &numAnimal, sizeof(int), 0);
                        if (r < 0)
                        {
                            perror("Error send 4 in function clientMenu");
                            exit(-1);
                        }


                        r = send(clientfd, animal, SIZEDOGTYPE, 0);
                        if (r < 0)
                        {
                            perror("Error send 4 in function clientMenu");
                            exit(-1);
                        }

                        r = recv(clientfd, animal, SIZEDOGTYPE, 0);
                        if (r < 0)
                        {
                            perror("Error recv 4 in function clientMenu");
                        }

                        animal = modifyStruct(animal, numAnimal);  
                        registerOperation(ip, action, array);  
                    }                
                }

                break;

            case 3:

                strcpy(action, "borrado");

                sizeFileDogs = getBytesFile(nameBinFile)/SIZEDOGTYPE;

                r = send(clientfd, &sizeFileDogs, sizeof(sizeFileDogs), 0);
                if (r < 0)
                {
                    perror("Error send 5 in function clientMenu");
                    exit(-1);
                }
                if (sizeFileDogs > 0)
                {
                    r = recv(clientfd, &numAnimal, sizeof(numAnimal), 0);
                    if (r < 0)
                    {
                        perror("Error recv 5 in function clientMenu");
                    }

                    if (numAnimal > 0)
                    {        
                        sprintf(array, "%d", numAnimal); 

                        numAnimal --;
                        findAnimalToDelete(numAnimal);  

                        strcpy(message, "El animal ha sido borrado correctamente");
                        r = send(clientfd, message, sizeof(message), 0);
                        if (r < 0)
                        {
                            perror("Error send 6 in function clientMenu");
                            exit(-1);
                        }

                        registerOperation(ip, action, array);
                    }
                }
                break;

            case 4:

                strcpy(action, "busqueda");

                sizeFileDogs = getBytesFile(nameBinFile)/SIZEDOGTYPE;

                r = send(clientfd, &sizeFileDogs, sizeof(sizeFileDogs), 0);
                if (r < 0)
                {
                    perror("Error send 5 in function clientMenu");
                    exit(-1);
                }
                if (sizeFileDogs > 0)
                {
                    r = recv(clientfd, animalName, sizeof(animalName), 0);
                    if (r < 0)
                    {
                        perror("Error recv 5 in function clientMenu");
                    }
                    numAnimal = searchAnimal(animalName);

                    strcpy(array, animalName);
                    registerOperation(ip, action, array);

                    r = send(clientfd, &numAnimal, sizeof(numAnimal), 0);
                    if (r < 0)
                    {
                        perror("Error send 5 in function clientMenu");
                        exit(-1);
                    }

                    if (numAnimal > 0)
                    {   
                        strcpy(action, "lectura");

                        r = recv(clientfd, &animalSelected, sizeof(animalSelected), 0);
                        if (r < 0)
                        {
                            perror("Error recv 5 in function clientMenu");
                        }

                        if (animalSelected > 0)
                        {
                            posInFile = consultAnimalSearched(animalName, animalSelected);
                            animal = consultAnimal(posInFile);

                            tempAnimal = (dogType*)animal;

                            strcpy(array, tempAnimal->name); 

                            r = send(clientfd, &posInFile, sizeof(int), 0);
                            if (r < 0)
                            {
                                perror("Error send 4 in function clientMenu");
                                exit(-1);
                            }

                            r = send(clientfd, animal, SIZEDOGTYPE, 0);
                            if (r < 0)
                            {
                                perror("Error send 5 in function clientMenu");
                                exit(-1);
                            }

                            r = recv(clientfd, animal, SIZEDOGTYPE, 0);
                            if (r < 0)
                            {
                                perror("Error recv 4 in function clientMenu");
                            }

                            animal = modifyStruct(animal, posInFile);  

                            registerOperation(ip, action, array);   
                        }                   
                    }
                }

                break;
            
            case 5:
                
                strcpy(action, "generar aleatorios");

                r = recv(clientfd, &numAnimal, sizeof(numAnimal), 0);
                if (r < 0)
                {
                    perror("Error recv 4 in function clientMenu");
                }

                if (numAnimal > 0)
                {                    
                    generateRandomAnimal(numAnimal);

                    strcpy(message, "los animales han sido registrados correctamente");

                    r = send(clientfd, message, sizeof(message), 0);
                    if (r < 0)
                    {
                        perror("Error send 6 in function clientMenu");
                        exit(-1);
                    }

                    sprintf(array, "%d", numAnimal); 
                    registerOperation(ip, action, array);
                }

                break;
        }

    }while(option!=6);

    free(animal);
    close(clientfd);
    clientArray[countClient] = 0;
}

int main(int argc, char **argv)
{
    struct sockaddr_in server;
    struct sockaddr_in client;

    int     serverfd    = 0,
            numbytes    = 0,
            option      = 0,
            countClient = 0,
            i           = 0,
            r           = 0;
    struct clientPackage* tempClient;

    for (i = 0; i < BACKLOG; ++i)
    {
        clientArray[i] = 0;
    }

    socklen_t longC = 0,
              longS = 0;

    checkFiles();

    system("clear");

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0)
    {
        perror("Error socket in function main");
        exit(-1);
    }

    server.sin_family= AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    
    bzero(server.sin_zero, 8);

    longC = sizeof(struct sockaddr_in);
    longS = sizeof(struct sockaddr_in);

    r = bind(serverfd, (struct sockaddr*) &server, longS);
    if (r < 0)
    {
        perror("Error bind in function main");
        close(serverfd);
        exit(-1);
    }

    r = listen(serverfd, BACKLOG);
    if (r < 0)
    {
        perror("Error listen in function main");
        exit(-1);
    }


    do
    {
        r = accept(serverfd, (struct sockaddr*) &client, (socklen_t*) &longC);

        if (r > 0)
        {
            for (i = 0; i < BACKLOG; ++i)
            {
                if (clientArray[i] == 0)
                {
                    clientArray[i] = r;    

                    tempClient = malloc(sizeof(struct clientPackage));
                    if (tempClient == NULL)
                    {
                        perror("Error malloc 0");
                        exit(-1);
                    }

                    tempClient->count = i;
                    tempClient->tmpClientfd = clientArray[i];
                    tempClient->tmpClient = client;

                    pthread_create(&thread[i], NULL, (void *)clientMenu, tempClient);

                    r = 0;
                    i = BACKLOG;
                }   
            }
        }

    }while(1);

    return 0;
}
