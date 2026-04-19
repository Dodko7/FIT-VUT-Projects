/**
 * dynamický string pro parametry funkcí
 * Vytvořil: Lukáš Šidlík (xsidlil00)
 */

#include "dymString.h"
int MAX_LEN = MAX_STRING_LENGTH;

bool DymString_Init(DymString *string){
    string->data = (char*)malloc(MAX_LEN);
    if(string->data == NULL)
        return false;
    
    string->max_length = MAX_LEN;
    string->data[0] = '\0';
    string->length = 0;
    return true;
}

bool DymString_Insert_Char(DymString *string, char insert){
    if(string->max_length == string->length + 1){
        int new_length = string->max_length * 2;
        string->data = (char*)realloc(string->data, new_length);
        if(string->data == NULL){
            return false;
        }
        
        string->max_length = new_length;
    }

    string->data[string->length] = insert;
    string->length++;
    string->data[string->length] = '\0';
    return true;
}


void DymString_Destroy(DymString *string){
    if(string->data != NULL){
        free(string->data);
    }
}
