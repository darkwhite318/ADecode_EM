//***************************************************************************//
//MD HW1-2
//using EM to decode probabilic encoding text
//Yu Tseng
//***************************************************************************//
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cmath>
#include <memory.h>
#include <vector>
using namespace std;

void parseText2Int(vector<int>(&textInt),string&);
int c2n(string);
int calcSSIZE(char*);
void initEncode(char*,bool**,double**);
void initBigram(double**,bool**,vector<int>(&));
void Prop(double**,double*,bool**,double**,double**,vector<int>(&),bool);
double sumlog(double,double);
void EM(double*,bool**,double**,double**,vector<int>(&));

int SSIZE;
const double ZERO = 10;//assignde a possitive number as the sybol of log(0)

// ./ADecode_EM2 encode.bin test.num out.txt
int main(int argc,char* argv[])
{
   //=========variable=========//
    string text;//parse input text into string
    vector<int> textInt;//parse text into vector<int>
    bool **EncodeCk;//query an encode table is the prob is 0 or not
    double ** encode;//encode table(change in each iteration)
    double ** bigram;//transition prob table
    double* pi;//initial state

   //=======read in test file=========//
   ifstream textFile;
   textFile.open(argv[2]);
   stringstream buffer;
   buffer<<textFile.rdbuf();
   text = buffer.str(); 
   textFile.close();

   //===parse text into a vector<int>==//
   parseText2Int(textInt,text);

   //===get # of all possible observation==//
   SSIZE = calcSSIZE(argv[1]);

   //==initialize encode table==//
   EncodeCk = new bool*[SSIZE];
   encode = new double*[SSIZE];
   for(int i=0;i<SSIZE;i++)
   {
      EncodeCk[i] = new bool[SSIZE];
      encode[i] = new double[SSIZE];
   }
   initEncode(argv[1],EncodeCk,encode);
   /*
   for(int i=0;i<SSIZE;i++)
   {
     for(int j=0;j<SSIZE;j++)
     {
        cout<<encode[i][j]<<" ";
     }
     cout<<endl;
   }
   */
  //==initialize bigram table==//
   bigram = new double*[SSIZE];
   for(int i=0;i<SSIZE;i++)
   {
      bigram[i] = new double[SSIZE];
   }
   initBigram(bigram,EncodeCk,textInt);
  
  //=====initial pi=======//
  pi = new double[SSIZE];
  for(int i=0;i<SSIZE;i++)
      pi[i] = log(1/SSIZE);

 //======EM Algorithm======//
 int iterNum = 1;
 for(int i=0;i<iterNum;i++)
    EM(pi,EncodeCk,encode,bigram,textInt);

 //=====output testing===//
 /*
 cout<<"pi:"<<endl;
 for(int i=0;i<SSIZE;i++)
    cout<<pi[i]<<" ";

 cout<<endl;
 cout<<"encode:"<<endl;
 for(int i=0;i<SSIZE;i++)
 {
    for(int j=0;j<SSIZE;j++)
    {
       cout<<encode[i][j]<<" ";
    }
    cout<<endl;
 }
cout<<endl;
cout<<"bigram:"<<endl;
for(int i=0;i<SSIZE;i++)
{
  for(int j=0;j<SSIZE;j++)
  {
     cout<<bigram[i][j]<<" ";
  }
  cout<<endl;
}
*/
cout<<"temp";
  
  //===delete pointer====//
  for(int i=0;i<SSIZE;i++)
  {
     delete[] EncodeCk[i];
     delete[] encode[i];
     delete[] bigram[i];
  }
  delete[] EncodeCk;
  delete[] encode;
  delete[] bigram;

      return 0;
   
}

void parseText2Int(vector<int> (&textInt),string &text)
{
   char split_char =' ';
   istringstream split(text);
   int num;
   for(string each;getline(split,each,split_char);)
   {
      num = c2n(each);
      if(num>=0)
      {
         textInt.push_back(num);
      }
   }
}


//covert string to number
int c2n(string bu)
{
   if(bu.length() >1 && (bu[1]-'0')>=0 && (bu[1]-'0')<10)
     return 10*(bu[0]-'0')+(bu[1]-'0');
   else
     return (bu[0]-'0');
}


int calcSSIZE(char* filename)
{
  char buffer[20];
  fstream file;
  char split_char = ' ';
  string tempC;;
  int tempNum;
  int maxNum = 0;
  
  file.open(filename,ios::in);
  while(file.getline(buffer,sizeof(buffer),'\n'))
       {
       istringstream str(buffer);
       getline(str,tempC,split_char);
       tempNum = c2n(tempC);
       if(tempNum>maxNum)
          maxNum = tempNum;
       }
  return maxNum+1;
}

void initEncode(char* fileName,bool**EncodeCk,double**encode)
{
   fstream file;
   char buffer[20];
   char split_char = ' ';
   int parser[3];

   file.open(fileName,ios::in);
   if(!file)
     cerr<<"can not read encode file"<<endl;
   else
   {
     //update EncodeCk
     int counter;
     while(file.getline(buffer,sizeof(buffer),'\n'))
     {
        counter =0;
	istringstream str(buffer);
	for(string each;getline(str,each,split_char);counter++)
	    parser[counter] = c2n(each);

	EncodeCk[parser[1]][parser[0]] = bool(parser[2]);	
     }//finish update EncodeCk
   
     //update encode
     double denominator[SSIZE];
     memset(denominator,0,sizeof(denominator));
     for(int i=0;i<SSIZE;i++)
     {
        for(int j=0;j<SSIZE;j++)
        {
           denominator[i] += int(EncodeCk[j][i]);
        }
        for(int j=0;j<SSIZE;j++)
        {
        if(!EncodeCk[j][i])
           encode[j][i] = ZERO;
        else
           encode[j][i] = log(1/denominator[i]);
        }
     }
   }
}

void initBigram(double**bigram,bool**EncodeCk,vector<int>(&textInt))
{
  //parse EncodeCk to a hash 
  vector<vector<int> > encodeIdx;

  for(int i=0;i<SSIZE;i++)
  {
  vector<int> newLine;
     for(int j=0;j<SSIZE;j++)
     {
        if(EncodeCk[i][j]){
           newLine.push_back(j);
	   }
  }
     encodeIdx.push_back(newLine);
  }

  //use text to vote for initial bigram
  double vote[SSIZE][SSIZE];
  memset(vote,0,sizeof(vote));

  vector<int>::iterator it;
  vector<int>::iterator itIdx1;
  vector<int>::iterator itIdx2;
  for(it = textInt.begin();it!=textInt.end()-1;it++)
  {
     for(itIdx1 = encodeIdx[*it].begin();itIdx1!=encodeIdx[*it].end();itIdx1++)
     {
        for(itIdx2 = encodeIdx[*(it+1)].begin();itIdx2!=encodeIdx[*(it+1)].end();itIdx2++)
        {
            vote[*itIdx1][*itIdx2]++;  
        }
      }
   }

   //normalize and covert vote to bigram
   double deno[SSIZE];//denominator: bigram = normalized vote
   memset(deno,0,sizeof(deno));
   for(int i=0;i<SSIZE;i++)
   {
      for(int j=0;j<SSIZE;j++)
      {
         deno[i] += vote[i][j];
      }
      for(int j=0;j<SSIZE;j++)
      {
        if(!vote[i][j])
	  bigram[i][j] = ZERO;
	else
	  bigram[i][j] = log(vote[i][j]/deno[i]);
	//cout<<bigram[i][j]<<" ";
      }
      //cout<<endl;
   }
}

//forward & backwark propagate
//forward:ck = 0; pi = initial state prob
//backward:ck =1; pi = a vector of 0.
void Prop(double**Map,double*pi,bool**EncodeCk,double**encode,double**bigram,vector<int>(&textInt),bool ck)
{
//initialize
   int start,end,step;
   if(ck)//backward propagate
   {
      for(int i=0;i<SSIZE;i++)
         Map[i][textInt.size()-1] = 0;//log(1)
      start =textInt.size()-2;
      end = -1;
      step = -1;
   }
   else//forward propagate
   {
       for(int i=0;i<SSIZE;i++){
          if(encode[textInt[0]][i]<=0)
	    Map[i][0] = pi[i] + encode[textInt[0]][i];
	  else
	    Map[i][0] = ZERO;
	  }
       start = 1;
       end = textInt.size();
       step = 1;
   }
//update map
   for(int i=start;i!=end;i+=step)
   {
      for(int j=0;j<SSIZE;j++)
      {
         if(!EncodeCk[textInt[i]][j])
	   Map[j][i] = ZERO;
	 else
	 {
	    double tempSum = ZERO;
	    for(int k=0;k<SSIZE;k++)
	    {
	      if(ck)
	       tempSum = sumlog(tempSum,Map[k][i-step]+bigram[j][k] + encode[textInt[i]][j]);
	       else
	       tempSum = sumlog(tempSum,Map[k][i-step]+bigram[k][j] + encode[textInt[i]][j]);
	    }
	    Map[j][i] = tempSum;
	 }
      }
   }

}
//calculate log(x1+x2) given p1 = log(x1),p2 = log(x2)
double sumlog(double p1,double p2)
{
   if(p1>0)//ZERO
   {
     if(p2>0)
       return ZERO;
     else
       return p2;
   }
   else//p1<=0
   {
      if(p2>0)
         return p1;
      else
         return p1 + log(1+exp(p2-p1));
   }
}
//EM
void EM(double*pi,bool**EncodeCk,double**encode,double**bigram,vector<int>(&textInt))
{
  double**forMap;
  double**backMap;
  double gamma[SSIZE][textInt.size()];
  double epsilon[SSIZE][SSIZE][textInt.size()-1];

  forMap = new double*[SSIZE];
  backMap = new double*[SSIZE];
  for(int i=0;i<SSIZE;i++)
  {
     forMap[i] = new double[SSIZE];
     backMap[i] = new double[SSIZE];
  }
  //update forward and backward map
  Prop(forMap,pi,EncodeCk,encode,bigram,textInt,0);
  Prop(backMap,pi,EncodeCk,encode,bigram,textInt,1);
  //upadte gama
  for(int i=0;i<textInt.size();i++)
  {
     double sum = ZERO;
     for(int j=0;j<SSIZE;j++)
     {
        if(forMap[j][i]<=0 && backMap[j][i]<=0)
	{
           gamma[j][i] = forMap[j][i] + backMap[j][i];//log(forMap*backMap)
	   sum = sumlog(sum, forMap[j][i] + backMap[j][i]); 
        }
	else
	   gamma[j][i] = ZERO;
     }
     for(int j=0;j<SSIZE;j++)
     {
        if(gamma[j][i]<=0)//non-ZERO
           gamma[j][i] = gamma[j][i] - sum;//log(gamma/sum)
     }
  }
  //update epsilon
  for(int i=1;i<textInt.size();i++)//i start from 1
  {  
     double deno =ZERO;
     for(int j=0;j<SSIZE;j++)
     {
        for(int k=0;k<SSIZE;k++)
	{
	   if(forMap[j][i-1]<=0 && bigram[j][k]<=0 && encode[textInt[i]][k]<=0 && backMap[k][i]<=0)
	   {   
	      epsilon[j][k][i] = forMap[j][i-1] + bigram[j][k] + encode[textInt[i]][k] + backMap[k][i];
	      deno = sumlog(deno,epsilon[j][k][i]);
	   }
	   else
	      epsilon[j][k][i] = ZERO;
	}
     }
     for(int j=0;j<SSIZE;j++)
     {
        for(int k=0;k<SSIZE;k++)
	{	
	   if(epsilon[j][k][i]<=0)//non-ZERO
	     epsilon[j][k][i] = epsilon[j][k][i] - deno;
	}
     }
  }
  //update gama sum
  double gammaSum[SSIZE];
  memset(gammaSum,ZERO,sizeof(gammaSum));
  for(int i=0;i<SSIZE;i++)
  {
     for(int j=0;j<textInt.size();j++)
     {
        if(gamma[i][j]<=0)
           gammaSum[i] = sumlog(gammaSum[i],gamma[i][j]);
     }
  }
  //update epsilon sum
  double epsilonSum[SSIZE][SSIZE];
  memset(epsilonSum,ZERO,sizeof(epsilonSum));
  for(int i=0;i<SSIZE;i++)
  {
     for(int j=0;j<SSIZE;j++)
     {
        for(int k=0;k<textInt.size();k++)
	{
	   if(epsilon[i][j][k]<=0)
	     epsilonSum[i][j] = sumlog(epsilonSum[i][j],epsilon[i][j][k]);
	}
     }
  }
  //re update pi
  for(int i=0;i<SSIZE;i++)
  {
     if(gammaSum[i]<=0)
       pi[i] = gammaSum[i]-log(textInt.size());
     else
       pi[i] = ZERO;
  
  }
  //re update bigram
  for(int i=0;i<SSIZE;i++)
  {
     for(int j=0;j<SSIZE;j++)
     {
        if(epsilonSum[i][j]<=0 && gammaSum[i]<=0)
	  bigram[i][j] = epsilonSum[i][j]-gammaSum[i];
	else
	  bigram[i][j] = ZERO;
     }
  }
  //re update encode
  double gammaO[SSIZE][SSIZE];
  memset(gammaO,ZERO,sizeof(gammaO));
  for(int i=0;i<textInt.size();i++)
  {
     for(int j=0;j<SSIZE;j++)
     {
        if(gamma[j][i]<=0)
	  gammaO[textInt[i]][j]=sumlog(gammaO[textInt[i]][j],gamma[j][i]);
     }
  }

 for(int i=0;i<SSIZE;i++)
 {
    for(int j=0;j<SSIZE;j++)
    {
    if(gammaO[j][i]<=0 && gammaSum[i]<=0)
      encode[j][i] = gammaO[j][i]-gammaSum[i];
    else
      encode[j][i] = ZERO;
    }
 }
  
  //delete
  for(int i=0;i<SSIZE;i++)
  {
     delete[] forMap[i];
     delete[] backMap[i];
  }
  delete[] forMap;
  delete[] backMap;
}
