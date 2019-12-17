#include <stdio.h>
#include <unistd.h>
int chdir(const char *path){

}

int main(){
    char field[100];
    scanf("%s",&field);
    chdir(field);
}