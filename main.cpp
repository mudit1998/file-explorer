#include<bits/stdc++.h>
#include<iostream>
#include <sys/wait.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<sys/ioctl.h>
#include<termios.h>
#include<fcntl.h>
#include<dirent.h>
#include<pwd.h>
#include <grp.h>

#define cursorupward(x) printf("\033[%dA", (x))
#define cursordownward(x) printf("\033[%dB", (x))
#define KEY_ESCAPE  0x001b
#define KEY_k 0x006b
#define KEY_l 0x006c
#define KEY_ENTER 0x000a
#define KEY_UP  0x0105
#define KEY_DOWN 0x0106
#define KEY_LEFT 0x0107
#define KEY_RIGHT 0x0108

using namespace std;
string root=get_current_dir_name();
stack<string>bacw,forw;
bool goto_f,search_f;
vector<char>cmdstring;
vector<string>tokens;
static struct termios term, oterm;
static int getch(void);
static int kbhit(void);
static int kbesc(void);
static int kbget(void);
//to change to non-canonical mode
static int getch(void)
{
    int c = 0;

    tcgetattr(0, &oterm);
    memcpy(&term, &oterm, sizeof(term));
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 1;
    term.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &term);
    c = getchar();
    tcsetattr(0, TCSANOW, &oterm);
    return c;
}

static int kbhit(void)
{
    int c = 0;

    tcgetattr(0, &oterm);
    memcpy(&term, &oterm, sizeof(term));
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 1;
    tcsetattr(0, TCSANOW, &term);
    c = getchar();
    tcsetattr(0, TCSANOW, &oterm);
    if (c != -1) ungetc(c, stdin);
    return ((c != -1) ? 1 : 0);
}

static int kbesc(void)
{
    int c;

    if (!kbhit()) return KEY_ESCAPE;
    c = getch();
    if (c == '[') 
    {
        switch (getch()) 
        {
            case 'A':
                c = KEY_UP;
                break;
            case 'B':
                c = KEY_DOWN;
                break;
            case 'D':
                c = KEY_LEFT;
                break;
            case 'C':
                c = KEY_RIGHT;
                break;
            default:
                c = 0;
                break;
        }
    } else 
    {
        c = 0;
    }
    if (c == 0) while (kbhit()) getch();
    return c;
}

static int kbget(void)
{
    int c;

    c = getch();
    return (c == KEY_ESCAPE) ? kbesc() : c;
}

vector<string> list_dir(char * path)// stores the list in a vector of string
{
   vector<string>s;
   DIR * thedirectory;
   struct dirent *thefile;
   struct stat thestat;
   thedirectory = opendir(path);

   char buff[512];  
    while((thefile = readdir(thedirectory)) != NULL) 
    {             
        stat(buff, &thestat);
        string ptr1=path;
        s.push_back(ptr1 + "/" + thefile->d_name);
           
    } 
    closedir(thedirectory);
    return s;
}
void single_print(char * path) // single print of a directory of file prsent in the directory
{
  	 struct  stat sb;
  	 stat(path,&sb);
     string s=path;
     char tmp[512];
     string name=s.substr(s.find_last_of("/\\")+1);
     strcpy(tmp,name.c_str());
     printf("%-20s\t",tmp);
     switch (sb.st_mode & S_IFMT) 
     {
            case S_IFBLK:  printf("b "); break;
            case S_IFCHR:  printf("c "); break; 
            case S_IFDIR:  printf("d "); break;  
            case S_IFIFO:  printf("p "); break; 
            case S_IFLNK:  printf("l "); break; 
            case S_IFSOCK: printf("s "); break;
        
            default:       printf("- "); break;
     }
    
        printf( (sb.st_mode & S_IRUSR) ? " r" : " -");
        printf( (sb.st_mode & S_IWUSR) ? "w" : "-");
        printf( (sb.st_mode & S_IXUSR) ? "x" : "-");    
        printf( (sb.st_mode & S_IRGRP) ? "r" : "-");
        printf( (sb.st_mode & S_IWGRP) ? "w" : "-");
        printf( (sb.st_mode & S_IXGRP) ? "x" : "-");
        printf( (sb.st_mode & S_IROTH) ? "r" : "-");
        printf( (sb.st_mode & S_IWOTH) ? "w" : "-");
        printf( (sb.st_mode & S_IXOTH) ? "x" : "-");

       cout<<setw(20)<<sb.st_size<<setw(40)<<ctime(&sb.st_mtime);
}
 
 void display_dir(vector<string>s,int start,int end)
 {
                
      char path[512]; 
      if(s.size()<end)end=s.size();         
      for(int i=start;i<end;i++)
      {
      	strcpy(path,s[i].c_str());
         single_print(path);
      }
 }
 void renameFile(vector<string>tokens) // rename for file
 {
 	if(tokens.size()==3)
 	{
       string initial=tokens[1];
       string later=tokens[2];
       rename(initial.c_str(),later.c_str());
 	}
 	else
 	{
 		cout<<endl;
	    cout<<"\033[0;31m"<<"invalid arg"<<endl;
	    cout<<"\033[0m";
	    cout<<":";
 	}
 }
 void createNewFile(vector<string>tokens)
 {
 	if(tokens.size()!=3)
 	{
 		cout<<endl;
	    cout<<"\033[0;31m"<<"less arg"<<endl;
	    cout<<"\033[0m";
	    cout<<":";
	    return;
 	}
 	string name=tokens[2];
 	name=name+"/"+tokens[1];
 	char * names=new char[name.length()+1];
 	strcpy(names,name.c_str());
 	int status=open(names,O_RDONLY | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
 	if(status==-1)
 	{
 		cout<<endl;
	    cout<<"\033[0;31m"<<"error in creating file"<<endl;
	    cout<<"\033[0m";
	    cout<<":";
 	}
 }
 void deleteFile(vector<string>tokens)
 {
 	if(tokens.size()!=2)
 	{
 		cout<<endl;
	    cout<<"\033[0;31m"<<"invalid arg"<<endl;
	    cout<<"\033[0m";
	    cout<<":";
	    return;
 	}
 	char * name=new char[tokens[1].length()+1];
 	strcpy(name,tokens[1].c_str());
    int status=remove(name);
    if(status!=0)
    {
    	cout<<endl;
	    cout<<"\033[0;31m"<<"error in deleting file"<<endl;
	    cout<<"\033[0m";
	    cout<<":";
    }
 }
 void makeDirectory(vector<string>tokens)
 {
 	if(tokens.size()!=3)
 	{
 		cout<<endl;
	    cout<<"\033[0;31m"<<"invalid arg"<<endl;
	    cout<<"\033[0m";
	    cout<<":";
 	}
 	else
 	{
         string name=tokens[2];
         name=name+"/"+tokens[1];
         char * names=new char[name.length()+1];
         strcpy(names,name.c_str());
          int status= mkdir(names,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
          if(status==-1)
          {
          	cout<<endl;
	        cout<<"\033[0;31m"<<"error in creating directory"<<endl;
	        cout<<"\033[0m";
	        cout<<":";
          }
    }
 }

 void removeDir(char * name){ //recursively delete directory 
 	DIR *d;
 		struct dirent *dir;
 		d=opendir(name);
 		if(d)
 		{
 			while((dir=readdir(d))!=NULL)
 			{
 				if((string(dir->d_name)=="..") || (string(dir->d_name)==".") ){}
 				else
 				{
 						string final =string(name)+"/"+string(dir->d_name);
 						char * finals=new char[final.length()+1];
 						strcpy(finals,final.c_str());
 						struct stat a;
 						if(stat(finals,&a)==-1)perror("lstat");
 						else
 						{
 							if(S_ISDIR(a.st_mode)){
                              removeDir(finals);
 							}
 							else
 							{
 								int status= remove(finals);
	                            if(status != 0)
	                            {
	                              cout<<endl;
	                              cout<<"\033[0;31m"<<"error in deleting file"<<endl;
	                              cout<<"\033[0m";
	                              cout<<":";
	                            }
	
 							}
 						}
 				}
 		    }
 			closedir(d);
 			int sta=rmdir(name);
 			if(sta==-1){
 				cout<<endl;
	            cout<<"\033[0;31m"<<"error in deleting "<<endl;
	            cout<<"\033[0m";
	            cout<<":";
 			}


 	    }
 		else{
 			cout<<endl;
	            cout<<"\033[0;31m"<<"no dir exist"<<endl;
	            cout<<"\033[0m";
	            cout<<":";
 		}
  }
 void deleteDirectory(vector<string>tokens){
 	if(tokens.size()!=2){
 		cout<<endl;
	    cout<<"\033[0;31m"<<"invalid arg"<<endl;
	    cout<<"\033[0m";
	    cout<<":";
 	}
 	else
 	{
 		char * name=new char[tokens[1].length()+1];
 		strcpy(name,tokens[1].c_str());
 		removeDir(name);
 	}
 }
 void copy_file(char * path,char *des){
 	char block[512];
	int in, out;
	int nread;
	
	in = open(path, O_RDONLY);
	out = open(des, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	while((nread = read(in,block,sizeof(block))) > 0)
		write(out,block,nread);

	struct stat sourcestat,deststat;
	if (stat(path,&sourcestat) != -1) {  
    }
    if (stat(des,&deststat) != -1) {  
    }
    
    int status1=chown(des,sourcestat.st_uid, sourcestat.st_gid);
	if(status1!=0){
		cout<<endl;
	    cout<<"\033[0;31m"<<"error in setting ownership"<<endl;
	    cout<<"\033[0m";
	    cout<<":";
	}

    int status2=chmod(des,sourcestat.st_mode);
	if(status2!=0){
		cout<<endl;
	    cout<<"\033[0;31m"<<"error in setting permission"<<endl;
	    cout<<"\033[0m";
	    cout<<":";
    }
 }
 void copy_dir(char * path,char* des)
 {
 	int status= mkdir(des,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	 if( status==-1)
	{
	 	cout<<endl;
	    cout<<"\033[0;31m"<<"error in creating dir"<<endl;
	    cout<<"\033[0m";
	    cout<<":";
	}

	DIR *d;
	struct dirent *dir;
	d = opendir(path);
	if (d) 
	{
	    while ((dir = readdir(d)) != NULL) 
	    {	 
		      if( (string(dir->d_name) == "..") || (string(dir->d_name) == ".") )	
		      {   } 
		  	  else
		  	  {		
		  	  		string finalpath=string(path) + "/" +string(dir->d_name);
					char* newpath = new char[finalpath.length() + 1];
					strcpy(newpath, finalpath.c_str());

					string finaldestpath=string(des) + "/" +string(dir->d_name);
					char* newdestpath = new char[finaldestpath.length() + 1];
					strcpy(newdestpath, finaldestpath.c_str());

		  			struct stat sb;
					if (stat(newpath,&sb) == -1) {
				        perror("lstat");
				    }
				    else{

				    	if((S_ISDIR(sb.st_mode)))
					    {
					    	copy_dir(newpath,newdestpath);
					    }
					    else
					    {
					    	 copy_file(newpath,newdestpath);
					    }
				    }
		  	  		
		  	  }
	    }
	}
	else
	{
        cout<<endl;
	    cout<<"\033[0;31m"<<"error in creating dir"<<endl;
	    cout<<"\033[0m";
	    cout<<":";		
	}
 }
 
 void copycommand(vector<string>tokens){
 	int len=tokens.size();
 	if(len<3){
 		cout<<endl;
	    cout<<"\033[0;31m"<<"invalid arg"<<endl;
	    cout<<"\033[0m";
	    cout<<":";
 	}
 	else
 	{
 		for(int i=1;i<len-1;i++)
		{
			string newData = tokens[i];
			string name=newData.substr(newData.find_last_of("/\\")+1,newData.length());
			string des=tokens[len-1];
			des=des+"/"+name;
			char * dest=new char[des.length()+1];
			char * sor=new char[newData.length()+1];
			strcpy(dest,des.c_str());
			strcpy(sor,newData.c_str());
			struct stat b;
			stat(sor,&b);
			if(S_ISDIR(b.st_mode)){
				copy_dir(sor,dest); // copy directory
			}
			else{
				copy_file(sor,dest);//copy files
			}

 	    }
    }
 }
 void movecommand(vector<string>tokens){ // move the content by use of copy and remove
 	int len=tokens.size();
 	if(len<3){
 	   cout<<endl;
	    cout<<"\033[0;31m"<<"invalid arg"<<endl;
	    cout<<"\033[0m";
	    cout<<":";	
 	}
 	else
 	{
        for(int i=1;i<len-1;i++)
		{			
			string newData = tokens[i];
			string name = newData.substr(newData.find_last_of("/\\")+1,newData.length());

			string destpath= tokens[len-1];
			destpath =destpath + "/" + name;
			char *des = new char[destpath.length() + 1];
			strcpy(des, destpath.c_str());
	
			char *path = new char[newData.length() + 1];
			strcpy(path, newData.c_str());
			struct stat b;
			stat(path,&b);
			if(S_ISDIR(b.st_mode)){
			
				copy_dir(path,des);
				removeDir(path);
			}
			else
			{
				copy_file(path,des);
				int status=remove(path);
				if(status!=0){
                 cout<<endl;
	             cout<<"\033[0;31m"<<"error in deleting file"<<endl;
	             cout<<"\033[0m";
	             cout<<":";
				}
			}
 	    }
    }
 }
 void gotos(vector<string>tokens){// goto to the directory which we want
 	if(tokens.size()!=2){
 		cout<<endl;
	    cout<<"\033[0;31m"<<"error "<<endl;
	    cout<<"\033[0m";
	    cout<<":";
 	}
 	else{ string name=tokens[1];
 		char * path=new char[name.length()+1];
 		strcpy(path,name.c_str());
 		int status=chdir(path);
 		if(status!=0){
 			cout<<endl;
	    cout<<"\033[0;31m"<<"error "<<endl;
	    cout<<"\033[0m";
	    cout<<":";
 		}
 	}
 }
 int searchall(char * path,string filename){ // search recusively

	DIR *d;
	struct dirent *dir;
	d = opendir(path);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if ((string(dir->d_name) == "..") || (string(dir->d_name) == "."))
			{
			}
			else
			{
				string name = string(dir->d_name);
				string finalpath = string(path) + "/" + name;
				char *newpath = new char[finalpath.length() + 1];
				strcpy(newpath, finalpath.c_str());
                struct stat sb;
				if (stat(newpath, &sb) == -1)
				{
					perror("lstat");
				}
				else
				{

					if ((S_ISDIR(sb.st_mode)))
					{
						if(name==filename){
							return 1;
						}
						int var= searchall(newpath,filename);
						if(var==1)return 1;
					}
					else{
						if(name==filename){
							return 1;
						}
					}
                }
            }
        }return 0;
    }else
    {
	     return 0;
    }
 }
 void searchcommand(vector<string>tokens)
 {
 	int len=tokens.size();
 	if(len!=2)
 	{
 		cout<<endl;
	    cout<<"\033[0;31m"<<"error "<<endl;
	    cout<<"\033[0m";
	    cout<<":";
 	}
 	else
 	{
 		string name=tokens[1];
 		char * path=new char[root.length()+1];
 		strcpy(path,root.c_str());
 		cout<<searchall(path,name);// return 1 if find otherwise 0
 	}
 }
 int command_mode(){
 	char ch;
 	do{
 		string input;
 		while(((ch = getchar())!= 10) && ch!=27)
		{
				input = input + ch;
				//cout<<ch;	
	    }
	    tokens.clear();

	    int i=0;
	    while(i<input.length()){
	    string store="";
	    while((input[i]!=' ') && (i<input.length())){

			if(input[i]=='\\')
			{
				store += input[i+1];
				i=i+2;
			}
			else{

				store += input[i];
				i++;
			}

	    }
	    tokens.push_back(store);
	    i++;

      }
      // stored the parser string into tokens (vector of string) and the check for the first string of vector which will be our command
       if(ch==10){
       	string command=tokens[0];
       	if(command == "copy")
			{
				copycommand(tokens);
				//cout<<endl;
				cout<<":";
			}
			else if(command == "move")
			{
				movecommand(tokens);
				//	cout<<endl;
				cout<<":";
			}
			else
			 if(command == "rename")
			{
				renameFile(tokens);
				//	cout<<endl;
				cout<<":";
			}
			else if(command == "create_file")
			{
				createNewFile(tokens);
				//	cout<<endl;
				cout<<":";
			}
			else if(command == "create_dir")
			{
				makeDirectory(tokens);
				cout<<":";
			}
			else if(command == "delete_file")
			{
				deleteFile(tokens);
				cout<<":";
			}
			else if(command == "delete_dir")
			{
				deleteDirectory(tokens);
				cout<<":";
			}
			else if(command=="goto"){
				gotos(tokens);
				cout<<":";
			}
			else if(command=="search"){
				searchcommand(tokens);
					cout<<endl;
					cout<<":";
			}
			else{
				cout<<"invalid"<<endl;
				//cout<<":";
			}
       } 

 	}while(ch!=27); // used escape for exiting

  return 0;
 }

int main()
{
  long size;
  char *buf;
  char *ptr;
  int start=0;
  int end=20;
  size = pathconf(".", _PC_PATH_MAX);

//give current directory path
  if ((buf = (char *)malloc((size_t)size)) != NULL)
     ptr = getcwd(buf, (size_t)size);

//list the content of directory upto some limit
  vector<string>s;
  s=list_dir(ptr);
  int k=s.size();
  int count=min(k,end);
  display_dir(s,start,end);
  //pushing the current directory in backward stack
  struct stat sb;  
  string z=ptr;
  char path[512];
  bacw.push(z);

  int c;
 // changing mode and also taking user input key and perform different task
  while (1) 
    {  
        c = kbget();
        if ( c == KEY_ESCAPE ) // to exit from normal mode
        {
            break;
        }
        else if(c == KEY_ENTER)// to get into particular directory or to open a file
        {
           strcpy(path,s[count].c_str());
          
           stat(path,&sb);
           if ((sb.st_mode & S_IFMT) == S_IFREG)
				{ 
					char * args[1];
					args[0]=(char*)s[count].c_str();
					int fileOpen=open("/dev/null",O_WRONLY);
					dup2(fileOpen,2);
					close(fileOpen);
					pid_t processID = fork();
					if(processID == 0)
					{    
						execl("/usr/bin/gedit","gedit",args[0]
					,NULL);
						exit(0);
					}
					else{
						wait(NULL);
					} 
				}
				else
				{
				
				 z=path;
				  bacw.push(z);
				  while(!forw.empty())
        	      {
        	 	    forw.pop();
        	      }
				 printf("\033[2J");
             	 printf("\033[0;0H");
				 s=list_dir(path);
				 k=s.size();
				 start=0;
				 end=20;
				 count=min(k,end);
				 display_dir(s,start,end);   	
			    }  	
        }
         else
        if (c == KEY_RIGHT) //for go forward
        {
            if(forw.size()>=1)
            {
            	bacw.push(forw.top());
            	printf("\033[2J");
        	    printf("\033[0;0H");
        	    z=forw.top();
        	    forw.pop();
        	    strcpy(path,z.c_str());
        	 s=list_dir(path);
        	 k=s.size();
        	 start=0;end=20;
        	 count=min(k,end);
        	 display_dir(s,start,end);
            }

        } else
        if (c == KEY_LEFT)//for go backward
        {
            if(bacw.size()>=2)
            {
            	forw.push(bacw.top());
            	printf("\033[2J");
        	    printf("\033[0;0H");
        	    bacw.pop();
        	    z=bacw.top();
        	    strcpy(path,z.c_str());
        	    s=list_dir(path);
        	    k=s.size();
        	    start=0;end=20;
        	    count=min(k,end);
        	    display_dir(s,start,end);
            }
        }
        else if(c==KEY_UP) //for positioning cursor upward
        {
             count--;
            cursorupward(1);
        } 
        else if(c==KEY_DOWN) // for postioning cursor downward
        {
        	count++;
        	cursordownward(1);
        }
        else if(c==KEY_k) // for scrolling upward
        {
        	if(start>0)
        	{
        		printf("\033[2J");
        	    printf("\033[0;0H");
        		start--;
        		end--;
        		count=end;
        		display_dir(s,start,end);
            }
        }
        else if(c==KEY_l) // for scrolling downward
        {
        	if(end<k)
        	{
        		printf("\033[2J");
        	    printf("\033[0;0H");
                start++;
                end++;
                count=end;
                display_dir(s,start,end);
        	}
        }
        else if(c==104)
        { // for home key(h)
            z=ptr;
            if(bacw.top()!=z)
            {
            	printf("\033[2J");
        	    printf("\033[0;0H");
        	    while(!forw.empty())
        	    {
        	 	forw.pop();
        	    }
        	    bacw.push(z);
        	    strcpy(path,z.c_str());
        	    s=list_dir(path);
        	    k=s.size();
        	    start=0;end=20;
        	    count=min(k,end);
        	    display_dir(s,start,end);
            }
        }
        else if(c==127)
        { //backspace
            string check=ptr;
            if(check!=z)
            {
            	while(!forw.empty())
            	{
        	 	forw.pop();
        	    }
              z=z.substr(0,z.find_last_of("/\\"));
              printf("\033[2J");
        	  printf("\033[0;0H");
        	  bacw.push(z);
        	  strcpy(path,z.c_str());
        	  s=list_dir(path);
        	  k=s.size();
        	  start=0;end=20;
        	  count=min(k,end);
        	  display_dir(s,start,end);

            }
        }
        else if(c==58)
        { // command mode (:)
              printf("\033[2J");
              printf("\033[0;0H");
              printf(":");
              command_mode();
              strcpy(path,z.c_str());
        	  s=list_dir(path);
        	  k=s.size();
        	  start=0;end=20;
        	  count=min(k,end);
        	  display_dir(s,start,end);
        }
        else {
            putchar(c);
             }
    }
    printf("\n");
}


