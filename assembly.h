#ifndef ASSEMBLY_H
#define ASSEMBLY_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<QString>
#include<QDebug>

class assembly{
    char operation[6];
    char operands[100];
    char ch;
    unsigned long instruction;
    unsigned long regist[3];
    long imm;
    long count;
    int i;
    int p;
    int type;
    QString ins;

    struct list{
        char name[10];
        int position;
        struct list *next;
    }*head, *tail, *temp;


    void getreg()
    {
        while(operands[p]!='$'){
            p++;
        }
        switch(operands[++p]){
            case 'z':{
                regist[i]=0;
                break;
            }
            case 'a':{
                if(operands[++p]=='t')
                    regist[i]=1;
                else regist[i]=operands[p]-'0'+4;
                break;
            }
            case 'v':{
                regist[i]=operands[++p]-'0'+2;
                break;
            }
            case 't':{
                if(operands[++p]<='7')
                    regist[i]=operands[p]-'0'+8;
                else regist[i]=operands[p]-'0'+16;
                break;
            }
            case 's':{
                if(operands[++p]=='p')
                    regist[i]=29;
                else regist[i]=operands[p]-'0'+16;
                break;
            }
            case 'k':{
                regist[i]=operands[++p]-'0'+26;
                break;
            }
            case 'g':{
                regist[i]=28;
                break;
            }
            case 'f':{
                regist[i]=30;
                break;
            }
            case 'r':{
                regist[i]=31;
                break;
            }
        }
    }

    void regregreg()
    {
        p=0;
        for(i=0;i<3;i++){
            getreg();
        }
    }
    void getimm()
    {
        while(operands[p]==' ')p++;

        if(operands[p]>='a'&&operands[p]<='z'){
            char tt[100];
            int t=0;
            while(operands[p]!=' '&&operands[p]!='\n')
                tt[t++]=operands[p++];
            tt[t]='\0';

            temp=head->next;
            while(temp!=NULL){
                if(!strcmp(temp->name,tt)){
                    imm=temp->position;
                    break;
                }else
                temp=temp->next;
            }
            type=0;
        }
        else{
            int flag=1;
            imm=0;
            while(operands[p]>='0'&&operands[p]<='9'||operands[p]=='-'){
                if(operands[p]=='-')
                    flag=-1;
                else
                    imm=imm*10+operands[p]-'0';
                p++;
            }
            imm=imm*flag;
            type=1;
        }
    }

    void regimmreg()
    {
        p=0;
        for(i=0;i<3;i++){
            if(i!=1)
                getreg();
            else{
                while(operands[p++]!=',');
                getimm();
            }
        }
    }

    void regregimm()
    {
        p=0;
        for(i=0;i<3;i++){
            if(i!=2)
                getreg();
            else{
                while(operands[p++]!=',');
                getimm();
            }
        }
    }

public:
    assembly(){}
    ~assembly(){}

    QString start_assembly(QString path)
    {
        QByteArray ppath = path.toLatin1();

        const char* p=ppath.data();
        //QDebug<<path;

        FILE *fp = fopen(p, "r");
        FILE *fbi = fopen("binary","wb");
        FILE *ftext = fopen("binary.o","w");
        if(!fp){
            printf("ERROR!!!\n");
            return 0;
        }
        head = (list*)malloc(sizeof(struct list));
        strcpy(head->name,"");
        head->position = 0;
        head->next = NULL;

        tail = head;

        count = 0;

        while(fscanf(fp,"%s",operation)!=EOF){
            if(operation[strlen(operation)-1]==':'){
                operation[strlen(operation)-1]='\0';
                temp = (list*)malloc(sizeof(struct list));
                strcpy(temp->name,operation);
                temp->position = count;
                temp->next = NULL;
                tail->next = temp;
                tail = tail->next;
            }
            do{
                if(fscanf(fp,"%c",&ch)==EOF)break;
            }while(ch!='\n');

            count++;
        }
        fseek(fp , 0, SEEK_SET);
        count=0;
        while(fscanf(fp,"%s",operation)!=EOF){
            if(operation[strlen(operation)-1]==':')
                fscanf(fp,"%s",operation);
            //fscanf(fp,"%s",operands);
            fgets(operands,100,fp);

            //printf("{%s:%s}",operation,operands);

            if(!strcmp(operation,"add")){
                regregreg();
                instruction=0x00000020|(regist[1]<<21)|(regist[2]<<16)|(regist[0]<<11);
            }
            else if(!strcmp(operation,"sub")){
                regregreg();
                instruction=0x00000022|(regist[1]<<21)|(regist[2]<<16)|(regist[0]<<11);
            }
            else if(!strcmp(operation,"slt")){
                regregreg();
                instruction=0x0000002A|(regist[1]<<21)|(regist[2]<<16)|(regist[0]<<11);
            }
            else if(!strcmp(operation,"lw")){
                regimmreg();
                instruction=0x8C000000|(regist[2]<<21)|(regist[0]<<16);
                instruction=instruction|(((unsigned long)imm)<<16>>16);
            }
            else if(!strcmp(operation,"sw")){
                regimmreg();
                instruction=0xAC000000|(regist[2]<<21)|(regist[0]<<16);
                instruction=instruction|(((unsigned long)imm)<<16>>16);
            }
            else if(!strcmp(operation,"beq")){
                regregimm();
                instruction=0x10000000|(regist[1]<<21)|(regist[0]<<16);
                if(type==1)
                    instruction=instruction|(((unsigned long)imm)<<16>>16);
                else if(type==0){
                    //printf("(%x %x %x)",imm,count,(imm-count-1));
                    instruction=instruction|(((unsigned long)(imm-count-1))<<16>>16);
                }

            }
            else if(!strcmp(operation,"j")){
                p=0;
                getimm();
                instruction=0x08000000|(((unsigned long)imm)<<6>>6);
                instruction=0x08000003;
            }
            else{
                return QString("error!");
                return 0;
            }
            fwrite(&instruction,4,1,fbi);

            //fprintf(ftext,"%x\n",instruction);
            //printf("%x\n",instruction);

            QString insline = QString::number(instruction,2);
            int zero=insline.size();
            for(int j=zero;j<32;j++)
                insline.prepend("0");
            ins+=insline;
            ins+='\n';
            count++;
        }
        return ins;
    }
};


#endif // ASSEMBLY_H
