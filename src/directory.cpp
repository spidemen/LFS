#include "directory.h"
// #include "cfile.cpp"
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
using namespace std;

// useful Link: http://man7.org/linux/man-pages/man7/inode.7.html

//  first character: # directory
//                   -  link
//                   eg {"#next",7} directory next  {"-a",8}  link a
map<string, vector<pair<string, int>>> FileSystemMap; // <path/directory, filename>


#define TYPE_FILE 3
#define TYPE_DIRECTORY 2
#define TYPE_LINK 4


char allFileName[40][10];

struct stat stbuf;
void testD3();
int SplitPath(const char *path, string &Entrypath, string &filename);

int initDirectory(int cachesize)
{
  cout << "C++ compile success, fuse call function" << endl;
  
  currentinum = initFile(cachesize);
  cout << "currentinum " << currentinum << endl;
  char buf[BLOCK_SIZE];
  FileSystemMap.clear();
  
  for (int i = 1; i <= currentinum; i++)
  {
    struct Inode node;
    if (!File_Get(i, &node) && node.in_use)
    {
      vector<pair<string, int>> tmp;
      string a = node.path;
      string b = node.filename;
      auto it=FileSystemMap.find(a);
      if(it!=FileSystemMap.end()){
        tmp=it->second;
      }
      // parent directory
      if(b=="."){
          string path1,filename;
         SplitPath(a.c_str(),path1,filename);
          vector<pair<string, int>> tmpParent;
          auto it2=FileSystemMap.find(path1);
          if(it2!=FileSystemMap.end()){  // parent directory exit
               tmpParent=it2->second;
               filename="#"+filename;
               tmpParent.push_back({filename, node.inum});
                FileSystemMap[path1]=tmpParent;
          }
      }
      tmp.push_back({b,node.inum});
      FileSystemMap[a]=tmp;
      cout<<" Path "<<b<<" filename "<<a<<endl;
    }
  }
  if(currentinum==0){    // emtpy , create a root entry
      currentinum++;
     vector<pair<string, int>> tmp;
     tmp.push_back({".", currentinum});
     FileSystemMap.insert({"/", tmp});
     File_Create(currentinum, 2);  // add directory KATY
     File_Naming(currentinum, "/", ".", &stbuf);
  }

  return 0;
}


int SplitPath(const char *path, string &Entrypath, string &filename)
{
  string tmp = path;
  int position = tmp.find_last_of("/");
  //   string path1;
  if (position == 0)
  {
    Entrypath = "/";
  }
  else
  {
    Entrypath = tmp.substr(0, position);
  }
  filename = tmp.substr(position + 1);
  cout << "Split path =" << Entrypath << " filename =" << filename << endl;
  return 0;
}

// get inum , stat , inode of file or entry
/**
*  return:   num --inum of file
    sfbuf-- fuse structure 
   input: path full path of entry
*/
int Directory_Types(const char *path, struct stat *stbuf, int *num)
{
  string tmp = path;
  auto it = FileSystemMap.find(path);
  if (it != FileSystemMap.end())
  {

    for(auto it1:it->second){
     
      if (it1.first == ".")
      {
        int numSize = it1.second;
        memcpy(num, &numSize, sizeof(int));
        cout<<"debug   filename "<<it1.first<<endl;
        if (!convertInodeToStat(it1.second, stbuf)) {
              printf("Copied inode %d data into stat\n", it1.second);
          } else { 
            printf("Error: Directory_Types - calling File_Get\n"); 
          }
         return TYPE_DIRECTORY; // directory
      }
    }
   
  }
  else
  {
    string path1, filename;
    int position = tmp.find_last_of("/");

    if (position == 0)
    {
      path1 = "/";
    }
    else
    {
      path1 = tmp.substr(0, position);
    }
    filename = tmp.substr(position + 1);
    //     SplitPath(path,path1,filename);
    cout << "path =" << path1 << " filename =" << filename << endl;
    it = FileSystemMap.find(path1);
    if (it != FileSystemMap.end())
    {
      for (auto it2 : it->second)
      {
        //   cout<<"iteration   &"<<path1<<"&   "<<it2.first<<endl;
        if (strcmp(it2.first.c_str(), filename.c_str()) == 0)
        {
          if (!convertInodeToStat(it2.second, stbuf)) {
              printf("Copied inode %d data into stat\n", it2.second);
          } else { 
            printf("Error: Directory_Types - calling File_Get\n"); 
          }
          int numSize = it2.second; //first is filename, second is inum
          memcpy(num, &numSize, sizeof(int));
          return TYPE_FILE; // file
        }
        else
        {
          if (it2.first[0] == '-')
          {
            tmp = it2.first.substr(1);
            if (strcmp(tmp.c_str(), filename.c_str()) == 0)
            {

              stbuf->st_mode = S_IFLNK | 0700;
              stbuf->st_nlink = 1;
              int numSize = it2.second;
              memcpy(num, &numSize, sizeof(int));
              return TYPE_LINK; // link
            }
          }
        }
      }
    }
  }

  return 0; // not find
}
int Directory_getAllFiles(const char *path, struct stat *stbuf, int *size, char allFilename[50][10])
{
  auto it = FileSystemMap.find(path);
  if (it != FileSystemMap.end())
  {
    int arraySize = it->second.size();
    memcpy(size, &arraySize, sizeof(int));
    auto file = it->second.begin();
    for (int i = 0; i < arraySize && file != it->second.end(); i++)
    {
      struct stat t;
      InitStat(&t);
      if (file->first.find("#") != std::string::npos)
      {
        string tmp = file->first.substr(1);
        t.st_mode = S_IFDIR | 0700;
        t.st_nlink = 2;
        strcat(allFilename[i], tmp.c_str());
      }
      else if (file->first[0] == '-')
      {
        string tmp = file->first.substr(1);
        t.st_mode = S_IFREG | 0774;
        t.st_nlink = 1;
        strcat(allFilename[i], tmp.c_str());
      }
      else
      {
        t.st_mode = S_IFLNK | 0700;
        t.st_nlink = 1;
        strcat(allFilename[i], file->first.c_str());
      }
      stbuf[i] = t;

      cout << "filename " << file->first << endl;
      file++;
    }
    return 0;
  }
  else
  {
    string path1, filename;
    SplitPath(path, path1, filename);
    it = FileSystemMap.find(path1);
    if (it != FileSystemMap.end())
    {
      for (auto it1 : it->second)
      {
        if (strcmp(it1.first.c_str(), filename.c_str()) == 0)
        {
          int arraySize = 1;
          memcpy(size, &arraySize, sizeof(int));
          strcat(allFilename[0], it1.first.c_str());
          //InitStat(&stbuf[0]);
          if (!convertInodeToStat(it1.second, &stbuf[0])) {
              printf("Copied inode %d data into stat\n", it1.second);
          } else {
            printf("Error in directory: couldn't get file %d\n", it1.second);
          }

          return 0; // one entry found
        }
      }
    }

    return 1; // no entry found
  }
  return 0;
}

int Directory_Open(const char *path)
{
  auto it = FileSystemMap.find(path);
  if (it != FileSystemMap.end())
  {           // entry exitf
    return 0; // success
  }
  else
    return -1; // fail
}

int Directory_EntyUpdate(const char *path, int type)
{
  auto it = FileSystemMap.find(path);
  string path1, filename;
  SplitPath(path, path1, filename);
  if (it != FileSystemMap.end())
  { // entry exit
    if (type == 1)
    { // add  directory
      cout << "Error: entry already exit " << endl;
      return 0;
    }
    else if (type == 2)
    {                          // delete directory
     
      // need to call File_Free
       int num;
      Directory_Types(path, &stbuf, &num);
      File_Free(num);
      FileSystemMap.erase(it); // delete directory
      auto it2 = FileSystemMap.find(path1); // directory from parents
      filename = "#" + filename;
      if (it2 != FileSystemMap.end())
      {
        vector<pair<string, int>> tmp = it2->second;
        vector<pair<string, int>>::iterator it3 = tmp.begin();
        while (it3 != tmp.end())
        {
          //   cout<<path1<<"& "<<it3->first<<"   &"<<filename<<endl;
          if (strcmp(it3->first.c_str(), filename.c_str()) == 0)
          {
            tmp.erase(it3);
            FileSystemMap.erase(it2);
            FileSystemMap.insert({path1, tmp});
            cout << "delete entry  " << path << "   " << it3->first << endl;
            break;
          }
          it3++;
        }
      }
      return 1;
    }
  }
  else
  { // entry not exit in table
    if (type == 1)
    {                       // add  directory
      vector<pair<string, int>> tmp;
      currentinum++;
      tmp.push_back({".", currentinum}); // later use currentinum for num
      FileSystemMap.insert({path, tmp});
      File_Create(currentinum, TYPE_D);
      File_Naming(currentinum,path, ".", &stbuf);
      // parent directoy
      auto it2 = FileSystemMap.find(path1);
      if (it2 != FileSystemMap.end())
      {
        filename = "#" + filename;
        it2->second.push_back({filename, currentinum});
      }
   //   cout << "debug  path=" << path1 << "& filename=" << filename << "&" << endl;
      cout << " Add entry " << path << endl;
      return 1;
    }
    else if (type == 2)
    {                       // delete directory
      cout << " Error: directory not exit delete " << endl;
      return 0;
    }
    if (type == 3)
    {                                       // delete file
      auto it2 = FileSystemMap.find(path1); //
      if (it2 != FileSystemMap.end())
      {
        vector<pair<string, int>> tmp = it2->second;
        vector<pair<string, int>>::iterator it3 = tmp.begin();
        while (it3 != tmp.end())
        {
          if (strcmp(it3->first.c_str(), filename.c_str()) == 0)
          {
             int num;
            Directory_Types(path, &stbuf, &num);
            File_Free(num);
            tmp.erase(it3);
            FileSystemMap.erase(it2);
            FileSystemMap.insert({path1, tmp});
            //need to call File_Free
            
            cout << "delete file  " << path << "   " << it3->first << endl;
            break;
          }
          it3++;
        }
      } 
   }
 }
 
  return 1;
}

int Directory_EntryRename(const char *from,const char *to , int type)
{
  auto it = FileSystemMap.find(from);
  string path1, filename;
  string tmpTo=to;
  SplitPath(from, path1, filename);
  if (it != FileSystemMap.end()){   // exit
       if(type==TYPE_DIRECTORY||type==0){     //
          vector<pair<string, int>>tmp = it->second;
          FileSystemMap.erase(it);
          FileSystemMap.insert({to, tmp});            // update entry
        // need to call File_Naming
        int num;
        Directory_Types(from, &stbuf, &num);
        File_Naming(num, path1.c_str(), filename.c_str(), &stbuf);
        auto it2 = FileSystemMap.find(path1); // directory from parents
        filename = "#" + filename;
        if (it2 != FileSystemMap.end())
        {
          vector<pair<string, int>> tmp = it2->second;
          vector<pair<string, int>>::iterator it3 = tmp.begin();
          while (it3 != tmp.end())
          {
            //   cout<<path1<<"& "<<it3->first<<"   &"<<filename<<endl;
            if (strcmp(it3->first.c_str(), filename.c_str()) == 0)
            {
              path1="";
              filename="";
              SplitPath(to, path1, filename);
              filename = "#" + filename;
              it3->first=filename;
              tmp.erase(it3);
              tmp.push_back({filename,it3->second});
              FileSystemMap[path1]=tmp;
              // FileSystemMap.erase(it2);
              // FileSystemMap.insert({path1, tmp});
              cout << "update entry  " << from << " to  " << it3->first << endl;
              return 1;
          //    break;
            }
            it3++;
          }
        }
      return 1;
    }
  } else{     // entry not exit
      auto it2 = FileSystemMap.find(path1); //  file or link
      if (it2 != FileSystemMap.end())
      {
        vector<pair<string, int>> tmp = it2->second;
        vector<pair<string, int>>::iterator it3 = tmp.begin();
        while (it3 != tmp.end())
        {
          if (strcmp(it3->first.c_str(), filename.c_str()) == 0)
          {
             //update entry
            filename="";
            SplitPath(to, path1, filename);
            tmp.erase(it3);
            tmp.push_back({filename,it3->second});
            FileSystemMap[path1]=tmp;
            // FileSystemMap.erase(it2);
            // FileSystemMap.insert({path1, tmp});
            //need to call File_Free
            int num;
            Directory_Types(from, &stbuf, &num);
           //    File_Naming(num, path1.c_str(), filename.c_str(), stbuf);
            cout << "update file  " << it3->first << "   " << filename << endl;
            return 1;
        //    break;
          }
          it3++;
        }
      } 
  }
     return 0;
}


void Directory_Destroy()
{

    File_Destroy();
    FileSystemMap.clear();
}

int InitStat(struct stat *stbuf)
{
  stbuf->st_uid = getuid();     // The owner of the file/directory is the user who mounted the filesystem
  stbuf->st_gid = getgid();     // The group of the file/directory is the same as the group of the user who mounted the filesystem
  stbuf->st_atime = time(NULL); // The last "a"ccess of the file/directory is right now
  stbuf->st_mtime = time(NULL); // The last "m"odification of the file/directory is right now
  return 1;
}


int Directory_createFile(const char *path, struct stat *stbuf)
{
  
  string path1,filename;
  SplitPath(path,path1,filename);
  currentinum++;
  if (!File_Create(currentinum, TYPE_F))   // 0-file , 1-entry  directory
  {
    File_Naming(currentinum, path1.c_str(), filename.c_str(), stbuf);

    auto it = FileSystemMap.find(path1);
    if (it != FileSystemMap.end())
    {
      vector<pair<string, int>> tmp=it->second;
      tmp.push_back({filename, currentinum});
      FileSystemMap.erase(it);
      FileSystemMap.insert({path1, tmp});
      cout<<" create a new file  inum="<<currentinum<<"  filename="<<filename<<endl;
    }
    else
    {
       cout<<" Error: directory not exit, please first make a directory "<<path1<<endl;
       return 1;
     // it->second.push_back({filename, currentinum});
      // FileSystemMap.insert({path,tmp});
    }
  }

  return 0;
}

int Directory_deleteFile(const char *path, struct stat *stbuf)
{
  // mark inode user=-1, then mark block point to be default value--call Log_writeDeadBlock

    return 0;
}

int Directory_readFile(const char *path, int offset, int size, char *buf)
{
  // get num
  int num;
  if (Directory_Types(path, &stbuf, &num) == TYPE_FILE)
  { // file
    size=size>1024?800:size;
    cout << "debug  directory  read |" << buf << " num=" << num << " size =" << size << " offset=" << offset << endl;
    File_Read(num, offset, size, buf);
  }
  else
  {
    cout << " entry  " << path << "  not file  num=" << num << endl;
  }

  // check file exit
  // check write permission
  // do read

  return 0;
}

int Directory_writeFile(const char *path, int offset, int size, char *buf)
{
  // get inum
  int num;
  if (Directory_Types(path, &stbuf, &num) == TYPE_FILE)
  { // file
    cout << "debug  directory  write |" << buf << " num=" << num << " size =" << size << " offset=" << offset << endl;
    //printf("Permissions: %lu \n", stbuf->st_mode);
    // if (stbuf->st_mode)

    File_Write(num, offset, size, buf);
  }
  else
  {
    cout << " entry  " << path << "  not file  num=" << num << endl;
  }

  // check file exists

  // check write permission

  // do write
  //   File_Write(2, offset, ,buf)

  return 0;
}

// int main(int argc, char *argv[])
// {
// 	cout<<"hell World"<<endl;
  
//    	printf("Passed test1\n");
//     initDirectory(4);
//     // test1();
//     testD3();
//     test1();
//   //  	Test_File_Create(1);
//   //  	//File_Write(1, 0, 5, (void *) "hello");
//   //  	Show_Ifile_Contents();

// 	//  delete segmentCache;
//     return 1;
// }
