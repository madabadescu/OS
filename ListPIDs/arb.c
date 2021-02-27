#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

struct process_node {
    pid_t pid;
    pid_t ppid;
    int children_count;
    pid_t *children_pids;
    char nice;
    uid_t user_id;
};

void print_spaces(int level) {
    int i;
    for (i = 0; i < level * 4; i++) {
        printf(" ");
    }
}

void print(struct process_node *p, int level) {
    print_spaces(level);
    printf("pid: %d\n", p->pid);

    print_spaces(level);
    printf("ppid: %d\n", p->ppid);

    print_spaces(level);
    printf("children: %d\n", p->children_count);

    print_spaces(level);
    printf("nice: %d\n", (int)p->nice);

    print_spaces(level);
    printf("user_id: %d\n", (int)p->user_id);

    printf("\n");
}

void dfs(pid_t pid, pid_t ppid, int level) {
    //  Check if the pid is valid.
    int children_count = sys_call(332, pid);
    if (children_count < 0) return;

    //  Allocate memory for the new node
    struct process_node *p = (struct process_node*) malloc(sizeof(struct process_node));
    p->children_count = children_count;
    p->children_pids = (pid_t*) malloc(sizeof(pid_t) * children_count);
    p->pid = pid;
    p->ppid = ppid;

    //  Get process stats
    int verdict = syscall(333, pid, p);

    if (verdict < 0) {
        printf("Hmm, seems like this process does not exist anymore :(\n");
        return;
    }
    print(p, level);
    //  Walk the process tree
    int i = 0;
    for (i = 0; i < p->children_count; i++) {
        dfs(p->children_pids[i], pid, level + 1);
    }

    //  Deallocating heap memory
    free(p->children_pids);
    free(p);
}

int main(int argc, char **argv) {
    pid_t starting_pid = atoi(argv[1]);

    int checker = syscall(332, starting_pid);

    if (checker < 0) {
        printf("The given pid does not correspond to any process.\n");
    } else {
        printf("Starting DFS on the process tree...\n\n\n");
        dfs(starting_pid, -1, 0);
    }
    return 0;
}
