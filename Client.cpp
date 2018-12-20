// Client.cpp : Defines the entry point for the console application.
//
// 1. open the *.c in the Visual C++, then "rebuild all".
// 2. click "yes" to create a project workspace.
// 3. You need to -add the library 'ws2_32.lib' to your project 
//    (Project -> Properties -> Linker -> Input -> Additional Dependencies) 
// 4. recompile the source.

#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define DEFAULT_PORT	5019

char menu();

int main(int argc, char **argv) {

	char szBuff[100];
	char szBuff1[100];
	char szBuff2[100];
	char szBuff11[100];
	int msg_len;
	//int addr_len;
	struct sockaddr_in server_addr;
	struct hostent *hp;
	SOCKET connect_sock;
	WSADATA wsaData;

	char			*server_name = "localhost";
	unsigned short	port = DEFAULT_PORT;
	unsigned int	addr;

	if (argc != 3) {
		printf("echoscln [server name] [port number]\n");
		return -1;
	}
	else {
		server_name = argv[1];
		port = atoi(argv[2]);
	}

	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
		// stderr: standard error are printed to the screen.
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		//WSACleanup function terminates use of the Windows Sockets DLL.
		WSACleanup();
		return -1;
	}

	if (isalpha(server_name[0]))
		hp = gethostbyname(server_name);
	else {
		addr = inet_addr(server_name);
		hp = gethostbyaddr((char*)&addr, 4, AF_INET);
	}

	if (hp == NULL)
	{
		fprintf(stderr, "Cannot resolve address: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//copy the resolved information into the sockaddr_in structure
	memset(&server_addr, 0, sizeof(server_addr));
	memcpy(&(server_addr.sin_addr), hp->h_addr, hp->h_length);
	server_addr.sin_family = hp->h_addrtype;
	server_addr.sin_port = htons(port);


	connect_sock = socket(AF_INET, SOCK_STREAM, 0);	//TCp socket


	if (connect_sock == INVALID_SOCKET) {
		fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	
	

	printf("Client connecting to: %s\n", hp->h_name);

	if (connect(connect_sock, (struct sockaddr *)&server_addr, sizeof(server_addr))
		== SOCKET_ERROR) {
		fprintf(stderr, "connect() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}





	while (1) {
		char sign[2];
		struct tm *pt;																									//Struct time*
		time_t t;
		t = time(NULL);
		pt = localtime(&t);
		printf("\t\t\t Choose the model you want!\n");
		printf("\t\t******************MENU*******************\n");
		printf("\t\t|\t\t\t\t\t|\n");
		printf("\t\t|\t\t0: registration\t\t|\n");
		printf("\t\t|\t\t\t\t\t|\n");
		printf("\t\t|\t\t1: login\t\t|\n");
		printf("\t\t|\t\t\t\t\t|\n");
		printf("\t\t|\t\t2: exit\t\t\t|\n");
		printf("\t\t|\t\t\t\t\t|\n");
		printf("\t\t*****************************************\n");
		printf("\t\t\tSYSTEM TIME:%d-%d-%d\n", pt->tm_year + 1900, pt->tm_mon + 1, pt->tm_mday);
		printf("Enter the model number:");
		gets(sign);


		if (strcmp(sign, "0") == 0) {
			//registration
			//getting the information about the user
			char username[64];
			char password[32];
			printf("username:");
			gets(username);
			printf("password:");
			gets(password);



			char loginSend[200] = "0";
			char loginDot[2] = ",";
			strcpy(loginSend, sign);
			strcat(loginSend, loginDot);
			strcat(loginSend, username);
			strcat(loginSend, loginDot);
			strcat(loginSend, password);



			msg_len = send(connect_sock, loginSend, sizeof(loginSend), 0);
			if (msg_len == SOCKET_ERROR) {
				fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
				WSACleanup();
				return -1;
			}
			if (msg_len == 0) {
				printf("server closed connection\n");
				closesocket(connect_sock);
				WSACleanup();
				return -1;
			}


			memset(szBuff1, 0, sizeof(szBuff1));
			msg_len = recv(connect_sock, szBuff1, sizeof(szBuff1), 0);
			if (msg_len == SOCKET_ERROR) {
				fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
				closesocket(connect_sock);
				WSACleanup();
				return -1;
			}
			if (msg_len == 0) {
				printf("server closed connection\n");
				closesocket(connect_sock);
				WSACleanup();
				return -1;
			}
			char five[3] = "5";
			if (strcmp(szBuff1, five) == 0) {
				printf("Successfully register!\n");
				continue;
			}
			else {
				printf("Fail to register.\n");
				continue;
			}

		}

		else if (strcmp(sign, "1") == 0) {
			//getting the current user information
			char currentUserName[64];
			char currentUserPassword[64];
			printf("Enter user name:");
			gets(currentUserName);
			printf("Enter passwd:");
			gets(currentUserPassword);






			//
			char loginSend[200] = "0";
			char loginDot[2] = ",";
			strcpy(loginSend, sign);
			strcat(loginSend, loginDot);
			strcat(loginSend, currentUserName);
			strcat(loginSend, loginDot);
			strcat(loginSend, currentUserPassword);



			msg_len = send(connect_sock, loginSend, sizeof(loginSend), 0);
			if (msg_len == SOCKET_ERROR) {
				fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
				WSACleanup();
				return -1;
			}
			if (msg_len == 0) {
				printf("server closed connection\n");
				closesocket(connect_sock);
				WSACleanup();
				return -1;
			}


			memset(szBuff1, 0, sizeof(szBuff1));
			msg_len = recv(connect_sock, szBuff1, sizeof(szBuff1), 0);
			if (msg_len == SOCKET_ERROR) {
				fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
				closesocket(connect_sock);
				WSACleanup();
				return -1;
			}
			if (msg_len == 0) {
				printf("server closed connection\n");
				closesocket(connect_sock);
				WSACleanup();
				return -1;
			}






			char five[3] = "5";
			if (strcmp(szBuff1, five) == 0) {
				printf("Successfully login!\n");
			}
			else {
				printf("Fail to login.\n");
				continue;
			}




			while (1) {
				char menuInput;
				menuInput = menu();
				int out = 0;
				char toServer[100];
				switch (menuInput) {



				case '1':
				{
					memset(szBuff, 0, sizeof(szBuff));
					memset(szBuff2, 0, sizeof(szBuff2));
					printf("Enquiry Keyword:\n");
					char toServer1[100] = "1";
					char c;
					scanf("%s", szBuff);
					while ((c = getchar()) != EOF && c != '\n');

					char dot[2] = ",";
					strcat(toServer1, dot);
					strcat(toServer1, szBuff);




					msg_len = send(connect_sock, toServer1, sizeof(toServer1), 0);
					if (msg_len == SOCKET_ERROR) {
						fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
						WSACleanup();
						return -1;
					}
					if (msg_len == 0) {
						printf("server closed connection\n");
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}


					memset(szBuff11, 0, sizeof(szBuff11));
					msg_len = recv(connect_sock, szBuff11, sizeof(szBuff11), 0);
					if (msg_len == SOCKET_ERROR) {
						fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}
					if (msg_len == 0) {
						printf("server closed connection\n");
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}
					printf("Echo from the server\n%s\n", szBuff11);
					break;
				}


				case '2':
				{
					char c;
					fflush(stdin);
					printf("please input the information you want to append.\n");
					char name[20];
					printf("Name:");
					scanf("%s", name);
					while ((c = getchar()) != EOF && c != '\n');

					printf("Phone:");
					char phone[20];
					scanf("%s", phone);
					while ((c = getchar()) != EOF && c != '\n');

					printf("Tax:");
					char tax[20];
					scanf("%s", tax);
					while ((c = getchar()) != EOF && c != '\n');

					printf("Info:");
					char info[100];
					scanf("%s", info);
					while ((c = getchar()) != EOF && c != '\n');

					char toServer2[200] = "2";
					char dot2[2] = ",";
					strcat(toServer2, dot2);
					strcat(toServer2, name);
					strcat(toServer2, dot2);
					strcat(toServer2, phone);
					strcat(toServer2, dot2);
					strcat(toServer2, tax);
					strcat(toServer2, dot2);
					strcat(toServer2, info);

					msg_len = send(connect_sock, toServer2, sizeof(toServer2), 0);
					if (msg_len == SOCKET_ERROR) {
						fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
						WSACleanup();
						return -1;
					}
					if (msg_len == 0) {
						printf("server closed connection\n");
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}
					memset(szBuff1, 0, sizeof(szBuff1));
					msg_len = recv(connect_sock, szBuff1, sizeof(szBuff1), 0);
					if (msg_len == SOCKET_ERROR) {
						fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}
					if (msg_len == 0) {
						printf("server closed connection\n");
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}
					printf("Echo from the server\n%s\n", szBuff1);
					break;
				}


				case '3':
				{
					char name[30], c;
					memset(name, 0, sizeof(name));
					printf("Please input the user you want to change:");
					scanf("%s", name);
					while ((c = getchar()) != EOF && c != '\n');
					int contentChange;
					printf("Please input the number of correspoding content you want to change\n");
					printf("Name(1),Phone(2),Tax(3),Info(4):");
					scanf("%d", &contentChange);
					char numSend[3];
					itoa(contentChange, numSend, 10);
					char newContent[50];
					printf("Please input the new content:");
					scanf("%s", newContent);
					while ((c = getchar()) != EOF && c != '\n');
					char toServer3[200] = "3";
					char dot3[2] = ",";
					strcat(toServer3, dot3);
					strcat(toServer3, name);
					strcat(toServer3, dot3);
					strcat(toServer3, numSend);
					strcat(toServer3, dot3);
					strcat(toServer3, newContent);
					msg_len = send(connect_sock, toServer3, sizeof(toServer3), 0);
					if (msg_len == SOCKET_ERROR) {
						fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
						WSACleanup();
						return -1;
					}
					if (msg_len == 0) {
						printf("server closed connection\n");
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}
					memset(szBuff1, 0, sizeof(szBuff1));
					msg_len = recv(connect_sock, szBuff1, sizeof(szBuff1), 0);
					if (msg_len == SOCKET_ERROR) {
						fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}
					if (msg_len == 0) {
						printf("server closed connection\n");
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}
					printf("Echo from the server\n%s\n", szBuff1);
					break;
				}
				// Delete
				case '4':
				{
					char name[30], c;
					memset(name, 0, sizeof(name));
					printf("Please input the user's name of information you want to delete:");
					scanf("%s", name);
					while ((c = getchar()) != EOF && c != '\n');
					char toServer4[200] = "4";
					char dot4[2] = ",";
					strcat(toServer4, dot4);
					strcat(toServer4, name);
					msg_len = send(connect_sock, toServer4, sizeof(toServer4), 0);
					if (msg_len == SOCKET_ERROR) {
						fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
						WSACleanup();
						return -1;
					}
					if (msg_len == 0) {
						printf("server closed connection\n");
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}
					memset(szBuff1, 0, sizeof(szBuff1));
					msg_len = recv(connect_sock, szBuff1, sizeof(szBuff1), 0);
					if (msg_len == SOCKET_ERROR) {
						fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}
					if (msg_len == 0) {
						printf("server closed connection\n");
						closesocket(connect_sock);
						WSACleanup();
						return -1;
					}
					printf("Echo from the server\n%s\n", szBuff1);
					break;
				}

				case '5':
				{
					out = 1;
					break;
				}
				default:
					printf("Invalid input, please try again.\n");
					break;
				}
				if (out == 1) {
					printf("\t\tThanks for logging, see you next time!\n");
					break;
				}
			}
			closesocket(connect_sock);
			WSACleanup();
			break;









			/*
			char szUser[20] = { 0 };

			char *PUser = "user";
			char *Pword = "1234";
			char chPwd[5];

			int flag_u = 0, i = 0, flag_p = 0;
			printf("Enter user name:");
			gets_s(szUser);
			//test user name
			//printf("szUser=%s\n",szUser);
			if (strcmp(szUser, PUser) == 0)
			flag_u = 1;
			else
			flag_u = 0;
			printf("Enter passwd:");
			while (i<4)
			{
			chPwd[i] = getch();
			if (chPwd[i] == 13)
			break;
			printf("%c", chPwd[i]);
			Sleep(500);
			printf("\b*");//hide password input to *
			i++;
			}
			//press enter to end your input
			getch();
			chPwd[4] = '\0';
			//test password
			//printf("chPwd=%s\n",chPwd);
			printf("\n");
			if (strcmp(chPwd, Pword) == 0)
			flag_p = 1;
			else
			flag_p = 0;
			if ((flag_u == 1) && (flag_p == 1))
			printf("Success...Login complete??");
			else
			printf("Wrong??");
			getch();


			*/







		}
		else if (strcmp(sign, "2") == 0) {
			break;
		}
	}




	/*

	*/

}


char menu() {
	struct tm *pt;																									//Struct time*
	time_t t;
	t = time(NULL);
	pt = localtime(&t);																								//Load the local time give the value to the struct
	printf("\n");
	printf("\t\t  Welcome to Information Center!\n");
	printf("\t\t******************MENU*******************\n");
	printf("\t\t|\t\t\t\t\t|\n");
	printf("\t\t|\t\t1.Enquiry\t\t|\n");
	printf("\t\t|\t\t\t\t\t|\n");
	printf("\t\t|\t\t2.Add Information\t|\n");
	printf("\t\t|\t\t\t\t\t|\n");
	printf("\t\t|\t\t3.Modification\t\t|\n");
	printf("\t\t|\t\t\t\t\t|\n");
	printf("\t\t|\t\t4.Delete Info\t\t|\n");
	printf("\t\t|\t\t\t\t\t|\n");
	printf("\t\t|\t\t5.Quit\t\t\t|\n");
	printf("\t\t|\t\t\t\t\t|\n");
	printf("\t\t*****************************************\n");
	printf("\t\t\tSYSTEM TIME:%d-%d-%d\n", pt->tm_year + 1900, pt->tm_mon + 1, pt->tm_mday);
	char enterInput;
	while (1) {
		printf("\tPlease input the corresponding number to enter the operation>>");
		fflush(stdin);
		char c;
		scanf("%c", &enterInput);
		while (getchar() != '\n');
		if (enterInput == '1' || enterInput == '2' || enterInput == '3' || enterInput == '4' || enterInput == '5')
			break;
		else {
			printf("\tNo valid input,try again.\n");
			continue;
		}
	}
	return enterInput;
}