#ifndef SUB_PROC_H
#define SUB_PROC_H

typedef char* (*task_function)(char *task_arg);
int subprocess_start(task_function task_fun);
char *execute_task_in_subprocess(char *task);
#endif
