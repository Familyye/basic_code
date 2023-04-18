# 使用dlopen、dlsym、dlclose运行时装载动态库

一个可执行程序可能与多个动态库有关联，通常是在程序运行时将必要的动态库装载入进程实体(内存中);

另外一种，则是使用dlopen/dlsym/dlclose来动态地将动态库装载到当前进程实体中.

加载动态链接库，首先为共享库分配物理内存，然后在进程对应的页表项中建立虚拟页和物理页面之间的映射。

你可以认为系统中存在一种引用计数机制， 每当一个进程加载了共享库（在该进程的页表中进行一次映射），引用计数加一；

一个进程显式卸载（通过dlclose等）共享库或进程退出时，引用计数减一,当减少到0时，系统卸载共享库。

(1)打开动态链接库：dlopen，函数原型void *dlopen (const char *filename, int flag); dlopen用于打开指定名字(filename)的动态链接库，并返回操作句柄。

(2)取函数执行地址：dlsym，函数原型为: void *dlsym(void *handle, char *symbol); dlsym根据动态链接库操作句柄(handle)与符号(symbol)，返回符号对应的函数的执行代码地址。当handle == NULL 时，获取进程已经加载的系统模块里的函数，在x86-64 上是 libc-2.31.so 这个模块里的函数.

(3)关闭动态链接库：dlclose，函数原型为: int dlclose (void *handle); dlclose用于关闭指定句柄的动态链接库，只有当此动态链接库的使用计数为0时,才会真正被系统卸载。

(4)动态库错误函数：dlerror，函数原型为: const char *dlerror(void); 当动态链接库操作函数执行失败时，dlerror可以返回出错信息，返回值为NULL时表示操作函数执行成功。

通常的三段式就是：先打开（dlopen），然后获得需要的函数（dlsym），然后调用函数，最后关闭（dlclose）


## 函数介绍
函数: dlopen
功能：打开一个动态链接库 
包含头文件： #include <dlfcn.h> 
函数定义： void * dlopen(const char * pathname, int mode); 
函数描述： 在dlopen() 函数以指定模式打开指定的动态连接库文件，并返回一个句柄给调用进程。使用dlclose() 来卸载打开的库。 
mode：
　　RTLD_LAZY 暂缓决定，等有需要时再解出符号
　　RTLD_NOW 立即决定，返回前解除所有未决定的符号。 
　　RTLD_LOCAL
　　RTLD_GLOBAL 允许导出符号
　　RTLD_GROUP
　　RTLD_WORLD
返回值: 
　　打开错误返回NULL 
　　成功，返回库引用 
编译时候要加入 -ldl (指定dl库) 
　　例如 
　　gcc test.c -o test -ldl

　　dlopen()是一个强大的库函数。该函数将打开一个新库，并把它装入内存。该函数主要用来加载库中的符号，这些符号在编译的时候是不知道的。一个配置文件控制了加载模块的过程。这种机制使得在系统中添加或者删除一个模块时，都不需要重新编译了。dlopen()函数的返回值是一个句柄，然后后面的函数就通过使用这个句柄来做进一步的操作。如果打开失败dlopen()就返回一个NULL。如果一个函数库被多次打开，它会返回同样的句柄。如果一个函数库里面有一个输出的函数名字为_init,那么_init就会在dlopen()这个函数返回前被执行。我们可以利用这个函数在我的函数库里面做一些初始化的工作。
　　可以在自己的程序中使用 dlopen()。dlopen()在dlfcn.h中定义，并在dl库中实现。它需要两个参数：一个文件名和一个标志。文件名可以是我们学习过的库中的soname。标志指明是否立刻计算库的依赖性。如果设置为RTLD_NOW的话，则立刻计算；如果设置的是RTLD_LAZY，则在需要的时候才计算。另外，可以指定RTLD_GLOBAL，它使得那些在以后才加载的库可以获得其中的符号。

　　当库被装入后，可以把dlopen()返回的句柄作为给dlsym()的第一个参数，以获得符号在库中的地址。使用这个地址，就可以获得库中特定函数的指针，并且调用装载库中的相应函数。


函数: dlsym
dlsym()的函数原型是 void* dlsym(void* handle,const char* symbol) 该函数在<dlfcn.h>文件中.
handle是由dlopen打开动态链接库后返回的指针，symbol就是要求获取的函数的名称，函数返回值是void*,指向函数的地址，供调用使用

取动态对象地址：
#include <dlfcn.h>
void *dlsym(void *pHandle, char *symbol);
dlsym根据动态链接库操作句柄(pHandle)与符号(symbol),返回符号对应的地址。
使用这个函数不但可以获取函数地址，也可以获取变量地址。
比如，假设在so中定义了一个void mytest()函数，那在使用so时先声明一个函数指针：void(*pMytest)(),然后使用dlsym函数将函数指针pMytest指向mytest函数,
pMytest = (void (*)())dlsym(pHandle, "mytest");


函数: dlclose
dlclose（） 包含头文件： #include <dlfcn.h> 
函数原型为: int dlclose (void *handle); 
函数描述： dlclose用于关闭指定句柄的动态链接库，只有当此动态链接库的使用计数为0时,才会真正被系统卸载。


函数: dlerror
dlerror（） 包含头文件： #include <dlfcn.h> 
函数原型: const char *dlerror(void); 
函数描述： 当动态链接库操作函数执行失败时，dlerror可以返回出错信息，返回值为NULL时表示操作函数执行成功。


注:如果你的程序中使用dlopen、dlsym、dlclose、dlerror 显示加载动态库，gcc编译时需要设置链接选项 -ldl
如 gcc main.c -o main -ldl