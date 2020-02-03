//-----ChatProgram Server------By:Soheil Mahdizade----Student number : 98106083------
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dirent.h>
#include <time.h>
#include <process.h>
#include <winsock2.h>
#define MAX 150
#define PORT 12345
#define SA struct sockaddr
#include "mycJSON.h"
/* DEFINING THE STRUCT FOR USERS */
typedef struct user {
    int Online_User_Flag;
    char name[MAX];
    char Authenticator_Code[32];
    char corrent_channel_name[MAX];
    int Channel_Flag;
} User;
/*------GLOBAL VARIABLES---------*/
int client_socket,server_socket;
User Users[MAX];
int User_Counter = 0;
const char Random_characters_Pool[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz123456789-_";
//------------------------
//-------FUNCTIONS--------
void Connect_and_Send(char Answer[],int sizeofthearray){
    send(client_socket, Answer, sizeofthearray, 0);
    closesocket(server_socket);
}
void Creating_cJSON_String(char *ptr,char FirstStr[],char SecondStr[],char FirstStr_Content[],char SecondStr_Content[]){
    cJSON *First_Content;
    cJSON *JSON;
    cJSON *Second_Content;
    First_Content = cJSON_CreateString(FirstStr_Content);
    Second_Content = cJSON_CreateString(SecondStr_Content);
    JSON = cJSON_CreateObject();
    cJSON_AddItemToObject(JSON,FirstStr,First_Content);
    cJSON_AddItemToObject(JSON,SecondStr,Second_Content);
    char *MainStr = cJSON_Print(JSON);
    sprintf(ptr,"%s", MainStr);
}
int Find_User(char AuthToken[]){
    for(int i = 0;i < MAX;i++){
        if(strcmp(Users[i].Authenticator_Code,AuthToken) == 0){
            return i;
        }
    }
    return -1;
}
void Refresh(char AuthToken[]){
    if (Find_User(AuthToken) == -1){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","Authentication Failed!");
        Connect_and_Send(Answer, sizeof(Answer));
        return;
    }
    if (Users[Find_User(AuthToken)].Channel_Flag == 0){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","User not in a channel!");
        Connect_and_Send(Answer, sizeof(Answer));
        return;
    }
    //-----Creating the cJSON Answer!---------

    cJSON *Answer_cJSON = cJSON_CreateObject();
    cJSON *first_content;
    first_content = cJSON_CreateString("list");
    cJSON_AddItemToObject(Answer_cJSON,"type",first_content);
    cJSON *second_content = cJSON_CreateArray();
    cJSON_AddItemToObject(Answer_cJSON,"content",second_content);
    FILE *fptr;
    char Directory[MAX];
    sprintf(Directory,"./Channels/%s.channel.txt",Users[Find_User(AuthToken)].corrent_channel_name);
    fptr = fopen(Directory,"r");
    char String[10000];
    fscanf(fptr,"%[^EOF]",String);
    cJSON *cptr;
    cptr = cJSON_Parse(String);
    fclose(fptr);
    cJSON *sender = NULL;
    cJSON *content = NULL;
    cJSON *messages = cJSON_GetObjectItem(cptr, "messages");
    for(int i = 0;i < cJSON_GetArraySize(messages);i++){
        cJSON *subcontent = cJSON_GetArrayItem(messages, i);
        sender = cJSON_GetObjectItem(subcontent, "sender");
        content = cJSON_GetObjectItem(subcontent, "content");
        char adding[5*MAX];
        Creating_cJSON_String(adding,"sender","content",sender -> valuestring,content->valuestring);
        cJSON *ADDING = cJSON_Parse(adding);
        cJSON_AddItemToArray(second_content, ADDING);
    }
    char String_2[10000];
    char *ptr = cJSON_Print(Answer_cJSON);
    sprintf(String_2,"%s",ptr);
    Connect_and_Send(String_2,sizeof(String));
    return;
}
void Channel_Members(char AuthToken[]){
    if (Find_User(AuthToken) == -1){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","Authentication Failed!");
        Connect_and_Send(Answer, sizeof(Answer));
        return;
    }
    if (Users[Find_User(AuthToken)].Channel_Flag == 0){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","User not in a channel!");
        Connect_and_Send(Answer, sizeof(Answer));
        return;
    }
    //------Creating the cJSOn Answer message!--------------
    cJSON *Answer_cJSON = cJSON_CreateObject();
    cJSON *first_content;
    first_content = cJSON_CreateString("list");
    cJSON_AddItemToObject(Answer_cJSON,"type",first_content);
    cJSON *second_content = cJSON_CreateArray();
    cJSON_AddItemToObject(Answer_cJSON,"content",second_content);
    for(int i = 0;i < User_Counter;i++){
        if(Users[i].Online_User_Flag == 1 && Users[i].Channel_Flag == 1){
            if(strcmp(Users[Find_User(AuthToken)].corrent_channel_name, Users[i].corrent_channel_name)==0){
                cJSON *Account_Name = cJSON_CreateString(Users[i].name);
                cJSON_AddItemToArray(second_content,Account_Name);
        }
    }
    }
    char String[10000];
    char *ptr = cJSON_Print(Answer_cJSON);
    sprintf(String,"%s",ptr);
    Connect_and_Send(String,sizeof(String));
    return;
}
void LogOut(char AuthToken[]){
    if (Find_User(AuthToken) != -1){
        Users[Find_User(AuthToken)].Online_User_Flag = 0;
         char Answer[MAX];
         Creating_cJSON_String(Answer,"type","content","successful","");
         Connect_and_Send(Answer,sizeof(Answer));
         return;
    }
    char Answer[MAX];
    Creating_cJSON_String(Answer,"type","content","Error","Authentication Failed!");
    return;
}
void Leave_Channel(char AuthToken[]){
    if (Find_User(AuthToken) == -1){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","Authentication Failed!");
        return;
    }
    if (Users[Find_User(AuthToken)].Channel_Flag == 0){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","User not in a channel!");
        return;
    }
    Users[Find_User(AuthToken)].Channel_Flag = 0;
    FILE *fptr;
    char Directory[MAX];
    sprintf(Directory,"./Channels/%s.channel.txt",Users[Find_User(AuthToken)].corrent_channel_name);
    fptr = fopen(Directory,"r");
    char String[10000];
    fscanf(fptr,"%[^EOF]",String);
    cJSON *cptr;
    cptr = cJSON_Parse(String);
    fclose(fptr);
    //-----MAKING THE cJSON STRING AGAIN WITH THE PREVIOUS CONTENT-----------
    cJSON *sender = NULL;
    cJSON *content = NULL;
    cJSON *New_cJSON = cJSON_CreateObject();
    cJSON *name = cJSON_CreateString(Users[Find_User(AuthToken)].corrent_channel_name);
    cJSON_AddItemToObject(New_cJSON,"name", name);
    cJSON *New_Messages = cJSON_CreateArray();
    cJSON_AddItemToObject(New_cJSON,"messages",New_Messages);
    cJSON *messages = cJSON_GetObjectItem(cptr, "messages");
    for(int i = 0;i < cJSON_GetArraySize(messages);i++){
        cJSON *subcontent = cJSON_GetArrayItem(messages, i);
        sender = cJSON_GetObjectItem(subcontent, "sender");
        content = cJSON_GetObjectItem(subcontent, "content");
        char adding[5*MAX];
        Creating_cJSON_String(adding,"sender","content",sender->valuestring , content->valuestring);
        cJSON *ADDING = cJSON_Parse(adding);
        cJSON_AddItemToArray(New_Messages, ADDING);
    }
    //---------------------------------------------------------------------------
    //-------ADDING THE NEW MESSAGE----------------------------------------------
    char sent_message_answer[MAX];
    char new_msg[MAX];
    sprintf(new_msg,"%s leaved the channel!", Users[Find_User(AuthToken)].name);
    Creating_cJSON_String(sent_message_answer,"sender","content","server",new_msg);
    cJSON *JSON_Answer = cJSON_Parse(sent_message_answer);
    cJSON_AddItemToArray(New_Messages,JSON_Answer);
    char *ptr = cJSON_Print(New_cJSON);
    char String_2[10000];
    sprintf(String_2,"%s",ptr);
    fptr = fopen(Directory,"w");
    fprintf(fptr,"%s",String_2);
    fclose(fptr);
    char Answer[MAX];
    Creating_cJSON_String(Answer,"type","content","successful","");
    Connect_and_Send(Answer, sizeof(Answer));
    return;
}
void Send_Message(char AuthToken[],char sent_message[]){
    //-----CHEKING THE AUTHTOKEN-----
    if(strcmp(AuthToken, Users[Find_User(AuthToken)].Authenticator_Code)!=0){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","Authentication Failed");
        Connect_and_Send(Answer, sizeof(Answer));
        return;
    }
    //----CHEKING IF THE USER IS IN A CHANNEL OR NOT!--
    if(Users[Find_User(AuthToken)].Channel_Flag == 0){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","You are not joined to a channel!");
        Connect_and_Send(Answer, sizeof(Answer));
        return;
    }
    FILE *fptr;
    char Directory[MAX];
    sprintf(Directory,"./Channels/%s.channel.txt",Users[Find_User(AuthToken)].corrent_channel_name);
    fptr = fopen(Directory,"r");
    char String[10000];
    fscanf(fptr,"%[^EOF]",String);
    cJSON *cptr;
    cptr = cJSON_Parse(String);
    fclose(fptr);
    //-----MAKING THE cJSON STRING AGAIN WITH THE PREVIOUS CONTENT-----------
    cJSON *sender = NULL;
    cJSON *content = NULL;
    cJSON *New_cJSON = cJSON_CreateObject();
    cJSON *name = cJSON_CreateString(Users[Find_User(AuthToken)].corrent_channel_name);
    cJSON_AddItemToObject(New_cJSON,"name", name);
    cJSON *New_Messages = cJSON_CreateArray();
    cJSON_AddItemToObject(New_cJSON,"messages",New_Messages);
    cJSON *messages = cJSON_GetObjectItem(cptr, "messages");
    for(int i = 0;i < cJSON_GetArraySize(messages);i++){
        cJSON *subcontent = cJSON_GetArrayItem(messages, i);
        sender = cJSON_GetObjectItem(subcontent, "sender");
        content = cJSON_GetObjectItem(subcontent, "content");
        char adding[5*MAX];
        Creating_cJSON_String(adding,"sender","content",sender -> valuestring,content -> valuestring);
        cJSON *ADDING = cJSON_Parse(adding);
        cJSON_AddItemToArray(New_Messages, ADDING);
    }
    //---------------------------------------------------------------------------
    //-------ADDING THE NEW MESSAGE----------------------------------------------
    char sent_message_answer[MAX];
    Creating_cJSON_String(sent_message_answer,"sender","content",Users[Find_User(AuthToken)].name,sent_message);
    cJSON *JSON_Answer = cJSON_Parse(sent_message_answer);
    cJSON_AddItemToArray(New_Messages,JSON_Answer);
    char *ptr = cJSON_Print(New_cJSON);
    char String_2[10000];
    sprintf(String_2,"%s",ptr);
    fptr = fopen(Directory,"w");
    fprintf(fptr,"%s",String_2);
    fclose(fptr);
    char Answer[MAX];
    Creating_cJSON_String(Answer, "type","content","successful","");
    Connect_and_Send(Answer, sizeof(Answer));
    return;
}
void Join_Channel(char ChannelName[], char AuthToken[]){
    //Making the Path String!
    char Directory[MAX];
    sprintf(Directory,"./Channels/%s.channel.txt",ChannelName);
    FILE *fptr;
    fptr = fopen(Directory,"r");
    //-------CHECKING IF THE CHANNEL EXISTS OR NOT!---
    if(fptr == NULL){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","No Such Channel!");
        Connect_and_Send(Answer,MAX);
        fclose(fptr);
        return;
    }
    //------CHECKING IF THE USER IS IN A CHANNEL OR NOT!-----
    if (Users[Find_User(AuthToken)].Channel_Flag == 1){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","Already in channel");
        Connect_and_Send(Answer, sizeof(Answer));
        return;
    }
    //-------CHECKING THE AUTH TOKEN------------------
    if (strcmp(Users[Find_User(AuthToken)].Authenticator_Code, AuthToken) != 0){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","Authentication failed!");
        Connect_and_Send(Answer, sizeof(Answer));
        return;
    }
    Users[Find_User(AuthToken)].Channel_Flag = 1;
    strcpy(Users[Find_User(AuthToken)].corrent_channel_name, ChannelName);
    char String[100000];
    fscanf(fptr,"%[^EOF]",String);
    cJSON *cptr;
    cptr = cJSON_Parse(String);
    fclose(fptr);
    //-----MAKING THE cJSON STRING AGAIN WITH THE PREVIOUS CONTENT-----------
    cJSON *sender = NULL;
    cJSON *content = NULL;
    cJSON *New_cJSON = cJSON_CreateObject();
    cJSON *name = cJSON_CreateString(ChannelName);
    cJSON_AddItemToObject(New_cJSON,"name", name);
    cJSON *New_Messages = cJSON_CreateArray();
    cJSON_AddItemToObject(New_cJSON,"messages",New_Messages);
    cJSON *messages = cJSON_GetObjectItem(cptr, "messages");
    for(int i = 0;i < cJSON_GetArraySize(messages);i++){
        cJSON *subcontent = cJSON_GetArrayItem(messages, i);
        sender = cJSON_GetObjectItem(subcontent, "sender");
        content = cJSON_GetObjectItem(subcontent, "content");
        char adding[10000];
        Creating_cJSON_String(adding,"sender","content",sender->valuestring , content->valuestring);
        cJSON *ADDING = cJSON_Parse(adding);
        cJSON_AddItemToArray(New_Messages, ADDING);
    }
    //---------------------------------------------------------------------------
    //-------ADDING THE NEW MESSAGE WHICH IS THE #USER JOINED #CHANNELNAME-------

    char Join_channel_message[MAX];
    char joining_message[MAX];
    sprintf(joining_message,"%s joined %s", Users[Find_User(AuthToken)].name, ChannelName);
    Creating_cJSON_String(Join_channel_message,"sender","content","server",joining_message);
    cJSON *JSON_Answer = cJSON_Parse(Join_channel_message);
    cJSON_AddItemToArray(New_Messages,JSON_Answer);
    char *ptr = cJSON_Print(New_cJSON);

    sprintf(String,"%s",ptr);

    fptr = fopen(Directory,"w");
    fprintf(fptr,"%s",String);
    fclose(fptr);
    char Answer[MAX];
    Creating_cJSON_String(Answer, "type","content","successful","");
    Connect_and_Send(Answer, sizeof(Answer));
    return;
}
void Create_Channel(char ChannelName[], char AuthToken[]){
    //--------CHEKING IF THE CHANNEL NAME ALREADY EXISTS OR NOT!------------
    DIR *d;
    struct dirent *dir;
    d = opendir("./Channels");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            char channel[MAX];
            sprintf(channel,"%s.channel.txt", ChannelName);
            if(strcmp(channel, dir->d_name) == 0){
                char Answer[MAX];
                Creating_cJSON_String(Answer,"type","content","Error","Channel Already Exists!");
                Connect_and_Send(Answer, sizeof(Answer));
                return;
            }
        }
        closedir(d);
    }
    //-----------------------------------------------------------------------
    if (Find_User(AuthToken) != -1){
        Users[Find_User(AuthToken)].Channel_Flag = 1;
        strcpy(Users[Find_User(AuthToken)].corrent_channel_name, ChannelName);
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Successful","");
        Connect_and_Send(Answer, sizeof(Answer));
        //creating the file data base for channels!
        FILE *fptr;
        char string[10000];
        cJSON *message;
        cJSON *name;
        message = cJSON_CreateArray();
        cJSON *JSON;
        char msg[MAX];
        char content[MAX];
        sprintf(content,"%s created %s", Users[Find_User(AuthToken)].name, ChannelName);
        Creating_cJSON_String(msg,"sender","content","server",content);
        cJSON *adding = cJSON_Parse(msg);
        JSON = cJSON_CreateObject();
        name = cJSON_CreateString(ChannelName);
        cJSON_AddItemToObject(JSON,"messages",message);
        cJSON_AddItemToArray(message, adding);
        cJSON_AddItemToObject(JSON,"name",name);
        char *ptr = cJSON_Print(JSON);
        sprintf(string,"%s",ptr);
        char Directory[MAX];
        sprintf(Directory,"./Channels/%s.channel.txt", ChannelName);
        fptr = fopen(Directory,"w");
        fprintf(fptr, "%s", string);
        fclose(fptr);
    }
    else{
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","Authentication Failed!");
        Connect_and_Send(Answer, sizeof(Answer));
        return;
    }
}
void Auth_Generator(char string[],int n){
    srand(time(NULL));
    for(int i = 0;i < n-1;i++){
        int key = rand() % (int)(sizeof(Random_characters_Pool) -1);
        string[i] = Random_characters_Pool[key];
    }
    string[n-1] = '\0';
}
void Saving_User(char Username[], char Password[]){
    //--------CHECKING IF THE USERNAME IS ALREADY USED OR NOT!----------
    DIR *d;
    struct dirent *dir;
    d = opendir("./Users");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            char user[MAX];
            sprintf(user,"%s.user.txt", Username);
            if(strcmp(user, dir->d_name) == 0){
                char Answer[MAX];
                Creating_cJSON_String(Answer,"type","content","Error","Already used username!");
                Connect_and_Send(Answer, sizeof(Answer));
                return;
            }
        }
        closedir(d);
    }
    //--------------------------------------------------------------------
    //Making the cJSON string!
    char Answer[MAX];
    Creating_cJSON_String(Answer,"type","content","successful","");
    Connect_and_Send(Answer, sizeof(Answer));
    char string[MAX];
    Creating_cJSON_String(string,"username","password",Username, Password);
    //Creating the notepad data base for users!
    FILE *fptr;
    char Directory[MAX];
    sprintf(Directory,"./Users/%s.user.txt", Username);
    fptr = fopen(Directory,"w");
    fprintf(fptr, "%s", string);
    fclose(fptr);
}
void Checking_the_Login_Request(char Username[],char Password[]){
    //Making the Path String!
    char Directory[MAX];
    sprintf(Directory,"./Users/%s.user.txt",Username);
    FILE *fptr;
    fptr = fopen(Directory,"r");
    if(fptr == NULL){
        char Answer[MAX];
        Creating_cJSON_String(Answer,"type","content","Error","No Such Username!");
        Connect_and_Send(Answer,MAX);
        fclose(fptr);
        return;
    }
    else{
        char String[MAX];
        fscanf(fptr,"%[^EOF]",String);
        //Parsing the cJSON!
        cJSON *cptr;
        cptr = cJSON_Parse(String);
        cJSON *username;
        username = cJSON_GetObjectItem(cptr, "username");
        cJSON *password = cJSON_GetObjectItem(cptr, "password");
        if(strcmp(Password,password -> valuestring)==0 && strcmp(Username, username -> valuestring)==0){

            char string[32];
            Auth_Generator(string, 32);
            strcpy(Users[User_Counter].Authenticator_Code,string);
            char Answer[MAX];
            Creating_cJSON_String(Answer,"type","content","AuthToken",Users[User_Counter].Authenticator_Code);
            strcpy(Users[User_Counter].name, Username);
            Users[User_Counter].Channel_Flag = 0;
            Users[User_Counter].Online_User_Flag = 1;
            User_Counter++;
            Connect_and_Send(Answer,MAX);
        }
        else{
            char Answer[MAX];
            Creating_cJSON_String(Answer,"type","content","Error","Wrong Password Or Username");
            Connect_and_Send(Answer,MAX);
        }
    }
}
void Connect_and_Recive(char * ptr){
    struct sockaddr_in server, client;
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
    }
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
    else
        printf("Socket successfully created..\n");
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully bound..\n");

    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    int len = sizeof(client);
    client_socket = accept(server_socket, (SA *)&client, &len);
    if (client_socket < 0)
    {
        printf("Server accceptance failed...\n");
        exit(0);
    }
    else
        printf("Server acccepted the client..\n");
    char buffer[MAX];
    recv(client_socket, buffer, sizeof(buffer), 0);
    sprintf(ptr,"%s",buffer);
}
int Online_Users(){
    int counter = 0;
    for(int i = 0;i < User_Counter;i++){
        if(Users[i].Online_User_Flag == 1){
            counter++;
        }
    }
    return counter;
}
int main()
{
    mkdir("Users");
    mkdir("Channels");
    while(true){
        char a[MAX];
        printf("Online Users: %d\n", Online_Users());
        Connect_and_Recive(a);
        char request[MAX];
        sscanf(a,"%s", request);

        if (strcmp(request,"register") == 0){
            char Username[MAX];
            char Password[MAX];
            char junk1;
            char junk2;
            sscanf(a ,"register %[^,]%c%c%[A-Za-z0-9]", Username,&junk1,&junk2, Password);
            Saving_User(Username, Password);
        }
        else if (strcmp(request,"login") == 0){
            char Username[MAX];
            char Password[MAX];
            char junk1;
            char junk2;
            sscanf(a ,"login %[^,]%c%c%[A-Za-z0-9]", Username,&junk1,&junk2, Password);
            Checking_the_Login_Request(Username, Password);
        }
        else if (strcmp(request,"create") == 0){
            char channelname[MAX];
            char authtoken[MAX];
            char junk1;
            char junk2;
            sscanf(a ,"create channel %[^,]%c%c%[A-Za-z0-9-_]", channelname,&junk1,&junk2, authtoken);
            Create_Channel(channelname,authtoken);
        }
        else if(strcmp(request,"join") == 0){
            char channelname[MAX];
            char authtoken[MAX];
            char junk1;
            char junk2;
            sscanf(a, "join channel %[^,]%c%c%[A-Za-z0-9-_]", channelname, &junk1, &junk2, authtoken);
            Join_Channel(channelname, authtoken);

        }
        else if(strcmp(request,"send") == 0){
            char message[MAX];
            char authtoken[MAX];
            char junk1;
            char junk2;
            sscanf(a, "send %[^,]%c%c%[A-Za-z0-9-_]", message, &junk1, &junk2, authtoken);
            Send_Message(authtoken,message);
        }
        else if(strcmp(request,"refresh") == 0){
            char authtoken[MAX];
            sscanf(a, "refresh %[^\n]", authtoken);
            Refresh(authtoken);
        }
        else if(strcmp(request,"leave") == 0){
            char authtoken[MAX];

            sscanf(a, "leave %[^\n]", authtoken);
            Leave_Channel(authtoken);
        }
        else if(strcmp(request,"logout") == 0){
            char authtoken[MAX];
            sscanf(a, "logout %[^\n]", authtoken);
            LogOut(authtoken);
        }
        else if(strcmp(request,"channel") == 0){
            char authtoken[MAX];
            sscanf(a, "channel members %[^\n]", authtoken);
            Channel_Members(authtoken);
        }


        }
}
