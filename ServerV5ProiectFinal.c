
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <libxml/parser.h>

/* portul folosit */
#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData
{
    int idThread; //id-ul thread-ului tinut in evidenta de acest program
    int cl;       //descriptorul intors de accept
} thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

struct Playeri
{
    struct timeval begin, end;
    char nume[100];
    int puncte;
    int bitlogare;
    int bitinitializare;
    int bitLivejoc;
    int bitSfarsitjoc;
    int bitIesit;
    char nrIntrebare[10];
} Player[100];

int numarClienti = 0;
char xQuestions[500];
char PrimaxQuestions[500];

int is_leaf(xmlNode *node)
{
    xmlNode *child = node->children;
    while (child)
    {
        if (child->type == XML_ELEMENT_NODE)
            return 0;

        child = child->next;
    }

    return 1;
}

void print_xml(xmlNode *node, int indent_len)
{
    strcat(xQuestions, node->name);
    if (node->type == XML_ELEMENT_NODE)
    {
        //printf("%*c%s:%s\n", indent_len*2, '-', node->name, xmlNodeGetContent(node));
        strcat(xQuestions, xmlNodeGetContent(node));
    }
}

xmlNode *find_node(xmlNode *node, char *prop_val)
{

    xmlNode *result;

    if (node == NULL)
        return NULL;

    while (node)
    {
        if ((node->type == XML_ELEMENT_NODE) && xmlGetProp(node, "id") && (strcmp(xmlGetProp(node, "id"), prop_val) == 0))
        {
            return node;
        }

        if (result = find_node(node->children, prop_val))
            return result;

        node = node->next;
    }

    return NULL;
}
void incrementare(int clienT)
{
    char aux[10];
    strcpy(aux, Player[clienT].nrIntrebare + 1);
    int x = strlen(aux) - 1;
    int numar = 0;
    for (int i = 0; i <= x; i++)
    {
        numar = numar * 10 + (aux[i] - '0');
    }
    numar++;
    char aux1[10];
    sprintf(aux1, "%d", numar);
    strcpy(aux1 + 1, aux1);
    aux1[0] = 'I';
    strcpy(Player[clienT].nrIntrebare, aux1);
}
int primireIntrebare(int fd)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    xmlNode *result;

    doc = xmlReadFile("intrebari.xml", NULL, 0);

    if (doc == NULL)
    {
        printf("Could not parse the XML file.\n");
        return 1;
    }

    root_element = xmlDocGetRootElement(doc);

    char prop_val[500];
    strcpy(prop_val, Player[fd].nrIntrebare);

    result = find_node(root_element, prop_val);
    if (result)
    {
        printf("[server] Node with id '%s' is found in the XML.\n", prop_val);
        fflush(stdout);
    }
    else
    {
        printf("[server] Node with id '%s' is not found in the XML.\n", prop_val);
        fflush(stdout);
    }

    print_xml(result, 1);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 1;
}
int primaIntrebare()
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    xmlNode *result;

    doc = xmlReadFile("intrebari.xml", NULL, 0);

    if (doc == NULL)
    {
        printf("Could not parse the XML file.\n");
        return 1;
    }

    root_element = xmlDocGetRootElement(doc);

    char prop_val[500];
    strcpy(prop_val, "I1");

    result = find_node(root_element, prop_val);
    if (result)
    {
        printf("[server] Node with id '%s' is found in the XML.\n", prop_val);
        fflush(stdout);
    }
    else
    {
        printf("[server] Node with id '%s' is not found in the XML.\n", prop_val);
        fflush(stdout);
    }

    print_xml(result, 1);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 1;
}
int verificareRaspuns(int fd, char variantaRaspuns[500])
{
    if(strlen(variantaRaspuns)>2)
    return 0;
    
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    xmlNode *result;

    doc = xmlReadFile("intrebari.xml", NULL, 0);

    if (doc == NULL)
    {
        printf("Could not parse the XML file.\n");
        return 1;
    }

    root_element = xmlDocGetRootElement(doc);

    char prop_val[500];

    char auxPtRaspuns[500];
    strcpy(auxPtRaspuns, Player[fd].nrIntrebare + 1);
    strcpy(auxPtRaspuns + 1, auxPtRaspuns);
    auxPtRaspuns[0] = 'R';
    strcpy(prop_val, auxPtRaspuns);

    result = find_node(root_element, prop_val);
    if (result)
    {
        printf("[server] Node with id '%s' is found in the XML.\n", prop_val);
        fflush(stdout);
    }
    else
    {
        printf("[server] Node with id '%s' is not found in the XML.\n", prop_val);
        fflush(stdout);
    }

    print_xml(result, 1);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    strcpy(xQuestions, xQuestions + 13);
    xQuestions[1] = '\0';
    variantaRaspuns[1] = '\0';
    if (strcmp(xQuestions, variantaRaspuns) == 0)
        Player[fd].puncte++;
    printf("%d %s %s\n", Player[fd].puncte, xQuestions, variantaRaspuns);
    fflush(stdout);

    return 1;
}
int VerificareIncrementare(int clienT)
{
    char aux[10];
    strcpy(aux, Player[clienT].nrIntrebare + 1);
    int x = strlen(aux) - 1;
    int numar = 0;
    for (int i = 0; i <= x; i++)
    {
        numar = numar * 10 + (aux[i] - '0');
    }
    numar++;
    printf("%d\n", numar);
    fflush(stdout);
    if (numar > 5)
        return 0;
    else
        return 1;
}
int returnarenrClienti()
{
    return numarClienti;
}
int main()
{
    struct sockaddr_in server; // structura folosita de server
    struct sockaddr_in from;
    int nr; //mesajul primit de trimis la client
    int sd; //descriptorul de socket
    int pid;
    pthread_t th[100]; //Identificatorii thread-urilor care se vor crea
    int i = 0;
    primaIntrebare();
    strcpy(PrimaxQuestions,xQuestions);
    /* crearea unui socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server]Eroare la socket().\n");
        return errno;
    }
    /* utilizarea optiunii SO_REUSEADDR */
    int on = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* pregatirea structurilor de date */
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons(PORT);

    /* atasam socketul */
    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen(sd, 2) == -1)
    {
        perror("[server]Eroare la listen().\n");
        return errno;
    }
    /* servim in mod concurent clientii...folosind thread-uri */
    while (1)
    {
        int client;
        thData *td; //parametru functia executata de thread
        int length = sizeof(from);

        printf("[server]Asteptam la portul %d...\n", PORT);
        fflush(stdout);

        // client= malloc(sizeof(int));
        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
        {
            perror("[server]Eroare la accept().\n");
            continue;
        }

        /* s-a realizat conexiunea, se astepta mesajul */

        // int idThread; //id-ul threadului
        // int cl; //descriptorul intors de accept

        td = (struct thData *)malloc(sizeof(struct thData));
        td->idThread = i;
        //INITIALIZARE CLIENT
        numarClienti = i;
        Player[td->idThread].bitinitializare = 1;
        //
        i++;
        td->cl = client;

        pthread_create(&th[i], NULL, &treat, td);

    } //while
};
static void *treat(void *arg)
{
    struct thData tdL;
    tdL = *((struct thData *)arg);
    printf("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
    fflush(stdout);
    pthread_detach(pthread_self());
    raspunde((struct thData *)arg);
    /* am terminat cu acest client, inchidem conexiunea */
    close((intptr_t)arg);
    return (NULL);
};

void raspunde(void *arg)
{

    struct thData tdL;
    tdL = *((struct thData *)arg);

    int okServer = 1;
    while (okServer)
    {
        char msg[500];
        if (read(tdL.cl, msg, 500) <= 0)
        {
            Player[tdL.idThread].bitIesit = 1;
            Player[tdL.idThread].bitlogare = 1;
            okServer = 0;
            return;
            /*printf("[Thread %d]\n", tdL.idThread);
            perror("Eroare la read() de la client.\n");*/
        }

        printf("[Thread %d]Mesajul a fost receptionat...\n", tdL.idThread);
        fflush(stdout);
        if (Player[tdL.idThread].bitinitializare = 1 && Player[tdL.idThread].bitlogare != 1)
        {
            strcpy(Player[tdL.idThread].nume, msg);
            Player[tdL.idThread].bitlogare = 1;
            Player[tdL.idThread].puncte = 0;
            strcpy(Player[tdL.idThread].nrIntrebare, "I1");

            for (int contor = 0; contor <= returnarenrClienti(); contor++)
                if (Player[contor].bitinitializare == 1 && Player[contor].bitlogare == 0)
                    contor = -1;

            Player[tdL.idThread].bitLivejoc = 1;
            ////////DEBUG
            /*printf("%d nr clienti\n",numarClienti);
            for (int contor = 0; contor <= returnarenrClienti(); contor++)
            printf("%d %d",Player[contor].bitinitializare,Player[contor].bitlogare);*/
            //////////DEBUG

            ////////////////RASPUNSUL PENTRU CLIENT/////////////
            
            char auxIntrebare[500];
            strcpy(auxIntrebare, PrimaxQuestions);
             //NU UITA RESETARE CHAR
            ///////////////RASPUNSUL PENTRU CLIENT////////////////
          
            gettimeofday(&Player[tdL.idThread].begin, 0);
            if (write(tdL.cl, auxIntrebare, 500) <= 0)
            {
                printf("[Thread %d] ", tdL.idThread);
                perror("[Thread]Eroare la write() catre client.\n");
            }
        }
        else if (strcmp(msg, "quit\n") == 0)
        {
            Player[tdL.idThread].bitIesit = 1;
            char msgrasp[500] = "Ai iesit";
            if (write(tdL.cl, msgrasp, 500) <= 0)
            {
                printf("[Thread %d] ", tdL.idThread);
                perror("[Thread]Eroare la write() catre client.\n");
            }
            okServer = 0;
        }
        else 
        {
            gettimeofday(&Player[tdL.idThread].end, 0);
            long seconds = Player[tdL.idThread].end.tv_sec - Player[tdL.idThread].begin.tv_sec;
            long microseconds = Player[tdL.idThread].end.tv_usec - Player[tdL.idThread].begin.tv_usec;
            double elapsed = seconds + microseconds*1e-6;
            printf("Time measured: %.3f seconds.Thread %d\n", elapsed,tdL.idThread);
            strcpy(xQuestions, "\0");

            if(elapsed<=10.0)
            {
            verificareRaspuns(tdL.idThread, msg);
            strcpy(xQuestions, "\0");
            }

            if (VerificareIncrementare(tdL.idThread) == 1)
            {
                //printf("suntem acilea1\n");

                incrementare(tdL.idThread);
                ////////////////RASPUNSUL PENTRU CLIENT/////////////
                primireIntrebare(tdL.idThread);
                char auxIntrebare[500];
                strcpy(auxIntrebare, xQuestions);
                strcpy(xQuestions, "\0");
                ///////////////RASPUNSUL PENTRU CLIENT////////////////

                gettimeofday(&Player[tdL.idThread].begin, 0);
                if (write(tdL.cl, auxIntrebare, 500) <= 0)
                {
                    printf("[Thread %d] ", tdL.idThread);
                    perror("[Thread]Eroare la write() catre client.\n");
                }
            }
            else
            {
                Player[tdL.idThread].bitIesit = 1;
                for (int contor = 0; contor <= returnarenrClienti(); contor++)
                    if (Player[contor].bitIesit != 1)
                        contor = -1;

                //printf("suntem acilea2\n");
                int Xpuncte = Player[tdL.idThread].puncte;
                char message1[500];
                sprintf(message1, "%d", Xpuncte);
                char message[500] = "Numarul tau de puncte acumulate: ";
                strcat(message, message1);
                int maximum = -1;
                int idPlayer;
                for (int contor = 0; contor <= returnarenrClienti(); contor++)
                {
                    if (Player[contor].puncte > maximum)
                    {
                        idPlayer = contor;
                        maximum = Player[contor].puncte;
                    }
                }
                char message2[500];
                sprintf(message2, "%d", maximum);
                char message3[500] = ". Castigatorul este ";
                strcat(message3, Player[idPlayer].nume);
                char message4[500] = "cu punctajul: ";
                strcat(message4, message2);
                strcat(message3, message4);
                strcat(message, message3);

                
                if (write(tdL.cl, message, 500) <= 0)
                {
                    printf("[Thread %d] ", tdL.idThread);
                    perror("[Thread]Eroare la write() catre client.\n");
                }
                ////////pregatire inchidere
                for (int contor = 0; contor <= returnarenrClienti(); contor++)
                    Player[contor].puncte = -1;
                okServer = 0;
                ////////
            }
        }
        /*else
        {
            char msgrasp[500] = "hmmmm?";
            if (write(tdL.cl, msgrasp, 500) <= 0)
            {
                printf("[Thread %d] ", tdL.idThread);
                perror("[Thread]Eroare la write() catre client.\n");
            }
        }*/
        /* returnam mesajul clientului */
        /* if (write(tdL.cl, msg, 100) <= 0)
        {
            printf("[Thread %d] ", tdL.idThread);
            perror("[Thread]Eroare la write() catre client.\n");
        }
        else
            printf("[Thread %d]Mesajul a fost trasmis cu succes.\n", tdL.idThread);*/
    }
}
