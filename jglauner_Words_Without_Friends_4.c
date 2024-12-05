#include <sys/types.h>
#include <sys/socket.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdbool.h>
#include <time.h>
#include <semaphore.h>
#include <ctype.h>


char *path;

char htmlTopBun[800];

char htmlBottomBun[300];

char htmlCheese[50];


//struct and methods used to create (and free) the full list of words from the dictionary
struct strNode{
    struct strNode *first;
    struct strNode *end;
    char data[30];
    struct strNode *next;
};
void startList(struct strNode *list){
    list->first=NULL;
    list->end=NULL;
    list->next=NULL;
}
void add(struct strNode *list,char data[30]){
    struct strNode *new = malloc(sizeof(struct strNode));
    strcpy(new->data,data);
    new->next=NULL;
    if(list->first==NULL){
        list->first=new;
        list->end=new;
    }else{
        list->end->next=new;
        list->end=new;    
    }
}
void iterateFree(struct strNode *list){
    struct strNode *temp;
    while(list->first){
        temp=list->first;
        list->first=list->first->next;
        free(temp);
    }
}

//struct and methods used to create (and free) 
struct gameNode{
    struct gameNode *first;
    struct gameNode *end;
    int found;
    char data[30];
    struct gameNode *next;
};
void startListG(struct gameNode *list){
    list->first=NULL;
    list->end=NULL;
    list->next=NULL;
    list->found=0;
}
void addG(struct gameNode *list,char data[30]){
    struct gameNode *new = malloc(sizeof(struct gameNode));
    strcpy(new->data,data);
    new->next=NULL;
    new->found=0;
    if(list->first==NULL){
        list->first=new;
        list->end=new;
    }else{
        list->end->next=new;
        list->end=new;    
    }
}
void iterateFreeG(struct gameNode *gList){
    struct gameNode *temp;
    while(gList->first){
        temp=gList->first;
        gList->first=gList->first->next;
        free(temp);
    }
}

struct gameNode *gameList;

struct strNode *strList;;

char masterWord[30];



int dicCount=0;

/*turn str lowercase
iterate through the str for each char in str
turn char numbers to indices in count*/

void getLetterDistribution(char* str,int holder[26]){
	for(int i=0;i<strlen(str);i++){
		holder[str[i]-97]++;
	}
}
/*we compare the letter distributions of the puzzle letters and the guessed word
if an index corresponding to a letter in the word distribution has a higher value than the equivalent
index in the puzzle distribution, we return false, since the guessed word cannot be made with the given letters*/

int compareCounts(char* inp,char* puz){
	int word[26]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int puzzle[26]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    getLetterDistribution(inp,word);
    getLetterDistribution(puz,puzzle);
	int boolean=1;
	int *boolPoint=&boolean;
	for(int i=0;i<26;i++){
		if(word[i]>puzzle[i]){
			boolean=0;
		}	
        
	}
	return boolean;

}

struct strNode *getRandomWord(){
    
    srand(time(NULL));
    
    const int x = rand()%(dicCount);
    int count=0;
    const int minlen=7;
    struct strNode *nodey=NULL;
    int len=0;
    struct strNode *list=strList->first;
    while(list){
        len=strlen(list->data);
        if(count<x||len<minlen){
            count++;
        }
        if(count>=x&&len>minlen){
            nodey=list;
            break;
        }
        list=list->next;
    }
    if(nodey==NULL){
        return getRandomWord(strList);
    }
    return nodey;
}

int isDone(){
    struct gameNode *temp=gameList->first;
    while(temp->next){
        if(temp->found==0){
            return 0;
        }
        temp=temp->next;
    }
	return 1;
}

char *displayWorld(){
    struct gameNode *temp=gameList->first;
    int length=11+strlen(temp->data);
    while(temp->next){
        temp=temp->next;
        length+=strlen(temp->data)+12;
        
    }
    temp=gameList->first;
    char *html=malloc(length);
    html[0]='\0';
    while(temp){
        char dataPoint[strlen(temp->data)+12];
        if(temp->found==0){
            char underlines[strlen(temp->data)+1];
            for(int i=0;i<strlen(temp->data);i++){
                underlines[i]='_';
            }
            underlines[strlen(temp->data)]='\0';
            sprintf(dataPoint,"<p>%s</p><br>",underlines);
        }else{
            sprintf(dataPoint,"<p>%s</p><br>",temp->data);
        }
        strcat(html,dataPoint);
        temp=temp->next;
    }
	return html;	
}

struct gameNode *findWords(char word[30]){
    gameList=malloc(sizeof(struct gameNode));
    startListG(gameList);
    struct strNode *current = strList->first;
    while (current != NULL) {
        if (strlen(current->data)>2&&compareCounts(current->data, word)&&(strcmp(current->data,masterWord))) {
            addG(gameList, current->data);
        }
        current = current->next;
    }
    return gameList;
}

//loads the dictionary into strList, grabs the gameword and loads all valid words into gameList
int initialization(){
    strList=malloc(sizeof(struct strNode));
	startList(strList);
	FILE *fp=fopen("2of12.txt","r");
	char c = fgetc(fp);
	char str[30];
	int strpos=0;
	int first = 1;
	while(c!=EOF){
		while(strpos<29&&c>32&&c<126){
            str[strpos]=c;
			c = fgetc(fp);
			strpos++;	
		}
		str[strpos++]='\0';
		dicCount++;
		add(strList,str);
		while(c==' '||c=='\r'||c=='\n'){
			c = fgetc(fp);
		}
		while(strpos>0){
			str[strpos]=0;
			strpos--;
		}
	}
    strcpy(masterWord,getRandomWord(strList)->data);
    gameList=findWords(masterWord);
	fclose(fp);
	return dicCount;
}

int teardown(){
    iterateFree(strList);
    iterateFreeG(gameList);
    free(gameList);
    free(strList);
	printf("All Done\n");
	return 0;
}
//takes an input, turns the whole word uppercase so it looks nice when we print it
void acceptInput(char inputURL[51]) {
    int i = 0;
    char input[30];
    while (inputURL[i] != '\0' && inputURL[i] != '=') {
        i++;
    }
    i++;
    int index = 0;
    while (inputURL[i] >= 'a' && inputURL[i] <= 'z' && index < sizeof(input) - 1) {
        input[index++] = inputURL[i++];
    }
    input[index] = '\0';
    struct gameNode *temp = gameList->first;
    while (temp) {
        if (strcmp(temp->data, input) == 0||strcmp("failquitter",input)==0) {
            temp->found = 1;
        }
        temp = temp->next;
    }
}


//localhost:8080/game?guess=test
void *serve(void *connection) {
    int newFD = *(int *)connection;
    struct dirent *file;
    char buffer[1024];
    int reader = read(newFD, buffer, sizeof(buffer));
    if (reader < 0) {
        perror("Error reading request");
        close(newFD);
        return NULL;
    }
    char fileGet[51];
    int index = 5;
    int fileIndex = 0;
    while (buffer[index] != ' ' && buffer[index] != '\0'&& buffer[index] != '\r'&& buffer[index] != '\n' && fileIndex < 50) {
        fileGet[fileIndex++] = buffer[index++];
    }
    fileGet[fileIndex] = '\0';
    printf("%s\n", fileGet);
    DIR *current = opendir(path);
    if (current == NULL) { 
        perror("Could not open current directory");
        close(newFD);
        return NULL;
    }
    int fileFound = 0;
    while ((file = readdir(current)) != NULL) {
        if (strcmp(file->d_name, "wwfWinPage.html") == 0) {
            fileFound = 1;
            break;
        }
    }
    
    if(!strcmp(fileGet,"game\0")){
        if(gameList!=NULL){
            teardown();
        }
        initialization();
    }else{
        acceptInput(fileGet);
    }
    closedir(current);  
    char filePath[200];
    char header[1024];
    if (!fileFound) {
        char code[27];
        sprintf(code, "HTTP/1.1 404 Not Found\r\n\r\n");
        send(newFD, code, strlen(code), 0);
        char reader[91];
        sprintf(reader,"Hi! I am the page you are looking for. I don't exist! You should instead visit wcdbfm.com!");
        send(newFD, reader, strlen(reader), 0);
    } 
    else if(!isDone()){
        char htmlBurger[200000];
        char htmlMeat[180000];
        strcpy(htmlMeat,displayWorld(gameList));
        sprintf(htmlBurger,"%s%s%s%s%s\r\n\r\n",htmlTopBun,masterWord,htmlCheese,htmlMeat,htmlBottomBun);
        sprintf(header, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", sizeof(htmlBurger));
        send(newFD, header, strlen(header), 0);
        send(newFD, htmlBurger, sizeof(htmlBurger), 0);
    }
    else{
        struct stat html;
        sprintf(filePath, "%s/wwfWinPage.html", path);
        stat(filePath, &html);
        int opener = open(filePath, O_RDONLY);
        sprintf(header, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", html.st_size);
        send(newFD, header, strlen(header), 0);
        char reader[html.st_size];
        int bytesRead = read(opener, reader, html.st_size);
        send(newFD, reader, bytesRead, 0);
        
        close(opener);
    }
    //printf("%s\n", header);
    close(newFD);
    return NULL;
}

int main(int argc, char* argv[]){
    sprintf(htmlTopBun,"<html><head><title>Palabras sin Amigos</title><link rel=\"icon\" href=\"data:,\"><style>body {background-color: rgb(63, 63, 63);font-family: Arial, sans-serif;margin: 1%%;line-height: 1.6;}.headiv{background-color: rgb(200,200,200);text-align:center;margin-left:4%%;margin-right:3.5%%;} .maindiv{background-color: rgb(200,200,200);color: #d83d00;text-align:center;margin-left:10%%;margin-right:10%%;}.submitdiv{background-color: rgb(200,200,200);}</style></head><body><div class=\"headiv\"><h1>");
    sprintf(htmlBottomBun,"<br><div class=\"submitdiv\"><form><input type=\"text\" id=\"guess\" name=\"guess\"><br><input type=\"submit\" value=\"Submit\"></form></div></div></body></html>");
    sprintf(htmlCheese,"</h1></div><div class=\"maindiv\">");
    char code[15];
    char http[10];
    path=argv[1];
    if(argc<2){
        printf("No Directory\n");
        exit(1);
    }
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *cur;
    struct sockaddr_storage their_addr;
    memset (&hints, 0, sizeof(struct addrinfo));
    int sockfd;
    int get;
    int newFD;
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;
    get=getaddrinfo("127.0.0.1","8080",&hints,&servinfo);
    for(cur=servinfo;cur!=NULL;cur=cur->ai_next){
        if((sockfd=socket(cur->ai_family,cur->ai_socktype,cur->ai_protocol))==-1){
            continue;
        }
        if((bind(sockfd,cur->ai_addr,cur->ai_addrlen))==-1){
            close(sockfd);
            continue;
        }
        break;
    }
    if(listen(sockfd,1000)==-1){
        printf("not listening\n");
        exit(1);
    }
    
    pthread_t id;
    while(1){
        
        socklen_t sin_size = sizeof(their_addr);
        newFD=accept(sockfd,(struct sockaddr *)&their_addr,&sin_size);
        pthread_create(&id,NULL,serve,(void *)(&newFD));
        
    }
    freeaddrinfo(servinfo);
    
}


