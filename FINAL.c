#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STR 16
//16 could be a good solution

unsigned int node_id = 0;
unsigned int maxnodes = 0;
bool fullstring = false;


typedef struct connectors {
    unsigned int id;   //could be useless, has to be checked
    struct connectors *next;  // this will be a pointer to the next node that is a connection of the global node
    char read;
    char write;
    char direction;

}conn;

typedef struct nodes {
    //int id;
    bool accept;
    conn *connection;  // this will be a pointer to a list of nodes which are connected to one node
}nod;
nod *globalnod;  //pointer to the main list of states

typedef struct tape {
    char content[STR];
    struct tape *next;
    struct tape *prev;
}tap;
tap *globaltape = NULL;

typedef struct stackelem {
    unsigned int id;
    struct stackelem *next;
    tap *owntape;
    int chunkp;
    
} stackel;


///////////////////////////////////////////////////////////////
//GLOBAL NODE (STATE) LIST
//////////////////////////////////////////////////////////////
/*
conn *InsNode(conn *curr, char read, char write, char direct, unsigned int id){
    conn *temp = curr;
    conn *newnod = malloc(sizeof(conn));
    //if(!newnod) printf("error memory allocation addin new node to the connection");

    newnod->id = id;
    newnod->read = read;
    newnod->write = write;
    newnod->direction = direct;

    
    if (curr == NULL){
        newnod->next = NULL;
        curr = newnod;       
        return curr;
    }
    newnod->next = curr;
    
    return newnod;
}
*/
/*
nod *InitNodList(){   // probably useless
    nod *curr = malloc(sizeof(nod));
    curr->connection = NULL;
    return curr;
}
*/
//////////////////////////////////////////////////////
//TAPE LIST
//////////////////////////////////////////////////////
/*
tap *InitTape(){
    tap *curr = malloc(sizeof(tap));
    curr->prev = NULL;
    curr->next = NULL;
    /*
    for(int i = 0; i < STR; i++){
        curr->content[i] = '_';
    }
    //
    return curr;
}
*/
void InsBlanc(tap *pos, char where){
    tap *newcell = malloc(sizeof(tap));

    for(int i = 0; i < STR; i++){
        newcell->content[i] = '_';
    }

    if(where == 'L'){
        newcell->prev = NULL;
        newcell->next = pos;
        pos->prev = newcell;
        return;
    }
    else{
        newcell->next = NULL;
        newcell->prev = pos;
        pos->next = newcell;
        return;
    }
}

tap *ClearTape(tap *toclear){ 
    tap *temp = toclear->next;
    tap *temp2;
    while(temp != NULL){
        temp2 = temp;
        temp = temp->next;
        free(temp2);
    }

    temp = toclear;    
    while(temp != NULL){
        temp2 = temp;
        temp = temp->prev;
        free(temp2);
    }
    
    return NULL;
}

////////////////////////////////////////////////////
//NODES STACK
//////////////////////////////////////////////////////
/*
stackel* InitNodeStack(){ 
    stackel *curr = malloc(sizeof(stackel));
    curr->next = NULL;
    curr->owntape = NULL;
    return curr;
}
*/

stackel *AddToNodeStack(conn *currconn, stackel *stack, tap *currtape, int pos){

    stackel *newel = malloc(sizeof(stackel));
    //if(!newel) printf("error memory allocation in the new element allocation");
    
    newel->id = currconn->id;
    //newel->next = NULL;
    newel->owntape = currtape;
    newel->chunkp = pos;

    if(stack == NULL){
        newel->next = NULL;
        return newel;
    }

    newel->next = stack;

    return newel;  // I want the bottom of the stack

}

stackel *FreeStackElem(stackel *todelete){
    stackel *toreturn = todelete->next;
    free(todelete);
    return toreturn;
    
}

////////////////////////////////////////////////////
//STRING STACK
//////////////////////////////////////////////////////

tap *CopyTape(tap *tocopy){ // copytape optimized
    tap *temp = tocopy, *newtape, *tempnew, *prevtemp;
    
    if(temp == NULL){
        return NULL;
    }

    newtape = malloc(sizeof(tap));  //first element
    for(int i = 0; i < STR; i++){
            newtape->content[i] = temp->content[i];
    }


    if(temp->next == NULL){
        newtape->next = NULL;
    }
    else{
        prevtemp = newtape;
        temp = temp->next;

        while(temp != NULL){ //goes to the right
            tempnew = malloc(sizeof(tap));
            tempnew->prev = prevtemp;
            prevtemp->next = tempnew;

            for(int i = 0; i < STR; i++){
                tempnew->content[i] = temp->content[i];
            }

            prevtemp = prevtemp->next;
            temp = temp->next;

        }
        tempnew->next = NULL;
    }

    temp = tocopy;
    if(temp->prev == NULL){
        newtape->prev = NULL;
    }
    else{
        prevtemp = newtape;
        temp = tocopy->prev;

        while(temp != NULL){ // goes to the left
            tempnew = malloc(sizeof(tap));
            tempnew->next = prevtemp;
            prevtemp->prev = tempnew;

            for(int i = 0; i < STR; i++){
                tempnew->content[i] = temp->content[i];
            }

            prevtemp = prevtemp->prev;
            temp = temp->prev;
        }
        tempnew->prev = NULL;

    }
    
    return newtape;
}


////////////////////////////////////////////////////
//TURING MACHINE
//////////////////////////////////////////////////////


int TM(int max){   // will return ACC, REJ or OUTMAX
    
    stackel *stack = NULL, *tempstack = NULL, *newstack = NULL;
    tap *personaltape = NULL, *tapepointer = NULL;
    conn *curr;
    bool emptystack = true, use = true, toclear = true;
    bool loop = false, addtostack = true;
    int tapepos, personalpos;


    //initialize the stack with the beginning 0
    stack = malloc(sizeof(stackel));
    stack->next = NULL; 
    stack->id = 0;
    stack->owntape = globaltape;
    stack->chunkp = 0;
    globaltape = NULL;


    for(int i = 0; i < max; i++){
        
        tempstack = stack;
        while(tempstack != NULL){
            emptystack = false;
            tapepointer = tempstack->owntape;
            tapepos = tempstack->chunkp;

            curr = globalnod[tempstack->id].connection;

            while(curr != NULL){


                if(curr->read == tapepointer->content[tapepos]){

                    if(curr->read == curr->write){
                        if(curr->direction == 'S' && tempstack->id == curr->id){ // optimization of the loop
                            
                            loop = true;
                            addtostack = false;
                            goto ENDIF;
                            
                            //use a flag to remamber that if all are 0 then the output is U
                            //go to end of main if (dont add this to the stack)
                                  
                        }

                        else{
                            if(use == true){
                                personaltape = tapepointer;
                                personalpos = tapepos; 
                                use = false;
                                toclear = false;
                            }
                            else{
                                personaltape = CopyTape(tapepointer);
                                personalpos = tapepos;
                            }
                        }

                        
                                                    
                    }
                    else{
                        personaltape = CopyTape(tapepointer);
                        personalpos = tapepos;
                        personaltape->content[tapepos] = curr->write;
                    }
                    

                    if(curr->direction == 'L'){
                        
                        personalpos--;
                        if(personalpos == -1){
                            if(personaltape->prev == NULL){
                                InsBlanc(personaltape, 'L');
                            }
                                personaltape = personaltape->prev;
                                personalpos = STR-1;
                        }

                        
                        if(personalpos == 0 && personaltape->content[personalpos] == '_' && curr->read == '_'){
                            loop = true;
                            addtostack = false;
                            personaltape = ClearTape(personaltape);
                            toclear = false;
                            
                        }
                        

                    }
                    else if(curr->direction == 'R'){
                        
                        personalpos++;
                        if(personalpos == STR){
                            if(personaltape->next == NULL){
                                InsBlanc(personaltape, 'R');
                        }
                            personaltape = personaltape->next;
                            personalpos = 0;
                        }

                        /*
                        if(personalpos == STR-1 && personaltape->content[personalpos] == '_' && curr->read == '_'){
                            loop = true;
                            addtostack = false;
                            personaltape = ClearTape(personaltape);
                            toclear = false;
                            
                        }
                        */
                    }



                    ENDIF: if(globalnod[curr->id].accept == true){ //found acceptance --> free everything
                        if(toclear == true){
                            personaltape = ClearTape(personaltape);
                        }
                        
                        while(tempstack != NULL){
                            tempstack->owntape = ClearTape(tempstack->owntape);
                            tempstack = FreeStackElem(tempstack);
                        }
                        tempstack = newstack;
                        while(tempstack != NULL){
                            tempstack->owntape = ClearTape(tempstack->owntape);
                            tempstack = FreeStackElem(tempstack);
                        }
                        return 1;
                    }
                    if(addtostack == true){
                        newstack = AddToNodeStack(curr, newstack, personaltape, personalpos);
                    }
                    else{
                        addtostack = true;
                    }
                }
                
                toclear = true;
                curr = curr->next;
            }

            if(use == true){
                tempstack->owntape = ClearTape(tapepointer);
            }
            use = true;
            tempstack = FreeStackElem(tempstack); //returns the next one anyway

        }

        if(emptystack == true){
            if(loop == true){
                return -1;
            }
            return 0;
        }
        emptystack = true;

        stack = newstack;
        newstack = NULL;
        
    }


    while(stack != NULL){
        stack->owntape = ClearTape(stack->owntape);
        stack = FreeStackElem(stack);
    }
    
    return -1;
}




int main(){


/*///////////////////
    long int c, d, k = 0;
    for (c = 1; c <= 327670000; c++)
        for (d = 1; d <= 327670000; d++)
        {
            k++;
        }
///////////////////*/

    int max;
    unsigned int firsts, seconds; //trans
    char firstc, secondc; //trans

    char tempdir; // direction
    char tempchar; //for the tape

    int res; 
    unsigned int t = 0;
    tap *temptape, *tapestart;

    conn *newnod;


    unsigned int acctemp;

    char temp[30];
    //nod *tempnode;
    
    
    
    gets(temp); //reading 'tr'
    //fgets(temp, 50, stdin);
    //printf("letto  %s\n", temp);

    gets(temp);
    //fgets(temp, 50, stdin);



    //globalnod =  InitNodList();  //initialize the global node list  USELESS CAN BE DELETED
    globalnod = malloc(sizeof(nod));
    globalnod->connection = NULL;

    while(strcmp(temp,"acc")){   //inserting the node in the global list

        
        sscanf( temp, "%d %c %c %c %d", &firsts, &firstc, &secondc, &tempdir, &seconds );
        //printf("la vera lettura è: %d %c %c %c %d\n", firsts, firstc, secondc, tempdir, seconds);
        
        
        if(seconds > maxnodes || firsts > maxnodes){

            if(seconds > firsts){
                maxnodes = seconds;
            }
            else{
                maxnodes = firsts;
            }
            

            globalnod = realloc(globalnod, (maxnodes+1)*sizeof(nod));
            
            //ResizeNodList(maxnodes+1);  //+ 1 because the states start from 0
            
            while(t<=maxnodes){
            
                //globalnod[t].id = t;
                globalnod[t].accept = false;

                
                globalnod[t].connection = NULL;

                t++;
            }

        }

        

        newnod = malloc(sizeof(conn));
        newnod->direction = tempdir;
        newnod->id = seconds;
        newnod->read = firstc;
        newnod->write = secondc;

        if (globalnod[firsts].connection == NULL){
            newnod->next = NULL;
            globalnod[firsts].connection = newnod;       
        }
        else{
            newnod->next = globalnod[firsts].connection;
    
            globalnod[firsts].connection = newnod;
        }



        //globalnod[firsts].connection= InsNode(globalnod[firsts].connection, firstc, secondc, tempdir, seconds);
        

        //fgets(temp, 50, stdin);
        gets(temp);

    }




    //printf("letto acc. \n");

    //fgets(temp, 50, stdin);
    gets(temp);
    while(strcmp(temp,"max")){     // here it reads the acception states and updates the nodes
        sscanf(temp, "%d",&acctemp);
        globalnod[acctemp].accept = true;        
        
        //fgets(temp, 50, stdin);
        gets(temp);
    }


    //printf("letto max. \n");
    scanf("%d",&max);
    //printf("max= %d \n",max);
    scanf("%s",temp); //reading 'run'


    //////////////////////first trial
    
    getchar();  //to get the \n of the previous string
    
    
    
    tempchar = getchar();
    
    while(tempchar != EOF){

        
        globaltape = malloc(sizeof(tap));
        globaltape->prev = NULL;
        globaltape->next = NULL;
        tapestart = globaltape;
        t = 0;
        while(tempchar != '\n' && tempchar != EOF && tempchar != '\r'){
            
            if(t == STR){
                temptape = globaltape;
                globaltape = malloc(sizeof(tap));
                globaltape->next = NULL;
                globaltape->prev = temptape;
                temptape->next = globaltape;
                t = 0;
            }
            

            globaltape->content[t] = tempchar;
            t++;
            tempchar = getchar();

        }

        for(t; t < STR; t++){ //im filling the rest of the tape with blanks
            globaltape->content[t] = '_';
        }

        


        
        /////////////////
        //HERE I NEED TO ADD THE TURING MACHIN FUNCTION WHICH CHECKS THE TAPE
        globaltape = tapestart;

        res = TM(max);

        if(res == 1){
            printf("1\n");
        }
        else if(res == 0){
            printf("0\n");
        }
        else{
            printf("U\n");
        }
        ///////////////


        tempchar = getchar();
        if(tempchar == '\r'){
            tempchar = getchar();
        }


    }


    return 0;
}
