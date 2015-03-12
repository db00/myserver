/**
 *
 gcc -Wall h.c -lm -o ~/a && ~/a
 gcc -Wall h.c -lm -lwsock32 && a
 gcc -Wall dict.c h.c -lm && ./a.out
 gcc -Wall dict.c h.c -liconv -lwsock32 && a
 gcc -Wall h.c sqlite3.c -lpthread -ldl && ./a.out &&
 gcc -Wall h.c sqlite3.c -lwsock32 && a

 zip -r doormen.zip src/ bat/ cert/ *.txt *.c *.xml *.bat
 adb push p.zip /sdcard/p.zip

git checkout
git push origin master
git commit --amend -m "More changes - now correct"
git pull  https://github.com/db00/myserver.git
git add --all
git push -u origin master
git clean -f

git clone https://github.com/db00/myserver.git
ssh-keygen -t rsa -C "db0@qq.com"  
ssh -T git@github.com

git config --global user.email "db0@qq.com"
git config --global user.name "db00"
git config --global push.default "matching"
git config --global color.status auto
git config --global color.branch auto
git config --list
git push origin  git@github.com:db00/myserver.git
git remote
git checkout
git branch -a
git commit -a -m "Will create merge conflict 1"
 * @author: db0@qq.com
 * @version 1.0.1
 * @date 2013-08-27
 */

//#include "dict.h"
#include <math.h>
#ifndef linux
#ifndef WIN32
#define linux
#endif
#endif
#define DEBUG
#ifdef linux
#define DEFAULTPORT 8809
#else
#define DEFAULTPORT 80
#endif
#define DEFAULTBACK 2
//#include "sqlite3.h"//如不需数据支持,注释此行,否则不注释此行,windows下加-lwsock32,linux加-lpthread编译参数,并加sqlite3.c
#ifdef WIN32
#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
extern unsigned int getpid();
extern size_t getpagesize(void);
# define MSG_WAITALL (1 << 3)
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/wait.h>
#endif

#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

#ifdef linux
char WWW_ROOT[]="/";//根目录地址,绝对地址,不设定时为当前目录
char DEFAULT_PAGE[]="/";//="/index.html";//默认主页,相对根目录的地址:如/index.html
#else
char WWW_ROOT[]=".";//根目录地址,绝对地址,不设定时为当前目录
char DEFAULT_PAGE[]="";//默认主页,相对根目录的地址:如/index.html
/*char DEFAULT_PAGE[]="/index.html";//默认主页,相对根目录的地址:如/index.html*/
#endif

int sock_fd;
#ifdef _SQLITE3_H_
sqlite3 *db = NULL;
char *db_name="test.db";
char * sql_result_str=NULL;
#endif

const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="; 
const unsigned char hexchars[] = "0123456789ABCDEF";
char * base64_encode(const char* data, int data_len); //需要free
char * base64_decode(const char* data, int data_len); //需要free
int url_decode(char *str, int len);//不需free
char *url_encode(char const *s, int len, int *new_length,int not_encode_url_split);//需要free

char *base64_encode(const char* data, int data_len) 
{
	//int data_len = strlen(data); 
	int prepare = 0; 
	int ret_len; 
	int temp = 0; 
	char *ret = NULL; 
	char *f = NULL; 
	int tmp = 0; 
	char changed[4]; 
	int i = 0; 
	ret_len = data_len / 3; 
	temp = data_len % 3; 
	if (temp > 0)
	{
		ret_len += 1; 
	}
	ret_len = ret_len*4 + 1; 
	ret = (char *)malloc(ret_len); 

	if ( ret == NULL) 
	{ 
		printf("No enough memory.\n"); 
		/*exit(0); */
	} 
	memset(ret, 0, ret_len); 
	f = ret; 
	while (tmp < data_len) 
	{ 
		temp = 0; 
		prepare = 0; 
		memset(changed, '\0', 4); 
		while (temp < 3) 
		{ 
			//printf("tmp = %d\n", tmp); 
			if (tmp >= data_len) 
			{ 
				break; 
			} 
			prepare = ((prepare << 8) | (data[tmp] & 0xFF)); 
			tmp++; 
			temp++; 
		} 
		prepare = (prepare<<((3-temp)*8)); 
		//printf("before for : temp = %d, prepare = %d\n", temp, prepare); 
		for (i = 0; i < 4 ;i++ ) 
		{
			if (temp < i)
			{
				changed[i] = 0x40;
			}
			else
			{
				changed[i] = (prepare>>((3-i)*6)) & 0x3F;
			}
			*f =(char)base[(int)changed[i]];
			//printf("%.2X", changed[i]); 
			f++; 
		} 
	} 
	*f = '\0'; 
	return ret; 
}

static char find_pos(char ch)   
{ 
	char *ptr = (char*)strrchr(base, ch);//the last position (the only) in base[] 
	return (ptr - base); 
} 
char *base64_decode(const char *data, int data_len) 
{ 
	int ret_len = (data_len / 4) * 3; 
	int equal_count = 0; 
	char *ret = NULL; 
	char *f = NULL; 
	int tmp = 0; 
	int temp = 0; 
	char need[3]; 
	int prepare = 0; 
	int i = 0; 
	if (*(data + data_len - 1) == '=') 
	{ 
		equal_count += 1; 
	} 
	if (*(data + data_len - 2) == '=') 
	{ 
		equal_count += 1; 
	} 
	if (*(data + data_len - 3) == '=') 
	{//seems impossible 
		equal_count += 1; 
	} 
	switch (equal_count) 
	{ 
		case 0: 
			ret_len += 4;//3 + 1 [1 for NULL] 
			break; 
		case 1: 
			ret_len += 4;//Ceil((6*3)/8)+1 
			break; 
		case 2: 
			ret_len += 3;//Ceil((6*2)/8)+1 
			break; 
		case 3: 
			ret_len += 2;//Ceil((6*1)/8)+1 
			break; 
	} 
	ret = (char *)malloc(ret_len); 
	if (ret == NULL) 
	{ 
		printf("No enough memory.\n"); 
		/*exit(0); */
	} 
	memset(ret, 0, ret_len); 
	f = ret; 
	while (tmp < (data_len - equal_count)) 
	{ 
		temp = 0; 
		prepare = 0; 
		memset(need, 0, 4); 
		while (temp < 4) 
		{ 
			if (tmp >= (data_len - equal_count)) 
			{ 
				break; 
			} 
			prepare = (prepare << 6) | (find_pos(data[tmp])); 
			temp++; 
			tmp++; 
		} 
		prepare = prepare << ((4-temp) * 6); 
		for (i=0; i<3 ;i++ ) 
		{ 
			if (i == temp) 
			{ 
				break; 
			} 
			*f = (char)((prepare>>((2-i)*8)) & 0xFF); 
			f++; 
		} 
	} 
	*f = '\0'; 
	return ret; 
}


#ifdef _SQLITE3_H_
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	int len=0;
	len = strlen(sql_result_str);

	if(len == 0)
		sprintf(sql_result_str+len,"{");
	else
		sprintf(sql_result_str+len,",{");

	for(i=0; i<argc; i++){
		/*printf("%s = %s\n", azColName[i], argv[i]?argv[i]:"NULL");*/
		len = strlen(sql_result_str);
		if(i==0)
			sprintf(sql_result_str+len,"\"%s\":\"%s\"",azColName[i],argv[i]?argv[i]:"");
		else
			sprintf(sql_result_str+len,",\"%s\":\"%s\"",azColName[i],argv[i]?argv[i]:"");
	}
	len = strlen(sql_result_str);
	sprintf(sql_result_str+len,"}\n");

	/*printf("\n");*/
	return 0;
}

int opendb()
{
	int rc = sqlite3_open(db_name, &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		strcpy(sql_result_str,"can not open database\n");
		sqlite3_close(db);
		db = NULL;
		return 1;
	}
	return 0;
}

int runsql(const char * sql)
{
	if(db == NULL)
		opendb();
	if(db == NULL)
		return 2;
	if(sql_result_str!=NULL)
	{
		free(sql_result_str);
		sql_result_str = NULL;
	}
	sql_result_str = malloc(0x100000);
	memset(sql_result_str,0,0x100000);
	int len = strlen(sql_result_str);
	char *zErrMsg = 0;
	int rc = sqlite3_exec(db, sql ,callback, 0, &zErrMsg);
	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sprintf(sql_result_str+len,"sql error:%s",zErrMsg);
		sqlite3_free(zErrMsg);
		return 1;
	}else{
		len = strlen(sql_result_str);
		sprintf(sql_result_str+len,"0");
	}
	/*printf("\nsql_result_str:%s",sql_result_str);*/
	return 0;
}
#endif

char* getvalue(char*head,const char*meta)
{
	char *pos;
	int len;
	char *curpos = strstr(head,meta);
	if(curpos == NULL)return NULL;
	curpos += strlen(meta);
	while(*curpos == ' ' || *curpos == '=' )curpos ++;
	len = strstr(curpos,"\r\n") - curpos;
	pos = malloc(len+1);
	if(pos == NULL)return NULL;
	memset(pos,0,len + 1);
	memcpy(pos,curpos,len);
	return pos;
}
char* getpath(char*head,const char*meta)
{
	char *pos;
	int len;
	char *curpos = strstr(head,meta);
	if(curpos == NULL)return NULL;
	curpos += strlen(meta);
	while(*curpos == ' ' || *curpos == ':' )curpos ++;
	len = strstr(curpos," ") - curpos;
	pos = malloc(len+1);
	if(pos == NULL)return NULL;
	memset(pos,0,len + 1);
	memcpy(pos,curpos,len);
	return pos;
}

char* getmetavalue(char*head,const char*meta)
{
	char *pos;
	int len;
	char *curpos = strstr(head,meta);
	if(curpos == NULL)return NULL;
	curpos += strlen(meta);
	while(*curpos == ' ' || *curpos == ':' )curpos ++;
	len = strstr(curpos,"\r\n") - curpos;
	pos = malloc(len+1);
	if(pos == NULL)return NULL;
	memset(pos,0,len + 1);
	memcpy(pos,curpos,len);
	return pos;
}

int getparas(char* paras,char**pos, int *len)
{
	if(paras==NULL)return 1;
	char *paras_end = strstr(paras," ");
	int paras_size;
	if(paras_end){
		paras_size = paras_end - paras;
		paras[paras_size] ='\0';
	}else{
		paras_size = strlen(paras);
		paras_end = paras + paras_size;
	}
	/*return 1;*/
	pos[*len] = strtok(paras,"&");
#ifdef DEBUG
	printf(" ----len:%d\n",strlen(paras));
#endif
	while(pos[(*len)] && pos[*len] < paras_end){
#ifdef DEBUG
		if(strlen(pos[*len]) < 100){
			printf("paras[%d]:%s\n",*len,pos[*len]);
		}else{
			printf("paras[%d]:(%d)",*len,strlen(pos[*len]));
			char *c = pos[*len];
			while(*c != ':' && *c != '='){
				printf("%c",*c);
				++c;
			}
			printf("\n");
		}
#endif
		*len += 1;
		pos[(*len)] = strtok(NULL,"&");
	}
	return 0;
}


static int htoi(char *s)
{
	int value;
	int c;

	c = ((unsigned char *)s)[0];
	if (isupper(c))
		c = tolower(c);
	value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;
	c = ((unsigned char *)s)[1];
	if (isupper(c))
		c = tolower(c);
	value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
	return (value);
}

char *url_encode(char const *s, int len, int *new_length,int not_encode_url_split)
{
	register unsigned char c;
	unsigned char *to, *start;
	unsigned char const *from, *end;
	from = (unsigned char *)s;
	end  = (unsigned char *)s + len;
	start = to = (unsigned char *) calloc(1, 3*len+1);
	if(start == NULL)return NULL;
	while (from < end)
	{
		c = *from++;
		if (c == ' ') {
			*to++ = '+';
		}else if(c == '/' && not_encode_url_split!=0){
			*to++ = '/';
		}
		else if ((c < '0' && c != '-' && c != '.') ||
				(c < 'A' && c > '9') ||
				(c > 'Z' && c < 'a' && c != '_') ||
				(c > 'z'))
		{
			to[0] = '%';
			to[1] = hexchars[c >> 4];
			to[2] = hexchars[c & 15];
			to += 3;
		}
		else
		{
			*to++ = c;
		}
	}
	*to = 0;
	if (new_length)
	{
		*new_length = to - start;
	}
	return (char *) start;
}

int url_decode(char *str, int len)
{
	if(str!=NULL && len>0){}else{return 0;}
	char *dest = str;
	char *data = str;

	if(len>0)
		while (len--)
		{
			if (*data == '+')
			{
				*dest = ' ';
			}
			else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2)))
			{
				*dest = (char) htoi(data + 1);
				data += 2;
				len -= 2;
			}
			else
			{
				*dest = *data;
			}
			data++;
			dest++;
		}
	if(dest && data)*dest = '\0';
	return dest - str;
}
/*
   int call_fun(char*fun,char*paras[],int len)
   {
   if(fun==NULL)return 1;
   typedef int (*intFunc)(char*,int); //此类型的函数指针指向的是无参、无返回值的函数。
   intFunc intArray[32]; //定义一个函数指针数组，其每个成员为INTFUN类型的函数指针
   intArray[10] = url_decode; //为其赋值
   intArray[10](NULL,0); //调用函数指针数组的第11个成员指向的函数
   return 0;
   }
   */

char * filehead_type[][2]=
{
	{"\x43\x57\x53",".swf"},
	{"\x46\x57\x53",".swf"},
	{"\x49\x54\x53\x46",".chm"},
	{"\x50\x4b\x03\x04\x14",".apk"},
	{"\x46\x4c\x56",".flv"},
	{"\x48\x38",".wma"},
	{"\x76\x68",".mp3"},
	{"\x80\x75",".docx"},
	{"\x49\x49\x2a",".tif"},
	{"\x2e\x52\x4d\x46",".rm"},
	{"\x6d\x6f\x6f\x76",".mov"},
	{"\x57\x41\x56\x45",".wav"},
	{"\x38\x42\x50\x53",".psd"},
	{"\x2e\x72\x61\xfd",".ram"},
	{"\xe3\x82\x85\x96",".pwl"},
	{"\xFF\x57\x50\x43",".wpd"},
	{"\x7b\x5c\x72\x74\x66",".rtf"},
	{"\x21\x42\x44\x4E",".pst"},
	{"\xAC\x9E\xBD\x8F",".qdf"},
	{"\x30\x36\xb2\x75\x8e\x66\xcf\x11",".asf"},
	{"\xCF\xAD\x12\xFE\xC5\xFD\x74\x6F",".dbx"},
	{"\x44\x65\x6C\x69\x76\x65\x72\x79\x2D\x64",".eml"},
	{"\x53\x74\x61\x6E\x64\x61\x72\x64\x20\x4A",".mdb"},
	{"\xff\xd8\xff\xfe",".jpeg"},
	{"\xff\xd8\xff\xe0",".jpeg"},
	{"\x47\x49\x46\x38\x39\x61",".gif"},
	{"\x47\x49\x46\x38\x37\x61",".gif"},
	{"\x42\x4d",".bmp"},
	{"\x4d\x5a",".exe"},
	{"\x50\x4b\x03\x04",".zip"},
	{"\x3a\x42\x61\x73\x65",".cnt"},
	{"\xd0\xcf\x11\xe0\xa1\xb1\x1a\xe1",".doc"},
	{"\x3f\x5f\x03",".gid"},
	{"\x1f\x8b\x08",".gz"},
	{"\x28\x54\x68\x69\x73\x20\x66\x69\x6c\x65",".hqx"},
	{"\x25\x50\x44\x46",".pdf"},
	{"\x52\x45\x47\x45\x44\x49\x54\x34",".reg"},
	{"\x7b\x5c\x72\x74\x66",".rtf"},
	{"\x0a\x05\x01\x08",".pcx"},
	{"\x25\x21\x50\x53",".eps"},
	{"\x21\x12",".ain"},
	{"\x1a\x02",".arc"},
	{"\x1a\x03",".arc"},
	{"\x1a\x04",".arc"},
	{"\x1a\x08",".arc"},
	{"\x1a\x09",".arc"},
	{"\x60\xea",".arj"},
	{"\x41\x56\x49\x20",".avi"},
	{"\x42\x5a\x68",".bz"},
	{"\x49\x53\x63\x28",".cab"},
	{"\x4c\x01",".obj"},
	{"\x30\x37\x30\x37\x30\x37",".tar"},
	{"\x43\x52\x55\x53\x48",".cru"},
	{"\x3a\xde\x68\xb1",".dcx"},
	{"\x1f\x8b",".gz"},
	{"\x91\x33\x48\x46",".hap"},
	{"\x3c\x68\x74\x6d\x6c\x3e",".htm"},
	{"\x3c\x48\x54\x4d\x4c\x3e",".htm"},
	{"\x3c\x21\x44\x4f\x43\x54",".htm"},
	{"\x5f\x27\xa8\x89",".jar"},
	{"\x2d\x6c\x68\x35\x2d",".lha"},
	{"\x5b\x76\x65\x72\x5d",".ami"},
	{"\x4d\x47\x58\x20\x69\x74\x70\x64",".ds4"},
	{"\x4d\x53\x43\x46",".cab"},
	{"\x4d\x54\x68\x64",".mid"},
	{"\x09\x04\x06",".xls"},
	{"\x7f\xfe\x34\x0a",".doc"},
	{"\x12\x34\x56\x78\x90\xff",".doc"},
	{"\x31\xbe",".doc"},
	{"\x7e\x42\x4b",".psp"},
	{"\x50\x4b\x03\x04",".zip"},
	{"\x89\x50\x4e\x47\x0d\x0a",".png"},
	{"\x6d\x64\x61\x74",".mov"},
	{"\x6d\x64\x61\x74",".qt"},
	{"\x52\x61\x72\x21",".rar"},
	{"\x2e\x72\x61\xfd",".ra"},
	{"\xed\xab\xee\xdb",".rpm"},
	{"\x2e\x73\x6e\x64",".au"},
	{"\x53\x49\x54\x21",".sit"},
	{"\x53\x74\x75\x66\x66\x49\x74",".sit"},
	{"\x1f\x9d",".z"},
	{"\x49\x49\x2a",".tif"},
	{"\x4d\x4d\x2a",".tif"},
	{"\x55\x46\x41",".ufa"},
	{"\x57\x41\x56\x45\x66\x6d\x74",".wav"},
	{"\xd7\xcd\xc6\x9a",".wmf"},
	{"\x50\x4b\x03\x04\x0a\x00",".ipa"},
	{"\x50\x4b\x30\x30\x50\x4b\x03\x04",".zip"},
	{"\xff\x57\x50\x47",".wpg"},
	{"\xff\x57\x50\x43",".wp"},
	{"\x3c\x21\x45\x4e\x54\x49\x54\x59",".dtd"},
	{"\x5a\x4f\x4f\x20",".zoo"},
	{0,0}
};
char*type_by_head(const char*buffer)
{
	int i=0;
	while(1){
		char *s = filehead_type[i][0];
		if(s){
			int t = strlen(s);
			if(t < strlen(buffer))
				t = strlen(buffer);
			if(memcmp(buffer,s,t)==0)
			{
				printf("\n filehead_type:%s",filehead_type[i][1]);
				return filehead_type[i][1];
			}
		}else{
			break;
		}
		++i;
	}
	return NULL;
}

struct client_item
{
	int fd;//sock fd
	char *recved_data;//recieved data
	int recved_len;//recieved data Length

	char *head;//head area point
	char *host;//host area point
	char *data;//data area point
	char *path;//path area point

	int data_len;//data area Length
	char *paras[0x100];//parameters
	int num_paras;//number of parameters
}clients[DEFAULTBACK];

int close_sock(int new_fd)
{
#ifdef __WIN32
	if(new_fd >0)closesocket(new_fd);
#else
	if(new_fd >0)close(new_fd);
#endif
	int i = 0;
	struct client_item*client;
	while(i<DEFAULTBACK){
		client = &clients[i];
		if(new_fd==client->fd){
			if(clients->head)free(client->head);
			if(clients->host)free(client->host);
			if(clients->path)free(client->path);
			if(clients->data)free(client->data);
			if(clients->recved_data)free(client->recved_data);

			client->fd=0;
			client->recved_data=0;
			client->recved_len=0;

			client->path=0;
			client->head=0;
			client->host=0;
			client->data=0;

			while(client->num_paras>0){
				free(client->paras[--client->num_paras]);
			}
			client->data_len=0;
			break;
		}
		++i;
	}
	return 0;
}


int vspf(char*buffer,char *fmt, ...)
{
	va_list argptr;
	int cnt;
	va_start(argptr, fmt);
	cnt = vsprintf(buffer, fmt, argptr);
	va_end(argptr);
	return(cnt);
}

char*append_str(char*p,const char*fmt,...)
{
	int n, size = getpagesize();
	int old_len=0;
	va_list ap;
	if(p==NULL){
		if ((p = (char *) malloc(size)) == NULL)
			return NULL;
	}else{
		old_len = strlen(p);
		size = ceil((float)(old_len+1)/getpagesize())*getpagesize(); 
	}
	while (1) {
		va_start(ap, fmt);
		n = vsnprintf (p+old_len, size-old_len, fmt, ap);
		va_end(ap);
		if (n > -1 && n+old_len < size)
			return p;
		size += getpagesize();
		if ((p = (char *)realloc(p+old_len, size)) == NULL)
			return NULL;
	}
}

int send_str(int sock,const char *fmt, ...) 
{
	int n, size = getpagesize();
	char *p;
	va_list ap;
	if ((p = (char *) malloc(size)) == NULL)
		return -1;
	while (1) {
		/* 尝试在申请的空间中进行打印操作 */
		va_start(ap, fmt);
		n = vsnprintf (p, size, fmt, ap);
		va_end(ap);
		/* 如果vsnprintf调用成功，返回该字符串 */
		if (n > -1 && n < size){
			break;
		}
		/* vsnprintf调用失败(n<0)，或者p的空间不足够容纳size大小的字符串(n>=size)，尝试申请更大的空间*/
		size += getpagesize(); /* 再申请一个内存页 */
		if ((p = (char *)realloc(p, size)) == NULL)
			return -1;
	}
	int len = strlen(p);
	int sended_len = 0;
	while(sended_len < len){
		int r = send(sock,p,len-sended_len,0);
		if(r<0)return -2;
		sended_len += r;
	}
	//printf(p);
	if(p)free(p);
	return sended_len;
}

/**
 * 调用系统命令，并获取输出（相当于使用system）
 *
 * @param input 要调用的系统命令  
 * @param output 调用命令后系统的输出  
 * @param maxlen 输出字符串的最大长度  
 *
 * @return  
 */
int mysystem(char *input, char *output, int maxlen)
{  
	if( NULL==input || NULL==output )  
		return -1;  
	int reslen;  
	FILE *f;  
	memset(output, 0, maxlen);  
	//创建管道，并将input里的内容写入管道  
	f = popen(input, "r");  
	//从管理中读出数据，并写入output数组  
	reslen = fread(output, 1, maxlen, f);  
	pclose(f);
	return reslen;  
}  

int data2file(const char *filename,const char *data,unsigned int *len)
{
	FILE *file=fopen(filename,"ab");
	if(file == NULL){
		return 1;
	}
	int ret = fwrite(data,1,*len,file);
	if(ret <0){
		printf("write Error!\n");
		return 2;
	}
	/*ret=lseek(handle,0L,SEEK_END);*/
	ret= ftell(file);
	printf("write file len:%d\n",ret);
	fflush(file);
	fclose(file);
	return 0;
}

char* types[][2] =
{
	{".tif","image/tiff"},
	{".001","application/x-001"},
	{".301","application/x-301"},
	{".323","text/h323"},
	{".906","application/x-906"},
	{".907","drawing/907"},
	{".a11","application/x-a11"},
	{".acp","audio/x-mei-aac"},
	{".ai","application/postscript"},
	{".aif","audio/aiff"},
	{".aifc","audio/aiff"},
	{".aiff","audio/aiff"},
	{".anv","application/x-anv"},
	{".asa","text/asa"},
	{".asf","video/x-ms-asf"},
	{".asp","text/asp"},
	{".asx","video/x-ms-asf"},
	{".au","audio/basic"},
	{".avi","video/avi"},
	{".awf","application/vnd.adobe.workflow"},
	{".biz","text/xml"},
	{".bmp","application/x-bmp"},
	{".bot","application/x-bot"},
	{".c4t","application/x-c4t"},
	{".c90","application/x-c90"},
	{".cal","application/x-cals"},
	{".cat","application/vnd.ms-pki.seccat"},
	{".cdf","application/x-netcdf"},
	{".cdr","application/x-cdr"},
	{".cel","application/x-cel"},
	{".cer","application/x-x509-ca-cert"},
	{".cg4","application/x-g4"},
	{".cgm","application/x-cgm"},
	{".cit","application/x-cit"},
	{".class","java/*"},
	{".cml","text/xml"},
	{".cmp","application/x-cmp"},
	{".cmx","application/x-cmx"},
	{".cot","application/x-cot"},
	{".crl","application/pkix-crl"},
	{".crt","application/x-x509-ca-cert"},
	{".csi","application/x-csi"},
	{".css","text/css"},
	{".cut","application/x-cut"},
	{".dbf","application/x-dbf"},
	{".dbm","application/x-dbm"},
	{".dbx","application/x-dbx"},
	{".dcd","text/xml"},
	{".dcx","application/x-dcx"},
	{".der","application/x-x509-ca-cert"},
	{".dgn","application/x-dgn"},
	{".dib","application/x-dib"},
	{".dll","application/x-msdownload"},
	{".doc","application/msword"},
	{".dot","application/msword"},
	{".drw","application/x-drw"},
	{".dtd","text/xml"},
	{".dwf","Model/vnd.dwf"},
	{".dwf","application/x-dwf"},
	{".dwg","application/x-dwg"},
	{".dxb","application/x-dxb"},
	{".dxf","application/x-dxf"},
	{".edn","application/vnd.adobe.edn"},
	{".emf","application/x-emf"},
	{".eml","message/rfc822"},
	{".ent","text/xml"},
	{".epi","application/x-epi"},
	{".eps","application/x-ps"},
	{".eps","application/postscript"},
	{".etd","application/x-ebx"},
	{".exe","application/x-msdownload"},
	{".fax","image/fax"},
	{".fdf","application/vnd.fdf"},
	{".fif","application/fractals"},
	{".fo","text/xml"},
	{".frm","application/x-frm"},
	{".g4","application/x-g4"},
	{".gbr","application/x-gbr"},
	{".","application/x-"},
	{".gif","image/gif"},
	{".gl2","application/x-gl2"},
	{".gp4","application/x-gp4"},
	{".hgl","application/x-hgl"},
	{".hmr","application/x-hmr"},
	{".hpg","application/x-hpgl"},
	{".hpl","application/x-hpl"},
	{".hqx","application/mac-binhex40"},
	{".hrf","application/x-hrf"},
	{".hta","application/hta"},
	{".htc","text/x-component"},
	{".htm","text/html"},
	{".html","text/html"},
	{".htt","text/webviewhtml"},
	{".htx","text/html"},
	{".icb","application/x-icb"},
	{".ico","image/x-icon"},
	{".ico","application/x-ico"},
	{".iff","application/x-iff"},
	{".ig4","application/x-g4"},
	{".igs","application/x-igs"},
	{".iii","application/x-iphone"},
	{".img","application/x-img"},
	{".ins","application/x-internet-signup"},
	{".isp","application/x-internet-signup"},
	{".IVF","video/x-ivf"},
	{".java","java/*"},
	{".jfif","image/jpeg"},
	{".jpe","image/jpeg"},
	{".jpe","application/x-jpe"},
	{".jpeg","image/jpeg"},
	{".jpg","image/jpeg"},
	{".jpg","application/x-jpg"},
	{".js","application/x-javascript"},
	{".jsp","text/html"},
	{".la1","audio/x-liquid-file"},
	{".lar","application/x-laplayer-reg"},
	{".latex","application/x-latex"},
	{".lavs","audio/x-liquid-secure"},
	{".lbm","application/x-lbm"},
	{".lmsff","audio/x-la-lms"},
	{".ls","application/x-javascript"},
	{".ltr","application/x-ltr"},
	{".m1v","video/x-mpeg"},
	{".m2v","video/x-mpeg"},
	{".m3u","audio/mpegurl"},
	{".m4e","video/mpeg4"},
	{".mac","application/x-mac"},
	{".man","application/x-troff-man"},
	{".math","text/xml"},
	{".mdb","application/msaccess"},
	{".mdb","application/x-mdb"},
	{".mfp","application/x-shockwave-flash"},
	{".mht","message/rfc822"},
	{".mhtml","message/rfc822"},
	{".mi","application/x-mi"},
	{".mid","audio/mid"},
	{".midi","audio/mid"},
	{".mil","application/x-mil"},
	{".mml","text/xml"},
	{".mnd","audio/x-musicnet-download"},
	{".mns","audio/x-musicnet-stream"},
	{".mocha","application/x-javascript"},
	{".movie","video/x-sgi-movie"},
	{".mp1","audio/mp1"},
	{".mp2","audio/mp2"},
	{".mp2v","video/mpeg"},
	{".mp3","audio/mp3"},
	{".mp4","video/mpeg4"},
	{".mpa","video/x-mpg"},
	{".mpd","application/vnd.ms-project"},
	{".mpe","video/x-mpeg"},
	{".mpeg","video/mpg"},
	{".mpg","video/mpg"},
	{".mpga","audio/rn-mpeg"},
	{".mpp","application/vnd.ms-project"},
	{".mps","video/x-mpeg"},
	{".mpt","application/vnd.ms-project"},
	{".mpv","video/mpg"},
	{".mpv2","video/mpeg"},
	{".mpw","application/vnd.ms-project"},
	{".mpx","application/vnd.ms-project"},
	{".mtx","text/xml"},
	{".mxp","application/x-mmxp"},
	{".net","image/pnetvue"},
	{".nrf","application/x-nrf"},
	{".nws","message/rfc822"},
	{".odc","text/x-ms-odc"},
	{".out","application/x-out"},
	{".p10","application/pkcs10"},
	{".p12","application/x-pkcs12"},
	{".p7b","application/x-pkcs7-certificates"},
	{".p7c","application/pkcs7-mime"},
	{".p7m","application/pkcs7-mime"},
	{".p7r","application/x-pkcs7-certreqresp"},
	{".p7s","application/pkcs7-signature"},
	{".pc5","application/x-pc5"},
	{".pci","application/x-pci"},
	{".pcl","application/x-pcl"},
	{".pcx","application/x-pcx"},
	{".pdf","application/pdf"},
	{".pdf","application/pdf"},
	{".pdx","application/vnd.adobe.pdx"},
	{".pfx","application/x-pkcs12"},
	{".pgl","application/x-pgl"},
	{".pic","application/x-pic"},
	{".pko","application/vnd.ms-pki.pko"},
	{".pl","application/x-perl"},
	{".plg","text/html"},
	{".pls","audio/scpls"},
	{".plt","application/x-plt"},
	{".png","image/png"},
	{".png","application/x-png"},
	{".pot","application/vnd.ms-powerpoint"},
	{".ppa","application/vnd.ms-powerpoint"},
	{".ppm","application/x-ppm"},
	{".pps","application/vnd.ms-powerpoint"},
	{".ppt","application/vnd.ms-powerpoint"},
	{".ppt","application/x-ppt"},
	{".pr","application/x-pr"},
	{".prf","application/pics-rules"},
	{".prn","application/x-prn"},
	{".prt","application/x-prt"},
	{".ps","application/x-ps"},
	{".ps","application/postscript"},
	{".ptn","application/x-ptn"},
	{".pwz","application/vnd.ms-powerpoint"},
	{".r3t","text/vnd.rn-realtext3d"},
	{".ra","audio/vnd.rn-realaudio"},
	{".ram","audio/x-pn-realaudio"},
	{".ras","application/x-ras"},
	{".rat","application/rat-file"},
	{".rdf","text/xml"},
	{".rec","application/vnd.rn-recording"},
	{".red","application/x-red"},
	{".rgb","application/x-rgb"},
	{".rjs","application/vnd.rn-realsystem-rjs"},
	{".rjt","application/vnd.rn-realsystem-rjt"},
	{".rlc","application/x-rlc"},
	{".rle","application/x-rle"},
	{".rm","application/vnd.rn-realmedia"},
	{".rmf","application/vnd.adobe.rmf"},
	{".rmi","audio/mid"},
	{".rmj","application/vnd.rn-realsystem-rmj"},
	{".rmm","audio/x-pn-realaudio"},
	{".rmp","application/vnd.rn-rn_music_package"},
	{".rms","application/vnd.rn-realmedia-secure"},
	{".rmvb","application/vnd.rn-realmedia-vbr"},
	{".rmx","application/vnd.rn-realsystem-rmx"},
	{".rnx","application/vnd.rn-realplayer"},
	{".rp","image/vnd.rn-realpix"},
	{".rpm","audio/x-pn-realaudio-plugin"},
	{".rsml","application/vnd.rn-rsml"},
	{".rt","text/vnd.rn-realtext"},
	{".rtf","application/msword"},
	{".rtf","application/x-rtf"},
	{".rv","video/vnd.rn-realvideo"},
	{".sam","application/x-sam"},
	{".sat","application/x-sat"},
	{".sdp","application/sdp"},
	{".sdw","application/x-sdw"},
	{".sit","application/x-stuffit"},
	{".slb","application/x-slb"},
	{".sld","application/x-sld"},
	{".slk","drawing/x-slk"},
	{".smi","application/smil"},
	{".smil","application/smil"},
	{".smk","application/x-smk"},
	{".snd","audio/basic"},
	{".sol","text/plain"},
	{".sor","text/plain"},
	{".spc","application/x-pkcs7-certificates"},
	{".spl","application/futuresplash"},
	{".spp","text/xml"},
	{".ssm","application/streamingmedia"},
	{".sst","application/vnd.ms-pki.certstore"},
	{".stl","application/vnd.ms-pki.stl"},
	{".stm","text/html"},
	{".sty","application/x-sty"},
	{".svg","text/xml"},
	{".swf","application/x-shockwave-flash"},
	{".tdf","application/x-tdf"},
	{".tg4","application/x-tg4"},
	{".tga","application/x-tga"},
	{".tif","image/tiff"},
	{".tif","application/x-tif"},
	{".tiff","image/tiff"},
	{".tld","text/xml"},
	{".top","drawing/x-top"},
	{".torrent","application/x-bittorrent"},
	{".tsd","text/xml"},
	{".txt","text/plain"},
	{".uin","application/x-icq"},
	{".uls","text/iuls"},
	{".vcf","text/x-vcard"},
	{".vda","application/x-vda"},
	{".vdx","application/vnd.visio"},
	{".vml","text/xml"},
	{".vpg","application/x-vpeg005"},
	{".vsd","application/vnd.visio"},
	{".vsd","application/x-vsd"},
	{".vss","application/vnd.visio"},
	{".vst","application/vnd.visio"},
	{".vst","application/x-vst"},
	{".vsw","application/vnd.visio"},
	{".vsx","application/vnd.visio"},
	{".vtx","application/vnd.visio"},
	{".vxml","text/xml"},
	{".wav","audio/wav"},
	{".wax","audio/x-ms-wax"},
	{".wb1","application/x-wb1"},
	{".wb2","application/x-wb2"},
	{".wb3","application/x-wb3"},
	{".wbmp","image/vnd.wap.wbmp"},
	{".wiz","application/msword"},
	{".wk3","application/x-wk3"},
	{".wk4","application/x-wk4"},
	{".wkq","application/x-wkq"},
	{".wks","application/x-wks"},
	{".wm","video/x-ms-wm"},
	{".wma","audio/x-ms-wma"},
	{".wmd","application/x-ms-wmd"},
	{".wmf","application/x-wmf"},
	{".wml","text/vnd.wap.wml"},
	{".wmv","video/x-ms-wmv"},
	{".wmx","video/x-ms-wmx"},
	{".wmz","application/x-ms-wmz"},
	{".wp6","application/x-wp6"},
	{".wpd","application/x-wpd"},
	{".wpg","application/x-wpg"},
	{".wpl","application/vnd.ms-wpl"},
	{".wq1","application/x-wq1"},
	{".wr1","application/x-wr1"},
	{".wri","application/x-wri"},
	{".wrk","application/x-wrk"},
	{".ws","application/x-ws"},
	{".ws2","application/x-ws"},
	{".wsc","text/scriptlet"},
	{".wsdl","text/xml"},
	{".wvx","video/x-ms-wvx"},
	{".xdp","application/vnd.adobe.xdp"},
	{".xdr","text/xml"},
	{".xfd","application/vnd.adobe.xfd"},
	{".xfdf","application/vnd.adobe.xfdf"},
	{".xhtml","text/html"},
	{".xls","application/vnd.ms-excel"},
	{".xls","application/x-xls"},
	{".xlw","application/x-xlw"},
	{".xml","text/xml"},
	{".xpl","audio/scpls"},
	{".xq","text/xml"},
	{".xql","text/xml"},
	{".xquery","text/xml"},
	{".xsd","text/xml"},
	{".xsl","text/xml"},
	{".xslt","text/xml"},
	{".xwd","application/x-xwd"},
	{".x_b","application/x-x_b"},
	{".sis","application/vnd.symbian.install"},
	{".sisx","application/vnd.symbian.install"},
	{".x_t","application/x-x_t"},
	{".ipa","application/vnd.iphone"},
	{".apk","application/vnd.android.package-archive"},
	{".xap","application/x-silverlight-app"},
	{".*","application/octet-stream"},
	{0,0}
};
const char*type2mine(char*type)
{
	int i=0;
	while(types[i][0]){
		if(strcmp(type, types[i][0])==0)
			return types[i][1];
		++i;
	}
	return "application/*";
}

int send_file(char*realPath,int client_sock)
{
	int len,ret;
	FILE *fp =fopen(realPath,"rb");
	if(fp == NULL){
		return 1;
	}
	fseek(fp, 0L, SEEK_END);
	len = ftell(fp);

	rewind(fp);
	char *filebuf=malloc(0x100);
	fflush(stdout);
	if(filebuf){
		memset(filebuf,0, 0x100);
		ret =fread(filebuf, 1,0x100,fp);
		if(ret >0){
		}else{
			free(filebuf);
			filebuf=NULL;
			return 2;
		}
	}else{
		return 3;
	}


	send_str(client_sock,"Connection: keep-alive\r\nAccept-Ranges: bytes\r\nContent-Length: %d\r\n",len);
	char * type = (char*)type_by_head(filebuf);
	if(type == NULL){
		type=strrchr(realPath,'.');
		printf("other type:-------------%s\n",type);
	}
	if(type==NULL)type = "*";
	send_str(client_sock, "Content-Type: %s",type2mine(type));
	send_str(client_sock,"\r\n\r\n");

	rewind(fp);
	int send_once_size=0x100000;
	char *p = malloc(send_once_size);
	if(p){
		memset(p,0, send_once_size);
		int remain_size = len;
		int cur_size = send_once_size;
		if(remain_size<send_once_size)cur_size = remain_size;
		while((ret =fread(p, 1,cur_size,fp))>0){
			send(client_sock,p,ret,0);
			memset(p,0, ret);
			remain_size=len - ftell(fp);
			if(remain_size<send_once_size)cur_size = remain_size;
			if(remain_size<=0)break;
		}
		fclose(fp);
		if(p)free(p);
		p = NULL;
	}
	if(filebuf)free(filebuf);
	filebuf = NULL;
	return 0;
}

/*
   int read_dir(char*dir_path,struct dirent**file_arr,int* num_files)
   {
   DIR *dir;
   struct dirent *dirent;
   if(num_files)*num_files = 0;
   dir = opendir(dir_path);
   if(dir == 0){
   printf(strerror(errno));
   return -1;
   }
   static char reletivepath[0x400];//相对WWW_ROOT的路径
   while ((dirent = readdir(dir)) != 0) {
   file_arr[*num_files] = dirent;
   if(num_files)*num_files += 1;
   }
   file_arr[*num_files] = NULL;
   return 0;
   }
   */
int get_strs_len(const char *s,...)
{
	int len = strlen(s);
	va_list argptr;
	va_start(argptr, s);
	int arg_no=0;
	while(1){
		char *s1 = va_arg(argptr,char*);
		if(s1==NULL)break;
		if(s1 && strlen(s1)>0){
			len += strlen(s1);
		}
		++arg_no;
		//printf("\narg_no:%d,%s\n",arg_no,s1); fflush(stdout);
	}
	va_end(argptr);
	return len;
}
char * path_conact(char*cur_dir,...) 
{//return current directory's real path + the file name 
	int path_len = strlen(cur_dir);
	char* ret=(char*)malloc(path_len);
	memset(ret,0,path_len);
	char *p = cur_dir;
	if(cur_dir){
		while(*p=='/')p++;
		memcpy(ret,p,strlen(p));
	}
	if(strlen(ret)>0){//delete the "/" at end of the cur_dir;
		while(*(ret+strlen(ret)-1)=='/' || *(ret+strlen(ret)-1)=='\\')
			*(ret+strlen(ret)-1)='\0';
	}
	va_list argptr;
	va_start(argptr, cur_dir);
	while(1){
		char*file_name = va_arg(argptr,char*);
		if(file_name){
			if(strlen(ret)+strlen(file_name)>path_len) {
				path_len += strlen(file_name)+1;
				ret = (char*)realloc(ret,path_len);
				memset(ret+strlen(ret),0,path_len-strlen(ret));
			}
			if(strcmp(file_name,"..")==0){
				while(strlen(ret)>0 && *(ret+strlen(ret)-1)!='/')
					*(ret+strlen(ret)-1)='\0';
				while(strlen(ret)>0 && *(ret+strlen(ret)-1)=='/')
					*(ret+strlen(ret)-1)='\0';
			}else if(strcmp(file_name,".")==0){
			}else{
				while(*(ret+strlen(ret)-1)=='/' || *(ret+strlen(ret)-1)=='\\')
					*(ret+strlen(ret)-1)='\0';
				if(strlen(ret)>0)
					sprintf(ret+strlen(ret),"/");
				sprintf(ret+strlen(ret),"%s",file_name);
			}
		}else{
			break;
		}
	}
	va_end(argptr);
	return ret;
}

char * web2realpath(char*path)
{
	int path_len = get_strs_len(path,WWW_ROOT,NULL)+2;
	char* ret=(char*)malloc(path_len);
	memset(ret,0,path_len);
	sprintf(ret,WWW_ROOT);
	while(ret[strlen(ret)-1]=='/')
		*(ret+strlen(ret)-1)='\0';
	if(path!=NULL){
		if(*path!='/')
			sprintf(ret+strlen(ret),"/");
		sprintf(ret+strlen(ret),"%s",path);
	}
	return ret;
}

int send_dir(int client_sock,char*Path,char*host)
{
	struct stat info;
	DIR *dir;
	struct dirent *dirent;
	char * real_path = web2realpath(Path);
	printf("\n Path: %s ==> real_path:%s\n",Path,real_path);
	dir = opendir(real_path);
	free(real_path);
	send_str(client_sock,"Connection: close\r\n");
	send_str(client_sock,"Content-Type: text/html\r\n\r\n");
	send_str(client_sock, "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html\" charset=\"UTF-8\" /><title>");
	if(dir == 0){
		send_str(client_sock,strerror(errno));
		return 1;
	}

	send_str(client_sock,Path);
	send_str(client_sock, "</title></head><body>");


	char* reletivepath;//相对WWW_ROOT的路径
	while ((dirent = readdir(dir)) != 0) {
		reletivepath = path_conact(Path,dirent->d_name,NULL);
#ifdef linux
		send_str(client_sock, "<a href=\"http://%s/%s\">%s",host,reletivepath,dirent->d_name);
#else
		char*encoded_url = url_encode(reletivepath,strlen(reletivepath),NULL,1);
		send_str(client_sock, "<a href=\"http://%s/%s\">%s",host,encoded_url,dirent->d_name);
		free(encoded_url);
#endif
		memset(&info,0,sizeof(info));
		real_path = web2realpath(reletivepath);
		//printf("\n host:%s , reletivepath: %s, real_path:%s\n",host,reletivepath,real_path);
		free(reletivepath);
		if(stat(real_path, &info)!=-1) {//if the file is a directory , append a "/" ;
			if(S_ISDIR(info.st_mode)){
				send_str(client_sock,"/");
			}
		}
		free(real_path);
		send_str(client_sock, "</a>");
		send_str(client_sock, "<br/>");
	}
	send_str(client_sock, "</body></html>");
	return 0;
}


int send_default(int client_sock)
{
	char *html = 
		"<html lang=\"en\">"
		"<head>"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
		"<title>main</title>"
		"</script>"
		"</head>"
		"<body>"
		"<object classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\""
		"	id=\"myExample\" width=\"100%\" height=\"100%\""
		"	codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab\">"
		"	<param name=\"movie\" value=\"index.swf\" />"
		"	<param name=\"quality\" value=\"high\" />"
		"	<param name=\"allowScriptAccess\" value=\"always\"/>"
		"	<param name=\"flashvars\" value=\"\" />"
		"	<embed src=\"index.swf\" quality=\"high\" width=\"100%\" height=\"100%\" name=\"myExample\" align=\"middle\" play=\"true\" quality=\"high\" allowScriptAccess=\"always\" flashvars=\"\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\">"
		"	</embed>"
		"</object>"
		"</body>"
		"</html>"
		;
	send_str(client_sock,"Connection: close\r\n\r\n");
	send_str(client_sock,html);
	return 0;
}

#ifdef _SQLITE3_H_
int exec_sql(char*Path,int client_sock)
{
	char *sql=strstr(Path,":");
	char *sqlite=Path;
	if(sql!=NULL){
		sql=sql+1;
		if(strlen(sql)>0){
			if(strncmp("/sql:",sqlite,4)==0){//访问数据库
				runsql(sql);
				send_str(client_sock,"Connection: close\r\n\r\n");
				send_str(client_sock,sql_result_str);
				return 0;
			}
		}
	}
	return 1;
}
#endif
int send_crossdomain(int client_sock)
{
	char *xml = 
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
		"<cross-domain-policy>"
		"  <site-control permitted-cross-domain-policies=\"all\"/>"
		"  <allow-access-from domain=\"*\" secure=\"false\"/>"
		"  <allow-http-request-headers-from domain=\"*\" headers=\"*\"/>"
		"</cross-domain-policy>"
		;

	send_str(client_sock,"Content-Type: text/xml\r\n");
	send_str(client_sock,"Connection: keep-alive\r\nAccept-Ranges: bytes\r\nContent-Length: %d\r\n",strlen(xml));
	send_str(client_sock,"\r\n");
	send_str(client_sock,"%s",xml);
	return 0;
}

int send_exec_end(int client_sock,char**paras,int num_paras)
{
	send_str(client_sock,"Accept-Ranges: bytes\r\n");
#ifdef DEBUG
	printf("has paras========\n");
#endif
	int i=0;
	char *value[0x100];
	while(i<0x100){
		value[i]=NULL;
		++i;
	}
	i=0;
	while(i<num_paras)
	{
		url_decode(paras[i],strlen(paras[i]));
		char *tmp= strstr(paras[i],"=");
		if(tmp!= NULL){
			*tmp= '\0';
			tmp+=1;
			if(strcmp(paras[i],"f")==0){
				value[0] = tmp;
#ifdef DEBUG
				printf("function:%s\n",tmp);
#endif
			}else if(strncmp(paras[i],"p",1 )==0 && *(paras[i]+1)!=0 && atoi(paras[i]+1)>0){
				value[atoi(paras[i]+1)] = tmp;
				if(strlen(tmp)<100)printf("paras%d:%s\n",atoi(paras[i]+1),tmp);
				else printf("paras%d:(%d)\n",atoi(paras[i]+1),strlen(tmp));
			}
		}
		++i;
	}
	int ret;
	int handle;
	if(value[0]!=NULL && value[1] !=NULL){

		if(strcmp(value[0],"fwrite")==0 && value[2]!=NULL && value[3]!=NULL && value[4]!=NULL){//fwrite,file_name,opentype,data,data_byte_len
			FILE *file=fopen(value[1],value[2]);
			if(file == NULL){
				return 1;
			}
			if(atoi(value[3])<=0){
				return 3;
			}
			char *data = base64_decode(value[3],strlen(value[3]));
			ret = fwrite(data,1,atoi(value[4]),file);
			fflush(file);
			if(ret <0 || ret < atoi(value[4])){
				return 2;
			}
			/*ret=lseek(handle,0L,SEEK_END);*/
			ret= ftell(file);
			fclose(file);

			char b[16];
			memset(b,0,sizeof(b));
			vspf(b,"%d",ret);
			send_str(client_sock,"Content-Length: %d\r\n",strlen(b));
			send_str(client_sock,"Connection: close\r\n\r\n");
			send_str(client_sock,"%d",ret);
			/*send_str(client_sock,"\"}\n");*/
			if(data){
				free(data);
				data = NULL;
			}
		}else if(strcmp(value[0],"rename" )==0 && value[1]!=NULL && value[2]!=NULL){
#ifdef DEBUG
			printf("rename:\n");
#endif
			ret=unlink(value[2]);//override the file;
			ret=rename(value[1],value[2]);
			send_str(client_sock,"Connection: close\r\n\r\n");
			send_str(client_sock,"{\"rename\":%d}\n",ret);
		}else if(strcmp(value[0],"remove" )==0){
			send_str(client_sock,"Connection: close\r\n\r\n");
			send_str(client_sock,"{\"remove\":%d}\n",remove(value[1]));
		}else if(strcmp(value[0],"unlink" )==0){
			send_str(client_sock,"Connection: close\r\n\r\n");
			send_str(client_sock,"{\"unlink\":%d}\n",unlink(value[1]));
		}else if(strcmp(value[0],"filelength")==0){
			handle=open(value[1],O_RDONLY);
			if(handle<=0){
				ret = 0;
			}else{
				ret=lseek(handle,0L,SEEK_END);
				close(handle);
			}
			send_str(client_sock,"Connection: close\r\n\r\n");
			send_str(client_sock,"%d",ret);

#ifdef dict_h
		}else if(strcmp(value[0],"mean" )==0 && value[1]!=NULL){
			char *mean = getmeans(value[1]);
			if(mean){
				send_str(client_sock,"Content-Length: %d\r\n",strlen(mean));
				send_str(client_sock,"Connection: close\r\n\r\n");
				int sended_len = send_str(client_sock,mean);
				if(sended_len == strlen(mean))printf("========sended complete==========");
				else printf("========not send complete==========");
				free(mean);
			}else{
				mean = "not exist";
				send_str(client_sock,"Content-Length: %d\r\n",strlen(mean));
				send_str(client_sock,"Connection: close\r\n\r\n");
				send_str(client_sock,mean);
			}
			printf("%s,%d\n",mean,strlen(mean));
			fflush(stdout);
#endif
		}else if(strcmp(value[0],"system" )==0){
			char *output=(char*)malloc(0x100000);
			if(output){
				memset(output,0,0x100000);
				url_decode(value[1],strlen(value[1]));
				printf("%s\n",value[1]);
				ret=mysystem( value[1] ,output,0x100000);
				send_str(client_sock,"Content-Length: %d\r\n",ret);
				send_str(client_sock,"Connection: close\r\n\r\n");
				send_str(client_sock,output);
				free(output);
			}
#ifdef _SQLITE3_H_
		}else if(strcmp(value[0],"sql")==0){
			runsql(value[1]);
			send_str(client_sock,"Content-Length: %d\r\n",strlen(sql_result_str));
			send_str(client_sock,"Connection: close\r\n\r\n");
			send_str(client_sock, sql_result_str);
#endif
		}else{
			send_str(client_sock, "{\"c_result\":\"no such function\"}\r\n");
		}
		return 3;
	}
	return 0;
}

struct client_item*get_client_by_fd(int fd)
{
	struct client_item*client;
	int i = 0;
	while(i<DEFAULTBACK){
		client = &clients[i];
		if(fd==client->fd){
			break;
		}
		++i;
	}
	if(i==DEFAULTBACK)
		return NULL;
	return client;
}

void give_response (int client_sock)
{
	struct client_item*client = get_client_by_fd(client_sock);
	char *Path = client->path;
	char **paras= client->paras;
	char *host= client->host;
	int num_paras= client->num_paras;
	send_str(client_sock,"HTTP/1.1 200 OK\r\n");
	send_str(client_sock,"Server: %s%s",host,"\r\n");

	url_decode(Path,strlen(Path));

#ifdef _SQLITE3_H_
	if(!exec_sql(Path,client_sock))
		goto out;
#endif

	if(num_paras> 0){
		send_exec_end(client_sock,paras,num_paras);
		goto out;
		return;
	}else if(strcmp(Path,"/")==0){
		if(strlen(DEFAULT_PAGE)>1){
			strcpy(Path,DEFAULT_PAGE);
		}else{
			/*
			   send_default(client_sock);
			   goto out;
			   return;
			   */
		}

	} else if(strcmp(Path,"/crossdomain.xml")==0){
		send_crossdomain(client_sock);
		goto out;
		return;
	}

	struct stat info;
	int len;

	len = strlen(WWW_ROOT) + strlen(Path) + 1;
	char *realPath=NULL;
	realPath = malloc(len + 1);
	memset(realPath,0, len + 1);
	if(strcmp(WWW_ROOT,"/")==0){//linux abs path
		sprintf(realPath, "%s", Path);
	}else{
		sprintf(realPath, "%s", WWW_ROOT);
		while(((realPath[strlen(realPath)-1])=='\\'|| (realPath[strlen(realPath)-1])=='/') && strlen(realPath)>0)realPath[strlen(realPath)-1]='\0';
		sprintf(realPath+strlen(realPath), "%s", Path);
		while(((realPath[strlen(realPath)-1])=='\\'|| (realPath[strlen(realPath)-1])=='/') && strlen(realPath)>0)realPath[strlen(realPath)-1]='\0';
	}
	if(strlen(realPath)==0) sprintf(realPath, "%s", ".");
#ifdef DEBUG
	printf("\nrealPath:%s,path:%s\n",realPath,Path);
	fflush(stdout);
#endif


	if(strcmp(realPath,"/mnt/sdcard/tmpshot.bmp") == 0
			|| strcmp(realPath,"/home/libiao/tmpshot.bmp") == 0
			|| strcmp(realPath,"/tmpshot.bmp") == 0
			|| strcmp(realPath,"/tmpshot.png") == 0
			|| strcmp(realPath,"/mnt/sdcard/tmpshot.png" )==0)
	{
		system("screenshot /sdcard/tmpshot.png || screencap /sdcard/tmpshot.png || scrot /home/libiao/tmpshot.png");
	}
	//screencap /mnt/sdcard/tmpshot.png

	if (stat(realPath, &info) < 0) {//failed
		send_str(client_sock,"Connection: close\r\n\r\n");
		send_str(client_sock,"Error:%s Path:%s errno:%d\r\n",strerror(errno),Path,errno);
		goto out;
		return;
	}

	if (S_ISREG(info.st_mode)) {//file
		send_file(realPath,client_sock);
	} else if (S_ISDIR(info.st_mode)) {//dir
		printf("isdir");
		send_dir(client_sock,Path,host);
	} else {//other forbid to access
		send_str(client_sock,"Connection: close\r\n");
		send_str(client_sock,"Conten-Type: text/html\r\n");
		send_str(client_sock,"\r\n");
		send_str(client_sock,"%s forbid!",Path);
	}
out:
	if(realPath) {
		free(realPath);
		realPath = NULL;
	}
}

void exitfun()  
{  
	if(sock_fd>0)close_sock(sock_fd);
	sock_fd = -1;

#ifdef _SQLITE3_H_
	sqlite3_close(db);
#endif
}

int open_with_browser()
{
	char *indexpage=(char*)malloc(0xff);
	if(indexpage){
		memset(indexpage,0,0xff);
#ifdef WIN32
		sprintf(indexpage,"cmd /c start \"\" \"http://127.0.0.1:%d\"",DEFAULTPORT);
		system(indexpage);
#endif
		//am start -a android.intent.action.VIEW -d  http://www.google.cn/
		//#else
#ifdef linux
		//xdg-open
		// xdg-open 'http://www.freedesktop.org/'
		// w3m 'http://www.freedesktop.org/'
		sprintf(indexpage,"xdg-open 'http://127.0.0.1:%d' &",DEFAULTPORT);
		system(indexpage);
#ifdef __ANDROID_API__
		sprintf(indexpage,"am start -a android.intent.action.VIEW -d http://127.0.0.1:%d ",DEFAULTPORT);
		system(indexpage);
#endif
#endif
		free(indexpage);
	}
	return 0;
}
#ifdef linux
void sig_handler(int sig)
{
	int s=0;
	pid_t i;
	switch(sig){
		case SIGCHLD:
			i= wait(&s);
			printf("pid:%d SIGCHLD stats:%d!\n",i,s);
			break;
		case SIGINT:
			printf("ctrl+c has been keydownd\n");
			exitfun();
			printf("pid:%d exit!\n",getpid());
			exit(0);
			break;
	}
}
void signal_ex(int signo, void* func)
{
	struct sigaction act, old_act;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask); //清空此信号集
	act.sa_flags = 0;
	if (sigaction(signo, &act, &old_act) < 0)
	{
		printf("sig err!\n");
	}
	sigaction(signo, &old_act, NULL); //恢复成原始状态
	return;
}
#endif
char* memstr(char* full_data, int full_data_len, char* substr)
{
	if (full_data == NULL || full_data_len <= 0 || substr == NULL) {
		return NULL;
	}

	if (*substr == '\0') {
		return NULL;
	}

	int sublen = strlen(substr);

	int i;
	char* cur = full_data;
	int last_possible = full_data_len - sublen + 1;
	for (i = 0; i < last_possible; i++) {
		if (*cur == *substr) {
			//assert(full_data_len - i >= sublen);
			if (memcmp(cur, substr, sublen) == 0) {
				//found
				return cur;
			}
		}
		cur++;
	}
	return NULL;
}
struct item
{
	char *start;
	char *end;
	char *name;
	char *data;
	unsigned int data_len;
	//Content-Disposition: form-data; name="Filedata"; filename="test.png"
	//Content-Type: application/octet-stream
};

int post_upload(char *data,int data_len,char * boundary)
{
	char *p = data;
	char *split = "\r\n\r\n";
	/*printf("\n==========:%s\n%s\n,data_len:%d\n",boundary,p,data_len);*/
	char *end = data + data_len;
	p = memstr(p,data_len,boundary);
	struct item list[0x100];
	unsigned int list_len=0;
	char *Filename =NULL;
	char *Filedata=NULL;
	unsigned int Filelen=0;
	while(p && p < end){
		p += strlen(boundary);
		char *p2 = memstr(p,end-p,boundary);
		if(p2){
			list[list_len].start = p;
			list[list_len].end= p2 - strlen(split);
			*(list[list_len].end) = '\0';
			list[list_len].data= strstr(p,split);
			if(list[list_len].data){
				*(list[list_len].data)='\0';
				list[list_len].data+= strlen(split);
				list[list_len].data_len = list[list_len].end - list[list_len].data;
			}
			printf("\nhead%d:%s",list_len,list[list_len].start);
			printf("\n(data_len:%d)",list[list_len].data_len);
			/*printf("\ndata:%s \n",list[list_len].data);*/
			char* names =" name=\"";
			char *name = strstr(p,names);
			if(name){
				list[list_len].name = name + strlen(names);
				char *name_end = strstr(name+strlen(names),"\"");
				if(name_end)*name_end='\0';

				if(strcmp(list[list_len].name,"Filename")==0){
					Filename=list[list_len].data;
					/*printf("\nFilename:%s",Filename);*/
				}else if(strcmp(list[list_len].name,"Filedata") ==0){
					Filedata = list[list_len].data;
					Filelen = list[list_len].data_len;
					/*printf("\nFilelen:%d",Filelen);*/
					/*printf("\n Filedata:%s",Filedata);*/
				}
			}
			list_len++;
		}
		p = p2;
	}

	if( Filename && Filedata && Filelen >0)
	{
		printf("\n write to file : %s,%d\n",Filename,Filelen);
		FILE *wfile=fopen(Filename,"wb");
		fwrite(Filedata,1,Filelen,wfile);
		fflush(wfile);
		fclose(wfile);
		wfile = NULL;
	}
	if(data){
		free(data);
		data = NULL;
	}
	printf("post upload end !\n");
	return 0;
}



int recv_data()
{
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	struct client_item*client;
	int page_size = getpagesize();
	char *_boundary = "boundary=";
	while (1) {
		client = get_client_by_fd(0);
		int new_fd = accept(sock_fd,(struct sockaddr*)&addr, (socklen_t *)&addrlen);
		if (new_fd < 0) {
			perror("accept()");
			break;
		}
		if(client==NULL){
			send_str(new_fd,"HTTP/1.1 200 OK\r\n");
			send_str(new_fd,"\r\n no clients any more");
			close_sock(new_fd);
			continue;
		}
		client->fd = new_fd;
		/*int i = 1; setsockopt( new_fd, IPPROTO_TCP, TCP_NODELAY, (void *)&i, sizeof(i));//立即发送*/
		printf("client: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
		int num_page=ceil(client->recved_len/page_size);
		if(client->recved_len%page_size==0)num_page++;
		if(client->head){
			client->head = (char*)realloc(client->head,page_size*num_page);
		}else{
			client->head = (char*)malloc(page_size*num_page);
		}
		int len=0;
		while((client->head!= NULL) && strstr(client->head,"\r\n\r\n") == NULL)
		{
			if(((len=recv(new_fd,client->head+client->recved_len,page_size*num_page-client->recved_len, 0)) > 0)){
			}else{
				break;
			}
			client->recved_len +=len;
			if(client->recved_len == page_size*num_page){//所有页面已满,还有更多,再分配
				char*tmp = (char*)realloc(client->head,(page_size)*(++num_page));
				if(tmp != NULL){//分配成功
					client->head = tmp;
				}else{//失败
					if(client->head){
						free(client->head);
						client->head= NULL;
					}
					break;
				}
				tmp = NULL;
			}else if(client->recved_len > page_size*num_page){//overflow
				client->recved_len = 0;
				goto response_end;
				break;
			}else{//recv end
				memset(client->head+client->recved_len,0,num_page*page_size-client->recved_len);
				break;
			}
			if(client->recved_len > 0x100000){//http头超过1M,退出,以防止大数据包攻击
				len = -1;
				break;
			}
		}
		if(len <= 0 || client->recved_len==0 || client->head== NULL){//closed
			goto response_end;
			continue;
		}
#ifdef DEBUG
		printf("\nrecv:=============\n%s %d\n ======================= \n",client->head,client->recved_len);
		/*printf("\nrecv:=============\n%d,,,%d\n =======================%d \n",num_page,page_size,client->recved_len);*/
		fflush(stdout);
#endif
		if(strcmp(client->head,"<policy-file-request/>")==0)
		{
			char *ss= "<?xml version=\"1.0\"?><cross-domain-policy><site-control permitted-cross-domain-policies=\"all\"/><allow-access-from domain=\"*\" secure=\"false\"/><allow-http-request-headers-from domain=\"*\" headers=\"*\"/></cross-domain-policy>";
			send_str(new_fd,ss);
			goto response_end;
			continue;
		}

		char *head_end = strstr(client->head,"\r\n\r\n");
		int head_len = 0;
		if(head_end ==NULL){//头部信息接收未完
			goto response_end;
			continue;
		}else{
			head_len = head_end - client->head+ 4;
			printf("head_len:%d\n",head_len);
		}

		if(0 == strncmp(client->head,"GET ",4)){
			printf("is GET\n");
			client->path = getpath(client->head,"GET");
		}else if(0 == strncmp(client->head,"POST ",5)){
			printf("is POST\n");
			client->path = getpath(client->head,"POST");
		}

		char *content_length = getmetavalue(client->head,"Content-Length"); 
		int datalen = 0;
		if(content_length) datalen=atoi(content_length);
		printf("Content-Length:%d\n",datalen);

		if(datalen >0){//数据区
			int recved_data_len = client->recved_len - head_len;
			int remain_size = datalen - recved_data_len;
			client->data = malloc(datalen+1);
			if(client->data == NULL){
				goto response_end;
				continue;
			}
			memset(client->data,0,datalen+1);
			memcpy(client->data,client->head+ head_len,recved_data_len);
			while(remain_size > 0){
				if((len=recv(new_fd,client->data+recved_data_len,remain_size,0))>0){
					recved_data_len += len;
					remain_size = datalen - recved_data_len;
				}else{
					break;
				}
			}
			if(len <= 0 && remain_size >0 ){
				goto response_end;
				continue;
			}

			char *boundary = strstr(client->head,_boundary);
			if(boundary)
			{
				boundary = boundary + strlen(_boundary);
				// Content-Type: multipart/form-data; boundary=----------Ij5ae0ae0KM7GI3KM7 
				/*data2file("data.txt",data,&datalen);*/
				char* boundary_str=malloc(0x100);
				if(boundary_str){
					memset(boundary_str,0,0x100);
					int boundary_len=strlen(boundary);
					if(boundary_len>0xff)boundary_len = 0xff;
					strncpy(boundary_str,boundary,boundary_len);
					char *boundary_end = boundary_str+5;
					while(isgraph(*boundary_end)){
						boundary_end++;
					}
					*boundary_end = '\0';
					/*printf("boundary is %s\n",boundary_str);*/
					/*printf("data len is %d\n",datalen);*/
					post_upload(client->data,datalen,boundary_str);
					free(boundary_str);
				}
				client->data = NULL;
				send_str(new_fd,"HTTP/1.1 200 OK\r\n");
				send_str(new_fd,"\r\n");
				goto response_end;
				continue;
			}
		}


		client->num_paras=0;

		if(client->path){//head 传入的参数
			char *paras_start = strstr(client->path,"?");
			if(paras_start){
				++paras_start;
				getparas(paras_start,client->paras,&client->num_paras);
			}
			printf("head num_paras:%d\n",client->num_paras);
		}
		if(datalen>0){//post数据区传入的参数
			/*printf("data:%s\n",data);*/
			getparas(client->data,client->paras,&client->num_paras);
			printf("all num_paras:%d\n",client->num_paras);
		}
		client->host = getmetavalue(client->head,"Host");
		printf("Host:%s\n",client->host);
		if(client->path && strlen(client->path)>0)
			give_response(new_fd);
response_end:
		if(new_fd >0){
			close_sock(new_fd);
			new_fd = -1;
		}
	}
	if(sock_fd>0){
		close_sock(sock_fd);
		sock_fd = -1;
	}
	return 0;
}

int init_sock()
{
	char *cwd=malloc(0x100);
	if(cwd){
		getcwd(cwd,0x100);
		if(strlen(WWW_ROOT)==0)
			strcpy(WWW_ROOT,cwd);
		printf("@author:db0@qq.com\n");
		printf ("port=%d back=%d dirroot=%s cwd=%s(pID:%d)\n", DEFAULTPORT, DEFAULTBACK, WWW_ROOT,cwd, getpid());
		free(cwd);
	}
#ifdef WIN32
	system("ipconfig");
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested=MAKEWORD(2, 2);
	int ret=WSAStartup(wVersionRequested, &wsaData);
	if(ret!=0)
	{
		printf("WSAStartup() failed!\n");
		return 4;
	}
#else
	system("ip a s");
	signal(SIGPIPE, SIG_IGN);
	signal_ex(SIGINT, sig_handler);
	signal_ex(SIGCHLD, sig_handler);
#endif
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
		return 3;
	}
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(DEFAULTPORT);
	//addr.sin_port = htons(0);
	//addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_addr.s_addr = INADDR_ANY;
	int addrlen = sizeof(addr);

	int opt = SO_REUSEADDR;
	setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof(opt));// 端口复用,有可能收到非期望数据,windows下最好注释此行

	if (bind(sock_fd, (struct sockaddr *) &addr, addrlen) < 0) {
		perror("bind()");
		exitfun();
		return 2;
	}
	open_with_browser();
	if (listen(sock_fd, DEFAULTBACK) < 0) {
		perror("listen()");
		exitfun();
		return 1;
	}
	atexit(exitfun);
	recv_data();
	exitfun();
	return 0;
}

int init_db()
{
#ifdef _SQLITE3_H_
	opendb();
#endif
	return 0;
}

int main(int argc,char **argv)
{
	/*mkdir("test/");*/
#ifdef linux
#endif
	init_db();
	init_sock();
	if(sock_fd>0)close_sock(sock_fd);
#ifdef _SQLITE3_H_
	sqlite3_close(db);
#endif
	perror("end");
	exit(0);
	return 0;
}

//create table if not exists member(id INTEGER primary key asc,time INTEGER,name,fromschool,age,INTEGER sex,idcard,policies,familyaddress,parents_tel,tel,workplace,studyfee,workfee,classname,classno,photo);
//insert into member(name,fromschool,age,sex,idcard,policies,familyaddress,parents_tel,tel,workplace,studyfee,workfee,classname,classno,photo) values ("name","fromschool","age",1,"430422000000000000","qunzhong","familyaddress","parents_tel","tel","workplace","studyfee","workfee","classname","classno","photo");
//adb push gcc /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/

/*ln -s /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/bin/arm-linux-androideabi-gcc /system/bin/gcc*/
/*ln -s /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/bin/arm-linux-androideabi-g++ /system/bin/g++*/
/*ln -s /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/bin/make /system/bin/make*/
/**/
/*lib = /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/arm-linux-androideabi/lib/*/
/*include = /mnt/sdcard/Android/data/com.n0n3m4.droidc/files/gcc/arm-linux-androideabi/include/*/
