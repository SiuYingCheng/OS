#include "app.h"
FILE*fp;
#define MAXLEN 1000000
unsigned char buf[MAXLEN];
unsigned char mem[MAXLEN]= {};
unsigned char cpu [MAXLEN]= {};
unsigned char version [MAXLEN]= {};
unsigned char timer [MAXLEN]= {};
int main (void)
{
    char input ='0';
    int i =0;
    int count = 0;
    int m =0;
    int cpu_loc =0 ;
    int time_loc =0;
    int mem_loc = 0;
    int bufflen = 0;
    fp = fopen ("/proc/my_info","rb");
    if (fp!=NULL)
    {
        while (!feof(fp))
        {
            bufflen = fread (buf,1,MAXLEN,fp);
        }
        fclose(fp);
    }
    if (bufflen > 1)
    {
        for (i=0; i<bufflen; i++)
        {
            if (buf[i]=='\n' && buf[i+1]=='=')
            {
                count++;
                if (count==2)	// the first time will be ver_loc
                {
                    cpu_loc = i;
                }
                else if (count == 3)
                {
                    mem_loc = i;
                }
                else if (count == 4)
                {
                    time_loc = i;
                }
            }
        }
        for (m=0; m<cpu_loc; m++)
        {
            version[m]=buf[m];
        }
        for (m=cpu_loc; m<mem_loc; m++)
        {
            cpu[m]=buf[m];
        }
        for (m=mem_loc; m<time_loc; m++)
        {
            mem[m]=buf[m];
        }
        for (m=time_loc; m<bufflen; m++)
        {
            timer[m]=buf[m];
        }
    }

    printf ("\nwhich information do you want?\n");
    printf ("Version(v),CPU(c),Memory(m),Time(t),All(a),Exit(e)\n");
    while (1)
    {
        if (scanf ("%c",&input)==1)
        {
            if (input == 'v' || input == 'a')
            {
                for (i = 0; i<MAXLEN; i++)
                {
                    if (version[i]!='0')
                    {
                        printf ("%c",version[i]);
                    }
                }
            }
            if (input == 'c' || input == 'a')
            {
                for (i = 0; i<MAXLEN; i++)
                {
                    printf ("%c",cpu[i]);
                }
            }
            if (input =='m' || input == 'a')
            {
                for (i = 0; i<MAXLEN; i++)
                {
                    printf ("%c",mem[i]);
                }
            }
            if (input == 't' || input == 'a')
            {
                for (i = 0; i<MAXLEN; i++)
                {
                    printf ("%c",timer[i]);
                }
                printf ("\n");
            }
            if (input == 'e')
            {
                break;
            }
            printf ("\nwhich information do you want?\n");
            printf ("Version(v),CPU(c),Memory(m),Time(t),All(a),Exit(e)\n");
            getchar();
        }
    }
    return 0;
}
