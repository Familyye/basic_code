#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/*
   头文件：
   #include <sys/types.h>
   #include <sys/stat.h>
   #include <fcntl.h>

   定义函数：
   int open(const char * pathname, int flags);
   int open(const char * pathname, int flags, mode_t mode);

   函数说明:
   参数 pathname 指向欲打开的文件路径字符串.
   下列是参数flags 所能使用的旗标:
   O_RDONLY 以只读方式打开文件
   O_WRONLY 以只写方式打开文件
   O_RDWR 以可读写方式打开文件. 上述三种旗标是互斥的, 也就是不可同时使用, 但可与下列的旗标利用OR(|)运算符组合.
   O_CREAT 若欲打开的文件不存在则自动建立该文件.
   O_EXCL 如果O_CREAT 也被设置, 此指令会去检查文件是否存在. 文件若不存在则建立该文件, 否则将导致打开文件错误. 此外, 若O_CREAT 与O_EXCL 同时设置, 并且欲打开的文件为符号连接, 则会打开文件失败.
   O_NOCTTY 如果欲打开的文件为终端机设备时, 则不会将该终端机当成进程控制终端机.
   O_TRUNC 若文件存在并且以可写的方式打开时, 此旗标会令文件长度清为0, 而原来存于该文件的资料也会消失.
   O_APPEND 当读写文件时会从文件尾开始移动, 也就是所写入的数据会以附加的方式加入到文件后面.
   O_NONBLOCK 以不可阻断的方式打开文件, 也就是无论有无数据读取或等待, 都会立即返回进程之中.
   O_NDELAY 同O_NONBLOCK.
   O_SYNC 以同步的方式打开文件.
   O_NOFOLLOW 如果参数pathname 所指的文件为一符号连接, 则会令打开文件失败.
   O_DIRECTORY 如果参数pathname 所指的文件并非为一目录, 则会令打开文件失败。注：此为Linux2. 2 以后特有的旗标, 以避免一些系统安全问题.

   参数 mode 则有下列数种组合, 只有在建立新文件时才会生效, 此外真正建文件时的权限会受到umask 值所影响, 因此该文件权限应该为 (mode-umaks).
   S_IRWXU00700 权限, 代表该文件所有者具有可读、可写及可执行的权限.
   S_IRUSR 或S_IREAD, 00400 权限, 代表该文件所有者具有可读取的权限.
   S_IWUSR 或S_IWRITE, 00200 权限, 代表该文件所有者具有可写入的权限.
   S_IXUSR 或S_IEXEC, 00100 权限, 代表该文件所有者具有可执行的权限.
   S_IRWXG 00070 权限, 代表该文件用户组具有可读、可写及可执行的权限.
   S_IRGRP 00040 权限, 代表该文件用户组具有可读的权限.
   S_IWGRP 00020 权限, 代表该文件用户组具有可写入的权限.
   S_IXGRP 00010 权限, 代表该文件用户组具有可执行的权限.
   S_IRWXO 00007 权限, 代表其他用户具有可读、可写及可执行的权限.
   S_IROTH 00004 权限, 代表其他用户具有可读的权限
   S_IWOTH 00002 权限, 代表其他用户具有可写入的权限.
   S_IXOTH 00001 权限, 代表其他用户具有可执行的权限.
   注: 此处 mode 与 int chmod(const char *path, mode_t mode); 中的 mode 完全一样

   返回值：若所有欲核查的权限都通过了检查则返回0 值, 表示成功, 只要有一个权限被禁止则返回-1.

   错误代码：
   EEXIST 参数pathname 所指的文件已存在, 却使用了O_CREAT 和O_EXCL 旗标.
   EACCESS 参数pathname 所指的文件不符合所要求测试的权限.
   EROFS 欲测试写入权限的文件存在于只读文件系统内.
   EFAULT 参数pathname 指针超出可存取内存空间.
   EINVAL 参数mode 不正确.
   ENAMETOOLONG 参数 pathname 太长.
   ENOTDIR 参数pathname 不是目录.
   ENOMEM 核心内存不足.
   ELOOP 参数pathname 有过多符号连接问题.
   EIO I/O 存取错误.

   附加说明：使用 access()作用户认证方面的判断要特别小心, 例如在access()后再作open()空文件可能会造成系统安全上的问题.
 **/

/*
  头文件：#include <unistd.h>

  定义函数：ssize_t read(int fd, void * buf, size_t count);

  函数说明：read()会把参数fd 所指的文件传送count 个字节到buf 指针所指的内存中. 若参数count 为0, 则read()不会有作用并返回0. 
  
  返回值为实际读取到的字节数, 如果返回0, 表示已到达文件尾或是无可读取的数据,此外文件读写位置会随读取到的字节移动.

  附加说明：
  如果顺利 read()会返回实际读到的字节数, 最好能将返回值与参数count 作比较, 若返回的字节数比要求读取的字节数少, 则有可能读到了文件尾、从管道(pipe)或终端机读、read()被信号中断了读取动作.

  当有错误发生时则返回-1, 错误代码存入errno 中, 而文件读写位置则无法预期.

  错误代码：
  EINTR 此调用被信号所中断.
  EAGAIN 当使用不可阻断I/O 时(O_NONBLOCK), 若无数据可读取则返回此值.
  EBADF 参数fd 非有效的文件描述词, 或该文件已关闭.
 **/

/*
   头文件：#include <unistd.h>

   定义函数：ssize_t write (int fd, const void * buf, size_t count);

   函数说明：write()会把参数buf 所指的内存写入count 个字节到参数fd 所指的文件内. 文件读写位置也会随之移动.

   返回值：如果顺利write()会返回实际写入的字节数. 当有错误发生时则返回-1, 错误代码存入errno 中.

   错误代码：
   EINTR 此调用被信号所中断.
   EAGAIN 当使用不可阻断I/O 时 (O_NONBLOCK), 若无数据可读取则返回此值.
   EADF 参数fd 非有效的文件描述词, 或该文件已关闭.
 **/

const char *file_path = "./temp";

void test_write()
{
  char buff[0x100] = {0};

  int fd = open(file_path, O_WRONLY | O_CREAT);
  if (fd == -1)
  {
    fprintf(stderr, "%s:%d error: %s\n", __FILE__, __LINE__, strerror(errno));
    return;
  }

  char *test_str = "This is a test string.";
  unsigned long len = write(fd, test_str, strlen(test_str));
  if (len == -1)
  {
    fprintf(stderr, "%s:%d error: %s\n", __FILE__, __LINE__, strerror(errno));
    return;
  }
  close(fd);
}

void test_read()
{
  char buff[0x100] = {0};

  int fd = open(file_path, O_RDONLY);
  if (fd == -1)
  {
    fprintf(stderr, "%s:%d error: %s\n", __FILE__, __LINE__, strerror(errno));
    return;
  }

  unsigned long len = read(fd, buff, sizeof(buff));
  if (len == -1)
  {
    fprintf(stderr, "%s:%d error: %s\n", __FILE__, __LINE__, strerror(errno));
  }
  close(fd);

  for (unsigned long i = 0; i < strlen(buff); i++)
  {
    printf("%c", buff[i]);
  }
  puts("");
}

int main(int argc, char *argv[])
{
  test_write();
  puts("==========================");
  test_read();
  return 0;
}
