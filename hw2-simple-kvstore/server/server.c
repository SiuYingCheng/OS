#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>
#include "hash.h"
#define MAX_CLIENTS 100000
#define BUFFER_SZ 5000

static _Atomic unsigned int cli_count = 0;
static int uid = 10;
HashTable* ht;
typedef struct
{
    struct sockaddr_in address;
    int sockfd;
    int uid;
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void queue_add_element(client_t *client)
{
    pthread_mutex_lock(&clients_mutex);

    for(int i=0; i < MAX_CLIENTS; ++i)
    {
        if(!clients[i])
        {
            clients[i] = client;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}
void queue_remove_element(int uid)
{
    pthread_mutex_lock(&clients_mutex);

    for(int i=0; i < MAX_CLIENTS; ++i)
    {
        if(clients[i])
        {
            if(clients[i]->uid == uid)
            {
                clients[i] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}
void send_message(char *s, int uid) // send message to the sender
{
    pthread_mutex_lock(&clients_mutex);

    for(int i=0; i<MAX_CLIENTS; ++i)
    {
        if(clients[i])
        {
            if(clients[i]->uid == uid)
            {
                if(send(clients[i]->sockfd, s, strlen(s),0) < 0)
                {
                    perror("[ERROR] Write fail!");
                    break;
                }
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg)
{
    char buff_out[BUFFER_SZ]="";
    char*key=malloc(sizeof(char)*(BUFFER_SZ+1));
    char*value=malloc(sizeof(char)*(BUFFER_SZ+1));
    char*res=malloc(sizeof(char)*(BUFFER_SZ+1));
    char*temp=malloc(sizeof(char)*(BUFFER_SZ+1));
    char*concat=malloc(sizeof(char)*(BUFFER_SZ+1));
    char*copy=malloc(sizeof(char)*(BUFFER_SZ+1));
    int leave_flag = 0;
    int find=0;
    cli_count++;
    client_t *client = (client_t *)arg;
    while(1)
    {
        if (leave_flag)
        {
            break;
        }

        int receive = recv(client->sockfd, buff_out, BUFFER_SZ, 0);
        if (receive > 0)
        {
            if(strlen(buff_out) > 0)
            {
                // trim \n by \0
                int i;
                for (i = 0; i < BUFFER_SZ; i++)
                {
                    if (buff_out[i] == '\n')
                    {
                        buff_out[i] = '\0';
                        break;
                    }
                }
                //printf ("buffout: %s\n",buff_out);
                if (buff_out[0]=='S')
                {
                    sprintf (copy,"%s",buff_out);
                    key = strtok_r (buff_out," ",&res); //token before
                    //printf ("token: %s\n",token);
                    key = strtok_r(NULL," ",&res);
                    //printf ("key: %s\n",key);
                    value = strtok_r (NULL," ",&res);
                    //printf ("value: %s\n",value);
                    if (hash_table_put(ht,key,value,NULL)==1)
                    {
                        temp = "Key Exists";
                        send_message(temp,client->uid);
                    }
                    else
                    {
                        send_message(copy,client->uid);
                    }
                }
                else if (buff_out[0]=='D')
                {
                    key = strtok_r (buff_out," ",&res); //token before
                    key = strtok_r(NULL," ",&res);
                    if(hash_table_remove(ht,key,find)==0)
                    {
                        temp="No value";
                        send_message(temp,client->uid);
                    }
                    else
                    {
                        send_message(key,client->uid);
                    }
                }
                else if (buff_out[0]=='G')
                {
                    //printf ("GET\n");
                    sprintf (copy,"%s",buff_out);
                    key = strtok_r (buff_out," ",&res); //token before
                    //printf ("token: %s\n",token);
                    key = strtok_r(NULL," ",&res);
                    //printf ("key: %s\n",key);
                    value = (char*)hash_table_get(ht,key);
                    //printf ("value: %s\n",value);
                    if (value!=NULL)
                    {
                        concat = malloc (strlen(copy)+strlen(value)+1);
                        sprintf (concat,"%s ",copy);
                        strcat (concat,value);
                        //printf ("concat: %s\n",concat);
                        send_message(concat,client->uid);
                    }
                    else
                    {
                        temp = "Nothing";
                        send_message(temp,client->uid);
                    }
                }
            }
        }
        else if (receive == 0 || strcmp(buff_out, "EXIT") == 0)
        {
            sprintf(buff_out, "EXIT");
            //printf("%s\n", buff_out);
            send_message(buff_out, client->uid);
            leave_flag = 1;
        }
        else
        {
            printf("ERROR: -1\n");
            leave_flag = 1;
        }
        bzero(buff_out, BUFFER_SZ);
    }
    close(client->sockfd);
    queue_remove_element(client->uid);
    free(client);
    cli_count--;
    pthread_detach(pthread_self());
    return NULL;
}

int main(int argc, char **argv)
{
    char *server_port = 0;
    int opt = 0;
    /* Parsing args */
    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            server_port = malloc(strlen(optarg) + 1);
            strncpy(server_port, optarg, strlen(optarg));
            break;
        case '?':
            fprintf(stderr, "Unknown option \"-%c\"\n", isprint(optopt) ?
                    optopt : '#');
            return 0;
        }
    }

    if (!server_port)
    {
        fprintf(stderr, "Error! No port number provided!\n");
        exit(1);
    }

    char *ip = "127.0.0.1";
    int option = 1;
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    pthread_t tid;

    /* Socket settings */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(atoi(server_port));

    /* Ignore pipe signals */
    signal(SIGPIPE, SIG_IGN);

    if(setsockopt(listenfd, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0)
    {
        perror("ERROR: setsockopt failed");
        return EXIT_FAILURE;
    }

    /* Bind */
    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR: Socket binding failed");
        return EXIT_FAILURE;
    }

    ht = hash_table_new();
    /* Listen */
    if (listen(listenfd, 10) < 0)
    {
        perror("ERROR: Socket listening failed");
        return EXIT_FAILURE;
    }

    printf ("[INFO] Start with a clean database...\n");
    printf ("[INFO] Initializing the server...\n");
    printf ("[INFO] Server initialized!\n");
    printf ("[INFO] Listening on the port %d\n",atoi(server_port));

    while(1)
    {
        socklen_t clilen = sizeof(cli_addr);
        connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);
        printf ("[CLIENT CONNECTED] Connected to client (localhost, 49581)\n");
        printf ("[INFO] Listening on port %d ...\n",atoi(server_port));
        printf ("[THREAD INFO] Thread %ld created, serving connection %d\n",tid,listenfd);

        /* Check if max clients is reached */
        if((cli_count + 1) == MAX_CLIENTS)
        {
            printf("Max clients number reached. Rejected connection!");
            printf("%d.%d.%d.%d", (cli_addr.sin_addr.s_addr & 0xff), (cli_addr.sin_addr.s_addr & 0xff00) >> 8, (cli_addr.sin_addr.s_addr & 0xff0000) >> 16, (cli_addr.sin_addr.s_addr & 0xff000000) >> 24);
            printf(":%d\n", cli_addr.sin_port);
            close(connfd);
            continue;
        }
        client_t *client = (client_t *)malloc(sizeof(client_t));
        client->address = cli_addr;
        client->sockfd = connfd;
        client->uid = uid++;
        queue_add_element(client);
        pthread_create(&tid, NULL, &handle_client, (void*)client);
        sleep(1);
    }
    hash_table_delete(ht);
    free(server_port);
    return EXIT_SUCCESS;
}
