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

void initEncode(char*,int(&EncodeCk)[SSIZE][SSIZE],vector<vector<double> >(&Encode),vector<vector<int> >(&idx));
void initBigram(double(&bigram)[SSIZE][SSIZE],vector<int>(&textInt),vector<vector<int> >(&idx));
int c2n(string);

// ./ADecode_EM encode.bin test.num out.txt
int main(int argc, char* argv[])
{
 
   // read in test file
   ifstream textFile;
   textFile.open(argv[2]);
   stringstream buffer;
   buffer<<textFile.rdbuf();
   string text = buffer.str();
   int textL = text.length();
   
   //parse text.num into a vector<int>
   vector<int> textInt;
   char split_char = ' ';
   istringstream split(text);
   for(string each;getline(split,each,split_char);textInt.push_back(c2n(each)));
   
   textFile.close();

   //initialize Encode table
   int EncodeCk[SSIZE][SSIZE];//used for quick check//0: prob=0, num:num's element in the vector
   vector<vector<int> > EncodeIdx;
   vector<vector<double> > Encode;//store the encode prob
   initEncode(argv[1],EncodeCk,Encode,EncodeIdx);

   //initialize Bigram table
   double bigram[SSIZE][SSIZE];
   initBigram(bigram,textInt,EncodeIdx);
   for(int i=0;i<SSIZE;i++){
      for(int j=0;j<SSIZE;j++){
        cout<<bigram[i][j]<<" ";
      }
      cout<<endl;
   }


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
   //outFile.open(argv[3]);
   //outFile<<text;
   //outFile.close();

return 0;
}

void initEncode(char* fileName, int(&EncodeCk)[SSIZE][SSIZE],vector<vector<double> >(&Encode),vector<vector<int> >(&idx))
{
//NOTE: we assume that the order of the encode table will obey the order
//0 0 x
//0 1 x
//...
//0 36x
//1 0 x
//...

//output:each row show the prob of all the true value encoded by k

   fstream file;//encode.num
   char buffer[20];//buffer for each line
   double lineSum;//calculate number of n0n-zero element in each row
   double validNum[SSIZE];
   memset(EncodeCk,0,sizeof(EncodeCk));
   memset(validNum,0,sizeof(validNum));
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
	    if(EncodeCk[j][i])
	      validNum[i]++;
	    }
	 }

      for(int i=0;i<SSIZE;i++)
         {
 	 lineSum=0;
   	 vector<double> EncodeLine;
	 vector<int> idxLine;
	 for(int j=0;j<SSIZE;j++)
	    lineSum = lineSum+EncodeCk[i][j];

         double lineiter=0;
	 for(int j=0;j<SSIZE;j++)	   
	    { 
	    if(EncodeCk[i][j])
	      {
	      lineiter++;
	      EncodeLine.push_back(double(1/validNum[j]));
	      idxLine.push_back(j);
	      //cout<<j<<" ";
  	      EncodeCk[i][j] = lineiter;
	      }
	    }
	 idx.push_back(idxLine);
	 Encode.push_back(EncodeLine);
         //cout<<endl;
	 }
     }
}

int c2n(string bu)
{
   if(bu.length() >1)
     return 10*(bu[0]-'0')+(bu[1]-'0');
   else
     return (bu[0]-'0');
}

void initBigram(double(&bigram)[SSIZE][SSIZE],vector<int> (&textInt),vector<vector<int> >(&idx))
{
   memset(bigram,0,sizeof(bigram));
   vector<int>::iterator iterTI;
   vector<int>::iterator iterIdx1;
   vector<int>::iterator iterIdx2;
   for(iterTI = textInt.begin();iterTI!=textInt.end()-1;iterTI++)
      {
      for(iterIdx1 = idx[*iterTI].begin();iterIdx1!=idx[*iterTI].end();iterIdx1++)
         {
         for(iterIdx2 = idx[*(iterTI+1)].begin();iterIdx2!=idx[*(iterTI+1)].end();iterIdx2++)
	    {
	    bigram[*iterIdx1][*iterIdx2]++;
	    }
	 }
      }
      
      for(int i=0;i<SSIZE;i++)
         {
	 double sum =0;
         for(int j=0;j<SSIZE;j++)
	    {
	    sum = sum + bigram[i][j];
	    }
	 for(int j=0;j<SSIZE;j++)
	    {
	    bigram[i][j] = bigram[i][j]/sum;
	    }
	 }
      
}
