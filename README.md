# Linux 系统调用实验

## 任务内容

添加一个系统调用，写一个用户态进程调用该系统调用。该系统调用获取以下内容：

[√]	1. 当前进程的所有已分配的虚拟内存对应的物理内存页框号

[√]	2. 当前进程的内核栈地址

[  ]	3. 当前进程打开文件占用的磁盘块总数，以及每个文件占用的磁盘块号

[  ]	4. 以上所有信息输出到 `proc` 文件系统 [可选]

## 文件目录

``` 目录
2021summer-system_group-lab_systemcall
├── README.md
├── call.c					//JIN层调用程序
├── output					//输出截图 `[实验序号]\ output.png`
│   ├── ··· ···
│   ├── 2\ output.png
│   └── test\ output.png
└── patchs
    ├── sys.c.patch(syscall).patch		//sys.c([实验序号]).patch
    ├── ··· ···
    ├── syscalls.h.patch
    └── unistd.h.patch
```

## 实验平台

`Linux-4.19.20` + `ubuntu`

## FROM

胡家骏
