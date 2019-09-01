#pragma once
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<errno.h>

void P(sem_t *sem);
void V(sem_t *sem);
void unix_error(char *msg);
