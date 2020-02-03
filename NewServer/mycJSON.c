//MycJSON by Soheil Mahdizadeh------S.N.:98106083----------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//--cJSON STRUCT!---
typedef struct json{
    char valuestring[10000];
    int arr_flag;
    char* Obj_Ptr;
} cJSON;
//------------------
#define MAX_2 sizeof(cJSON)
//-----FUNCTIONS----
void String_Insert(char modifier,char main_string[],char sub_string[]){

    char *newstring = (char*)malloc(MAX_2);
    int x;
    for(int i = 0;main_string[i] != '\0';i++){
        if(main_string[i] == '*' && main_string[i+1] == modifier ){
          x = i;
          break;
        }
    }
    strncpy(newstring, main_string, x);
    newstring[x] = '\0';
    strcat(newstring,sub_string);
    strcat(newstring,main_string+x);
    free(sub_string);
    strcpy(main_string, newstring);
    free(newstring);
    return;
}
cJSON* cJSON_Parse(char string[]){
    cJSON* newjson = malloc(MAX_2);
    strcpy(newjson->valuestring, string);
    return newjson;
}
cJSON* cJSON_CreateObject(){
    cJSON* newobject = malloc(MAX_2);
    newobject->valuestring[0]='{';
    newobject->valuestring[1]='*';
    newobject->valuestring[2]='}';
    newobject->valuestring[3]='\0';
    return newobject;
}
cJSON* cJSON_CreateString(char string[]){
    cJSON* newstring = malloc(MAX_2);
    sprintf(newstring->valuestring,"\"%s\"", string);
    return newstring;
}
cJSON* cJSON_CreateArray(){
    cJSON* newarr = malloc(MAX_2);
    newarr->arr_flag = 1;
    newarr->valuestring[0] = '[';
    newarr->valuestring[1] = '*';
    newarr->valuestring[2] = ']';
    newarr->valuestring[3] = '\0';
    return newarr;
}
void cJSON_AddItemToObject(cJSON* obj,char string[],cJSON* adding){
    char *adding_string = (char*)malloc(MAX_2);
    sprintf(adding_string,"\"%s\":%s", string,adding->valuestring);
    String_Insert('}', obj->valuestring, adding_string);
    if(adding->arr_flag == 1){
        adding->Obj_Ptr = (char*)malloc(MAX_2);
        adding->Obj_Ptr = obj->valuestring;
    }
    return;
}
void cJSON_AddItemToArray(cJSON* arr, cJSON* adding){
    String_Insert(']', arr->Obj_Ptr , adding->valuestring);
    return;
}
char* cJSON_Print(cJSON* Obj){
    for(int i = 0;Obj->valuestring[i] != '\0';i++){
        if(Obj->valuestring[i] == '*'){
            Obj->valuestring[i] = ' ';
        }
    }
    return Obj->valuestring;
}
cJSON* cJSON_GetObjectItem(cJSON* obj,char* Item_name){
    for(int i = 0;obj->valuestring[i] != '\0';i++){
        if(obj->valuestring[i] == '\"'){
            char temp[50];
            int h = 0, j = i + 1;
            for(;obj->valuestring[j] != '\"';){
                temp[h] = obj->valuestring[j];
                h++;
                j++;
            }
            temp[h] = '\0';
            if(strcmp(temp, Item_name) == 0){
                for(int z = j+1;obj->valuestring[z] != '\0';z++){
                    if(obj->valuestring[z] == '\"'){
                        int k = 0;
                        char returning[1000];
                        for(int y = z+1;obj->valuestring[y] != '\"';y++){

                            returning[k] = obj->valuestring[y];
                            k++;
                        }
                        returning[k] = '\0';
                        cJSON* newitem = malloc(MAX_2);
                        strcpy(newitem->valuestring, returning);
                        return newitem;
                    }
                    else if(obj->valuestring[z] == '['){
                        int k = 0;
                        char returning[1000];
                        for(int y = z+1;obj->valuestring[y] != ']';y++){

                            returning[k] = obj->valuestring[y];
                            k++;
                        }
                        returning[k] = '\0';
                        cJSON* newitem = malloc(MAX_2);
                        strcpy(newitem->valuestring, returning);
                        newitem->arr_flag = 1;
                        newitem->Obj_Ptr = obj->valuestring;
                        return newitem;
                    }
                }

            }
        }
    }
    return NULL;

}
int cJSON_GetArraySize(cJSON* arr){
    int number = 0;
    for(int i = 0;arr->Obj_Ptr[i] != '\0';i++){
        if(arr->Obj_Ptr[i] == '['){
            for(int j = i+1;arr->Obj_Ptr[j] != ']';j++){
                if(arr->Obj_Ptr[j] == '{'){
                    number++;
                }
            }
        }
    }
    return number;
}
cJSON* cJSON_GetArrayItem(cJSON* arr,int Index){
    int number = 0;
    for(int i = 0;arr->Obj_Ptr[i] != '\0';i++){
        if(arr->Obj_Ptr[i] == '['){
            for(int j = i+1;arr->Obj_Ptr[j] != ']';j++){
                if(arr->Obj_Ptr[j] == '{'){
                    if(number == Index){
                        int h = j;
                        int z = 0;
                        char returning[1000];
                        for(;arr->Obj_Ptr[h] != '}';){
                            returning[z] = arr->Obj_Ptr[h];
                            z++;
                            h++;
                        }
                        returning[z] = '}';
                        returning[z+1] = '\0';
                        cJSON* cptr = malloc(MAX_2);
                        strcpy(cptr->valuestring, returning);
                        return cptr;
                    }
                    number++;
                }
            }

        }
    }
    return NULL;

}
