#ifndef STIMULATE_H
#define STIMULATE_H

#include<stdio.h>
#include<ctype.h>
#include<QString>

#define MEMMAX 100
#define ISNUM 100
class stimulate{
    unsigned long instru[ISNUM], Memory[MEMMAX];
    int i, pc, op, rs, rt, rd, shmt, func, data, flag = 0, PCmax;
    long int addr;
    long reg[32];
    //FILE *fp1, *fp2, *fp3;
    QString state;

    void decode()
    {
        op = (instru[pc]>>26)&0x3f;
        rs = (instru[pc]>>21)&0x1f;
        rt = (instru[pc]>>16)&0x1f;
        rd = (instru[pc]>>11)&0x1f;
        shmt = (instru[pc]>>26)&0x3f;
        func = instru[pc]&0x3f;
        data = instru[pc]&0xffff;
        addr = instru[pc]&0x3ffffff;
    }

    void Add()
    {
        reg[rd] = reg[rs] + reg[rt];
    }
    void Sub()
    {
        reg[rd] = reg[rs] - reg[rt];
    }
    void And()
    {
        reg[rd] = reg[rs] & reg[rt];
    }
    void Or()
    {
        reg[rd] = reg[rs] | reg[rt];
    }
    void Nor()
    {
        reg[rd] = reg[rs] ^ reg[rt];
    }
    void Slt()
    {
        if (reg[rs]<reg[rt])
            reg[rd] = 1;
        else reg[rd] = 0;
    }
    void Jr()
    {
        pc = reg[rs];
    }
    void Sll()
    {
        reg[rd] = reg[rt] << shmt;
    }
    void Srl()
    {
        reg[rd] = reg[rt] >> shmt;
    }
    void Beq()
    {
        if (reg[rs] == reg[rt])
        {
            pc = pc + data;
            flag = 1;
        }
    }
    void Bne()
    {
        if (reg[rs] != reg[rt])
        {
            pc = pc + data;
            flag = 1;
        }
    }
    void Lw()
    {
        int p;
        p = reg[rs] + data;
        qDebug()<<rt;
        qDebug()<<p;
        reg[rt] = Memory[p];
    }
    void Sw()
    {
        int p;
        p = reg[rs] + data;
        Memory[p] = reg[rt];
    }
    void Addi()
    {
        reg[rt] = reg[rs] + data;
    }
    void Andi()
    {
        reg[rt] = reg[rs] & data;
    }
    void Ori()
    {
        reg[rt] = reg[rs] | data;
    }
    void Lui()
    {
        reg[rt] = reg[rs] & data;
    }
    void Slti()
    {
        reg[rt] = reg[rs] & data;
    }
    void J()
    {
        pc = addr;
        flag = 1;
    }
    void Jal()
    {
        pc = addr;
        flag = 1;
    }

    void exec()
    {
        switch (op){
        case 0:         //R-format
            switch (func){
            case 32: Add();		break;
                //		case 33: addu();	break;
            case 34: Sub();		break;
                //		case 35: subu();	break;
            case 36: And();		break;
            case 37: Or();		break;
            case 39: Nor();		break;
            case 42: Slt();		break;
                //		case 43: sltu();	break;
            case  8: Jr();		break;
            case  0: Sll();		break;
            case  2: Srl();		break;
            } break;
        case 4:		Beq();	break;
        case 5:		Bne();	break;
        case 35:	Lw();	break;
        case 43:	Sw();	break;
            //	case 40:	sb();	break;
            //	case 41:	sh();	break;
            //	case 36:	lbu();	break;
            //	case 37:	lhu();	break;
        case 8:		Addi(); break;
            //	case 9:		addiu();break;
        case 12:	Andi(); break;
        case 15:	Lui();	break;
        case 10:	Slti(); break;
            //	case 11:	sltiu();break;
        case 13:	Ori();	break;
        case 2:		J();	break;
        case 3:		Jal();	break;
        }
    }

    void read(QString bin)
    {
        bool ok;
        int n=bin.count( QRegExp("\n"));
        qDebug()<< n;
        int i=0;
        while(i<=n){
            QString qline=bin.section('\n',i,i);
            instru[i]=qline.toULong(&ok,2);
            i++;
        }
        PCmax = i - 1;
        for (i = 0; i<MEMMAX; i++)
            Memory[i] = i;
    }

    void moni()
    {
        decode();
        exec();
        if (flag == 0) pc++;
        flag = 0;
    }

    void display()
    {
        state+="\n\n\nPC=";
        state+=QString::number(pc*4,10);
        state+="\n";
        for (i = 0; i<32; i++)
        {
            state=state+"Reg["+QString::number(i,10)+"]="+QString::number(reg[i],10)+"\t";

//            if ((i + 1) % 4 == 0)
//                    state+="\n";

        }
        //fprintf(fp3, "pc=%d\t\n", pc * 4);
        //fprintf(fp3, "Reg[%d]=%ld\t", i, reg[i]);
        //fprintf(fp3, "\n");
        //for (i = 0; i<MEMMAX; i++)
        //    fprintf(fp3, "Memory[%d]=%d\n", i, Memory[i]);
        //fprintf(fp3, "\n\n");
    }
    void initialize()
    {
        for (i = 0; i<32; i++)
            reg[i] = 0;
        pc = 0;
        //reg[17] = 8;
        //reg[18] = 9;
        //reg[19] = 3;
        //reg[20] = 4;
    }

public:

    stimulate(){}
    ~stimulate(){}
    QString start_stimulate(QString bin)
    {

        initialize();
        read(bin);
        state+=QString::number(PCmax,10);
        state+=" instrus:";
        while (pc<PCmax)
        {
            moni();
            display();
        }
        return state;
    }
};

#endif // STIMULATE_H

