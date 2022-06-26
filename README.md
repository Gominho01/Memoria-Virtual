# Memoria-Virtual

## Proposito do código

>O codigo tem como objetivo implementar um gerenciador de memoria virtual

## Funções mínimas do makefile

> Tem 2 funções essenciais pedidas nessa tarefa, o make que compila o código e gera o binário e o make clear para apagar o binário gerado.

## Principais funções do código
```c
void backing_store(){
    FILE *bin_file;
                    
    bin_file = fopen("BACKING_STORE.bin", "rb");
    if (fseek(bin_file, page_number * 256 ,SEEK_SET) != 0){
        printf("Algo deu errado no fseek\n");
    }

    
    if (fread(buffer, sizeof(char), 256, bin_file) == 0){
        printf("Algo deu errado no fread\n");
    }
    fclose(bin_file);
    update(buffer);
}
```
> Pega o valor do backing_store e coloca no buffer
```c
void update(char buffer[]){    
    if (LRU == 1 && index_memoria >= 128){
        index_memoria = Lru_mem();
    }

    for (int k = 0; k < 256; k++)
            Phy_Memory[index_memoria][k] = buffer[k]; // adiciona as informacoes na memoria
    for (int i = 0; i < 256; i++){
        if (Page_table[i][0] == index_memoria){
            Page_table[i][1] = 0;
        }
    }
    Page_table[page_number][0] = index_memoria;
    Page_table[page_number][1] = 1; 
    
}
```
> Atualiza a page_table e pega as informações que ele armazenou no buffer e coloca na memoria fisica
```c
void updateTlbLRU(){
    Lru_TLB();
    TLB[index_maior][0] = page_number;
    TLB[index_maior][1] = frame;  
}
```
> Atualiza a TLB como LRU

```c
int Lru_TLB(){
    maior = -1;
    index_maior = 0;
    for(int i = 0; i < 16; i++) {
        if (Lru_tlb_Array[i][1] > maior) { // pega o maior(mais antigo)
            maior = Lru_tlb_Array[i][1]; 
            index_maior = i;
        }
    }
    
    return index_maior;
}
```
> Procura o maior numero no array da TLB e retrona o maior

```c
void updateLRU(){
     for (int i = 0; i < 16; i++){
        Lru_tlb_Array[i][0] = TLB[i][1];
        if (TLB[i][1] == frame){
            Lru_tlb_Array[i][1] = 0;
        }
        else if(Lru_tlb_Array[i][0] != 2048){
            Lru_tlb_Array[i][1]++;
        }
    }
}
```
> Reseta o valor do frame encontrado na TLB e soma 1 no resto

```c
int Lru_mem(){
    maior = 0;
    int index_maior = 0;
    for(int i = 0; i < 128; i++) { // caso esteja cheio ele procura
        if (maior < Lru_Array[i]) { // pega o maior(mais antigo)
            maior = Lru_Array[i]; 
            index_maior = i;
        }
    }
    
    return index_maior;
}
```
> Procura o maior valor no array da memoria fisica

```c
void updateTlbFIFO(){

    if (index_TLB == 16 & fifoTLB == 1)
        index_TLB = 0;
        
    TLB[index_TLB][0] = page_number;
    TLB[index_TLB][1] = index_memoria;
    index_TLB++;
}
```
> Atualiza a TLB como FIFO

```c
void inTLB(int i){ 
    if (TLB[i][0] == page_number){ //Ta na TLB
        in_TLB = 1;
        tlb_hit++;
        validate = 1;
        frame = TLB[i][1];
    }
}
```
> Checa se o page number está na TLB
