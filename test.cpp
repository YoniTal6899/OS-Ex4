#include "myStack.h"
#include <stdio.h>
#include <string.h>
using namespace std;

// PUSH - TOP - PUSH - PUSH - POP - TOP - PUSH - PUSH - TOP
int main()
{
    const char* temp="first";
    char ans1[5];
    strcpy(ans1,temp);
    push(ans1);
    if(strcmp(top(),"first")!=0){exit(1);}
    printf("1 passed successfully !\n");
    const char* temp1= "2nd";
    char ans2[5];
    strcpy(ans2,temp1);
    push(ans2);
    const char* temp2= "3rd";
    char ans3[5];
    strcpy(ans3,temp2);
    push(ans3);
    pop();
    if(strcmp(top(),"2nd")!=0){exit(1);}
    printf("2 passed successfully !\n");
    const char* temp3= "4th";
    char ans4[5];
    strcpy(ans4,temp3);
    push(ans4);
    const char* temp4= "5th";
    char ans5[5];
    strcpy(ans5,temp4);
    push(ans5);
    if(strcmp(top(),"5th")!=0){exit(1);}
    printf("All test passed successfully !");
    return 0;
}
