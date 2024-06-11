/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


#include <stdio.h> 
#include <string.h> 
#include <kv.h>
  
int size = 0; 
char keys[MAX_SIZE][50]; 
int values[MAX_SIZE]; 

int getIndex(char key[]) 
{ 
    for (int i = 0; i < size; i++) { 
        if (strcmp(keys[i], key) == 0) { 
            return i; 
        } 
    } 
    return -1;
} 
  
void insert(char key[], int value) 
{ 
    int index = getIndex(key); 
    if (index == -1) { 
        strcpy(keys[size], key); 
        values[size] = value; 
        size++; 
    } 
    else {
        values[index] = value; 
    } 
} 
  
int get(char key[]) 
{ 
    int index = getIndex(key); 
    if (index == -1) { 
        return -1; 
    } 
    else {  
        return values[index]; 
    } 
} 
