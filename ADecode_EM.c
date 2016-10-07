//******************************************************************************************//
//MD-HW 1-2
//using EM to decode probabilic enocding text
//Author: Yu Tseng 
//******************************************************************************************//
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cmath>
#include <memory.h>

using namespace std;
# define SSIZE 37
bool Encode[SSIZE][SSIZE];

void file2Table(char*,double(&table)[SSIZE][SSIZE]);


// ./ADecode_EM encode.txt test.txt out.txt
int main(int argc, char* argv[])
{
 
   // read in test file
   ifstream textFile;
   textFile.open(argv[2]);
   stringstream buffer;
   buffer<<textFile.rdbuf();
   string text = buffer.str();
   int textL = text.length();


   file2Table(argv[1],Encode);



   //write the outfile
   ofstream outFile;
   outFile.open(argv[3]);
   outFile<<text;
   outFile.close();

return 0;
}

void file2Table(char* fileName, double(&table)[SSIZE][SSIZE])
{
//NOTE: we assume that the order of the encode table will obey the order
//0 0 x
//0 1 x
//...
//0 36x
//1 0 x
//...

   memset(table,0,sizeof(table));
   fstream file;//encode.num
   char buffer[20];//buffer for each line
   int loSP;//location of \n

   file.open(fileName,ios::in);
   if(!file)
      cerr<<"can not read encode file"<<endl;
   else
      {
      for (int i=0;i<SSIZE;i++)
          {
	  for(int j=0;j<SSIZE;j++)
	     {
             file.getline(buffer,sizeof(buffer),'\n');
             Encode[i][j] = buffer[strlen(buffer)-1]-'0';
	     }
	  }
      }

}
