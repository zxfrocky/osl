#ifndef __STDAFX_H__
#define __STDAFX_H__

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#ifdef WIN32
//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

//#define _WIN32_WINNT 0x0501
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <errno.h>
#include <Iphlpapi.h>


#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "Iphlpapi.lib" )
#ifndef __BASE_TYPE__
#define __BASE_TYPE__
typedef unsigned char bool_t;
typedef char int8_t;
typedef unsigned char uint8_t;
typedef unsigned char uchar_t;
typedef char char_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned __int64 uint64_t;
typedef short int16_t;
typedef int int32_t;
typedef __int64 int64_t;
#endif

#ifdef WIN64
#define	PSTRING	LPCWSTR
#else
#define	PSTRING	LPCSTR
#endif
#define ECONNRESET	WSAECONNRESET//对方关闭socket
#define ETIMEDOUT	WSAETIMEDOUT//连接超时

/* LIBRTMP */
#include <stddef.h>
#include <stdarg.h>
//#include <cstdarg>
#include <MMSystem.h>
#include <Windows.h>
#pragma comment( lib, "Winmm.lib" )

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC // 不加的话，就不知道是哪个文件哪行泄漏。但此中没有new，所以new还需要自己手工加
#include <crtdbg.h>

//#ifdef new
//#undef new
//#endif
//#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#else
/* LINUX */
#include<unistd.h> 
#ifndef __USE_GNU
#define __USE_GNU 
#endif
#include<sched.h> 
#include<ctype.h> 
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <inttypes.h>
#include <signal.h>
#include <fcntl.h>
#include <getopt.h>
#include <termios.h>
#include <errno.h>
#include <netdb.h>
#include <stdint.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>

#include <net/if.h> 
#include <net/if_arp.h> 

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/param.h> 
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/reboot.h>
#include <sys/mount.h>
#include <dlfcn.h>
#include <linux/reboot.h>
#include <linux/if_ether.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <linux/if_packet.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>	
#include <linux/prctl.h>
#define IS_MULTICAST_IP(ip) ( 0xE0<=(ip&0xFF) && (ip&0xFF)<0xF0 )	
typedef unsigned char bool_t;
typedef unsigned char uchar_t;
typedef char char_t;
//typedef int64_t SOCKET;
typedef int SOCKET;
#define INVALID_SOCKET -1
#endif

#ifndef __cplusplus
#ifndef bool
#define bool uint8_t
#endif	

#ifndef true
#define true 1
#endif	

#ifndef false
#define false 0
#endif 
#endif /* __cplusplus */

#if 0
#ifndef INT64_C    
#define INT64_C(c) (c ## LL)    
#define UINT64_C(c) (c ## ULL)    
#endif
#endif

#endif /* __STDAFX_H__ */
