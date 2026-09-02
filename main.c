#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>


enum opcode_decode {R = 0x33, I = 0x13, S = 0x23, L = 0x03, B = 0x63, JALR = 0x67, JAL = 0x6F, AUIPC = 0x17, LUI = 0x37};

/* Speicherabgebildete Ausgabe (Memory Mapped IO):
 * Ein per SB auf diese Adresse geschriebenes Byte wird als Zeichen auf der
 * Standardausgabe ausgegeben, statt in den Datenspeicher zu wandern. */
#define MMIO_OUT_ADDR 0x5000u

typedef struct {
    size_t data_mem_size_;
    uint32_t regfile_[32];
    uint32_t pc_;
    uint8_t* instr_mem_;
    uint8_t* data_mem_;
} CPU;

void CPU_open_instruction_mem(CPU* cpu, const char* filename);
void CPU_load_data_mem(CPU* cpu, const char* filename);

CPU* CPU_init(const char* path_to_inst_mem, const char* path_to_data_mem) {
	CPU* cpu = (CPU*) malloc(sizeof(CPU));
	cpu->data_mem_size_ = 0x400000;
    cpu->pc_ = 0x0;
    CPU_open_instruction_mem(cpu, path_to_inst_mem);
    CPU_load_data_mem(cpu, path_to_data_mem);
    return cpu;
}

void CPU_open_instruction_mem(CPU* cpu, const char* filename) {
	uint32_t  instr_mem_size;
	FILE* input_file = fopen(filename, "r");
	if (!input_file) {
			printf("no input\n");
			exit(EXIT_FAILURE);
	}
	struct stat sb;
	if (stat(filename, &sb) == -1) {
			printf("error stat\n");
			perror("stat");
		    exit(EXIT_FAILURE);
	}
	printf("size of instruction memory: %ld Byte\n\n",(long)sb.st_size);
	instr_mem_size =  sb.st_size;
	cpu->instr_mem_ = malloc(instr_mem_size);
	fread(cpu->instr_mem_, sb.st_size, 1, input_file);
	fclose(input_file);
	return;
}

void CPU_load_data_mem(CPU* cpu, const char* filename) {
	FILE* input_file = fopen(filename, "r");
	if (!input_file) {
			printf("no input\n");
			exit(EXIT_FAILURE);
	}
	struct stat sb;
	if (stat(filename, &sb) == -1) {
			printf("error stat\n");
			perror("stat");
		    exit(EXIT_FAILURE);
	}
	printf("read data for data memory: %ld Byte\n\n",(long)sb.st_size);

    cpu->data_mem_ = malloc(cpu->data_mem_size_);
	fread(cpu->data_mem_, sb.st_size, 1, input_file);
	fclose(input_file);
	return;
}

// loads an 8-bit byte form data memory at the given address
uint8_t CPU_load_byte(CPU* cpu, uint32_t addr){
   return cpu -> data_mem_[addr];
}


uint16_t CPU_load_halfword(CPU* cpu, uint32_t addr){
    uint16_t halfword = cpu->data_mem_[addr+1];
    halfword =halfword<<8;
    halfword = (halfword | (cpu->data_mem_[addr])); 
    return halfword;
} 


uint32_t CPU_load_word(CPU* cpu, uint32_t addr){
    uint32_t word = ((uint32_t)cpu->data_mem_[addr + 3] << 24)
                  | ((uint32_t)cpu->data_mem_[addr + 2] << 16)
                  | ((uint32_t)cpu->data_mem_[addr + 1] << 8)
                  |  (uint32_t)cpu->data_mem_[addr];

    return word;
}

/**
 * Speichert ein Byte (Befehl SB) -- VORGEGEBEN, bitte nicht veraendern.
 *
 * Liegt die Zieladresse auf der speicherabgebildeten Ausgabeadresse
 * MMIO_OUT_ADDR (0x5000), wird das Byte als Zeichen auf der Standardausgabe
 * ausgegeben (Zeichenausgabe / Memory Mapped IO). Andernfalls wird es ganz
 * normal in den Datenspeicher geschrieben.
 *
 * Rufen Sie diese Funktion bei der Implementierung des Befehls SB auf:
 *     CPU_store_byte(cpu, cpu->regfile_[rs1] + imm, (uint8_t)cpu->regfile_[rs2]);
 * Saemtliche Textausgabe der Testprogramme laeuft ueber diese Ausgabe.
 */
void CPU_store_byte(CPU* cpu, uint32_t addr, uint8_t value) {
	if (addr == MMIO_OUT_ADDR) {
		putchar((int)value);
	} else {
		cpu->data_mem_[addr] = value;
	}
}

/**
 * Instruction fetch Instruction decode, Execute, Memory access, Write back
 */
// instruction & 0x7F opcode extrahieren
void CPU_execute(CPU* cpu,uint32_t instruction,int* pc_modified) {
    uint32_t old_x1 = cpu->regfile_[1];
	//uint32_t instruction = *(uint32_t*)(cpu->instr_mem_ + (cpu->pc_ & 0xFFFFF));
    uint32_t opcode = instruction & 0x7F;
    uint32_t rd     = (instruction >> 7)  & 0x1F;
    uint32_t rs1    = (instruction >> 15) & 0x1F;
    uint32_t rs2    = (instruction >> 20) & 0x1F;
    uint32_t funct3 = (instruction >> 12) & 0x07;
    uint32_t funct7 = (instruction >> 25) & 0x7F;

    switch (opcode) {

        case 0x13: { // ADDI (I-Format, funct3 == 0)
            // 12-Bit-Immediate aus den Bits 31:20 herausschneiden
            uint32_t imm_u = (instruction >> 20) & 0xFFF;

            // Vorzeichenerweiterung von 12 auf 32 Bit
            int32_t imm = (imm_u & 0x800)
                        ? (int32_t)(imm_u | 0xFFFFF000)
                        : (int32_t)imm_u;
            
            if (rd != 0) // Register x0 bleibt immer 0

                switch(funct3){
                    case 0x0 : {//ADDI

                cpu->regfile_[rd] = cpu->regfile_[rs1] + imm;
                        break;
                        }
                    case 0x1 : {//SLLI
                        cpu->regfile_[rd] = cpu->regfile_[rs1] << (imm & 0x1F);
                        break;
                    }
                    case 0x2 : {//SLTI
                        cpu->regfile_[rd]= (int32_t)(cpu->regfile_[rs1]) < imm;
                        break;
                        }
                    case 0x3 : { //SLTIU
                        cpu->regfile_[rd]= cpu->regfile_[rs1]<(uint32_t)(imm);
                        break;
                    }
                    case 0x4 : {//XORI
                        cpu->regfile_[rd]=cpu->regfile_[rs1]^imm;
                        break;
                    }
                    case 0x5 :{//SRLI / SRAI
                        if(0x0== ((imm_u>>5)&0x7F)){
                            cpu->regfile_[rd]=cpu->regfile_[rs1]>>(imm & 0x1F);
                        }
                        else if(0x20 == ((imm_u>>5)&0x7F)){
                            cpu->regfile_[rd]=(int32_t)(cpu->regfile_[rs1])>>(imm & 0x1F);
                        }
                    break;
                    }
                    case 0x6:{ //ORI
                        cpu->regfile_[rd]=cpu->regfile_[rs1]|imm;
                        break;
                    }
                    case 0x7: {//ANDI
                        cpu->regfile_[rd]=cpu->regfile_[rs1]&imm;
                        break;
                    }
                    }
            break;
        }

        // R-Type
        case 0x33: { 
        if(rd!=0){

            if(funct7==0x01){ // M Erweiterung
                switch (funct3)
                {
                case 0x0:{ //MUL
                    cpu->regfile_[rd]=(uint32_t)((int64_t)(int32_t)cpu->regfile_[rs1] * (int64_t)(int32_t)cpu->regfile_[rs2]);
                    break;
                }
                 case 0x1:{ //MULH
                    cpu->regfile_[rd]=((int64_t)(int32_t)cpu->regfile_[rs1]*(int64_t)(int32_t)cpu->regfile_[rs2]) >> 32;
                    break;
                }
                 case 0x2:{ //MULHSU
                    cpu->regfile_[rd]= ((int64_t)(int32_t)cpu->regfile_[rs1]*(uint64_t)cpu->regfile_[rs2])>>32;
                    break;
                }
                 case 0x3:{ //MULHU
                    cpu->regfile_[rd]= ((uint64_t)cpu->regfile_[rs1]*(uint64_t)cpu->regfile_[rs2])>>32;
                    break;
                }
                case 0x4:{ //div
                    if(cpu->regfile_[rs2]==0){
                        cpu->regfile_[rd]=0xFFFFFFFF;
                    }
                    else if(cpu->regfile_[rs1]==0x80000000 && cpu->regfile_[rs2]==-1){
                        cpu->regfile_[rd]=0x80000000;
                    }
                    else{
                        cpu->regfile_[rd]=(int32_t)cpu->regfile_[rs1]/(int32_t)cpu->regfile_[rs2];
                    }
                    break;
                }
                case 0x5:{ //divu
                    if(cpu->regfile_[rs2]==0){
                        cpu->regfile_[rd]=0xFFFFFFFF;
                    }
                    else{
                        cpu->regfile_[rd]=cpu->regfile_[rs1]/cpu->regfile_[rs2];
                    }
                    break;
                }
                case 0x6:{ //rem
                    if(cpu->regfile_[rs2]==0){
                        cpu->regfile_[rd]=cpu->regfile_[rs1];
                    }
                    else if(cpu->regfile_[rs1]==0x80000000 && cpu->regfile_[rs2]==-1){
                        cpu->regfile_[rd]=0;
                    }
                    else{
                        cpu->regfile_[rd]=(int32_t)cpu->regfile_[rs1]%(int32_t)cpu->regfile_[rs2];
                    }
                    break;
                }
                case 0x7:{ //remu
                    if(cpu->regfile_[rs2]==0){
                        cpu->regfile_[rd]=cpu->regfile_[rs1];
                    }
                    else{
                        cpu->regfile_[rd]=cpu->regfile_[rs1]%cpu->regfile_[rs2];
                    }
                    break;
                }
                     
                default:
                    break;
                }
            }
            else{
            switch (funct3)
            {
            case 0x0:{
                if(funct7==0) //ADD
                    cpu->regfile_[rd]= cpu->regfile_[rs1]+ cpu->regfile_[rs2];
                else if (funct7==0x20) //SUB
                    cpu->regfile_[rd]=cpu->regfile_[rs1] - cpu->regfile_[rs2];
                break;
            }
            case 0x1:{ //SLL
                cpu->regfile_[rd]=cpu->regfile_[rs1]<< (cpu->regfile_[rs2]&0x1F);
                break;
            }
            case 0x2:{ //SLT
                cpu->regfile_[rd]=(int32_t)(cpu->regfile_[rs1])< (int32_t)(cpu->regfile_[rs2]);
                break;
            }
            case 0x3:{ //SLTU
                cpu->regfile_[rd]= cpu->regfile_[rs1] < cpu->regfile_[rs2];
                break;
            }
            case 0x4:{ //XOR
                cpu->regfile_[rd]=cpu->regfile_[rs1]^cpu->regfile_[rs2];
                break;
            }
            case 0x5:{ //SRL /SRA
                if(funct7==0x0) //SRL
                    cpu->regfile_[rd]= cpu->regfile_[rs1] >> (cpu->regfile_[rs2]&0x1F);
                else if(funct7==0x20) //SRA
                    cpu->regfile_[rd]= (int32_t)(cpu->regfile_[rs1]) >> (cpu->regfile_[rs2]&0x1F);
                break;
            }
            case 0x6:{ //OR
                cpu->regfile_[rd] = cpu->regfile_[rs1] | cpu->regfile_[rs2];
                break;
            }
            case 0x7:{ //AND
                cpu->regfile_[rd] = cpu->regfile_[rs1] & cpu->regfile_[rs2];
                break;
            }
            default:
                break;
            }
        }
        }
        break;
        }

/*
Bit 31 → imm[12] 
Bits 30–25 → imm[10:5]
Bits 11–8 → imm[4:1]
Bit 7 → imm[11]*/
        case 0x63: { //B-Type
                uint32_t imm_u= ((instruction >> 31) & 0x1) << 12  | ((instruction >> 25) & 0x3F) << 5 | ((instruction >> 8) & 0xF) << 1 | ((instruction >>7)& 0x1)<<11 ;
                if(imm_u&0x1000){
                    imm_u= imm_u | 0xFFFFE000;
                }
                int32_t imm= (int32_t)imm_u;

                switch (funct3)
                {
                case 0x0:{ //BEQ
                    if (cpu->regfile_[rs1]==cpu->regfile_[rs2])
                    {
                        cpu->pc_+=imm;
                        *pc_modified=1; 
                    }
                    break;
                }
                case 0x1:{  //BNE
                    if (cpu->regfile_[rs1]!=cpu->regfile_[rs2])
                    {
                        cpu->pc_+=imm;
                        *pc_modified=1; 
                    }
                    break;
                }
                case 0x4:{ // BLT 
                    if ((int32_t)(cpu->regfile_[rs1])<(int32_t)(cpu->regfile_[rs2]))
                    {
                        cpu->pc_+=imm;
                        *pc_modified=1; 
                    }
                    break;
                }
                case 0x5:{ //BGE
                    if ((int32_t)(cpu->regfile_[rs1])>=(int32_t)(cpu->regfile_[rs2]))
                    {
                        cpu->pc_+=imm;
                        *pc_modified=1; 
                    }
                    break;
                }
                case 0x6:{ //BLTU
                    if (cpu->regfile_[rs1]<cpu->regfile_[rs2])
                    {
                        cpu->pc_+=imm;
                        *pc_modified=1; 
                    }
                    else
                    break;
                }
                case 0x7:{ //BGEU
                    if (cpu->regfile_[rs1]>=cpu->regfile_[rs2])
                    {
                        cpu->pc_+=imm;
                        *pc_modified=1; 
                    }
                    break;
                }
                default:
                    break;
                }          
            
            break;
        }

        case 0x6F: { // JAL
            uint32_t imm_u = ((instruction >>31)&0x1)<<20 | ((instruction >> 21) & 0x3FF) << 1 | ((instruction >> 20)& 0x1) << 11 | ((instruction >> 12)&0xFF)<<12;
            if(imm_u&0x100000){
                imm_u=imm_u|0xFFE00000;
            }
            int32_t imm= (int32_t)imm_u;
            if(rd!=0) cpu->regfile_[rd]=cpu->pc_ +4;
            cpu->pc_+=imm;
            *pc_modified=1; 
            break;
        }
        case 0x67:{//JALR
            uint32_t imm_u = (instruction >> 20) & 0xFFF;
            if(imm_u & 0x800){
                imm_u |=0xFFFFF000;
            }
            int32_t imm= (int32_t)imm_u;
            uint32_t rs1_value = cpu->regfile_[rs1];
            uint32_t old_pc = cpu->pc_;
            uint32_t rs1value = cpu->regfile_[rs1];

            if (rd != 0) {
                cpu->regfile_[rd] = old_pc + 4;
            }

            cpu->pc_ = (rs1_value + imm) & ~1;
            *pc_modified=1; 
            break;
        }

    /*Immediate  Bits 31–20 → imm[11:0] */
        case 0x03: { // L-Type
            uint32_t imm_u=((instruction>>20)&0xFFF);
                if(imm_u&0x800){
                    imm_u=imm_u|0xFFFFF000;
                }
            int32_t imm= (int32_t)imm_u;
            uint32_t addr = cpu->regfile_[rs1]+imm;
            uint32_t value;
            switch (funct3)
            {   
                case 0x0:{ //LB
                    uint8_t value_8 = CPU_load_byte(cpu, addr);
                    value= value_8;
                    if(value_8 & 0x80){
                        value |=0xFFFFFF00;
                    }
                    break;
                    }
                case 0x1: {//LH
                    uint16_t value_16 = CPU_load_halfword(cpu,addr);
                    value=value_16;
                    if(value_16 & 0x8000){
                        value|=0xFFFF0000;
                    }
                    break;
                }
                case 0x2:{ // LW
                    value= CPU_load_word(cpu, addr);
                    break;
                }
                case 0x4: {//LBU
                    uint8_t value_8 = CPU_load_byte(cpu, addr);
                    value= value_8;
                    break;
                    }

                case 0x5: {//LHU
                    uint16_t value_16 = CPU_load_halfword(cpu,addr);
                    value=value_16;
                    break;
                    }
            
            default:
                break;
            }  
                if(rd!=0) cpu->regfile_[rd] = value; 
            
            break;
        }

/*Immediate Bits
31-25-> imm [11:5]
11-7 -> imm[4:0]

*/
    case 0x23: { // SW / SB

        uint32_t imm_u= ((instruction>>25)&0x7F)<<5 | ((instruction>>7)&0x1F);
        if(imm_u&0x800){
            imm_u=imm_u|0xFFFFF000;
        }
        int32_t imm = (int32_t)imm_u;
        uint32_t addr = cpu->regfile_[rs1]+ imm;

        switch (funct3)
        {
        case 0x0: //SB
           // printf("SB: pc=%08X rs1=%d rs2=%d addr=%08X value=%02X\n",cpu->pc_, rs1, rs2, addr, cpu->regfile_[rs2] & 0xFF);
            CPU_store_byte(cpu, addr, (cpu->regfile_[rs2]&0xFF));
            break;
        case 0x1: { //SH
            cpu->data_mem_[addr] = cpu->regfile_[rs2] &0xFF ;
            cpu->data_mem_[addr+1] = (cpu->regfile_[rs2]>>8) &0xFF ;
            break;
            }
        case 0x2: { //SW
            cpu->data_mem_[addr] = cpu->regfile_[rs2] &0xFF ;
            cpu->data_mem_[addr+1] = (cpu->regfile_[rs2]>>8) &0xFF ;
            cpu->data_mem_[addr+2] = (cpu->regfile_[rs2]>>16) &0xFF ;
            cpu->data_mem_[addr+3] = (cpu->regfile_[rs2]>>24) &0xFF ;
            break;
            }
        default:
            break;
        }  
        break;
    }

    case 0x37:{ //LUI
        uint32_t imm_u = instruction & 0xFFFFF000;

        if(rd!=0){
            cpu->regfile_[rd]=imm_u;
        }
        break;
    }
    case 0x17:{ // AUIPC
        uint32_t imm_u = instruction & 0xFFFFF000;

        if(rd!=0) cpu->regfile_[rd]= cpu->pc_ +imm_u;
        break;
    }
        default:
            // Unbekannter Befehl: pc bleibt stehen -> Programm haelt an
            break;
    }
	// TODO
	//
	// Hinweis zur Zeichenausgabe: Verwenden Sie fuer den Befehl SB die bereits
	// vorgegebene Funktion CPU_store_byte(). So erscheinen Schreibzugriffe auf
	// die Adresse 0x5000 als Zeichenausgabe auf dem Terminal.


}

/*
Expandiert einen 16-Bit-Compressed-Befehl in sein 32-Bit
Aequivalent. Gibt 0 zurueck, falls der Befehl ungueltig ist. */

uint32_t expand_compressed(uint16_t c) {
    uint8_t op= c & 0x3;
    // op[1:0]-> Quadrant
    uint8_t funct3 = (c >> 13) & 0x7; // funct3 ueber alle Quadranten
    switch (op)
    {
    case 0x0: //Quadrant 0
        switch (funct3)
        {
        case 0x0:
            
            return 0;
        case 0x2:
            
            return 0;
        case 0x6:
            
            return 0;
        }
        
        break;

    case 0x1: //Quadrant 1
        
        break;
    case 0x2: //Quadrant 2
        
        break;
    }

    return 0; // Ungueltige Instruktion
    }

int main(int argc, char* argv[]) {
	printf("C Praktikum\nHU Risc-V  Emulator 2026\n");

	CPU* cpu_inst;
	cpu_inst = CPU_init(argv[1], argv[2]);
    for(uint32_t i = 0; i <1000000; i++) { // Hauptschleife: fuehrt Befehle aus, bis die Obergrenze erreicht ist

        uint32_t instruction_32;
        int length;
        int pc_modified= 0;
        uint32_t old_pc=cpu_inst->pc_;
        uint16_t instruction_16 = *(uint16_t*)(cpu_inst->instr_mem_ + (cpu_inst->pc_ & 0xFFFFF));
        if((instruction_16 & 0x3)==0x3){ //nachladen da nicht compressed
            instruction_32 = (uint32_t)(*(uint16_t*)(cpu_inst->instr_mem_ + ((cpu_inst->pc_+2) & 0xFFFFF)))<<16|instruction_16;
            length=4;
        }
        else{ //16 bit Anweisung
        instruction_32 = expand_compressed(instruction_16);
        length=2;
        }
        //printf("PC=%08X INST=%08X\n", cpu_inst->pc_, instruction_32);
    	CPU_execute(cpu_inst,instruction_32,&pc_modified);
        if(!pc_modified) cpu_inst->pc_+=length;
    }
	printf("\n-----------------------RISC-V program terminate------------------------\nRegfile values:\n");
	//output Regfile
	for(uint32_t i = 0; i <= 31; i++) {
    	printf("%d: %X\n",i,cpu_inst->regfile_[i]);
    }
    fflush(stdout);

	return 0;
}