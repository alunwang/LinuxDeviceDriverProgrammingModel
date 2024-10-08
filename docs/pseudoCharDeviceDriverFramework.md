# 伪字符设备驱动程序基本框架

在本教程中，我们将指导你完成在Linux中创建伪字符设备驱动程序的基本框架。

## 设置项目结构

导航至自定义驱动程序文件夹，新建一个名为 “srcs”的文件夹。在该文件夹中，创建一个名为 “pseudoCharDeviceDriver.c”的源文件，我们将在此文件实现驱动程序代码。此外创建其头文件“pseudoCharDeviceDriver.h”和Makefile

该驱动程序目录结构请参见如下。
```
$ tree srcs/
srcs/
├── Makefile
├── pseudoCharDeviceDriver.c
└── pseudoCharDeviceDriver.h
```

## 实现驱动程序基本框架
使用文本编辑器打开 “pseudoCharDeviceDriver.c ”文件，并按照以下说明操作：

- 首先加入必要的头文件如：#include <linux/module.h>。该头文件对每个模块都至关重要。
- 创建模块初始化函数。可以从static int __init开始，给它取名为伪字符设备驱动程序初始化函数pcd_init。和模块卸载去初始化函数static void __exit pcd_exit()。
- 然后需要使用宏module_init和module_exit将模块初始化函数pcd_init和清理函数pcd_cleanup注册到内核系统里
- 添加驱动模块license，作者以及模块描述等信息。
```
#include <linux/module.h>
#include <linux/init.h>
#include "pseudoCharDeviceDriver.h"

static int __init pcd_init(void)
{
    return 0;
}

static void __exit pcd_cleanup(void)
{}

module_init(pcd_init);
module_exit(pcd_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alan Wang，alan.wang@gmail.com");
MODULE_DESCRIPTION("A driver for a pseudo char device.");
```

当用驱动程序模块加载命令```insmod pseudoCharDeviceDriver.ko```将该驱动程序模块时，module_init先被执行，模块初始化函数pcd_init()（清理函数pcd_cleanup90也同样）被注册到内核里，然后它被内核调用，执行初始化工作。

而当用驱动程序模块卸载命令```rmmod pseudoCharDeviceDriver```卸载驱动时，清理函数 pcd_cleanup()被内核调用，执行清理工作。

目前，还没有开始初始化工作，所以函数pcd_init()/pcd_cleanup()均还是空的。

## 关于宏__init和__exit的说明
__init、__exit和__initdata等都是Linux内核常用的宏，定义在linux/init.h中，用以告知编译器：使用这些宏修饰的变量或函数，要放置到特定的相应区域，这些区域定义在Linux内核链接脚本文件vmlinux.lds当中。

一般，C语言使用static声明的变量或函数，执行后会常驻于内存中，以便其它程序再次调用。对于像模块初始化函数pcd_init等，它仅仅在该模块加载时，内核系统调用它一次执行模块初始化，此后就不会再被调用，因此它是白占内存。所以必须使用宏__init声明，让它在执行一次后就立即释放回收，以节约内存。

__init宏的目的：
- __init是一个C宏，可扩展为编译器指令，指示编译器将该宏修饰的函数代码段放入Linux内核最终ELF 映像的.init.text段中。
- 内核在启动过程中执行初始化函数后，会从内存中释放.init部分。
- 它主要用于静态模块。
- 无法卸载的内置（built-in）驱动程序不需要在内存中保留对其初始化函数的引用。

__exit宏的目的：
- 对于内置（built-in）模块，不需要清理函数，因为它不能被卸载。
- 当__exit与清理函数一起使用时，内核编译系统就会在编译过程中排除这些函数。
- __exit作为一个标记，使联编系统从最终的内核映像中排除清理函数。

## 编写一个Makefile来负责伪字符设备驱动程序模块的编译
使用如下一个简单的Makefile编译内核驱动模块pseudoCharDeviceDriver.ko。
```
obj-m += pseudoCharDeviceDriver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```
obj-m表示编译生成可加载模块。相应地obj-y表示模块直接编译进链接内核映像里。

all和clean是Makefile文件中的伪目标，伪目标并不是一个真正的编译目标，它代表着一系列你想要执行的命令集合，通常一个makefile会对应多个操作。这些伪目标可作参数传递给make命令，执行它代表的命令集合，比如：
```
make all
```
就执行all伪目标代表的所有操作。当make后不带参数时，默认执行第一个伪目标的操作。

make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

标准的make指令是这样的：make -C $KDIR M=$PWD [target]，下面分别介绍每个字段的含义。

-C选项：此选项指定内核源码的位置，make在编译时将会进入内核源码目录，执行编译，编译完成时返回。

$KDIR：/lib/modules/$(shell uname -r)/build/，指定内核源码的位置。

直接在目标板上编译时，内核头文件默认存放在/lib/modules/$(shell uname -r)/build/中，这个build/目录是一个软连接，链接到源码头文件的安装位置。而内核真正的源码库则直接引用正在运行的内核镜像。

M=$(PWD)：需要编译的模块源文件地址

[target]：modules，事实上，这是个可选选项。默认行为是将源文件编译并生成内核模块，即module(s)，但是它还支持一下选项：
- modules_install:安装这个外部模块，默认安装地址是/lib/modules/$(uname -r)/extra/，同时可以由内建变量INSTALL_MOD_PATH指定安装目录
- clean:卸载源文件目录下编译过程生成的文件，在上文的makefile最后一行可以看到。
- help：帮助信息

## 编译
```
$ make
make -C /lib/modules/5.15.153.1-microsoft-standard-WSL2/build M=/home/alan/WorkSpace/LinuxDeviceDriverProgrammingModel/srcs modules
make[1]: Entering directory '/home/alan/WSL2-Linux-Kernel-linux-msft-wsl-5.15.153.1'
  CC [M]  /home/alan/WorkSpace/LinuxDeviceDriverProgrammingModel/srcs/pseudoCharDeviceDriver.o
  MODPOST /home/alan/WorkSpace/LinuxDeviceDriverProgrammingModel/srcs/Module.symvers
  CC [M]  /home/alan/WorkSpace/LinuxDeviceDriverProgrammingModel/srcs/pseudoCharDeviceDriver.mod.o
  LD [M]  /home/alan/WorkSpace/LinuxDeviceDriverProgrammingModel/srcs/pseudoCharDeviceDriver.ko
make[1]: Leaving directory '/home/alan/WSL2-Linux-Kernel-linux-msft-wsl-5.15.153.1'
```
成功编译后可察看生成的驱动程序模块：
```
$ ls
Makefile        pseudoCharDeviceDriver.c   pseudoCharDeviceDriver.mod    pseudoCharDeviceDriver.o
Module.symvers  pseudoCharDeviceDriver.h   pseudoCharDeviceDriver.mod.c
modules.order   pseudoCharDeviceDriver.ko  pseudoCharDeviceDriver.mod.o
```
会生成许多中间文件，我们只关注是否有pseudoCharDeviceDriver.ko。

## 加载驱动
我们可以加载该驱动程序模块：
```
$ sudo insmod pseudoCharDeviceDriver.ko
...
$ lsmod
Module                  Size  Used by
pseudoCharDeviceDriver    16384  0
```
表明该驱动程序模块已经成功被加载进内核系统里，但仅此而已，因为在该驱动程序里什么都还没有做。

## 卸载驱动
```
$ sudo rmmod pseudoCharDeviceDriver
...
$ lsmod
Module                  Size  Used by

```

[注册字符设备驱动](./charDeviceDriverRegister.md)