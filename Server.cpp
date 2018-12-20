// Server.cpp : create a console application, and include the sources in the project
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

HANDLE  g_hMutex = NULL;
#define DEFAULT_PORT	5019
void modify(FILE *x, FILE *y, char name[]);


typedef struct client_list_node
{
	SOCKET  socket_client;
	struct sockaddr_in c_sin;
	int     is_run;
	HANDLE  h;
}client_list_node_st, *client_list_node_t;

#define MAX_CLIENT_NUMS 2
static int    client_nums = 0;

static client_list_node_st client_list[MAX_CLIENT_NUMS] = { 0 };
SOCKET sock;
static struct sockaddr_in local;

DWORD WINAPI myfun1(LPVOID lpParameter);
int main(int argc, char **argv) {

	char szBuff[100];
	int msg_len;
	int addr_len;
	WSADATA wsaData;

	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
		// stderr: standard error are printed to the screen.
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		//WSACleanup function terminates use of the Windows Sockets DLL. 
		WSACleanup();
		return -1;
	}
	// Fill in the address structure
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(DEFAULT_PORT);
	sock = socket(AF_INET, SOCK_STREAM, 0);	//TCp socket


	if (sock == INVALID_SOCKET) {
		fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	if (bind(sock, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR) {
		fprintf(stderr, "bind() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//waiting for the connections
	if (listen(sock, 5) == SOCKET_ERROR) {
		fprintf(stderr, "listen() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	printf("Waiting for the connections ........\n");
	while (1) {
		SOCKET msg_sock;
		struct sockaddr_in  client_addr;;
		addr_len = sizeof(client_addr);

		msg_sock = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
		int sendOut = 0;
		g_hMutex = CreateMutex(NULL, FALSE, NULL);
		if (msg_sock == INVALID_SOCKET) {
			fprintf(stderr, "accept() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			continue;
			//return -1;
		}
		else {
			if (client_nums + 1 > MAX_CLIENT_NUMS) {
				send(msg_sock, "You have been cut off due to reach the maximum number of connection \n", strlen("You have been cut off due to reach the maximum number of connection \n"), 0);
				printf("New client requests to connect IP: %s PORT: %d, the client connection has been cut off due to reach the maximum connection \n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
				closesocket(msg_sock);
				char full[3] = "1";

				//sendOut = send(msg_sock,full, sizeof(full), 0);
				continue;
			}
			else {
				//char empty[3] = "0";
				//sendOut = send(msg_sock, empty, sizeof(empty), 0);
				int j = 0;
				for (j = 0; j < MAX_CLIENT_NUMS; j++) {
					if (client_list[j].is_run == 0) {
						client_list[j].is_run = 1;
						client_list[j].socket_client = msg_sock;
						client_list[j].c_sin;
						memcpy(&(client_list[j].c_sin), &client_addr, sizeof(client_addr));
						client_list[j].h = CreateThread(NULL, 0, myfun1, &(client_list[j]), 0, NULL);
						if (client_list[j].h) {
							CloseHandle(client_list[j].h);
						}
						client_nums++;
						break;  
					}
				}
				//!!!
				//for (j = 0; j < MAX_CLIENT_NUMS; j++) {
					//if (client_list[j].is_run == 0) {
						//if (client_list[j].h) {
							//CloseHandle(client_list[j].h);
						//}
					//}
				//}
			}
		}
	}
	WaitForMultipleObjects(client_nums, client_list, TRUE, INFINITE);
	CloseHandle(g_hMutex);
	closesocket(sock);
	WSACleanup();
	return 0;
}

DWORD WINAPI myfun1(LPVOID lpParameter)
{
	char revData[500];
	int  ret;
	client_list_node_t  node = (client_list_node_t)lpParameter;
	printf("New client has connected to the server , IP = %s PORT = %d \n", inet_ntoa(node->c_sin.sin_addr), node->c_sin.sin_port);
	printf("Maximum number of client connection:%d, current number of client connection:%d\n", MAX_CLIENT_NUMS, client_nums);
	int success = 0;
	while (1)
	{
		ret = recv(node->socket_client, revData, sizeof(revData), 0);
		if (ret == SOCKET_ERROR) {
			fprintf(stderr, "recv() failed withr eror %d\n", WSAGetLastError());
			WSACleanup();
			break;
			//return -1;
		}
		if (ret == 0) {
			printf("Client closed connection\n");
			closesocket(node->socket_client);
			break;
			
		}
		printf("Bytes Received: %d, message: %s from %s, PORT = %d\n", sizeof(revData), revData, inet_ntoa(node->c_sin.sin_addr), node->c_sin.sin_port);
		
		if(client_nums>1)
			WaitForMultipleObjects(client_nums, client_list, TRUE, INFINITE);


		// Decode login info
		if (success == 0) {
			char logInter[100] = "0";
			int k = 0;
			char sign[5]="0";
			char userName[20];
			char passWord[20];
			int counter = 0;

			for (int i = 0; i < strlen(revData); i++)
			{
				if (revData[i] != ',')
				{
					logInter[k] = revData[i];
					k++;
					continue;
				}
				else if (revData[i] == ',')
				{
					if (counter == 0) {
						logInter[k++] = '\0';
						strcpy(sign, logInter);
						memset(logInter, 0, sizeof(logInter));
						k = 0;
						counter++;
						continue;
					}
					else {
						logInter[k++] = '\0';
						strcpy(userName, logInter);
						memset(logInter, 0, sizeof(logInter));
						k = 0;
						counter++;
						continue;
					}

				}
			}
			strcpy(passWord, logInter);


			if (strcmp(sign, "1") == 0){
				FILE *fpRead = fopen("user_data.txt", "r");
				if (fpRead == NULL)
				{
					return 0;
				}
				char user[64];
				char password[64];
				char returnValue[2]="0";
				while (1) {
					//end of the user_data.txt
					if (feof(fpRead)) {
						char five[3] = "-5";
						strcpy(returnValue, five);
						break;
					}
					fscanf(fpRead, "%s %s", &user, &password);
					if (strcmp(password, passWord) == 0 && strcmp(user, userName) == 0) {
						success = 1;
						char five[3] = "5";
						strcpy(returnValue, five);
						break;
					}

				}
				fclose(fpRead);
				ret = send(node->socket_client, returnValue, sizeof(returnValue), 0);
				continue;
			}
			else if (strcmp(sign, "0") == 0) {
				//putting the information into the user_data.txt
				FILE *fpWrite = fopen("user_data.txt", "ab+");
				if (fpWrite == NULL) {
					return 0;
				}
				fprintf(fpWrite, "%s", userName);
				fprintf(fpWrite, " ");
				fprintf(fpWrite, "%s", passWord);
				fprintf(fpWrite, "\n");
				fclose(fpWrite);
				char five1[3] = "5";
				ret = send(node->socket_client, five1, sizeof(five1), 0);
				continue;
			}
		}











		// Divide and get the funciton

		// Distinguish the function
		char conver[100] = "0";
		int j = 0;
		char name[20] = "0";
		char content[100] = "0";
		char function[100] = "-1";
		char area[3] = "0";
		char phone[30] = "0";
		char tax[30] = "0";
		int count = 0;
		for (int i = 0; i < strlen(revData); i++)
		{
			if (revData[i] != ',')
			{
				conver[j] = revData[i];
				j++;
				continue;
			}
			else if (revData[i] == ',')
			{
				char minus[5] = "-1";
				char two[2] = "2";
				char three[2] = "3";
				if (strcmp(function, minus) == 0) {
					conver[j++] = '\0';
					strcpy(function, conver);
					memset(conver, 0, sizeof(conver));
					j = 0;
					continue;
				}
				if (strcmp(function, two) == 0) {
					if (count == 0) {
						conver[j++] = '\0';
						memset(name, 0, sizeof(name));
						strcpy(name, conver);
						memset(conver, 0, sizeof(conver));
						j = 0;
						count++;
						continue;
					}
					if (count == 1) {
						conver[j++] = '\0';
						memset(phone, 0, sizeof(phone));
						strcpy(phone, conver);
						memset(conver, 0, sizeof(conver));
						j = 0;
						count++;
						continue;
					}
					if (count == 2) {
						conver[j++] = '\0';
						memset(tax, 0, sizeof(tax));
						strcpy(tax, conver);
						memset(conver, 0, sizeof(conver));
						j = 0;
						count++;
						continue;
					}

				}
				if (strcmp(function, three) == 0) {
					if (count == 0) {
						conver[j++] = '\0';
						memset(name, 0, sizeof(name));
						strcpy(name, conver);
						memset(conver, 0, sizeof(conver));
						j = 0;
						count++;
						continue;
					}
					if (count == 1) {
						conver[j++] = '\0';
						memset(area, 0, sizeof(area));
						strcpy(area, conver);
						memset(conver, 0, sizeof(conver));
						j = 0;
						count++;
						continue;
					}
				}
			}
		}
		strcpy(content, conver);
		char functionOne[2] = "1";
		char functionTwo[2] = "2";
		char functionThree[2] = "3";
		char functionFour[2] = "4";





		// Search function

		if (strcmp(function, functionOne) == 0) {

			char buf[1000];
			char buf1[1000];
			char buf2[1000];
			char buf3[1000];
			char buf4[1000];
			FILE *fp;
			int len;
			char str[1000];
			char send_out[1000] = "No record found\n";
			if ((fp = fopen("phonebook_users.txt", "r")) == NULL)
			{
				perror("fail to read");
				exit(1);
			}
			while (!feof(fp)) {
				fgets(buf, 1000, fp);
				int counter = 0;
				int j = 0;
				memset(str, 0, sizeof(str));
				for (int i = 0; i < strlen(buf); i++)
				{
					if (buf[i] != ',')
					{
						str[j] = buf[i];
						j++;
					}
					else if (buf[i] == ',')
					{
						if (counter == 0)
							strcpy(buf1, str);
						else if (counter == 1)
							strcpy(buf2, str);
						else if (counter == 2)
							strcpy(buf3, str);
						memset(str, 0, sizeof(str));
						counter++;
						j = 0;
					}
				}
				strcpy(buf4, str);
				if (strcmp(content, buf1) == 0 || strcmp(content, buf2) == 0 ||
					strcmp(content, buf3) == 0 || strcmp(content, buf4) == 0) {
					memset(send_out, 0, sizeof(send_out));
					strcat(send_out, "Name:");
					strcat(send_out, buf1);
					strcat(send_out, "\n");
					strcat(send_out, "Phone:");
					strcat(send_out, buf2);
					strcat(send_out, "\n");
					strcat(send_out, "Tax:");
					strcat(send_out, buf3);
					strcat(send_out, "\n");
					strcat(send_out, "Info:");
					strcat(send_out, buf4);
					break;
				}
				fflush(stdin);
			}
			fclose(fp);
			char b[100] = "Hello!\n";
			strcpy(b, send_out);
			ret = send(node->socket_client, b, sizeof(b), 0);
		}







		// Function two - Adding

		if (strcmp(function, functionTwo) == 0) {
			FILE *fp;
			fp = fopen("phonebook_users.txt", "a+");
			if (fp == NULL) {
				perror("fail to read");
				exit(1);
			}
			fseek(fp, 0, SEEK_END);
			char addInfo[300] = "0";
			char dots[2] = ",";
			strcpy(addInfo, name);
			strcat(addInfo, dots);
			strcat(addInfo, phone);
			strcat(addInfo, dots);
			strcat(addInfo, tax);
			strcat(addInfo, dots);
			strcat(addInfo, content);
			fwrite(addInfo, strlen(addInfo), 1, fp); // write into oringinal txt file
			char space[2] = "\n";
			fwrite(space, strlen(space), 1, fp); // add \n in the end
			fflush(stdin);
			fclose(fp);
			char out[100] = "Adding successfully!\n";
			ret = send(node->socket_client, out, sizeof(out), 0);

		}





		// Function3 - modification
		if (strcmp(function, functionThree) == 0) {
			char buf[1000];
			char buf1[1000];
			char buf2[1000];
			char buf3[1000];
			char buf4[1000];
			FILE *fp;
			char str[1000];
			if ((fp = fopen("phonebook_users.txt", "r")) == NULL)
			{
				perror("fail to read");
				exit(1);
			}
			while (!feof(fp)) {
				fgets(buf, 1000, fp);
				int counter = 0;
				int j = 0;
				memset(str, 0, sizeof(str));
				for (int i = 0; i < strlen(buf); i++)
				{
					if (buf[i] != ',')
					{
						str[j] = buf[i];
						j++;
					}
					else if (buf[i] == ',')
					{
						if (counter == 0)
							strcpy(buf1, str);
						else if (counter == 1)
							strcpy(buf2, str);
						else if (counter == 2)
							strcpy(buf3, str);
						memset(str, 0, sizeof(str));
						counter++;
						j = 0;
					}
				}
				strcpy(buf4, str);
				if (strcmp(name, buf1) == 0) {
					break;
				}
				fflush(stdin);
			}
			fclose(fp);
			if (strcmp(area, functionOne) == 0)
				strcpy(buf1, content);
			if (strcmp(area, functionTwo) == 0)
				strcpy(buf2, content);
			if (strcmp(area, functionThree) == 0)
				strcpy(buf3, content);
			if (strcmp(area, functionFour) == 0)
				strcpy(buf4, content);

			FILE *file1 = NULL;
			FILE *file2 = NULL;
			char filename1[] = "phonebook_users.txt";
			char filename2[] = "copy.txt"; // we will copy the new data into this file, 
										   //and in the end, we name the file with original file name

			if ((file1 = fopen(filename1, "r")) == NULL)
			{
				printf("File Open Error??\n");
				return 1;
			}

			if ((file2 = fopen(filename2, "w")) == NULL)
			{
				printf("File Open Error??\n");
				return 1;
			}
			modify(file1, file2, name); // sub function modify
			fflush(stdin);
			fclose(file1);
			fclose(file2);
			file1 = file2 = NULL;
			remove(filename1);
			rename(filename2, filename1);


			FILE *fa;
			fa = fopen("phonebook_users.txt", "a+");
			if (fa == NULL) {
				perror("fail to read");
				exit(1);
			}
			fseek(fa, 0, SEEK_END);

			char modiInfo[300] = "0";
			char dots[2] = ",";
			strcpy(modiInfo, buf1);
			strcat(modiInfo, dots);
			strcat(modiInfo, buf2);
			strcat(modiInfo, dots);
			strcat(modiInfo, buf3);
			strcat(modiInfo, dots);
			strcat(modiInfo, buf4);
			fwrite(modiInfo, strlen(modiInfo), 1, fa);// we write the midified data into file
			char space[2] = "\n";
			fwrite(space, strlen(space), 1, fa);
			fflush(stdin);
			fclose(fa);
			char out[50] = "successfully modify the information!\n";
			ret = send(node->socket_client, out, sizeof(out), 0);

		}




		//!!!!!!
		// Four - Delete the specific information
		if (strcmp(function, functionFour) == 0) {
			FILE *file1 = NULL;
			FILE *file2 = NULL;
			char filename1[] = "phonebook_users.txt";
			char filename2[] = "copy.txt";

			if ((file1 = fopen(filename1, "r")) == NULL)
			{
				printf("File Open Error??\n");
				return 1;
			}

			if ((file2 = fopen(filename2, "w")) == NULL)
			{
				printf("File Open Error??\n");
				return 1;
			}

			modify(file1, file2, content); // sub function modify
			fflush(stdin);
			fclose(file1);
			fclose(file2);
			file1 = file2 = NULL;
			remove(filename1);
			rename(filename2, filename1);
			char out[100] = "Successfully delete the information!\n";
			ret = send(node->socket_client, out, sizeof(out), 0);
		}


		if (ret == 0) {
			printf("Client closed connection\n");
			closesocket(node->socket_client);
			break;
			//return -1;
		}
		if (ret == SOCKET_ERROR) {
			fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			break;
			//return -1;
		}
		fflush(stdin);
		if(client_nums>1)
			ReleaseMutex(g_hMutex);
	}
	node->is_run = 0;
	client_nums--;
	printf("Maximum number of client connection:%d, current number of client connection:%d\n", MAX_CLIENT_NUMS, client_nums);
	return -1;
}



// Delete file
void modify(FILE *x, FILE *y, char name[])
{
	char temp[1024] = { 0 };
	char *p1 = NULL;
	char *p2 = NULL;

	while (fgets(temp, 1024, x))
	{
		p2 = temp;

		while ((p1 = strstr(p2, name)) != NULL)
		{
			for (int i = 0; &p2[i] != p1; ++i) // if we find the information that we wang to modify, 
											   //we do not write it in the new file
				fputc(p2[i], y);

			p2 = p1 + strlen(temp);
		}

		for (int i = 0; p2[i] != '\0'; ++i)
			fputc(p2[i], y);
	}

}