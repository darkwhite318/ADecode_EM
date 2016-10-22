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

void parseText2Int(vector<int>(&),string);
int c2n(string);
int calcSSIZE(char*);
void initEncode(char*,bool**,double**);
void initBigram(double**,bool**,vector<int>(&));
void forProp(double**,bool**,double**,double**,vector<int>(&));
void backProp(double**,bool**,double**,double**,vector<int>(&));
double sumlog(double,double);
void EM(bool**,double**,double**,vector<int>(&));
void updPath(int,double*,int*,double**,double**);

int SSIZE;
//const double ZERO = 10;//assigned a possitive number as the sybol of log(0)
const double ZERO = -1e50;
const double ZERO_thre = -1e40;

// ./ADecode_EM2 encode.bin test.num out.txt
int main(int argc,char* argv[])
{
   //=========variable=========//
    string text;//parse input text into string
    vector<int> textInt;//parse text into vector<int>
    bool **EncodeCk;//query an encode table is the prob is 0 or not
    double ** encode;//encode table(change in each iteration)
    double ** bigram;//transition prob table
    int ** viterbiMap;

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
  
   //==initialize bigram table==//
   bigram = new double*[SSIZE];
   for(int i=0;i<SSIZE;i++)
   {
      bigram[i] = new double[SSIZE];
   }
   initBigram(bigram,EncodeCk,textInt);
  
 //======EM Algorithm======//
  int iterNum = 10;
  for(int i=0;i<iterNum;i++)
  {

    EM(EncodeCk,encode,bigram,textInt);
 
  }
 //=====output testing===//

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

  //==Viterbi algorithm==//
  viterbiMap = new int*[SSIZE];
  for(int i=0;i<SSIZE;i++)
      viterbiMap[i] = new int[textInt.size()-1];
  
  int *optPath = new int[SSIZE];
  double *viLast = new double[SSIZE];
  for(int i=0;i<SSIZE;i++)
    viLast[i] = encode[textInt[0]][i];
  
  for(int i=0;i<textInt.size()-1;i++)
  {
     updPath(textInt[i],viLast,optPath,bigram,encode);
     for(int j=0;j<SSIZE;j++)
     {
        viterbiMap[j][i] = optPath[j];
     }  
  }
  //==calculate bestPath==//
  double highestScore = ZERO;
  int startIdx =0;
  for(int i=0;i<SSIZE;i++)
  {
    if(viLast[i]>highestScore)
    {
      highestScore = viLast[i];
      startIdx = i;
    }
  }
   int *textIntout = new int[textInt.size()];
   textIntout[textInt.size()-1] = startIdx;
   int bestIdx = startIdx;
   int t = textInt.size()-2;
   while(t>=0)
   {
      bestIdx = viterbiMap[bestIdx][t];
      textIntout[t] = bestIdx;
      t--;
   }

   ofstream outFile;
   char fileLoc[40];
   strcpy(fileLoc,"../");
   strcat(fileLoc,argv[4]);  
   //outFile.open(fileLoc);//not yet output file

  
  //===validation===//
   ifstream validFile;
   textFile.open(argv[4]);
   stringstream vaBuffer;
   vaBuffer<<validFile.rdbuf();
   string textV = vaBuffer.str(); 
   validFile.close();

   //===parse text into a vector<int>==//
   vector<int>textIntV;
   parseText2Int(textIntV,textV);
   int correctNum=0;
   for(int i=0;i<textIntV.size();i++)
   {
      if(textIntV[i] == textIntout[i])
         correctNum++;
   }
   cout<<"accuracy:"<<double(correctNum)/double(textInt.size())<<endl;
  
  //===delete pointers====//
  for(int i=0;i<SSIZE;i++)
  {
     delete[] EncodeCk[i];
     delete[] encode[i];
     delete[] bigram[i];
     delete[] viterbiMap;
  }

  delete[] EncodeCk;
  delete[] encode;
  delete[] bigram;
  delete[] viterbiMap;
  delete[] textIntout;
  delete[] optPath;
  delete[] viLast;

  EncodeCk = NULL;
  encode = NULL;
  bigram = NULL;
  viterbiMap = NULL;
  textIntout = NULL;
  optPath = NULL;
  viLast = NULL;


  return 0;
   
}

void parseText2Int(vector<int> (&textInt),string text)
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
  string tempC;
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
   
   for(int i=0;i<SSIZE;i++)
   {
      for(int j=0;j<SSIZE;j++)
      {
         EncodeCk[i][j] = false;
      }
   }

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

	EncodeCk[parser[1]][parser[0]] = static_cast <bool>(parser[2]);	
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
 	        {
              encode[j][i] = ZERO;
              //cout<<"0"<<" ";
	        }
           else
	         {
	     // cout<<"1"<<" ";
              encode[j][i] = -log(denominator[i]);
           }
	//cout<<encode[j][i]<<" ";
	      }
	//cout<<endl;
     }
   }
}

void initBigram(double**bigram,bool**EncodeCk,vector<int>(&textInt))
{
  
  //use text to vote for initial bigram
  double vote[SSIZE][SSIZE];
  //memset(vote,0,sizeof(vote));
  for(int i=0;i<SSIZE;i++)
  {
     for(int j=0;j<SSIZE;j++)
     {
        vote[i][j] =0;
     }
  }

 for(int i=1;i<textInt.size();i++)
  {
  //cout<<textInt[i]<<" ";
     for(int j=0;j<SSIZE;j++)
     {
        if(EncodeCk[textInt[i-1]][j])
	{
          for(int k=0;k<SSIZE;k++)
	  {
	      if(EncodeCk[textInt[i]][k])
                 vote[j][k]++;  
          }
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
      //	cout<<bigram[i][j]<<" ";
      }
      //cout<<endl;
   }

}

//forward & backwark propagate
//forward:ck = 0; pi = initial state prob
//backward:ck =1; pi = a vector of 0.
void forProp(double**Map,bool**EncodeCk,double**encode,double**bigram,vector<int>(&textInt))
{

//initialize

       for(int i=0;i<SSIZE;i++)
       {
          if(encode[textInt[0]][i]>ZERO_thre)
	           Map[i][0] = encode[textInt[0]][i];	    
	        else
	          Map[i][0] = ZERO;
	    }

double alpha;
//update map
   for(int i=1;i<textInt.size();i++)
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
	       alpha = Map[k][i-1] + bigram[k][j];
		     if(Map[k][i-1]<ZERO_thre || bigram[k][j]<ZERO_thre)
		       alpha = ZERO;
	       tempSum = sumlog(tempSum,alpha);
	   }
	   if(tempSum<ZERO_thre)
	      Map[j][i] = ZERO;
	   else
	      Map[j][i] = tempSum + encode[textInt[i]][j];
	    //cout<<"("<<j<<","<<i<<")"<<Map[j][i];
	 }
      }
      //cout<<endl;
   }

}


void backProp(double**Map,bool**EncodeCk,double**encode,double**bigram,vector<int>(&textInt))
{//not yet done

//initialize
      for(int i=0;i<SSIZE;i++)
      {   
	       Map[i][textInt.size()-1] = 0;//log(1)
      }
      //cout<<"tsize:"<<textInt.size()-1<<endl;

double beta;
//update map
   for(int i=textInt.size()-2;i>=0;i--)
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
	          beta = Map[k][i+1] + bigram[j][k] + encode[textInt[i+1]][k];
	          if(Map[k][i+1]<ZERO_thre || bigram[j][k]<ZERO_thre || encode[textInt[i+1]][k]<ZERO_thre)
		          beta = ZERO;
	          tempSum = sumlog(tempSum,beta);
	       }
	       if(tempSum<ZERO_thre)
	         Map[j][i] = ZERO;
	       else
	         Map[j][i] = tempSum;
	    //cout<<"("<<j<<","<<i<<")"<<Map[j][i];
	      }
     }
      //cout<<endl;
   }

}

//calculate log(x1+x2) given p1 = log(x1),p2 = log(x2)
double sumlog(double p1,double p2)
{
   if(p1<ZERO_thre)//ZERO
   {
     if(p2<ZERO_thre)
       return ZERO;
     else
       return p2;
   }
   else//p1 non-ZERO
   {
      if(p2<ZERO_thre)
         return p1;
      else
     {
	      if(p1-p2 > 4)//test if this criteria rail
	        return p1;
	      else if(p2-p1 > 4)
	        return p2;
	      else
	      {
	        //if((p1 + log(1+exp(p2-p1)))>0)
	        //cout<<"("<<p1<<","<<p2<<")";
          return p1 + log(1+exp(p2-p1));
	       //return log(exp(p1)+exp(p2));
	      }
	   }
   }
}
//EM

void EM(bool**EncodeCk,double**encode,double**bigram,vector<int>(&textInt))
{
  double**forMap;
  double**backMap;
  //double gamma[SSIZE][textInt.size()];

  forMap = new double*[SSIZE];
  backMap = new double*[SSIZE];
  for(int i=0;i<SSIZE;i++)
  {
     forMap[i] = new double[textInt.size()];
     backMap[i] = new double[textInt.size()];
  }
  //int tSize = textInt.size();
  //cout<<"tSize:"<<tSize<<endl;
  //vector<int>::iterator iter;
  //for(iter= textInt.begin();iter!=textInt.end();iter++)
  //   cout<<*iter<<" ";

  //update forward and backward map
  forProp(forMap,EncodeCk,encode,bigram,textInt);//garentee no element bigger than 0 except ZERO
  backProp(backMap,EncodeCk,encode,bigram,textInt);//garentee no element bigger than 0 except ZERO
  

 
  //upadte gamaO, gammaSum
  double gammaSum[SSIZE];
  double gammaO[SSIZE][SSIZE];
  double gamma_per_c[SSIZE];
  for(int i=0;i<SSIZE;i++)
  {
    gammaSum[i] = ZERO;
    for(int j=0;j<SSIZE;j++)
     {
       gammaO[i][j] = ZERO;
     }   
  }


  for(int i=0;i<textInt.size();i++)
  {
     double sum = ZERO;
     for(int j=0;j<SSIZE;j++)
     {
        if(forMap[j][i]>ZERO_thre && backMap[j][i]>ZERO_thre)
	      {
           gamma_per_c[j] = forMap[j][i] + backMap[j][i];//log(forMap*backMap)
	         sum = sumlog(sum, forMap[j][i] + backMap[j][i]); 
        }
	      else
	         gamma_per_c[j] = ZERO;
     }
     for(int j=0;j<SSIZE;j++)
     {
        if(gamma_per_c[j]>ZERO_thre)//non-ZERO
        {
	        //if(gamma[j][i]>=sum)
          // cout<<"("<<gamma[j][i]<<","<<sum<<")";
          gamma_per_c[j] = gamma_per_c[j] - sum;//log(gamma/sum)
          gammaSum[j] = sumlog(gammaSum[j],gamma_per_c[j]);
          gammaO[textInt[i]][j] = sumlog(gammaO[textInt[i]][j],gamma_per_c[j]);
        }   
     }

  }
  
  //update epsilon sum
  double epsilonSum[SSIZE][SSIZE];
  double epsilonTemp[SSIZE][SSIZE];
  for(int i=0;i<SSIZE;i++)
  {
    for(int j=0;j<SSIZE;j++)
    {
      epsilonSum[i][j] = ZERO;
      epsilonTemp[i][j] = ZERO;
    }
  }  
  
  
  for(int i=1;i<textInt.size();i++)//i start from 1
  { 
     double deno =ZERO;
     for(int j=0;j<SSIZE;j++)
     {
        for(int k=0;k<SSIZE;k++)
	      {
	         if(forMap[j][i-1]>ZERO_thre && bigram[j][k]>ZERO_thre && encode[textInt[i]][k]>ZERO_thre && backMap[k][i]>ZERO_thre)
	         {   
	           epsilonTemp[j][k] = forMap[j][i-1] + bigram[j][k] + encode[textInt[i]][k] + backMap[k][i];
	           deno = sumlog(deno,epsilonTemp[j][k]);
	         }
	         else
	           epsilonTemp[j][k] = ZERO;
	      }
     }
     for(int j=0;j<SSIZE;j++)
     {
        for(int k=0;k<SSIZE;k++)
	      {	
	         if(epsilonTemp[j][k]>ZERO_thre)//non-ZERO
	         {
	           epsilonTemp[j][k] = epsilonTemp[j][k] - deno;
	           epsilonSum[j][k] = sumlog(epsilonSum[j][k],epsilonTemp[j][k]);
	         }
	      }
     }
  }

  //re update bigram
  for(int i=0;i<SSIZE;i++)
  {
     for(int j=0;j<SSIZE;j++)
     {
        if(epsilonSum[i][j]>ZERO_thre && gammaSum[i]>ZERO_thre)
	         bigram[i][j] = epsilonSum[i][j]-gammaSum[i];
	      else
	         bigram[i][j] = ZERO;
     }
  }

//update encode table

 for(int i=0;i<SSIZE;i++)
 {
    for(int j=0;j<SSIZE;j++)
    {
    if(gammaO[j][i]>ZERO_thre && gammaSum[i]>ZERO_thre)
      encode[j][i] = gammaO[j][i]-gammaSum[i];
    else
      encode[j][i] = ZERO;
    }
 } 

  //normalize encode and bigram table
  double deno_encode;
  double deno_bigram;
  for(int i=0;i<SSIZE;i++)
  {
    deno_encode = ZERO;
    for(int j=0;j<SSIZE;j++)
    {
      deno_encode = sumlog(deno_encode,encode[i][j]);
      deno_bigram = sumlog(deno_bigram,bigram[i][j]);
    }
    for(int j=0;j<SSIZE;j++)
    {
      if(encode[i][j]>ZERO_thre)
        encode[i][j] = encode[i][j]-deno_encode;
      if(bigram[i][j]>ZERO_thre)
        bigram[i][j] = bigram[i][j]-deno_bigram;
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
  forMap =NULL;
  backMap =NULL;

}
void updPath(int nowIdx,double* viLast,int *viterbiLast,double**bigram,double**encode)
{
   double viMax,viTemp;
   double viLastTemp[SSIZE];
   int viIndex=0;
   for(int j=0;j<SSIZE;j++)
     {
         viMax = ZERO;
         viTemp = ZERO;
        for(int k=0;k<SSIZE;k++)
        {
            viTemp = viLast[k] + bigram[k][j] + encode[nowIdx][j];
            if(viTemp>viMax)
            {
              viMax = viTemp;
              viIndex = k;
            }
        }
        viLastTemp[j] = viMax;
        viterbiLast[j] = viIndex;//viterbiMap[j][i-1] corresponds to the index of i-1 character at state j
     }
     for(int j=0;j<SSIZE;j++)
     {
        viLast[j] = viLastTemp[j];
     }
}
