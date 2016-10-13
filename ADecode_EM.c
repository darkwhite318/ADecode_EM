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
#include <vector>

using namespace std;
# define SSIZE 37

void initEncode(char*,int(&EncodeCk)[SSIZE][SSIZE],vector<vector<double> >(&Encode));
void initBigram();

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

   //initialization
   int EncodeCk[SSIZE][SSIZE];//used for quick check//0: prob=0, num:num's element in the vector
   vector<vector<double> > Encode;//store the encode prob
   initEncode(argv[1],EncodeCk,Encode);


/*
   vector<vector<double> >::iterator it1;
   vector<double>::iterator  it2;
   for(it1 = Encode.begin();it1!=Encode.end();it1++){
      for(it2 = it1->begin();it2!=it1->end();it2++){
         cout<<*it2<<" ";
      }
   cout<<endl;
   }
*/
   //write the outfile
   ofstream outFile;
   outFile.open(argv[3]);
   outFile<<text;
   outFile.close();

return 0;
}

void initEncode(char* fileName, int(&EncodeCk)[SSIZE][SSIZE],vector<vector<double> >(&Encode))
{
//NOTE: we assume that the order of the encode table will obey the order
//0 0 x
//0 1 x
//...
//0 36x
//1 0 x
//...

//output:each row show the prob of all the true value encoded by k

   memset(EncodeCk,0,sizeof(EncodeCk));
   fstream file;//encode.num
   char buffer[20];//buffer for each line
   int loSP;//location of \n
   double lineSum;//calculate number of n0n-zero element in each row

   file.open(fileName,ios::in);
   if(!file)
      cerr<<"can not read encode file"<<endl;
   else
      {
      for(int i=0;i<SSIZE;i++)
         {
	 for(int j=0;j<SSIZE;j++)
	    {
            file.getline(buffer,sizeof(buffer),'\n');
            EncodeCk[j][i] = buffer[strlen(buffer)-1]-'0'; // i(true value) is encoded by j(observation)
	    }
	}

     for(int i=0;i<SSIZE;i++)
        {
	lineSum=0;
   	vector<double> EncodeLine;
	for(int j=0;j<SSIZE;j++)
	   lineSum = lineSum+EncodeCk[i][j];

        double lineiter=0;
	for(int j=0;j<SSIZE;j++)	   
	  { 
	  if(EncodeCk[i][j])
	    {
	    lineiter++;
	    EncodeLine.push_back(double(1/lineSum));
  	    EncodeCk[i][j] = lineiter;
	    }
	  }
	Encode.push_back(EncodeLine);
        }
     }

}
