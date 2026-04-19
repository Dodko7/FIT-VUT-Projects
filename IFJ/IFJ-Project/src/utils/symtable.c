/** IFJ2024
 * xsidlil00; Lukáš Šidlík
 */

#include "symtable.h"
#include <stdbool.h>
#include <string.h>

/**
 * @brief Vypočítá hash pro řetězec pomocí algoritmu djb2.
 *
 * @param str řetězec, pro který chceme vypočítat hash
 * @return hash jako unsigned int
 */
unsigned int getHash(char *str)
{
    unsigned int hash = 5381;
    for (int i = 0; str[i] != '\0'; i++)
    {
        unsigned char c = (unsigned char)str[i];
        hash = ((hash << 5) + hash) + c;
    }
    return hash % SYMTABLE_SIZE;
}


bool symtable_init(symtable_t *symtable){
    if(symtable == NULL){
        // vnitřní chyba
        return false;
    }

    for (int i = 0; i < SYMTABLE_SIZE; i++)
    {
        (*symtable)[i] = NULL;
    }
    return true;
}

data_t *symtable_get_item(symtable_t *symtable, char *name){
    if(symtable == NULL || name == NULL){
        //vnitřní chyba
        return NULL;
    }

    unsigned int hash = getHash(name);
    symtable_item_t *item = (*symtable)[hash];
    while (item != NULL)
    {
        if (strcmp(item->key, name) == 0)
        {
            return &item->data;
        }
        item = item->next;
    }
    return NULL;
}

data_t *symtable_insert(symtable_t *symtable, char *name){
    if(symtable == NULL || name == NULL){
        //vnitřní chyba
        return NULL;
    }
    if(symtable_get_item(symtable, name) != NULL){
        //jmeno jiz existuje
        return NULL;
    }

    symtable_item_t *new_item = (symtable_item_t *)malloc(sizeof(symtable_item_t));
    if (new_item == NULL)
    {
        // vnitřní chyba
        return NULL;
    }

    new_item->data.parameters = (DymString *)malloc(sizeof(DymString));
    if (new_item->data.parameters == NULL)
    {
        // vnitřní chyba
        free(new_item);
        return NULL;
    }

    bool aloc = DymString_Init(new_item->data.parameters);
    if (!aloc)
    {
        // vnitřní chyba
        free(new_item->data.parameters);
        free(new_item);
        return NULL;
    }

    new_item->key = (char *)malloc((strlen(name) + 1) * sizeof(char));
    if (new_item->key == NULL)
    {
        // vnitřní chyba
        free(new_item->data.parameters);
        free(new_item);
        return NULL;
    }

    //new_item->key[strlen(name)+1] = '\0';
    strcpy(new_item->key, name);
    new_item->data.id = new_item->key;
    new_item->data.type = DATA_TYPE_UND;
    new_item->data.init = false;
    new_item->data.canNull = false;
    new_item->data.isConst = false;
    new_item->data.used = false;
    new_item->next = NULL;

    //printf("Inserted %s\n", new_item->data.id);
    unsigned int hash = getHash(name);

    if ((*symtable)[hash] == NULL)
    {
        (*symtable)[hash] = new_item;
    }
    else
    {
        symtable_item_t *prev = (*symtable)[hash];
        hash = (hash + 1) % SYMTABLE_SIZE;
        while ((*symtable)[hash] != NULL)
        {
            if (prev->next == NULL)
            {
                prev->next = (*symtable)[hash];
            }
            hash = (hash + 1) % SYMTABLE_SIZE;
            prev = prev->next;
        }
        (*symtable)[hash] = new_item;
        prev->next = new_item;
    }
    return &new_item->data;
}

bool symtable_insert_params(data_t *data, token_type type){
    if(data == NULL){
        return false;
    }

    bool result = false;
    if (type == tok_t_i32)
    {
        result = DymString_Insert_Char(data->parameters, 'i');
    }
    else if (type == tok_t_i32_opt)
    {
        result = DymString_Insert_Char(data->parameters, 'I');
    }
    else if (type == tok_t_f64)
    {
        result = DymString_Insert_Char(data->parameters, 'd');
    }
    else if (type == tok_t_f64_opt)
    {
        result = DymString_Insert_Char(data->parameters, 'D');
    }
    else if (type == tok_t_u8)
    {
        result = DymString_Insert_Char(data->parameters, 'u');
    }
    else if (type == tok_t_u8_opt)
    {
        result = DymString_Insert_Char(data->parameters, 'U');
    }
    else if (type == tok_t_str)
    {
        result = DymString_Insert_Char(data->parameters, 's');
    }
    else if (type == tok_t_bool)
    {
        result = DymString_Insert_Char(data->parameters, 'b');
    }
    else {
        result = DymString_Insert_Char(data->parameters, 'q'); // pro ifj.write, kde může být víc typů v 1 parametru
    }
    
    if(!result){
        //vnitřní chyba
        return false;
    }
    return true;
}

bool symtable_destroy(symtable_t *symtable){
    if(symtable == NULL){
        //vnitřní chyba
        return false;
    }

    symtable_item_t *item;
    for (int i = 0; i < SYMTABLE_SIZE; i++)
    {
        item = (*symtable)[i];
        if (item == NULL)
            continue;

        if (!item->data.used)
        {
            // chyba nepoužita promněná
        }

        free(item->key);
        if (item->data.parameters != NULL)
        {
            DymString_Destroy(item->data.parameters);
            free(item->data.parameters);
        }

        free(item);
        (*symtable)[i] = NULL;
    }
    return true;
}


bool symtable_insert_builtin(symtable_t *symtable){
    bool internal_err = true;
    data_t * data;

    //ifj.readstr() ?[]u8
    data = symtable_insert(symtable, "ifj.readstr");
    if(data == NULL)
        return false;

    data->canNull = true;
    data->type = DATA_TYPE_U8;
    data->used = true;
    data->init = true;
    
    //ifj.readi32() ?i32
    data = symtable_insert(symtable, "ifj.readi32");
    if(data == NULL)
        return false;

    data->canNull = true;
    data->type = DATA_TYPE_INT;
    data->used = true;
    data->init = true;

    //ifj.readf64() ?f64
    data = symtable_insert(symtable, "ifj.readf64");
    if(data == NULL)
        return false;

    data->canNull = true;
    data->type = DATA_TYPE_DOUBLE;
    data->used = true;
    data->init = true;

    //ifj.write(term) void
    data = symtable_insert(symtable, "ifj.write");
    if(data == NULL)
        return false;

    data->type = DATA_TYPE_VOID;
    internal_err = symtable_insert_params(data, tok_t_null);
    data->used = true;
    data->init = true;

    if(!internal_err)
        return false;

    //ifj.i2f(term: i32) f64
    data = symtable_insert(symtable, "ifj.i2f");
    if(data == NULL)
        return false;

    data->type = DATA_TYPE_DOUBLE;
    internal_err = symtable_insert_params(data, tok_t_i32);
    data->used = true;
    data->init = true;

    if(!internal_err)
        return false;

    //ifj.f2i(term: f64) i32
    data = symtable_insert(symtable, "ifj.f2i");
    if(data == NULL)
        return false;

    data->type = DATA_TYPE_INT;
    internal_err = symtable_insert_params(data, tok_t_f64);
    data->used = true;
    data->init = true;

    if(!internal_err)
        return false;

    //ifj.string(term) []u8
    data = symtable_insert(symtable, "ifj.string");
    if(data == NULL)
        return false;

    data->type = DATA_TYPE_U8;
    internal_err = symtable_insert_params(data, tok_t_str);
    data->used = true;
    data->init = true;

    if(!internal_err)
        return false;

    //ifj.length(s: []u8) i32
    data = symtable_insert(symtable, "ifj.length");
    if(data == NULL)
        return false;

    data->type = DATA_TYPE_INT;
    internal_err = symtable_insert_params(data, tok_t_u8);
    data->used = true;
    data->init = true;

    if(!internal_err)
        return false;

    //ifj.concat(s1: []u8, s2: []u8) []u8
    data = symtable_insert(symtable, "ifj.concat");
    if(data == NULL)
        return false;

    data->type = DATA_TYPE_U8;
    data->used = true;
    data->init = true;
    internal_err = symtable_insert_params(data, tok_t_u8);
    if(!internal_err)
        return false;

    internal_err = symtable_insert_params(data, tok_t_u8);
    if(!internal_err)
        return false;

    //ifj.substring(s: []u8, i: i32, j: i32) ?[]u8
    data = symtable_insert(symtable, "ifj.substring");
    if(data == NULL)
        return false;

    data->type = DATA_TYPE_U8;
    data->used = true;
    data->init = true;
    internal_err = symtable_insert_params(data, tok_t_u8);
    if(!internal_err)
        return false;

    internal_err = symtable_insert_params(data, tok_t_i32);
    if(!internal_err)
        return false;

    internal_err = symtable_insert_params(data, tok_t_i32);
    if(!internal_err)
        return false;

    data->canNull = true;

    //ifj.strcmp(s1: []u8, s2: []u8) i32
    data = symtable_insert(symtable, "ifj.strcmp");
    if(data == NULL)
        return false;

    data->type = DATA_TYPE_INT;
    data->used = true;
    data->init = true;
    internal_err = symtable_insert_params(data, tok_t_u8);
    if(!internal_err)
        return false;

    internal_err = symtable_insert_params(data, tok_t_u8);
    if(!internal_err)
        return false;

    //ifj.ord(s: []u8, i: i32) i32
    data = symtable_insert(symtable, "ifj.ord");
    if(data == NULL)
        return false;

    data->type = DATA_TYPE_INT;
    data->used = true;
    data->init = true;
    internal_err = symtable_insert_params(data, tok_t_u8);
    if(!internal_err)
        return false;

    internal_err = symtable_insert_params(data, tok_t_i32);
    if(!internal_err)
        return false;

    //ifj.chr(i: i32) []u8
    data = symtable_insert(symtable, "ifj.chr");
    if(data == NULL)
        return false;

    data->type = DATA_TYPE_U8;
    data->used = true;
    data->init = true;
    internal_err = symtable_insert_params(data, tok_t_u8);
    if(!internal_err)   
        return false;

    //@as(i32, temp : i32) i32
    data = symtable_insert(symtable, "@as");
    if(data == NULL)
        return false;

    data->type = DATA_TYPE_INT;
    data->used = true;
    data->init = true;
    internal_err = symtable_insert_params(data, tok_t_i32);
    if(!internal_err)
        return false;
        
    return true;
}
