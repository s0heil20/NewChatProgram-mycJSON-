//The Chat Program Client ---------By: Soheil Mahdizade ---------student number : 98106083
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <conio.h>
#define MAX 80
#define PORT 12345
#define SA struct sockaddr
#include "mycJSON.h"
//----------------------------------------
//----------------Functions---------------
//function for creating socket for each request!
int Creating_Socket(){
    int client_socket;
	struct sockaddr_in servaddr;

	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

	// Create and verify socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	// Assign IP and port
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// Connect the client socket to server socket
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection to the server failed...\n");
		exit(0);
	}
    return client_socket;
}

//The refresh request function!

void Refresh(char User_Auth_Code[]){
    char Refresh_Request[1000000];
    cJSON *Refresh_Request_jason;
    int server_socket = Creating_Socket();
    sprintf(Refresh_Request,"refresh %s\n",User_Auth_Code);
    send(server_socket,Refresh_Request,sizeof(Refresh_Request), 0);
    memset(Refresh_Request, 0,sizeof(Refresh_Request));
    recv(server_socket, Refresh_Request, sizeof(Refresh_Request), 0);
    Refresh_Request_jason = cJSON_Parse(Refresh_Request);
    cJSON* sender = NULL;
    cJSON* content = NULL;
    cJSON *contents = cJSON_GetObjectItem(Refresh_Request_jason,"content");
    for(int i = 0;i < cJSON_GetArraySize(contents);i++){
        printf("\n");
        cJSON *subcontent = cJSON_GetArrayItem(contents, i);
        sender = cJSON_GetObjectItem(subcontent, "sender");
        content = cJSON_GetObjectItem(subcontent, "content");
        printf("%s says: %s\n", sender -> valuestring, content -> valuestring);
    }
}

//The Channel Members request function!

void Channel_Members(char User_Auth_Code[]){
    char Channel_Members_Request[1000000];
    cJSON *Channel_Members_Request_jason;
    int server_socket = Creating_Socket();
    sprintf(Channel_Members_Request,"channel members %s\n",User_Auth_Code);
    send(server_socket,Channel_Members_Request,sizeof(Channel_Members_Request), 0);
    memset(Channel_Members_Request, 0,sizeof(Channel_Members_Request));
    recv(server_socket, Channel_Members_Request, sizeof(Channel_Members_Request), 0);
    Channel_Members_Request_jason = cJSON_Parse(Channel_Members_Request);
    cJSON *contents = cJSON_GetObjectItem(Channel_Members_Request_jason,"content");
    for(int i = 0;i < cJSON_GetArraySize(contents);i++){
        cJSON *subcontent = cJSON_GetArrayItem(contents, i);
        printf("%d : %s\n", i+1, subcontent -> valuestring);
    }
}

//The sending message request function!

void Send_Message(char User_Auth_Code[], char Message[]){
    char Message_Request[1000];
    int server_socket = Creating_Socket();
    sprintf(Message_Request,"send %s, %s\n", Message, User_Auth_Code);
    send(server_socket,Message_Request,sizeof(Message_Request), 0);
    memset(Message_Request, 0,sizeof(Message_Request));
    recv(server_socket, Message_Request, sizeof(Message_Request), 0);
    return;
}

//Leaving Channel function!

void Leave_Channel(char User_Auth_Code[]){
    char Leave_Channel_Request[2*MAX];
    int server_socket = Creating_Socket();
    sprintf(Leave_Channel_Request, "leave %s\n", User_Auth_Code);
    send(server_socket,Leave_Channel_Request,sizeof(Leave_Channel_Request), 0);
    memset(Leave_Channel_Request, 0,sizeof(Leave_Channel_Request));
    recv(server_socket, Leave_Channel_Request, sizeof(Leave_Channel_Request), 0);
    printf("You have successfully Leaved the channel!\n");
    return;
    }

//Logging out function!

void LogOut(char User_Auth_Code[]){
    char Logout_Request[2*MAX];
    int server_socket = Creating_Socket();
    sprintf(Logout_Request, "logout %s\n", User_Auth_Code);
    send(server_socket,Logout_Request,sizeof(Logout_Request), 0);
    memset(Logout_Request, 0,sizeof(Logout_Request));
    recv(server_socket, Logout_Request, sizeof(Logout_Request), 0);
    printf("You have successfully Logged Out!\n");
    return;
}

//Conversation Menu function!

void Conversation_Menu(char User_Auth_Code[]){
    char a;
    while(true){
        printf("1: Send Message\n");
        printf("2: Refresh\n");
        printf("3: Channel Members\n");
        printf("4: Leave Channel\n");
        a = getch();
        if(a == '1'){
            printf("Enter your Message(with a Maximum of 400 characters): ");
            char My_Message[5*MAX];
            scanf(" %[^\n]", My_Message);
            Send_Message(User_Auth_Code, My_Message);
        }
        else if(a == '2'){
            Refresh(User_Auth_Code);

        }
        else if(a == '3'){
            Channel_Members(User_Auth_Code);

        }
        else if(a == '4'){
            Leave_Channel(User_Auth_Code);
            return;
        }
    }


}

//Creating Channel function!

void Creating_Channel(char User_Auth_Code[] ,char name[]){
    char Creating_Channel_Request[2*MAX];
    int server_socket = Creating_Socket();
    sprintf(Creating_Channel_Request,"create channel %s, %s\n",name,User_Auth_Code);
    send(server_socket, Creating_Channel_Request, sizeof(Creating_Channel_Request), 0);
    memset(Creating_Channel_Request, 0,sizeof(Creating_Channel_Request));
    recv(server_socket, Creating_Channel_Request, sizeof(Creating_Channel_Request), 0);
    cJSON *My_Answer = cJSON_Parse(Creating_Channel_Request);
    cJSON *type = cJSON_GetObjectItem(My_Answer, "type");
    if(strcmp("Error", type -> valuestring) == 0){
        printf("Cannot Create the Channel try again...\n");
        return;
    }
    else{
        printf("Channel successfully created!\n");
        Conversation_Menu(User_Auth_Code);
        return;
    }
}

//Joining Channel Function!

void Join_Channel(char name[], char User_Auth_Code[]){
    char Joining_Channel_Request[1000];
    int server_socket = Creating_Socket();
    sprintf(Joining_Channel_Request,"join channel %s, %s\n",name,User_Auth_Code);
    send(server_socket, Joining_Channel_Request, sizeof(Joining_Channel_Request),0);
    memset(Joining_Channel_Request, 0,sizeof(Joining_Channel_Request));
    recv(server_socket, Joining_Channel_Request, sizeof(Joining_Channel_Request), 0);
    cJSON *My_Answer = cJSON_Parse(Joining_Channel_Request);
    cJSON *type = cJSON_GetObjectItem(My_Answer, "type");
    if(strcmp("Error", type -> valuestring) == 0){
        printf("Can't join the channel...\n");
        return;
    }
    else{
        printf("You have joined the channel!\n");
        Conversation_Menu(User_Auth_Code);
        return;
    }
    }

//Register Menu!

void Register_Menu()
{
    char buffer_Username[MAX];
    int client_socket;
    char buffer_Password[MAX];
    char Register_Request[2*MAX];
    while(true){
        system("CLS");
        printf("Enter User name:\n");
        scanf("%s", buffer_Username);
        printf("Enter Password:\n");
        scanf(" %s", buffer_Password);
        sprintf(Register_Request,  "register %s, %s\n", buffer_Username,buffer_Password);
        client_socket = Creating_Socket();

        send(client_socket, Register_Request, sizeof(Register_Request), 0);
        memset(Register_Request, 0, sizeof(Register_Request));
        recv(client_socket, Register_Request, sizeof(Register_Request), 0);
        cJSON *My_Answer = cJSON_Parse(Register_Request);
        cJSON *type = cJSON_GetObjectItem(My_Answer,"type");

        if(strcmp("Error", type -> valuestring) == 0){
            printf("This user name is taken!\n");

            return;
        }
        else{
            printf("Account Created!\n");
            return;
        }
    }
}
//Main Menu function!

void Main_Menu(char User_Auth_Code[])
{
    char a;
    while(true)
    {
        printf("1: Create Channel\n");
        printf("2: Join Channel\n");
        printf("3: Logout\n");
        a = getch();
        if (a == '1'){
            char Channel_Name[MAX];
            system("CLS");
            printf("Enter your channel name:\n");
            scanf("%s", Channel_Name);
            Creating_Channel(User_Auth_Code, Channel_Name);
        }
        else if (a == '2'){
            char Channel_Name[MAX];
            system("CLS");
            printf("Enter the channel name:\n");
            scanf("%s", Channel_Name);
            Join_Channel(Channel_Name, User_Auth_Code);

        }
        else if (a == '3'){
            LogOut(User_Auth_Code);
            return;
        }
    }
}

//Login Menu!

void Login_Menu()
{
    int client_socket;
    char buffer_Username[MAX];
    char buffer_Password[MAX];
    char Login_Request[2*MAX];
    while(true)
    {
        printf("Enter User name:\n");
        scanf("%s", buffer_Username);
        printf("Enter Password:\n");
        scanf(" %s", buffer_Password);
        sprintf(Login_Request,  "login %s, %s\n", buffer_Username,buffer_Password);
        client_socket = Creating_Socket();
        send(client_socket, Login_Request, sizeof(Login_Request), 0);
        memset(Login_Request, 0, sizeof(Login_Request));
        recv(client_socket, Login_Request, sizeof(Login_Request), 0);
        cJSON *My_Answer = cJSON_Parse(Login_Request);
        cJSON *type = cJSON_GetObjectItem(My_Answer,"type");
        if (strcmp("Error", type -> valuestring) == 0){
            printf("Username or Password is not correct, pls try again...\n");
            return;
        }
        else
            printf("You have successfully logged in to your account!\n");
            cJSON *content = cJSON_GetObjectItem(My_Answer,"content");
            Main_Menu(content -> valuestring);
    }
}
//-------------------------------------------------
//--------------The Main function!-----------------
int main(void){

	char a;
    while(1){
        printf("Account Menu:\n");
        printf("1. Register!\n");
        printf("2. Login!\n");
        printf("3. Exit!\n");
        a = getch();
        if (a  == '1'){
            Register_Menu();

        }
        else if (a  == '2'){
            Login_Menu();

        }
        else if(a == '3')
            exit(0);
    }


}
//----------------------------------------------
//----------------------------------------------
