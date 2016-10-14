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
//# define SSIZE 37

void initEncode(char*,int**EncodeCk,vector<vector<double> >(&Encode),vector<vector<int> >(&idx));
void initBigram(double**bigram,vector<int>(&textInt),vector<vector<int> >(&idx));
int c2n(string);
int calcSSIZE(char*);

int SSIZE=0;

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
   int num;
   for(string each;getline(split,each,split_char);)
      {
	   num = c2n(each);
	   if(num>0)
	     textInt.push_back(num);
      }

   textFile.close();

   //calculate SSIZE
   SSIZE = calcSSIZE(argv[1]);
   
   //initialize Encode table
   int ** EncodeCk;//used for quick check//0: prob=0, num:num's element in the vector
   EncodeCk = new int*[SSIZE];
   for(int i=0;i<SSIZE;i++)
      EncodeCk[i] = new int[SSIZE];
   vector<vector<int> > EncodeIdx;
   vector<vector<double> > Encode;//store the encode prob
   initEncode(argv[1],EncodeCk,Encode,EncodeIdx);
   //cout<<"Encode end"<<endl;
 


   //initialize Bigram table
   double ** bigram;//used for quick check//0: prob=0, num:num's element in the vector
   bigram = new double*[SSIZE];
   for(int i=0;i<SSIZE;i++)
      bigram[i] = new double[SSIZE];
   initBigram(bigram,textInt,EncodeIdx);
   
  /* 
   for(int i=0;i<SSIZE;i++){
      for(int j=0;j<SSIZE;j++){
        cout<<EncodeCk[i][j]<<" ";
      }
      cout<<endl;
   }
   */

/*
   vector<vector<int> >::iterator it1;
   vector<int>::iterator  it2;
   for(it1 = EncodeIdx.begin();it1!=EncodeIdx.end();it1++){
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

void initEncode(char* fileName, int**EncodeCk,vector<vector<double> >(&Encode),vector<vector<int> >(&idx))
{

//output:each row show the prob of all the true value encoded by k

   fstream file;//encode.num
   char buffer[20];//buffer for each line
   double validNum[SSIZE];
   char split_char = ' ';
   int parser[3];

   //memset(EncodeCk,0,sizeof(EncodeCk));
   memset(validNum,0,sizeof(validNum));
   file.open(fileName,ios::in);
   if(!file)
      cerr<<"can not read encode file"<<endl;
   else
      {
           while( file.getline(buffer,sizeof(buffer),'\n'))
	        {
		int counter =0;
                istringstream str(buffer);
		for(string each;getline(str,each,split_char);counter++)
		   {
		      parser[counter] = c2n(each);
		      //cout<<counter<<":"<<parser[counter]<<" ";
		   }
		EncodeCk[parser[1]][parser[0]] = parser[2]; // i(true value) is encoded by j(observation)
	        
		if(EncodeCk[parser[1]][parser[0]])
	          validNum[parser[0]]++;
		}

      for(int i=0;i<SSIZE;i++)
         {
   	 vector<double> EncodeLine;
	 vector<int> idxLine;

         double lineiter=0;
	 for(int j=0;j<SSIZE;j++)	   
	    { 
	    if(EncodeCk[i][j])
	      {
	      lineiter++;
	      EncodeLine.push_back(double(1/validNum[j]));
	      idxLine.push_back(j);
  	      EncodeCk[i][j] = lineiter;
	      }
	    }
	 idx.push_back(idxLine);
	 Encode.push_back(EncodeLine);
	 }
     }
}

int c2n(string bu)
{
   if(bu.length() >1 && (bu[1]-'0')>0 && (bu[1]-'0')<10)
     return 10*(bu[0]-'0')+(bu[1]-'0');
   else
     return (bu[0]-'0');
}

void initBigram(double**bigram,vector<int> (&textInt),vector<vector<int> >(&idx))
{
   //memset(bigram,0,SSIZE*SSIZE*sizeof(*bigram));
   for(int i=0;i<SSIZE;i++)
      {
      for(int j=0;j<SSIZE;j++)
         {
         bigram[i][j]=0;
	 }
      }
   vector<int>::iterator iterTI;
   vector<int>::iterator iterIdx1;
   vector<int>::iterator iterIdx2;
   for(iterTI = textInt.begin()+1;iterTI!=textInt.end();iterTI++)
      {
       //cout<<*iterTI<<endl;     
      for(iterIdx1 = idx[*(iterTI-1)].begin();iterIdx1!=idx[*(iterTI-1)].end();iterIdx1++)
         {
         for(iterIdx2 = idx[*(iterTI)].begin();iterIdx2!=idx[*(iterTI)].end();iterIdx2++)
	    {
	    //cout<<*iterIdx1<<" "<<*iterIdx2<<" "<< bigram[*iterIdx1][*iterIdx2]<<"|";
	    //cout<<*iterIdx1<<" "<<*iterIdx2<<"|";
	    bigram[*iterIdx1][*iterIdx2]++;
	    }
	 //   cout<<endl;
	 }
	 //   cout<<endl;
      
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

int calcSSIZE(char* filename)
{
  char buffer[20];
  fstream file;
  char split_char = ' ';
  string tempC;
  int tempNum;
  int maxNum = 0;
  
  file.open(filename,ios::in);
  while(file.getline(buffer,sizeof(buffer),'\n'))
       {
       istringstream str(buffer);
       getline(str,tempC,split_char);
       tempNum = c2n(tempC);
       //cout<<tempNum;
       if(tempNum>maxNum)
          maxNum = tempNum;
       }
  return maxNum+1;
}
