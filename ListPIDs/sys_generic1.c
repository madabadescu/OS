int
sys_children_count(struct proc *p, void *v, register_t *retval) {
    struct sys_children_count_args *uap = v;
    pid_t pid = SCARG(uap, pid);

    struct process *current_process = prfind(pid);

    if (current_process == NULL) {
        *retval = -1;
    } else {
        *retval = 0;

        struct process *i;
        LIST_FOREACH(i, &current_process->ps_children, ps_sibling) {
            *retval += 1;
        }
    }

    return 0;
}


int
sys_store_process(struct proc *p, void *v, register_t *retval) {
    struct process_node {
        pid_t pid;
        pid_t ppid;
        int children_count;
        pid_t *children_pids;
        char nice;
        uid_t user_id;
    }

    //  Get the arguments
    struct sys_store_process_args *uap = v;
    pid_t pid = SCARG(uap, pid);
    struct process_node *user_process = (struct process_node*) SCARG(uap, process);
    struct process *kern_process = prfind(pid);

    //  Safe check
    if (kern_process == NULL) {
        *retval = -1;
        return 0;
    }

    //  Get children pids
    int i = 0;
    struct process *p_it;
    LIST_FOREACH(p_it, &kern_process->ps_children, ps_sibling) {
        if (i < user_process->children_count) {
            user_process->children_pids[i] = p_it->ps_pid;
            i += 1;
        }
    }

    //  Get process state
    user_process->nice = kern_process->ps_nice;
    user_process->user_id = kern_process->ps_ucred->cr_uid;

    *retval = 0;
    return 0;
}
