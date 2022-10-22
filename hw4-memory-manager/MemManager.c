#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
typedef struct page
{
    int pfdbi;
    int inuse;
    int present;
    int process_name;
} page;
typedef struct disk
{
    int disk_index;
    int process_num;

} disk;
int virtual_page, physical_frame, firstin = 0;
int first[20];
int page_fault[20][2];
char tlb_policy[10];
char page_policy[10];
char frame_policy[10];
int count_lookup[20];
int num_process=0;
page*pagetable;
int virtual=0;
disk*memdisk;
int TLB[32][3];
int*frame_list;
int**memory;
FILE *f_trace_output;
int find_minimum()
{
    int min1=TLB[0][2];
    int index=0;
    for(int i=1; i<32; i++)
    {
        if(min1>TLB[i][2])
        {
            min1=TLB[i][2];
            index=i;
        }
    }
    return index;
}
void LRU_replacement(int refer_num, int page_num,int temp)
{
    int index = find_minimum();
    TLB[index][0]=refer_num;
    TLB[index][1]=page_num;
    TLB[index][2]=temp;
}
void random_num (int refer_num, int page_num)
{
    srand(time(NULL));
    int index = rand()%32;
    TLB[index][0]=refer_num;
    TLB[index][1]=page_num;
}
char find_process (int index)
{
    char find_process = 0;
    if (index == 0)
    {
        find_process = 'A';
    }
    else if (index == 1)
    {
        find_process = 'B';
    }
    else if (index == 2)
    {
        find_process = 'C';
    }
    else if (index == 3)
    {
        find_process = 'D';
    }
    else if (index == 4)
    {
        find_process = 'E';
    }
    else if (index == 5)
    {
        find_process = 'F';
    }
    else if (index == 6)
    {
        find_process = 'G';
    }
    else if (index == 7)
    {
        find_process = 'H';
    }
    else if (index == 8)
    {
        find_process = 'I';
    }
    else if (index == 9)
    {
        find_process = 'J';
    }
    else if (index == 10)
    {
        find_process = 'K';
    }
    else if (index == 11)
    {
        find_process = 'L';
    }
    else if (index == 12)
    {
        find_process = 'M';
    }
    else if (index == 13)
    {
        find_process = 'N';
    }
    else if (index == 14)
    {
        find_process = 'O';
    }
    else if (index == 15)
    {
        find_process = 'P';
    }
    else if (index == 16)
    {
        find_process = 'Q';
    }
    else if (index == 17)
    {
        find_process = 'R';
    }
    else if (index == 18)
    {
        find_process = 'S';
    }
    else if (index == 19)
    {
        find_process = 'T';
    }
    return find_process;
}
int find_index (char refer_process)
{
    int index=-1;
    if (refer_process=='A')
    {
        index = 0;
    }
    else if (refer_process=='B')
    {
        index = 1;
    }
    else if (refer_process == 'C')
    {
        index = 2;
    }
    else if (refer_process == 'D')
    {
        index = 3;
    }
    else if (refer_process =='E')
    {
        index =4;
    }
    else if (refer_process == 'F')
    {
        index = 5;
    }
    else if (refer_process =='G')
    {
        index = 6;
    }
    else if (refer_process == 'H')
    {
        index = 7;
    }
    else if (refer_process == 'I')
    {
        index = 8;
    }
    else if (refer_process == 'J')
    {
        index = 9;
    }
    else if (refer_process == 'K')
    {
        index = 10;
    }
    else if (refer_process == 'L')
    {
        index = 11;
    }
    else if (refer_process == 'M')
    {
        index = 12;
    }
    else if (refer_process == 'N')
    {
        index = 13;
    }
    else if (refer_process == 'O')
    {
        index = 14;
    }
    else if (refer_process == 'P')
    {
        index = 15;
    }
    else if (refer_process == 'Q')
    {
        index = 16;
    }
    else if (refer_process == 'R')
    {
        index = 17;
    }
    else if (refer_process == 'S')
    {
        index = 18;
    }
    else if (refer_process == 'T')
    {
        index = 19;
    }
    return index;
}
void read_file()
{
    char line[10][50];
    for (int i=0; i<10; i++)
    {
        for (int j=0; j<50; j++)
        {
            line[i][j]=0;
        }
    }
    long long int line_count=0;
    FILE *fp = fopen("sys_config.txt", "r");
    while(fgets(line[line_count],sizeof(line[line_count]),fp)!=NULL)
    {
        line_count++;
    }
    sscanf(line[0],"TLB Replacement Policy: %s",tlb_policy);
    sscanf(line[1],"Page Replacement Policy: %s",page_policy);
    sscanf(line[2],"Frame Allocation Policy: %s",frame_policy);

    char process[10];
    sscanf(line[3],"Number of Processes: %s",process);
    num_process = atoi(process);

    char VP[10];
    sscanf(line[4],"Number of Virtual Page: %s",VP);
    virtual_page = atoi(VP);

    char PF[10];
    sscanf(line[5],"Number of Physical Frame: %s",PF);
    physical_frame = atoi(PF);
    fclose(fp);
}
void TLB_initialize()
{
    // TLB initialize
    for (int i=0; i<32; i++)
    {
        for (int j=0; j<3; j++)
        {
            TLB[i][j]=-1;
        }
    }
}
void FIFO (char refer_process, int vpi,int real_vpi)
{
    int index_out=-1,evpi = -1, dest = -1, src = -1;
    int temp_index=-1;
    temp_index = find_index(refer_process);
    char name=0;
    for(int i = 0; i < virtual_page; i++)
    {
        if(memdisk[i].disk_index == -1)
        {
            index_out = firstin;  // assign to the first one
            evpi = memory[firstin][0];  // virtual page number that will be swaped out
            dest = i; // destination of swap out page
            // put evicted page to disk
            memdisk[dest].disk_index = evpi;  // swap out evpi
            memdisk[dest].process_num=memory[firstin][1];
            pagetable[vpi].pfdbi= pagetable[evpi+(virtual*memory[firstin][1])].pfdbi;
            pagetable[evpi+(virtual*memory[firstin][1])].pfdbi=-1;
            pagetable[evpi+(virtual*memory[firstin][1])].present=0;
            name = find_process(memory[firstin][1]);
            pagetable[evpi+(virtual*memory[firstin][1])].process_name = -1;
            for (int m=0; m<32; m++)
            {
                if (TLB[m][0]==evpi && temp_index==memory[firstin][1])
                {
                    TLB[m][0]=-1;
                    TLB[m][1]=-1;
                    TLB[m][2]=-1;
                }
            }
            memory[firstin][0] = real_vpi; // put to end of memory
            memory[firstin][1]=(vpi-real_vpi)/virtual;
            firstin++;
            firstin %= physical_frame;
            break;
        }
    }
    for(int i = 0; i < virtual_page; i++)
    {
        if(memdisk[i].disk_index == real_vpi && memdisk[i].process_num==((vpi-real_vpi)/virtual))
        {
            src = i;
            memdisk[i].disk_index=-1;
            memdisk[i].process_num=-1;
            break;
        }
        else
        {
            src = -1;
        }
    }
    fprintf(f_trace_output,"Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n", refer_process, index_out, evpi,name, dest, real_vpi, src);
    count_lookup[temp_index]++;
    name=0;
}
void FIFO_local(char refer_process, int vpi,int real_vpi,int pro_index)
{
    int index_out=-1,evpi = -1, dest = -1, src = -1;
    char name=0;
    for(int i = 0; i < virtual_page; i++)
    {
        if(memdisk[i].disk_index == -1)
        {
            index_out = first[pro_index];  // assign to the first one
            evpi = memory[first[pro_index]][0];  // virtual page number that will be swaped out
            dest = i; // destination of swap out page
            // put evicted page to disk
            memdisk[dest].disk_index = evpi;  // swap out evpi
            memdisk[dest].process_num=memory[first[pro_index]][1];
            pagetable[vpi].pfdbi= pagetable[evpi+(virtual*memory[first[pro_index]][1])].pfdbi;
            pagetable[evpi+(virtual*memory[first[pro_index]][1])].pfdbi=-1;
            pagetable[evpi+(virtual*memory[first[pro_index]][1])].present=0;
            name = find_process(memory[first[pro_index]][1]);
            pagetable[evpi+(virtual*memory[first[pro_index]][1])].process_name = -1;
            for (int m=0; m<32; m++)
            {
                if (TLB[m][0]==evpi)
                {
                    TLB[m][0]=-1;
                    TLB[m][1]=-1;
                    TLB[m][2]=-1;
                }
            }
            memory[first[pro_index]][0] = real_vpi; // put to end of memory
            memory[first[pro_index]][1]=(vpi-real_vpi)/virtual;
            first[pro_index]++;
            first[pro_index] %= physical_frame;
            while(memory[first[pro_index]][1]!=pro_index)
            {
                first[pro_index]++;
                first[pro_index] %= physical_frame;
            }
            break;
        }
    }
    for(int i = 0; i < virtual_page; i++)
    {
        if(memdisk[i].disk_index == real_vpi && memdisk[i].process_num==((vpi-real_vpi)/virtual))
        {
            src = i;
            memdisk[i].disk_index=-1;
            memdisk[i].process_num=-1;
            break;
        }
        else
        {
            src = -1;
        }
    }
    fprintf(f_trace_output,"Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n", refer_process, index_out, evpi,name, dest, real_vpi, src);
    count_lookup[pro_index]++;
    name=0;
}
void clock_replacement(char refer_process, int vpi,int real_vpi)
{
    int temp_index = -1;
    int index_out,evpi = -1, dest = -1, src = -1;
    char name=0;
    int test_true=0;
    temp_index = find_index(refer_process);
    for(int i = 0; i < virtual_page; i++)
    {
        if(memdisk[i].disk_index == -1)
        {
            while(test_true==0)
            {

                if (memory[firstin][2]==0)
                {
                    index_out = firstin;  // assign to the first one
                    evpi = memory[firstin][0];  // virtual page number that will be swaped out
                    dest = i; // destination of swap out page
                    // put evicted page to disk
                    memdisk[dest].disk_index = evpi;  // swap out evpi
                    memdisk[dest].process_num=memory[firstin][1];
                    pagetable[vpi].pfdbi= pagetable[evpi+(virtual*memory[firstin][1])].pfdbi;
                    pagetable[evpi+(virtual*memory[firstin][1])].pfdbi=-1;
                    pagetable[evpi+(virtual*memory[firstin][1])].present=0;
                    name = find_process(memory[firstin][1]);
                    pagetable[evpi+(virtual*memory[firstin][1])].process_name = -1;
                    for (int m=0; m<32; m++)
                    {
                        if (TLB[m][0]==evpi && temp_index==memory[firstin][1])
                        {
                            TLB[m][0]=-1;
                            TLB[m][1]=-1;
                            TLB[m][2]=-1;
                        }
                    }
                    memory[firstin][0] = real_vpi; // put to end of memory
                    memory[firstin][1]=(vpi-real_vpi)/virtual;
                    memory[firstin][2]=1;
                    firstin++;
                    firstin %= physical_frame;
                    test_true++;
                }
                else if (memory[firstin][2]==1)
                {
                    memory[firstin][2]=0;
                    firstin++;
                    firstin %= physical_frame;
                }
            }
            break;
        }
    }
    for(int i = 0; i < virtual_page; i++)
    {
        if(memdisk[i].disk_index == real_vpi && memdisk[i].process_num==((vpi-real_vpi)/virtual))
        {
            src = i;
            memdisk[i].disk_index=-1;
            memdisk[i].process_num=-1;
            break;
        }
        else
        {
            src = -1;
        }
    }
    fprintf(f_trace_output,"Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n", refer_process, index_out, evpi,name, dest, real_vpi, src);
    count_lookup[temp_index]++;
    name=0;
}
void clock_replacement_local(char refer_process, int vpi,int real_vpi,int pro_index)
{
    int index_out,evpi = -1, dest = -1, src = -1;
    char name=0;
    int test_true=0;
    for(int i = 0; i < virtual_page; i++)
    {
        if(memdisk[i].disk_index == -1)
        {
            while(test_true==0)
            {
                if (memory[first[pro_index]][2]==0)
                {
                    index_out = first[pro_index];  // assign to the first one
                    evpi = memory[first[pro_index]][0];  // virtual page number that will be swaped out
                    dest = i; // destination of swap out page
                    // put evicted page to disk
                    memdisk[dest].disk_index = evpi;  // swap out evpi
                    memdisk[dest].process_num=memory[first[pro_index]][1];
                    pagetable[vpi].pfdbi= pagetable[evpi+(virtual*memory[first[pro_index]][1])].pfdbi;
                    pagetable[evpi+(virtual*memory[first[pro_index]][1])].pfdbi=-1;
                    pagetable[evpi+(virtual*memory[first[pro_index]][1])].present=0;
                    name = find_process(memory[first[pro_index]][1]);
                    pagetable[evpi+(virtual*memory[first[pro_index]][1])].process_name = -1;
                    for (int m=0; m<32; m++)
                    {
                        if (TLB[m][0]==evpi)
                        {
                            TLB[m][0]=-1;
                            TLB[m][1]=-1;
                            TLB[m][2]=-1;
                        }
                    }
                    memory[first[pro_index]][0] = real_vpi; // put to end of memory
                    memory[first[pro_index]][1]=(vpi-real_vpi)/virtual;
                    memory[first[pro_index]][2]=1;
                    first[pro_index]++;
                    first[pro_index] %= physical_frame;
                    while(memory[first[pro_index]][1]!=pro_index)
                    {
                        first[pro_index]++;
                        first[pro_index] %= physical_frame;
                    }
                    test_true++;
                }
                else if (memory[first[pro_index]][2]==1)
                {
                    memory[first[pro_index]][2]=0;
                    first[pro_index]++;
                    first[pro_index] %= physical_frame;
                    while(memory[first[pro_index]][1]!=pro_index)
                    {
                        first[pro_index]++;
                        first[pro_index] %= physical_frame;
                    }
                }
            }
            break;
        }
    }
    for(int i = 0; i < virtual_page; i++)
    {
        if(memdisk[i].disk_index == real_vpi && memdisk[i].process_num==((vpi-real_vpi)/virtual))
        {
            src = i;
            memdisk[i].disk_index=-1;
            memdisk[i].process_num=-1;
            break;
        }
        else
        {
            src = -1;
        }
    }
    fprintf(f_trace_output,"Process %c, TLB Miss, Page Fault, %d, Evict %d of Process %c to %d, %d<<%d\n", refer_process, index_out, evpi,name, dest, real_vpi, src);
    count_lookup[pro_index]++;
    name=0;
}
int main(int argc, char *argv[])
{
    f_trace_output = fopen("trace_output.txt", "w");
    FILE *f_analysis = fopen("analysis.txt", "w");
    int test_full=0,test_hit=0,test_tlb=0,temp=0,pro_index=0,refer_num=0;
    int label_process = -1;  // use to distinguish btw process switch
    char refer_process=0;
    int count_in[20];
    for (int i=0; i<20; i++)
    {
        count_in[i]=0;
        count_lookup[i]=0;
    }
    TLB_initialize();
    // first count  (for local FIFO policy) and page fault initialize
    for (int i=0; i<20; i++)
    {
        first[i]=-1;
    }
    for (int i=0; i<20; i++)
    {
        for (int j=0; j<2; j++)
        {
            page_fault[i][j]=0;
        }
    }
    int Hit[20];
    for (int i=0; i<20; i++)
    {
        Hit[i]=0;
    }
    read_file();
    // page table
    virtual = virtual_page;
    virtual_page = num_process*virtual_page;
    memdisk = malloc(virtual_page * sizeof(disk));
    pagetable = malloc(virtual_page * sizeof(page));
    for(int i = 0; i < virtual_page; i++)
    {
        pagetable[i].pfdbi = -1;
        pagetable[i].process_name = -1;
        pagetable[i].inuse = 0;
        pagetable[i].present = -1;
        memdisk[i].disk_index=-1;
        memdisk[i].process_num=-1;
    }
    //free_frame
    frame_list = malloc(physical_frame * sizeof(int));
    memset(frame_list, -1, physical_frame * sizeof(int));
    //physical memory list
    memory = malloc(physical_frame * sizeof(int*));

    for (int i=0; i<physical_frame; i++)
    {
        memory[i]= malloc(3*sizeof(int));
    }
    for (int i=0; i<physical_frame; i++)
    {
        for (int j=0; j<3; j++)
        {
            memory[i][j]=-1;
        }
    }
    char trace[1000][20];
    for (int i=0; i<1000; i++)
    {
        for (int j=0; j<20; j++)
        {
            trace[i][j]=0;
        }
    }
    long long int trace_count = 0;
    FILE *f_trace = fopen("trace.txt", "r");
    while(fgets(trace[trace_count],sizeof(trace[trace_count]),f_trace)!=NULL)
    {
        trace_count++;
    }
    while (temp<trace_count)
    {
        sscanf(trace[temp],"Reference(%c, %d)",&refer_process, &refer_num);
        pro_index = find_index (refer_process);
        page_fault[pro_index][1]++;  // total time the process is referenced
        if (pagetable[refer_num+pro_index*virtual].process_name==-1)
        {
            pagetable[refer_num+pro_index*virtual].process_name = pro_index;
        }
        if (label_process!=pro_index)   // first flush the table of TLB
        {
            label_process = pro_index;
            TLB_initialize();
        }
        for (int i=0; i<32; i++)
        {
            if (TLB[i][0]==refer_num)
            {
                test_tlb=1;
                Hit[pro_index]++;
                //count_lookup[pro_index]+=i;  // put i times look up in count look up
                TLB[i][2]=temp;
                fprintf (f_trace_output,"Process %c, TLB Hit, %d=>%d\n",refer_process,refer_num,TLB[i][1]);
                count_lookup[pro_index]++;
                for (int i=0; i<physical_frame; i++)
                {
                    if (memory[i][0]==refer_num && memory[i][1]==pro_index)
                    {
                        memory[i][2]=1;
                    }
                }
                break;
            }
            else
            {
                if (TLB[i][0]==-1)
                {
                    test_full++;
                }
                test_hit++;
            }
        }
        if (test_hit==32)  // not find data in TLB
        {
            test_hit=0;
            if (pagetable[refer_num+pro_index*virtual].pfdbi!=-1)
            {
                fprintf (f_trace_output,"Process %c, TLB Miss, Page Hit, %d=>%d\n",refer_process, refer_num, pagetable[refer_num+pro_index*virtual].pfdbi);
                count_lookup[pro_index]++;
                for (int i=0; i<physical_frame; i++)
                {
                    if (memory[i][0]==refer_num && memory[i][1]==pro_index)
                    {
                        memory[i][2]=1;
                    }
                }
                if (test_full >0 && test_full<=32)
                {
                    for (int i=0; i<32; i++)
                    {
                        if (TLB[i][0]==-1)
                        {
                            TLB[i][0]=refer_num;
                            TLB[i][1]=pagetable[refer_num+pro_index*virtual].pfdbi;
                            TLB[i][2]=temp;
                            break;
                        }
                    }
                }
                else
                {
                    if (strcmp(tlb_policy,"RANDOM")==0)
                    {
                        random_num(refer_num,pagetable[refer_num+pro_index*virtual].pfdbi);
                    }
                    else if (strcmp(tlb_policy, "LRU")==0)
                    {
                        // do LRU replacement
                        LRU_replacement(refer_num,pagetable[refer_num+pro_index*virtual].pfdbi,temp);
                    }
                }
            }
            else
            {
                //free frame list
                page_fault[pro_index][0]++;
                int test_freeframe = 0;
                for (int i=0; i<physical_frame; i++)
                {
                    if (frame_list[i]==-1)
                    {
                        frame_list[i]=refer_num; // record the virtual page num that uses frame
                        pagetable[refer_num+pro_index*virtual].pfdbi = i;  // mark to frame i
                        pagetable[refer_num+pro_index*virtual].present = 1; // it is on physical mem
                        pagetable[refer_num+pro_index*virtual].inuse = 1; // it is referenced
                        memory[i][0]=refer_num;  // have to change sth here, change to 2D array to record the frame of each process
                        memory[i][1]=pro_index;
                        memory[i][2]=1;
                        if (count_in[pro_index]==0)
                        {
                            first[pro_index]=i;
                            count_in[pro_index]=1;
                        }
                        if (test_full >0 && test_full<=32)
                        {
                            for (int i=0; i<32; i++)
                            {
                                if (TLB[i][0]==-1)
                                {
                                    TLB[i][0]=refer_num;
                                    TLB[i][1]=pagetable[refer_num+pro_index*virtual].pfdbi;
                                    TLB[i][2]=temp;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            // check if TLB full, if it is full then do replacement
                            if (strcmp(tlb_policy,"RANDOM")==0)
                            {
                                random_num(refer_num,pagetable[refer_num+pro_index*virtual].pfdbi);
                            }
                            else if (strcmp(tlb_policy, "LRU")==0)
                            {
                                // do LRU replacement
                                LRU_replacement(refer_num,pagetable[refer_num+pro_index*virtual].pfdbi,temp);
                            }
                        }
                        fprintf (f_trace_output,"Process %c, TLB Miss, Page Fault, %d, Evict -1 of Process %c to -1, %d<<-1\n",refer_process, i, refer_process, refer_num);
                        count_lookup[pro_index]++;

                        break;
                    }
                    else
                    {
                        test_freeframe++;
                    }
                }
                if (test_freeframe==physical_frame)
                {
                    test_freeframe=0;
                    if (strcmp(page_policy,"FIFO")==0 && strcmp(frame_policy,"GLOBAL")==0)
                    {
                        FIFO(refer_process,(refer_num+pro_index*virtual),refer_num);
                    }
                    else if (strcmp(page_policy,"FIFO")==0&& strcmp(frame_policy,"LOCAL")==0)
                    {
                        FIFO_local(refer_process,(refer_num+pro_index*virtual),refer_num,pro_index);
                    }
                    else if (strcmp(page_policy,"CLOCK")==0 && strcmp(frame_policy,"GLOBAL")==0)
                    {
                        clock_replacement(refer_process,(refer_num+pro_index*virtual),refer_num);
                    }
                    else if (strcmp(page_policy,"CLOCK")==0 && strcmp(frame_policy,"LOCAL")==0)
                    {
                        clock_replacement_local(refer_process,(refer_num+pro_index*virtual),refer_num,pro_index);
                    }
                    // check if TLB full, if it is full then do replacement
                    if (test_full >0 && test_full<=32)
                    {
                        for (int i=0; i<32; i++)
                        {
                            if (TLB[i][0]==-1)
                            {
                                TLB[i][0]=refer_num;
                                TLB[i][1]=pagetable[refer_num+pro_index*virtual].pfdbi;
                                TLB[i][2]=temp;
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (strcmp(tlb_policy,"RANDOM")==0)
                        {
                            random_num(refer_num,pagetable[refer_num+pro_index*virtual].pfdbi);
                        }
                        else if (strcmp(tlb_policy, "LRU")==0)
                        {
                            // do LRU replacement
                            LRU_replacement(refer_num,pagetable[refer_num+pro_index*virtual].pfdbi,temp);
                        }
                    }
                }
            }
        }
        if (test_tlb==0)
        {
            for (int i=0; i<32; i++)
            {
                if (TLB[i][0]==refer_num)
                {
                    Hit[pro_index]++;
                    TLB[i][2]=temp;
                    fprintf (f_trace_output,"Process %c, TLB Hit, %d=>%d\n",refer_process,refer_num,TLB[i][1]);
                    count_lookup[pro_index]++;
                    break;
                }
            }
        }
        else
        {
            test_tlb=0;
        }
        refer_num = 0;
        test_full=0;
        test_hit=0;
        pro_index = -1;
        refer_process = 0;
        temp++;
    }
    char pro_name = 0;
    double page_fault_rate = 0.0;
    double hit_ratio=0.0;
    for (int i=0; i<20; i++)
    {
        if (page_fault[i][0]!=0 || Hit[i]!=0)
        {
            pro_name = find_process(i);
            page_fault_rate = (double)(((double)(page_fault[i][0]))/((double)page_fault[i][1]));
            hit_ratio = (double)(((double)(Hit[i]))/((double)count_lookup[i]));
            fprintf (f_analysis,"Process %c, Effective Access Time = %.3f\n",pro_name, (hit_ratio*120 + (1-hit_ratio)*220));
            fprintf (f_analysis,"Page Fault Rate: %.3f\n",page_fault_rate);
            pro_name=0;
            page_fault_rate=0.0;
            hit_ratio=0.0;
        }
    }
    fclose(f_trace);
    fclose(f_analysis);
}
