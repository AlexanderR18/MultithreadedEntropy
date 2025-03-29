#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cmath>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <iomanip>


struct data
{
  std :: map<std :: string,int> trackFreq;
  std :: vector<int> extraFrequencies;
  std :: vector< std:: string> tasksVec;
  std :: vector<double> entropyVec;
};


void* threadFunction(void *arg_ptr)
{
    data* arg = (data*) arg_ptr;

    int newFrequency=0;             //Current Frequency + extra frequency used as next current frequency
    int Freq=0;
    int currentFreq=0;              //Running total of Frequencies
    std :: string selectedTask;     //CurrentTask is letter of current Task
    int extraFrequency;             //Frequency of current Task
    double H=0;                     //The entropy calculation of the current term
    double currH=0;                 //CurrH is the running total of all entropys
    double newTerm =0;
    double currentTerm=0;           //currentTerm will equal 0 unless the selectedTask is a duplicate in which 
    
                                    //case it will equal the frequency of the previous selectedTask


 
   for(int i=0;i<arg->tasksVec.size();i++)
   {
   currH =H;
   selectedTask = arg->tasksVec[i].substr(0,1);
   //cout << "The current Selected Task is " << selectedTask << endl;

   extraFrequency = arg->extraFrequencies[i]; 
   //cout << "The current extra freq is " << extraFrequency << endl;

   //cout << "The current frequency is " << currentFreq << endl;

   newFrequency = currentFreq + extraFrequency;
   //cout << "The current new freq is " << newFrequency << endl;

    if(newFrequency == extraFrequency)
    {
        H=0;
        arg->entropyVec.push_back(H);
        //cout << "H = 0 " << endl;
        arg->trackFreq[selectedTask] = extraFrequency;
        currentFreq += extraFrequency;
        continue;
    }
    
    else if(arg->trackFreq[selectedTask] == 0)
        {
            
            currentTerm = 0;
        }
    else
        {

            currentTerm = arg->trackFreq[selectedTask] * log2(arg->trackFreq[selectedTask]);
        
           
        }

            newTerm = (arg->trackFreq[selectedTask] + extraFrequency) * log2(arg->trackFreq[selectedTask] + extraFrequency);
            //cout << "newTerm is " << newTerm <<endl;

            H = log2(newFrequency) - ((log2(currentFreq) - currH) * (currentFreq) - currentTerm + newTerm) / newFrequency;

            arg->trackFreq[selectedTask] += extraFrequency;

            currentFreq += extraFrequency;

            arg->entropyVec.push_back(H);
            
   }
  

    return nullptr;
}


int main() {

  std :: string line;
  int NTHREADS=0;

  std :: map< std :: string,int> trackingFrequency;
  std :: vector<int> frequencies;
  std :: vector< std :: string> tasks;
  std :: vector<double> Entropy;
  

  std :: ifstream in("input.txt");
  //ofstream out("output.txt");

   std :: vector<std :: string> readingInput;
  
    //Reads input first to get number of threads
    
    while(std::getline(in,line))
    {
        readingInput.push_back(line);
        NTHREADS++; 
    }
    
    data structure[NTHREADS];
    pthread_t tid[NTHREADS];

 
    //For the number of threads begin to parse input
    for(int i=0;i<NTHREADS;i++)
    {
      trackingFrequency.clear();
      frequencies.clear();
      tasks.clear();

      //Entropy.clear();

      std :: string hold = readingInput[i];
      std :: string copy = readingInput[i];
      
     
      while(hold.length() != 0)
        {
          if(hold.substr(0,hold.find(" ")) >= "A" && hold.substr(0,hold.find(" ")) <= "Z" || hold.substr(0,hold.find(" ")) >= "a" && hold.substr(0,hold.find(" ")) <= "z")
          tasks.push_back(hold.substr(0,hold.find(" ")));
          else
          {
            int n;
            n=stoi(hold.substr(0,hold.find(" ")));
            frequencies.push_back(n);
          }
          
          //cout << hold << endl;
          if(hold.find(" ") != -1)
          hold.erase(0,hold.find(" ") +1); 
          else
          hold.erase(0,hold.length()); 
          
        }

        for(int i=0;i<tasks.size();i++)
        {
            trackingFrequency.insert( std :: pair< std::string, int> (tasks[i],0));
        }
          
        structure[i] = {trackingFrequency, frequencies, tasks, Entropy};
       
        if (pthread_create(&tid[i], NULL, threadFunction, &structure[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
        
    }


    // Wait for threads to finish.
    for (int i = 0; i < NTHREADS; i++)
    {
        pthread_join(tid[i],nullptr);
    }


    std :: string schedulingInformation;
    std :: vector< std :: string> outString;
    //Take scheduling information from calculated frequencies and currentTask letters and make a vector to print
    for(int i=0;i<NTHREADS;i++)
        {

            schedulingInformation="";

            for(int j=0;j<structure[i].extraFrequencies.size();j++)
            {
                if(j == structure[i].extraFrequencies.size()-1)
                {
                schedulingInformation.append(structure[i].tasksVec[j]);
                schedulingInformation.append("(");
                schedulingInformation.append(std :: to_string(structure[i].extraFrequencies[j]));
                schedulingInformation.append(")");
                }
                else
                {
                schedulingInformation.append(structure[i].tasksVec[j]);
                schedulingInformation.append("(");
                schedulingInformation.append(std :: to_string(structure[i].extraFrequencies[j]));
                schedulingInformation.append(")");
                schedulingInformation.append(",");
                schedulingInformation.append(" ");
                }
            }

            outString.push_back(schedulingInformation);



            std :: cout << std :: endl;
        }
        
    
    //Last print with all of task scheduling information from cpo 1 to cpu NTHREADS
    for(int i=0;i<NTHREADS;i++)
   {
        
        std :: cout << "CPU " << i+1 << std :: endl;
        std :: cout << "Task scheduling information: " << outString[i] << std :: endl;
        std :: cout << "Entropy for CPU " << i+1 << std :: endl;
        
        for(int j=0;j<structure[i].entropyVec.size();j++)
        {
            printf("%.2f",structure[i].entropyVec[j]);
            std :: cout << " ";  
        }
        

        std :: cout << std :: endl;  
        std :: cout << std :: endl; 
   } 
    


  
  return 0;
}
