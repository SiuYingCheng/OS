#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio_ext.h>
#define LENGTH 10000
volatile sig_atomic_t flag = 0;
int sockfd = 0;
int count = 0;
void catch_ctrl_c_and_exit(int sig)
{
    flag = 1;
}
void send_mess_handler()
{
    char message[LENGTH] = {};
    char buffer[LENGTH+32]= {};
    char*key_set=malloc(sizeof(char)*(LENGTH+1));
    char*value_set=malloc(sizeof(char)*(LENGTH+1));
    char*res;
    while(1)
    {
        fflush(stdout);
        if (count==0)
        {
            printf ("> ");
            count++;
        }
        fgets(message, LENGTH, stdin);
        // trim \n by \0
        int i;
        for (i = 0; i < LENGTH; i++)
        {
            if (message[i] == '\n')
            {
                message[i] = '\0';
                break;
            }
        }
        if (strcmp(message, "EXIT") == 0)
        {
            break;
        }
        else
        {
            sprintf (buffer,"%s",message);

            if (strcmp(message,"HELP")==0)
            {
                printf ("Commands\t\t\t\tDescription\n");
                printf("SET [key] [value]\t\t\tStore the key value pair ([key],[value])into the database.\n");
                printf ("GET [key]\t\t\t\tGet the value of [key] from the database.\n");
                printf ("DELETE [key]\t\t\t\tDelete [key] and it's associated value from the database.\n");
                printf ("Exit\t\t\t\t\tExit.\n");
            }
            else if (message[0]=='S'&&message[1]=='E'&&message[2]=='T')
            {
                res=malloc(sizeof(char)*(LENGTH+33));
                sprintf (res,"%s",buffer);
                //printf ("res: %s\n",res);
                key_set = strtok_r (res," ",&res); //token_Set bf
                //printf ("token in send %s\n",token_set);
                key_set = strtok_r(NULL,"",&res);
                //printf ("key in send %s\n",key_set);
                if (key_set==NULL)
                {
                    printf ("Invalid Instruction\n");
                    count=0;
                }
                else
                {
                    value_set=strtok_r(key_set," ",&res);
                    //printf ("value in send %s\n",value_set);
                    value_set=strtok_r(NULL," ",&res);
                    // printf ("value in send two %s\n",value_set);
                    if (value_set==NULL)
                    {
                        printf ("Invalid Instruction\n");
                        count=0;
                    }
                    else
                    {
                        value_set = strtok_r(NULL," ",&res);
                        //printf ("value in send three: %s\n",value_set);
                        if (value_set==NULL)
                        {
                            send(sockfd, buffer, strlen(buffer), 0);
                        }
                        else
                        {
                            printf ("Invalid Instruction\n");
                            count=0;
                        }
                    }
                }
            }
            else if (buffer[0]=='G'&&buffer[1]=='E'&&buffer[2]=='T')
            {
                res=malloc(sizeof(char)*(LENGTH+33));
                sprintf (res,"%s",buffer);
                key_set = strtok_r (res," ",&res); //token_set bf
                key_set = strtok_r(NULL,"",&res);
                //printf ("key %s\n",key_set);
                if (key_set!=NULL)
                {
                    value_set=strtok_r(key_set," ",&res);
                    //printf ("value %s\n",value_set);
                    value_set = strtok_r(NULL," ",&res);
                    //printf ("value twice %s\n",value_set);
                    if (value_set==NULL)
                    {
                        send(sockfd, buffer, strlen(buffer), 0);
                    }
                    else
                    {
                        printf ("Invalid Instruction!\n");
                        count=0;
                    }
                }
                else
                {
                    printf ("Invalid Instruction!\n");
                    count=0;
                }
            }
            else if (buffer[0]=='D'&&buffer[1]=='E'&&buffer[2]=='L'&&buffer[3]=='E'&&buffer[4]=='T'&&buffer[5]=='E')
            {
                res=malloc(sizeof(char)*(LENGTH+33));
                sprintf (res,"%s",buffer);
                key_set = strtok_r (res," ",&res);  //token_set bf
                key_set = strtok_r(NULL,"",&res);
                //printf ("key %s\n",key_set);
                if (key_set!=NULL)
                {
                    value_set=strtok_r(key_set," ",&res);
                    //printf ("value %s\n",value_set);
                    value_set = strtok_r(NULL," ",&res);
                    //printf ("value twice %s\n",value_set);
                    if (value_set==NULL)
                    {
                        send(sockfd, buffer, strlen(buffer), 0);
                    }
                    else
                    {
                        printf ("Invalid Instruction!\n");
                        count=0;
                    }
                }
                else
                {
                    printf ("Invalid Instruction!\n");
                    count=0;
                }
            }
            else
            {
                printf ("Invalid Instruction\n");
                count=0;
            }
        }
        bzero(buffer, LENGTH + 32);

    }
    catch_ctrl_c_and_exit(2);
}

void recv_mess_handler()
{
    char message[LENGTH] = {};
    char*buff_set;
    char*key_set=malloc(sizeof(char)*(LENGTH+1));
    char*value_set=malloc(sizeof(char)*(LENGTH+1));
    //char*token_set=malloc(sizeof(char)*(LENGTH+1));
    char*res;
    while (1)
    {
        int receive = recv(sockfd, message, LENGTH, 0);
        if (receive > 0)
        {

            if (strcmp(message,"Nothing")!=0)	// don't have value to GET
            {

                if (strcmp(message,"No value")==0)
                {
                    printf ("[ERROR] Key doesn't exist to delete\n");
                }
                else if ((message[0]=='G' && message[1]=='E' && message[2]=='T')|| (message[0]=='S' && message[1]=='E'&& message[2]=='T')) // value of GET return
                {
                    // trim \n by \0
                    int i;
                    for (i = 0; i < LENGTH; i++)
                    {
                        if (message[i] == '\n')
                        {
                            message[i] = '\0';
                            break;
                        }
                    }
                    if (message[0]=='S')
                    {

                        res=malloc(sizeof(char)*(LENGTH+33));
                        buff_set=malloc(sizeof(char)*(LENGTH+33));
                        //printf ("message received: %s\n",message);
                        sprintf (buff_set,"%s",message);
                        sprintf (res,"%s",buff_set);

                        key_set = strtok_r (buff_set," ",&res); //token_set bf
                        //printf ("token %s\n",token_set);

                        key_set = strtok_r(NULL,"",&res);
                        //printf ("key %s\n",key_set);


                        value_set=strtok_r(key_set," ",&res);
                        //printf ("value %s\n",value_set);
                        value_set = strtok_r(NULL," ",&res);
                        //printf ("value twice %s\n",value_set);
                        printf ("[OK] Key value pair (%s,%s) is stored!\n",key_set,value_set);
                    }
                    if (message[0]=='G')
                    {

                        res=malloc(sizeof(char)*(LENGTH+33));
                        buff_set=malloc(sizeof(char)*(LENGTH+33));
                        sprintf (buff_set,"%s",message);
                        sprintf (res,"%s",buff_set);
                        key_set = strtok_r (buff_set," ",&res);  //token_set bf
                        key_set = strtok_r(NULL,"",&res);
                        //printf ("key %s\n",key_set);

                        value_set=strtok_r(key_set," ",&res);
                        //printf ("value %s\n",value_set);
                        value_set = strtok_r(NULL," ",&res);
                        //printf ("value twice %s\n",value_set);

                        printf ("[OK] The value of %s is %s\n",key_set,value_set);
                    }
                }
                else if (strcmp(message,"Key Exists")==0)
                {
                    printf ("[ERROR] Key exists in hash table\n");
                }
                else // removed
                {
                    printf("[OK] Key \"%s\" is removed\n", message);
                }
            }
            else
            {
                printf ("[ERROR] Key does not exist!\n");
            }
            printf ("> ");
            fflush(stdout);
        }
        else if (receive == 0 || message==NULL)
        {
            printf ("Key doesn't exist\n");
            break;
        }
        else
        {
            // -1
        }
        memset(message, 0, sizeof(message));
    }
}

int main(int argc, char **argv)
{
    int opt;
    char *server_host_name = NULL, *server_port = NULL;

    /* Parsing args */
    while ((opt = getopt(argc, argv, "h:p:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            server_host_name = malloc(strlen(optarg) + 1);
            strncpy(server_host_name, optarg, strlen(optarg));
            break;
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
    if (!server_host_name)
    {
        fprintf(stderr, "Error!, No host name provided!\n");
        exit(1);
    }
    if (!server_port)
    {
        fprintf(stderr, "Error!, No port number provided!\n");
        exit(1);
    }
    printf ("[INFO] Connected to localhost:%d\n",atoi(server_port));
    printf ("[INFO] Welcome! Please type HELP for available commands\n");
    char *ip = "127.0.0.1";
    signal(SIGINT, catch_ctrl_c_and_exit);
    struct sockaddr_in server_addr;
    /* Socket settings */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(atoi(server_port));
    // Connect to Server
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("ERROR: connect\n");
        return EXIT_FAILURE;
    }
    pthread_t send_mess_thread;
    if(pthread_create(&send_mess_thread, NULL, (void *) send_mess_handler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }
    pthread_t recv_mess_thread;
    if(pthread_create(&recv_mess_thread, NULL, (void *) recv_mess_handler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }
    while (1)
    {
        if(flag)
        {
            printf("Good bye\n");
            free(server_port);
            free(server_host_name);
            break;
        }
    }
    pthread_join(send_mess_thread,NULL);
    close(sockfd);
    return EXIT_SUCCESS;
}

