#include "cachelab.h"
#include<getopt.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>

#define MAX_SIZE 1024
typedef unsigned long add;

struct block{
    int tag;//数据是否有效
    add head;//存放的头地址
    int clock;//被调用的时间，越小说明时间越近
};
void init_cache();//初始化块的信息
void data_load(add address,int size);//读数据
void data_store(add address,int size);//写数据
int find_lattest(int index);//找到最久未被访问的块
void clock_num(int position);//计时函数

int s,E,b,B,S;
/*S是组数，B是字节数，E是每个组的行数*/
char *file;//文件名
int eviction,hint,miss,verbose;//t用于计数调用次数
struct block cache[MAX_SIZE];//块的信息

int main(int argc,char *argv[])
{   
    /*对输入的cache的相关信息进行处理，得到对应的E,s,b*/
    int opt;
    verbose = 0;
    while((opt = getopt(argc,argv,"v s:E:b:t:"))!=-1){
        switch(opt){
        case 's':
        s = atoi(optarg);
        break;
        case 'E':
        E = atoi(optarg); 
        break;
        case 'b':
        b = atoi(optarg);
        break;
        case 't':
        file = optarg;
        break;
        case 'v':
        verbose = 1;
        break;
        }
    }
    /*处理文件里的命令行*/
    S = 1 << s;
    B = 1 << b;
    hint = 0;
    eviction = 0;
    miss = 0;
    init_cache();//初始化模拟缓存，实际需要的是S*E个block
    char cmdline[MAX_SIZE];
    char *cmd = cmdline;
    add address;
    int size;
    FILE *fp = fopen(file,"r");
    while(fgets(cmd,64,fp) != NULL){
        if(*cmd != ' '){
            continue;
        }
        if(verbose == 1){
            printf("\n");
        }//处理M指令
        if(*(cmd+1) == 'M'){
            address = strtol((cmd + 2),NULL,16);
            while(*(cmd)!=',')
            cmd++;
            size = atoi((cmd + 1));
            if(verbose == 1){
                while(*cmd!='\n')
                cmd++;
                *(cmd)=' ';
                printf("%s ",cmdline);
            }
            data_load(address,size);
            data_store(address,size);
        }/*先读数据再写数据*/
        else if(*(cmd+1) == 'L'){
            address = strtol((cmd + 2),NULL,16);
            while(*(cmd)!=',')
            cmd++;
            size = atoi((cmd + 1));
            if(verbose == 1){
                while(*cmd!='\n')
                cmd++;
                *(cmd)=' ';
                printf("%s ",cmdline);
            }
            data_load(address,size);
        }/*读数据,得到对应的地址和字节数*/
        else if(*(cmd+1) == 'S'){
            address = strtol((cmd + 2),NULL,16);
            while(*(cmd)!=',')
            cmd++;
            size = atoi((cmd + 1));
            if(verbose == 1){
                while(*cmd!='\n')
                cmd++;
                *(cmd)=' ';
                printf("%s ",cmdline);
            }
            data_store(address,size);
        }/*写数据，这里采用最久未访问访问覆写*/
        cmd = cmdline;
    }
    printSummary(hint, miss, eviction);
    return 0;
}
/*用于读操作*/
void data_load(add address,int size){
    int index;
    index = address/B%S;//计算组索引
    for(int i=0;i<E;i++){
        if(cache[i+index*E].tag == 1){
            if(cache[i+index*E].head == (address/B)*B){
            hint++;
            clock_num(i+index*E);
            cache[i+index*E].clock = 0;//对其他块进行计时，并清空该计时器
            if(verbose == 1){
                printf("%lu %ld ",cache[i+index*E].head,(address/B)*B);
                printf(" hit");
            }
            return;
        }
    }
    }
    miss++;
    int position;
    position = find_lattest(index);//得到被替换的块的下标
    if(cache[position].tag == 1){
        eviction++;
    }
    cache[position].head = (address/B)*B;//这里得到对应块的头地址，利用了计算的截断
    cache[position].clock = 0;
    cache[position].tag = 1;//确定数据是否有意义
    clock_num(position);
    if(verbose == 1){
        printf(" miss");
    }
    return;
}
/*用于写操作*/
void data_store(add address,int size){
    int index;
    index = address/B%S;//得到组索引
    //printf("%d ",index);
    for(int i=0;i<E;i++){
        if(cache[i+index*E].tag == 1){
            if(cache[i+index*E].head == (address/B)*B){//判断块是否有数据
            hint++;//命中计数器加1
            clock_num(i+index*E);
            cache[i+index*E].clock = 0;
            if(verbose == 1){
                printf(" hit");
            }//调试输出
            return;
        }
        }
    }
    miss++;
    int position;
    position = find_lattest(index);//得到要调整的块的位置
    if(verbose == 1){
        printf(" miss");
    }
    if(cache[position].tag == 1){
        eviction++;//替换计数器加一
        if(verbose == 1){
                printf(" eviction");
            }
    }
    cache[position].head = (address/B)*B;//这里得到对应块的头地址，利用了计算的截断
    cache[position].clock = 0;//调用次数复原
    cache[position].tag = 1;//设置为有效数据
    clock_num(position);
    return;
}
/*初始化全部的块*/
void init_cache(){
    for(int i=0;i<MAX_SIZE;i++){
        cache[i].tag = 0;
        cache[i].head = 0;
        cache[i].clock = 0;
    }
    return;
}
/*锁定目标块*/
int find_lattest(int index){
    int ans = -1,temp = -1;
    for(int i=0;i<E;i++){
        if(cache[i+index*E].tag == 0){
            temp = i+index*E;
            break;
        }
        if(ans<cache[i+index*E].clock){
            ans = cache[i+index*E].clock;
            temp = i+index*E;
        }
    }
    return temp;
}
/*计时模块*/
void clock_num(int position){
    for(int i=0;i<E*S;i++){
        if(i == position){
            continue;
        }
        cache[i].clock++;
    }
    return;
}