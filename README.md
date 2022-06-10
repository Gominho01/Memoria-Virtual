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
```
> Atualiza a page_table e pega as informações que ele armazenou no buffer e coloca na memoria fisica
```c
void updateTLB(){
    TLB[index_TLB][0] = page_number;
    TLB[index_TLB][1] = index_memoria;
    index_TLB++;
    if (index_TLB == 16 & fifoTLB == 1){
        index_TLB = 0;
    }
}
```
> Atualiza a TLB
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

##FUNÇÕES DA LISTA ENCADEADA

```c
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
 ```
 > Deleta valor especifico da lista
 ```c
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
 ```
 > Adiciona valor na primeira posição da lista e empurra o resto
 ```c
void print_list(struct node * head) {
    struct node * current = head;

    while (current != NULL) {
        printf("%d\n", current->data);
        current = current->next;
    }
}
```
> Printa a lista
```c
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
```
> Procura por um elemento especifico da lista e chama a função delete se achar
```c
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
```
> Remove o ultimo elemento da lista encadeada
