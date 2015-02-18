#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include "threads.h"
#include <malloc.h>

#define CAST(t,e) ((t)(e))
#define TYPED_MALLOC(t) CAST(t*, malloc(sizeof(t)))
#define STACK 131072

struct thread {
	unsigned int thread_num;
	int dispatched;
	jmp_buf env;
	uintptr_t *esp, *ebp;
	void CAST(*fs, void *);
	void *args;
	struct thread *next;
};

struct scheduler {
	unsigned int size;
	struct thread* last;
	struct thread* first;
	struct thread* next;
	struct thread* previous;	
};

struct scheduler* round_robin;
int thread_number = 0;
jmp_buf env;
uintptr_t sp, bp;

struct thread *thread_create(void (*f)(void *arg), void *arg) {
	uintptr_t *memptr;
	struct thread *process;
	if (!posix_memalign(CAST(void **, &memptr),8,STACK)) {
		process = TYPED_MALLOC(struct thread);
		process->ebp = memptr;
		process->esp = memptr + STACK;
		process->thread_num = ++thread_number;
		process->fs = f;
		process->args = arg;

	}
	else {
		process = NULL;
	}
	return process;
}

void thread_add_runqueue(struct thread *t) {
	if (round_robin == NULL) {
		round_robin = TYPED MALLOC(struct scheduler);
		round_robin->first = NULL;
	}
	insert(t);
}

void thread_yield() {

	printf("Thread yielding for thread: %d\n", round_robin->current->thread_num);
	if(setjmp(round_robin->current->env) == 0) {
		schedule();
		dispatch();
	}
}

void dispatch() {
	if (round_robin->thread_num != 1) {
		__asm__ volatile("mov %%rax, %%rsp" : : "a" (round_robin->previous->esp));
		__asm__ volatile("mov %%rax, %%rsp" : : "a" (round_robin->previous->ebp));
	}
	if (!round_robin->current->dispatched) {
		round_robin->current->dispatched = 1;
		printf("Thread %d has been changed to dispatched.\n", round_robin->current->thread_num);
		round_robin->current->fs(robin->current->args);
	}
	else {
		__asm__ volatile("mov %%rsp, %%rax" : "=a" (round_robin->current->esp) :); 
		__asm__ volatile("mov %%rbp, %%rax" : "=a" (round_robin->current->ebp) :);
		longjmp(robin->current->env,1);
	}
	thread_exit();
}


void schedule() {
	advance();
}

void thread_exit() {
	remove(round_robin->current);
	if (robin->size > 0)
		dispatch();
  	 __asm__ volatile("mov %%rsp, %%rax" : "=a" (sp) :);  
  	 __asm__ volatile("mov %%rbp, %%rax" : "=a" (bp) :);
  	 longjmp(env, 1);
}

void thread_start_threading() {
	if(setjmp(env) != 0)
		return;
	dispatch();
}

void insert (struct thread* t) {
	if(round_robin->first == NULL) {
		round_robin->start = t;
		t->next = t;
		round_robin->current = t;
		round_robin->previous = t;
		round_robin->last = t;
		round_robin->size = 1;
	}
	else {
		t->next = round_robin->current;
		round_robin->previous->next = t;
		round_robin->previous = t;
		round_robin->size++;
	}
}

void remove (struct thread* t) {
	struct thread* temp = robin->start;
	struct thread* dummy = check->next;
	unsigned int count = 0;

	while (dummy != t && count == 0) {
		temp = dummy;
		dummy = temp->next;
		if (temp == round_robin->start)
			count++;
	}
	if (count == 1) {
		return;
	}
	temp->next = dummy->next;
	round_robin->current = round_robin->current->next;
	round_robin->previous->next = round_robin->current;
	free(dummy);
	round_robin->size -= 1;
}

void advance() {
	round_robin->previous = round_robin->current;
	round_robin->previous->next = round_robin->current->next;
	printf("current thread is %d\n", round_robin->current->thread_num);
}

















