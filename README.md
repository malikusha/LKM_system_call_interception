# LKM_system_call_interception
Loadable Kernel Module Programming

Hello, this is the project for system call interceptions that me and my partner Remy Kaldawy were working on.

Part 1
Source file for loading the module: opencall.c
How to compile and link: make
The command to insert the module: sudo insmod opencall.ko
To check if the intercepted call is loaded and working, open /var/log/syslog
Description: We intercepted three calls: close, open and read. The intercepted open call prints the message in the syslog, that indicates the user id, who opened the file and also indicates the path to the file opened.
For close call, the message printed indicates the user id and the file descriptor. Read call checks for the "VIRUS" string in the opened file. If it sees the string "VIRUS" in the file, it prints a warning message and the file descriptor of the file being scaned.
Implementation: To implement the read system call, we used the strstr in the kernel space to loop through all the contents of the file. If it contains virus we are printing the warning message.
To implement the open system call, we called the old version of open and then printed the message with id. We obtained id of the user by taking current_pid.val value.
To implement the close system call, we called the old version of close and printed the message with id and file descriptor.
The problem we faced when we were working on part 1, we override the read system call and because of that we could not open anything from the virtual machine. All the code was only on one computer without any back up. So, we had to retype all the source file on another computer. We could not use the VM since it crashed flawlessly, thus the checkpoint was submitted later. Professor Shue was notified and has given us extra time for submission.


Part 2
Executable file: proc_ancestry
Kernel module: test_intercept
How to compile and link: make
The command to insert the module: sudo insmod test_intercept.ko
To run the testcall: ./proc_Ancestry
To clean up the source and object directories: make clean

Description: The code for system call intercept is in the test_intercept.c source file.
In the user space we allocated the target_pid and the ancestry structure in the memory, then we passed them as parameters to a new_sys_cs3013_syscall2. Then, target_pid and ancestry structure are transferred into kernel space code through copy_from_user() function. We put them into the empty buffers. It returns error if either parameter was not allocated in user space. If it was allocated beforehand, we initialize three pointers. Each of them holds the pointer to the fields of the buffer structure that we are about to fill. Next, we initialize the task_struct variable to hold the structure of the process we are looking for(the macro that we used for that finds the task_struct based on the process id).
Next, we are searching for all the siblings of the process(all the children of process' parent). We are searching the process' children. Then, we are putting siblings and children we found to the structure. We check if the process is not init. If so, we are climbing up to parent and grandparent and so on and go on. We print all the parents we found. And lastly, we copy the structure we just filled into the user space.
