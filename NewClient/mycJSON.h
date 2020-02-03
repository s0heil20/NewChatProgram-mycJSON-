typedef struct json{
    char valuestring[10000];
    int arr_flag;
    char* Obj_Ptr;
} cJSON;
void String_Insert(char modifier,char main_string[],char sub_string[]);
cJSON* cJSON_Parse(char string[]);
cJSON* cJSON_CreateObject();
cJSON* cJSON_CreateString(char string[]);
cJSON* cJSON_CreateArray();
void cJSON_AddItemToObject(cJSON* obj,char string[],cJSON* adding);
void cJSON_AddItemToArray(cJSON* arr, cJSON* adding);
char* cJSON_Print(cJSON* Obj);
cJSON* cJSON_GetObjectItem(cJSON* obj,char* Item_name);
int cJSON_GetArraySize(cJSON* arr);
cJSON* cJSON_GetArrayItem(cJSON* arr,int Index);

