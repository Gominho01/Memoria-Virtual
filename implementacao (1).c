#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

struct node{
    int data;
    struct node *next; 
}*head=NULL,*tail=NULL;

int tamanho;

int Phy_Memory[128][256];
int Page_table[256][2];
char buffer[256];
int TLB[16][2];
int Lru_Array[256];

int replace;
int LRU, LRUtlb;
int fifo = 0, fifoTLB = 1;    // FIFO
int adresses, frame, page_number, offset;
int index_memoria = 0, index_TLB = 0; // Variaveis contadoras globais
int in_TLB = 0, validate = 0;    //Variaveis de validacao
float page_fault = 0, tlb_hit = 0;
int tamanho;

void push(struct node ** head, int val);
void delete(int );
void print_list(struct node * head);
int search(struct node* head, int x);
int remove_last(struct node * head);
void update(char buffer[]);
void backing_store();
void updateTLB();
void inTLB(int i);

int main(int argc, char *argv[])
{
    unsigned char mask = 0xFF;
    int translated_adress = 0;
    float fault_rate, tlb_hits_rate;
    int value;
    FILE *correct = fopen("correct.txt","w");
    if (strcmp(argv[2],"fifo") == 0){
        fifo = 1;
    }
    else if(strcmp(argv[2],"lru") == 0){
        fprintf(correct,"LRU INCOMPLETO, so possivel rodar caso argv[2] e argv[3] sejam fifo");
        return 0;
    }
    else{
        fprintf(correct,"Não foi identificar um argv[2] correto");
        return 0;
    }
    if (strcmp(argv[3],"fifo") == 0){
        fifoTLB = 1;
    }
    else if(strcmp(argv[3],"lru") == 0){
        fprintf(correct,"LRU INCOMPLETO, so possivel rodar caso argv[2] e argv[3] sejam fifo");
        return 0;
    }
    else{
        fprintf(correct,"Não foi identificar um argv[3] correto");
        return 0;
    }
    FILE *adress = fopen(argv[1], "r");
    while (fscanf(adress, "%d", &adresses) != EOF){
        if (adresses < 0){
            printf("Arquivo no formado invalido");
            break;
        }
        else {
            pthread_t tlbcheck[16];
            int phy_adress = 0;
            validate = 0;
            in_TLB = 0;
            page_number = (adresses >> 8) & mask;
            offset = adresses & mask;

            for (int i = 0; i < 16; i++){
                pthread_create(&tlbcheck[i], NULL, inTLB, i);
            }
            for (int i = 0; i < 16; i++){
                pthread_join(tlbcheck[i], NULL);
            }
    
            while (validate == 0){ //nao ta na tlb     
                if (Page_table[page_number][1] == 1){ // ver se ta na page_table
                    validate = 1;    
                }
                else {// nao ta em nenhum dos dois
                    page_fault++;
                    backing_store();
                    index_memoria++;

                    if (index_memoria == 128 && fifo == 1){
                        index_memoria = 0;
                    }
                }
                frame = Page_table[page_number][0];
            }
            if (in_TLB == 0)
                updateTLB();
            phy_adress = (frame * 256) + offset;
            value = Phy_Memory[frame][offset];
            fprintf(correct,"Virtual address: %d Physical address: %d Value: %d\n",adresses, phy_adress, value);
        }
        translated_adress++;
    }
    fclose(adress);
    fprintf(correct,"Number of Translated Addresses = %d\n", translated_adress);
    fprintf(correct,"Page Faults = %.0f\n", page_fault);
    fault_rate = page_fault / translated_adress;
    tlb_hits_rate = tlb_hit/ translated_adress;
    fprintf(correct,"Page Fault Rate = %.3f\n", fault_rate);
    fprintf(correct,"TLB Hits = %.0f\n",tlb_hit);
    fprintf(correct,"TLB Hit Rate = %.3f\n",tlb_hits_rate);
    fclose(correct);
}

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

void update(char buffer[]){    
    if (fifo == 1 || tamanho < 128){ // FIFO
        
    }
    else{ // LRU
        int value = remove_last(head);
        printf("a");
        
    }
    
    for (int k = 0; k < 256; k++)
            Phy_Memory[index_memoria][k] = buffer[k]; // adiciona as informacoes na memoria
    for (int i = 0; i < 256; i++){
        if (Page_table[i][0] == index_memoria){
            Page_table[i][1] = 0;
        }
    }
    push(&head, index_memoria);
    Page_table[page_number][0] = index_memoria;
    Page_table[page_number][1] = 1; 
    
}

void updateTLB(){
    TLB[index_TLB][0] = page_number;
    TLB[index_TLB][1] = index_memoria;
    index_TLB++;
    if (index_TLB == 16 & fifoTLB == 1){
        index_TLB = 0;
    }
}
void inTLB(int i){ 
    if (TLB[i][0] == page_number){ //Ta na TLB
        in_TLB = 1;
        tlb_hit++;
        validate = 1;
        frame = TLB[i][1];
    }
}

void delete(int pos)
{
    struct node *temp = head;       
    int i;                    
    if(pos==0)
    {
        head=head->next;        
        temp->next=NULL;
        free(temp);       
    }
    else
    {
        for(i=0;i<pos-1;i++)
        {
            temp=temp->next;
        }
        struct node *del =temp->next;       
        temp->next=temp->next->next;  
        del->next=NULL;
        free(del);                         
    }
    return ;
}
 
void push(struct node ** head, int val) {
    
    if (search(*head, val) == 0 && tamanho > 127){
        remove_last(*head);
    }
        
    struct node * new_node;
    new_node = (struct node *) malloc(sizeof(struct node));

    new_node->data = val;
    new_node->next = *head;
    *head = new_node;
    tamanho++;
}
 
void print_list(struct node * head) {
    struct node * current = head;

    while (current != NULL) {
        printf("%d\n", current->data);
        current = current->next;
    }
}

int search(struct node* head, int x)
{
    struct node* current = head;  
    int index = 0;
    while (current != NULL)
    {
        if (current->data == x){
            delete(index);
            return 1;
        }
        current = current->next;
        index++;
    }
    return 0;
}

int remove_last(struct node * head) {
    int retval = 0;
    if (head->next == NULL) {
        retval = head->data;
        free(head);
        return retval;
    }
    struct node * current = head;
    while (current->next->next != NULL) {
        current = current->next;
    }
    retval = current->next->data;
    free(current->next);
    current->next = NULL;
    return retval; //frame do removido

}

// referencias

/*
https://github.com/jri8/VirtualMemoryManager -- formato do offset, page_number
Coisas sobre lista encadeada
https://www.geeksforgeeks.org/delete-a-linked-list-node-at-a-given-position/?ref=lbp
https://www.learn-c.org/en/Linked_lists
https://www.geeksforgeeks.org/search-an-element-in-a-linked-list-iterative-and-recursive/
*/

// make file

/*
bin: implementacao.c
	gcc $< -o $@ -pthread

.PHONY: run clean

run:
	./bin

clean:
	rm bin
*/

/*
LISTA ENCADEADA LRU
---------------------------
1.Fazer uma lista que guarda o valo do frame
2.Enquanto for adicionando, colocar ele no primeiro lugar e mover o resto pra direita
3.quando ele for chamado novamente, ele vai trocar da posição que ele tava, para o primeiro lugar e o resto anda pra direita
*/



