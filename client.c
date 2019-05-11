#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdbool.h>

//Method prototypes
void parseAnswer(char* problem, char* answer);
bool containsBYE(char* str);

int main(int argc, char* argv[]){
    //Open TCP stream socket
    int sock = socket(AF_INET , SOCK_STREAM , 0);
    if(sock == -1){
        printf("Socket failed to create");
        exit(1);
    }
    //char* ID = argv[1];
    int port = atoi(argv[2]);
    char* IP = argv[3];

    //Declare socket address and populate with IP, port, and other info
    struct sockaddr_in sockAddress;
    sockAddress.sin_family = AF_INET;
    sockAddress.sin_port = htons(port);
    sockAddress.sin_addr.s_addr = inet_addr(IP);
    

    //Connect to the remote server
    if(connect(sock, (struct sockaddr *)&sockAddress, sizeof(sockAddress)) < 0){
        printf("Connect Failed");
        exit(1);
    }

    //Sending the initial mesage with netID
    char* netID = argv[1];
    char buf[29];
    sprintf(buf, "cs230 HELLO %s\n", netID);
    if(send(sock, buf, strlen(buf), 0) < 0){
        printf("NetID Failed to Send");
        exit(1);
    }

    //Loops through math problems until encountering the flag
    while(true){
        char mathProb[100];
        char probAnswer[100];
        //Receive the first math problem
        if(recv(sock, mathProb, 100, 0) < 0){
            printf("Error occurred while Receiving Problem");
            exit(1);
        }

        //Flag received, exit loop
        if(containsBYE(mathProb)){
            printf("Captured Flag %s", mathProb);
            close(sock);
            break;
        }

        //Parse math problem and return answer with helper method
        parseAnswer(mathProb, probAnswer);

        //Send correct solution to server
        if(send(sock, probAnswer, strlen(probAnswer), 0) < 0){
            printf("Problem Occurred Sending Answer");
            exit(1);
        }
    }
     return 0;
}

//Checks if the problem contains BYE, signifying the end of the problems
bool containsBYE(char* str){
    //Loops through the problem, and finds the letters BYE
    for(int j = 0; j < strlen(str)-3; ++j){
        if(str[j] == 'B' && str[j+1] == 'Y' && str[j+2] == 'E'){
                return true;
        }
    }
    return false;
}

//Takes in the math problem and returns a char* answer in the correct format
void parseAnswer(char* problem, char* finalAns){
    int answer;
    char num1[20];
    int num1Index = 0;
    char num2[20];
    int num2Index = 0;
    int probIndex = 13;
   
    //Fills num1 with first number
    while(48 <= problem[probIndex] && problem[probIndex] <= 57){
        //While isNumber, add to String
        num1[num1Index] = problem[probIndex];
        num1Index++;
        probIndex++;
    }
    //End string
    num1[num1Index] = '\0';

    //Assign first number
    int firstNum = atoi(num1);
    
    //Get to operator index
    probIndex++;
    //Assign operator
    char operator = problem[probIndex];

    //Get to next number's index
    probIndex++;
    probIndex++;

    //Fills num2 with second number
    while(48 <= problem[probIndex] && problem[probIndex] <= 57){
        num2[num2Index] = problem[probIndex];
        probIndex++;
        num2Index++;
    }
    //End string
    num2[num2Index] = '\0';

    //Assing second number
    int secondNum = atoi(num2);

    //Perform operation based on operator
    if(operator == '+'){
        answer = firstNum + secondNum;
    }
    if(operator == '-'){
        answer = firstNum - secondNum;
    }
    if(operator == '*'){
        answer = firstNum * secondNum;
    }
    if(operator == '/'){
        answer = firstNum / secondNum;
    }

    //Format and return answer string
    sprintf(finalAns, "cs230 %i\n", answer);
}
