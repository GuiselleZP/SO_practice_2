/*
  - Oscar Julian Tinjaca Reyes        - ojtinjacar@unal.edu.co
  - Guiselle Tatiana Zambrano Penagos - gtzambranop@unal.edu.co
*/

#include <stdio.h>
#include <ctype.h> //tolower
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termio.h>
#include <sys/ioctl.h>

#define STDINFD  0 //inkey
#define SIZEDOGTYPE sizeof(dogType)

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

char nameClinicHistory[] = "clinicHistoryClient";

int menu();
void inkey();
void goMenu();
char* checkFiles(int countClient);
long int getCodeName(char name[]);
void* createAnimal();
void clinicHistory(void* animal, int position, char nameHistory[]);
void* modifyStruct(void* animal,char nameHistory[]);

//cambiar
int menu(int countClient)
{
    char option[20];
    char welcome[40] = "	Bienvenido, usted es el cliente #";
    char count[3];

    if (countClient < 10)
    {
    	strcat(welcome, "0");
    }
    sprintf(count, "%d", countClient); 
    strcat(welcome, count);

    printf("%s\n", welcome);

    printf(" 1. Ingresar Registro\n 2. Ver Registro\n"
        " 3. Borrar registro\n 4. Buscar Registro \n 5. generar animales aleatorios.\n 6. Salir\n");

    scanf("%s", option);
    system("clear");
    if((strcmp(option,"1")==0) || (strcmp(option,"2")==0) || (strcmp(option,"3")==0) || (strcmp(option,"4")==0) || (strcmp(option,"5")==0) || (strcmp(option,"6")==0)){
       return atoi(option); 
    }else{
        return 0;
    }
}

void inkey()
{
  char c[20];
  int d;
  struct termio param_ant, params;

  int q = ioctl(STDINFD,TCGETA,&param_ant);
  params = param_ant;
  params.c_lflag &= ~(ICANON|ECHO);
  params.c_cc[20] = 20;

  ioctl(STDINFD,TCSETA,&params);

  fflush(stdin); fflush(stderr); fflush(stdout);
  read(STDINFD,c,20);

  ioctl(STDINFD,TCSETA,&param_ant);
}

void goMenu()
{
    printf("Presione cualquier tecla para dirigirse al menu.\n");
    inkey();
    system("clear");
}

char* checkFiles(int countClient)
{  
  FILE *fl;
  char arrayCount[2];
  char* nameHistory;
  char tempNameClinicHistory[30];

  nameHistory = malloc(sizeof(30));
  if (nameHistory == NULL)
  {
  	perror("Error malloc in function checkFiles");
  	exit(-1);
  }

  strcpy(tempNameClinicHistory, nameClinicHistory);
  sprintf(arrayCount, "%d", countClient); 

  if (countClient < 10 )
  {
  	strcat(tempNameClinicHistory, "0");
  }

  strcat(tempNameClinicHistory, arrayCount);
  strcpy(nameHistory, tempNameClinicHistory);

  fl = fopen(nameHistory, "w+");
  if (fl == NULL)
  {
    perror("Error fopen");
    exit(-1);
  }
  fclose(fl);
  return nameHistory;
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

void* createAnimal() 
{
    dogType *aux;

    long codeName = 0;
    char TempName[32];
    char tempClinicH[100] = "";

    aux = malloc(sizeof (dogType));

    if (aux == NULL) {
        perror("Error en malloc.");
        exit(-1);
    }

    printf("Digite el nombre: \n");
     scanf(" %31[^\n]", TempName);

    for (int i = 0; TempName[i] != '\0'; ++i) {
        TempName[i] = tolower(TempName[i]);
    }
    strcpy(aux->name, TempName);

    printf("Digite el tipo: \n");
    scanf(" %31[^\n]", aux -> type);

    printf("Digite la edad: \n");
    scanf(" %i", &aux -> age);

    printf("Digite la raza: \n");
    scanf(" %31[^\n]", aux -> race);

    printf("Digite la estatura: \n");
    scanf(" %i", &aux -> height);

    printf("Digite el peso: \n");
    scanf(" %f", &aux -> weight);

    printf("Digite el genero: \n");
    scanf(" %c", &aux -> sex);

    strcpy(aux -> clinicHistory, tempClinicH);

    aux -> nextPosition = -1;

    return aux;
}

void clinicHistory(void* animal, int position, char nameHistory[])
{    
	FILE* fl;
    dogType* auxAnimal;

    auxAnimal = (dogType*) animal;

    fl = fopen(nameHistory, "w+");
    if (fl == NULL) {
        perror("Error fopen in function clinicHistory");
        exit(-1);
    }

    fprintf(fl, "Historia clinica del animal cuyo registro es el: %d\n", position);
    fprintf(fl, "Nombre: %s\n", auxAnimal->name);
    fprintf(fl, "Tipo: %s\n", auxAnimal->type);
    fprintf(fl, "Edad: %i\n", auxAnimal->age);
    fprintf(fl, "Raza: %s\n", auxAnimal->race);
    fprintf(fl, "Estatura: %i\n", auxAnimal->height);
    fprintf(fl, "Peso: %.2f\n", auxAnimal->weight);
    fprintf(fl, "Genero: %c\n", auxAnimal->sex);
    fprintf(fl, "Historia Clinica: %s\n", auxAnimal->clinicHistory);

    fclose(fl);
}

void* modifyStruct(void* animal,char nameHistory[])
{  
    FILE* fl;

    int signal  = 0,
        i       = 0,
        r       = 0;
    char line[100]      = "";
    char input[100]     = "";
    char option[100]    = "";
    char separator[2]   = ":";
    char tempCH[100]    = "";
    char* ptr;
    char* ptr2;

    dogType* auxAnimal;
    dogType* tempDog;

    tempDog = (dogType*)animal;

    auxAnimal = malloc(SIZEDOGTYPE);
    if (auxAnimal == NULL)
    {
        perror("Error malloc 1 in function modifyStruct");
        exit(-1);
    }

    fl = fopen(nameHistory,"r+");
    if (fl == NULL)
    {
        perror("Error fopen in function modifyStruct");
        exit(-1);
    }

    signal == 0;
    while(!feof(fl))
    {
        ptr=NULL;
        strcpy(line, "");
        strcpy(option,"");
        strcpy(input, "");
        ptr2 = fgets(line, sizeof(line), fl);

        if (signal != 1)
        {
            signal = 0;
            ptr= strtok(line, separator);
            strcpy(option, ptr);
            ptr= strtok(NULL, separator);
            strcpy(input, ptr);
            i = strlen(input) - 1;
            input[i] = '\0';

            strcpy(line, "");
            for (i = 0; i < strlen(input); ++i)
            {   
                line[i] = input[i+1];
            }

            if (strcmp("Nombre", option) == 0)
            {   
                strcpy(auxAnimal->name, line);          
            }else if (strcmp("Tipo", option) == 0)
            {
                strcpy(auxAnimal->type, line);
            }else if (strcmp("Edad", option) == 0)
            {
                auxAnimal->age = atoi(line);         
            }else if (strcmp("Raza", option) == 0)
            {
                strcpy(auxAnimal->race, line);
            }else if (strcmp("Estatura", option) == 0)
            {
                auxAnimal->height = atoi(line);               
            }else if (strcmp("Peso", option) == 0)
            {
                auxAnimal->weight = atof(line);
            }else if (strcmp("Genero", option) == 0)
            {
                strcpy(&auxAnimal->sex, line);
            }else if (strcmp("Historia Clinica", option) == 0)
            {
                strcpy(auxAnimal->clinicHistory, line);
                strcat(auxAnimal->clinicHistory, "\n");
                signal = 1;
            }   
        }else if((ptr2 != NULL) && (signal == 1)){
            strcat(auxAnimal->clinicHistory, line);
        }
    }
    strcpy(tempCH, auxAnimal->clinicHistory);
    strcpy(auxAnimal->clinicHistory, "");
    fclose(fl);

    strcpy(auxAnimal->name, tempDog->name);
    strcpy(auxAnimal->type, tempDog->type);
    strcpy(auxAnimal->race, tempDog->race);
    strcpy(&auxAnimal->sex, &tempDog->sex);
    auxAnimal->nextPosition = tempDog->nextPosition;
    
    tempCH[strlen(tempCH) - 1] = '\0';
    strcpy(auxAnimal->clinicHistory, tempCH);
    
    return auxAnimal;
}


int main(int argc, char const *argv[])
{
	struct sockaddr_in server;
  	FILE* fl;
  	dogType* newDog;

  	int sizeFileDogs	= 0,
  		numAnimal		= 0,
  		countClient 	= 0,
  		posInFile 		= 0,
  		serverfd 		= 0,
  		select			= 0,
  		option			= 0,
  		port 			= 0,		
  		r 				= 0;

  	socklen_t longC = 0;

  	char animalName[32];
  	char* nameHistory;
 	char message[64];
 	char deleteHistory[35] = "rm -r ";
 	char openHistory[35] = "xdg-open ";

 	dogType* tempAnimal;

  	system("clear");

  	if(argc < 2)
	{
		printf("./p2-dogClient.c <ip> <port>\n");
		return 1;
	}

	serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverfd < 0)
	{
		perror("Error socket");
		exit(-1);
	}

	port = atoi(argv[2]);

	server.sin_family = AF_INET;
  	server.sin_port = htons(port);
  	server.sin_addr.s_addr = inet_addr(argv[1]);
    bzero(server.sin_zero, 8);

  	longC = sizeof(struct sockaddr_in);

  	r = connect(serverfd, (struct sockaddr*) &server, longC);
	if (r < 0)
	{
		perror("Error connect");
		close(serverfd);
		exit(-1);
	}

	r = recv(serverfd, &countClient, sizeof(countClient), 0);
	if (r < 0)
	{
	    perror("Error recv 1 in function main");
	    exit(-1);
	}

	nameHistory = checkFiles(countClient);

	strcat(deleteHistory, nameHistory);
	strcat(openHistory, nameHistory);

	tempAnimal = malloc(SIZEDOGTYPE);
	if (tempAnimal == NULL)
	{
		perror("Error malloc in function main");
		exit(-1);
	}

	do
	{
	    fflush(stdin);
	    fflush(stdout);
	    system("clear");

	    option = menu(countClient);

	    r = send(serverfd, &option, sizeof(int), 0);
		if (r < 0)
		{
			perror("Error send 1 in function main");
			exit(-1);
		}

	    switch(option){
	    	case 1:
	    		newDog = (dogType *) createAnimal();
	    		r = send(serverfd, newDog, SIZEDOGTYPE, 0);
	    		if (r < 0)
	    		{
	    			perror("Error send 2 in function main");
	    			exit(-1);
	    		}

	    		r = recv(serverfd, message, sizeof(message), 0);
				if (r < 0)
				{
				    perror("Error recv 1 in function main");
				    exit(-1);
				}

				printf("%s\n", message);
	    		goMenu();
	    		break;

	    	case 2:

	    		r = recv(serverfd, &sizeFileDogs, sizeof(sizeFileDogs), 0);
				if (r < 0)
				{
				    perror("Error recv 2 in function main");
				    exit(-1);
				}

	    		if (sizeFileDogs == 0)
                {
                    printf("El sistema no contiene animales registrados.\n");  
                    goMenu(); 
                }else
                {
                	if (sizeFileDogs == 1)
                	{
                		printf("En el sistema hay actualmente un animal registrado.\n");
                	}else
                	{	
                		printf("En el sistema actualmente hay %i animales registrados.\n", sizeFileDogs);
                	}
                    
                    printf("Por favor ingrese el número del animal que desea consutar:\n");
                    scanf(" %i", &numAnimal);

                    if (numAnimal > sizeFileDogs || numAnimal <= 0)
                    {
                        printf("El numero no es vaido.\n");

                        numAnimal = 0; // jump signal on the server
                        r = send(serverfd, &numAnimal, sizeof(numAnimal), 0);
						if (r < 0)
						{
							perror("Error send 3 in function main");
							exit(-1);
						}
                        goMenu();
                    }else
                    {	
                    	r = send(serverfd, &numAnimal, sizeof(numAnimal), 0);
						if (r < 0)
						{
							perror("Error send 4 in function main");
							exit(-1);
						}

						r = recv(serverfd, &posInFile, sizeof(int), 0);
						if (r < 0)
						{
						    perror("Error recv 3 in function main");
						    exit(-1);
						}

						r = recv(serverfd, tempAnimal, SIZEDOGTYPE, 0);
						if (r < 0)
						{
						    perror("Error recv 3 in function main");
						    exit(-1);
						}

						posInFile ++;
                        clinicHistory(tempAnimal, posInFile, nameHistory);

                        r = system(openHistory);
						if (r < 0)
						{
							perror("Error system in function consultAnimal");
							exit(-1);
						}

						sleep(7);
                        goMenu();

                        tempAnimal = modifyStruct(tempAnimal, nameHistory);

                        r = send(serverfd, tempAnimal, SIZEDOGTYPE, 0);
						if (r < 0)
						{
							perror("Error send 5 in function main");
							exit(-1);
						}
                    }
                }

	    		break;

	    	case 3:

	    		r = recv(serverfd, &sizeFileDogs, sizeof(sizeFileDogs), 0);
				if (r < 0)
				{
				    perror("Error recv 2 in function main");
				    exit(-1);
				}

	    		if (sizeFileDogs == 0)
                {
                    printf("El sistema no contiene animales registrados.\n");  
                }else
                {
                	if (sizeFileDogs == 1)
                	{
                		printf("En el sistema hay actualmente un animal registrado.\n");
                	}else
                	{	
                		printf("En el sistema actualmente hay %i animales registrados.\n", sizeFileDogs);
                	}
                    
                    printf("Por favor ingrese el número del animal que desea borrar:\n");
                    scanf(" %i", &numAnimal);

                    if (numAnimal > sizeFileDogs || numAnimal <= 0)
                    {
                        printf("El numero no es vaido.\n");

                        numAnimal = 0; // jump signal on the server
                        r = send(serverfd, &numAnimal, sizeof(numAnimal), 0);
						if (r < 0)
						{
							perror("Error send 6 in function main");
							exit(-1);
						}
                    }else
                    {	
                    	r = send(serverfd, &numAnimal, sizeof(numAnimal), 0);
						if (r < 0)
						{
							perror("Error send 7 in function main");
							exit(-1);
						}

						r = recv(serverfd, message, sizeof(message), 0);
						if (r < 0)
						{
						    perror("Error recv 2 in function main");
						    exit(-1);
						}
						printf(" %s\n", message);
					}
                }
                goMenu();
	    		break;

	    	case 4:

	    		r = recv(serverfd, &sizeFileDogs, sizeof(sizeFileDogs), 0);
				if (r < 0)
				{
				    perror("Error recv 2 in function main");
				    exit(-1);
				}

	    		if (sizeFileDogs == 0)
                {
                    printf("El sistema no contiene animales registrados.\n");  
                    goMenu();
                }else
                {
                	printf("Ingrese el nombre del animal que desea consultar:\n");
                	scanf(" %31[^\n]", animalName);

                	r = send(serverfd, animalName, sizeof(animalName), 0);
					if (r < 0)
					{
						perror("Error send 7 in function main");
						exit(-1);
					}

					r = recv(serverfd, &numAnimal, sizeof(numAnimal), 0);
					if (r < 0)
					{
					    perror("Error recv 2 in function main");
					    exit(-1);
					}

					switch(numAnimal)
                    {
                        case 0:
                            printf("No hay animales registrados con ese nombre.\n");
                            goMenu();
                            break;

                        case 1:
                            printf("Hay un animal registrado con ese nombre\n");
                            printf("¿Desea consultar la historia clinica de %s?\n 1. si\n 2. no\n", animalName);

                            scanf(" %d", &select);
                            if (select == 1)
                            {
                            	r = send(serverfd, &numAnimal, sizeof(numAnimal), 0);
								if (r < 0)
								{
									perror("Error send 7 in function main");
									exit(-1);
								}

								r = recv(serverfd, &posInFile, sizeof(int), 0);
								if (r < 0)
								{
								    perror("Error recv 3 in function main");
								    exit(-1);
								}

								r = recv(serverfd, tempAnimal, SIZEDOGTYPE, 0);
								if (r < 0)
								{
								    perror("Error recv 3 in function main");
								    exit(-1);
								}

								posInFile ++;
		                        clinicHistory(tempAnimal, posInFile, nameHistory);

		                        r = system(openHistory);
								if (r < 0)
								{
									perror("Error system in function consultAnimal");
									exit(-1);
								}

                                sleep(7);
		                        goMenu();

		                        tempAnimal = modifyStruct(tempAnimal, nameHistory);

		                        r = send(serverfd, tempAnimal, SIZEDOGTYPE, 0);
								if (r < 0)
								{
									perror("Error send 5 in function main");
									exit(-1);
								}

                            }else
                            {
                            	numAnimal = 0;
                            	r = send(serverfd, &numAnimal, sizeof(numAnimal), 0);
								if (r < 0)
								{
									perror("Error send 7 in function main");
									exit(-1);
								}
                            	goMenu();
                            }
                                           
                            break;

                        default:
                            printf("Hay %i animales registrados con el nombre de %s\n", numAnimal, animalName);
                            printf("¿Desea consultar la historia clinica de alguno de ellos?\n 1. si\n 2. no\n");

                            scanf(" %d", &select);
                            if (select == 1)
                            {
                            	printf("Por favor ingrese el numero de registro a revisar conforme al nombre ingresado\n");
                            	scanf(" %d", &select);
                            	
                            	if ((select < 1) || (select > numAnimal))
                            	{
                            		printf("El numero ingresado no es valido.\n");
                            		numAnimal = 0;
	                            	r = send(serverfd, &numAnimal, sizeof(numAnimal), 0);
									if (r < 0)
									{
										perror("Error send 7 in function main");
										exit(-1);
									}
	                            	goMenu();
                            	}else
                            	{
                            		r = send(serverfd, &select, sizeof(numAnimal), 0);
									if (r < 0)
									{
										perror("Error send 7 in function main");
										exit(-1);
									}

									r = recv(serverfd, &posInFile, sizeof(int), 0);
									if (r < 0)
									{
									    perror("Error recv 3 in function main");
									    exit(-1);
									}


									r = recv(serverfd, tempAnimal, SIZEDOGTYPE, 0);
									if (r < 0)
									{
									    perror("Error recv 3 in function main");
									    exit(-1);
									}

									posInFile ++;
			                        clinicHistory(tempAnimal, posInFile, nameHistory);

			                        r = system(openHistory);
									if (r < 0)
									{
										perror("Error system in function consultAnimal");
										exit(-1);
									}

									sleep(7);
			                        goMenu();

			                        tempAnimal = modifyStruct(tempAnimal, nameHistory);

			                        r = send(serverfd, tempAnimal, SIZEDOGTYPE, 0);
									if (r < 0)
									{
										perror("Error send 5 in function main");
										exit(-1);
									}                            		
                            	}

                            	
                            }else
                            {
                            	numAnimal = 0;
                            	r = send(serverfd, &numAnimal, sizeof(numAnimal), 0);
								if (r < 0)
								{
									perror("Error send 7 in function main");
									exit(-1);
								}
                            	goMenu();
                            }
                            break;
                    }
                }
	    		break;

	    	case 5:

	    		printf("ingrese la cantidad de animales que desea generar:\n");
                
                scanf(" %i", &numAnimal);
                if (numAnimal < 0 || numAnimal > 10000000)
                {
                    printf("el numero no es valido\n");

                    numAnimal = 0; // jump signal on the server
                    r = send(serverfd, &numAnimal, sizeof(numAnimal), 0);
					if (r < 0)
					{
						perror("Error send 5 in function main");
						exit(-1);
					} 

                }else
                {
                    r = send(serverfd, &numAnimal, sizeof(numAnimal), 0);
					if (r < 0)
					{
						perror("Error send 5 in function main");
						exit(-1);
					} 

					r = recv(serverfd, message, sizeof(message), 0);
					if (r < 0)
					{
					    perror("Error recv 3 in function main");
					    exit(-1);
					}

					printf(" %s\n", message);
				}
                goMenu();

	    		break;
	    }

	}while(option != 6);

	r = system(deleteHistory);
    if (r < 0){
        perror("Error system in function main");
        exit(-1);
    }

	free(tempAnimal);
	free(nameHistory);

	return 0;
}
 