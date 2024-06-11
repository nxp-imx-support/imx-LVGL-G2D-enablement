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
  
#define MAX_SIZE  100 
  
extern int size;  
extern char keys[MAX_SIZE][50]; 
extern int values[MAX_SIZE]; 

int getIndex(char key[]);
void insert(char key[], int value);
int get(char key[]);
