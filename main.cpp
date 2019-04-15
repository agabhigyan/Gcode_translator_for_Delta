/*Fore translating the G-Codes the file format needs to be changed from ".g" or ".gcode" or ".gco" to ".txt". In the same directory where the main.cpp file is stored, there must be at least two more files - "settings.txt" and "targets.txt". Save the G-Code files in the same directory
Calibration:
Upload cartesian firmware Sprinter/Marlin to your CNC machine. Connect your 
*/
#include<iostream>
#include<math.h>
#include<fstream>
#include<stdlib.h>
#include<string>
#include <sstream>
float cal_x(float x,float y, float z, float d, float r, float R, float l,float H)
{ 	float a1x=x+r;
	float a1y=y;
	float a1z=z+d;
	float a2x=R;
        float a2y=0;
        float aa=sqrt((a2x-a1x)*(a2x-a1x)+(a2y-a1y)*(a2y-a1y));
	float ha=sqrt((l*l)-(aa*aa));                                     //sqrt((l*l)–(aa*aa));
	float Az=H-(z+d+ha);
return Az;
}
float cal_y(float x,float y, float z, float d, float r, float R, float l,float H)
{	float b1x=x+r*(-0.5);
	float b1y=y+r*(0.866);
	float b1z=z+d;
	float b2x=R*(-0.5);
        float b2y=R*(0.866);
	float ab=sqrt((b2x-b1x)*(b2x-b1x)+(b2y-b1y)*(b2y-b1y));                   sqrt((b2x-b1x)*(b2x-b1x)+(b2y-b1y)*(b2y-b1y));
        float hb=sqrt((l*l)-(ab*ab));// hb=sqrt((l*l)–(ab*ab));
        float Bz=H-(z+d+hb);
return Bz;
}
float cal_z(float x,float y,float z, float d, float r, float R, float l,float H)
{	float c1x=x+x+r*(-0.5);
	float c1y=y+r*(-0.866);
	float c1z=z+d;
   	float c2x=R*(-0.5);
        float c2y=R*(-0.866);
	float ac=sqrt((c2x-c1x)*(c2x-c1x)+(c2y-c1y)*(c2y-c1y)); //ac = sqrt((c2x-c1x)*(c2x-c1x) + (c2y-c1y)*(c2y-c1y));
	float hc	=sqrt((l*l)-(ac*ac)); // hc = sqrt((l*l) – (ac*ac));
        float Cz=H-(z + d + hc);
return Cz;
}
using namespace std;
int main()
{       float x[10]; char ch;int gcde=0;
        bool got_values=false;
        bool got_F=false;
	bool got_E=false;
        bool got_only_z=false;
        bool check_quality=false;
	bool got_G_M=false;
	char ch_x;char ch_y;char ch_z; char ch_f;char g28;
        float x_value=0.0;float y_value=0.0;float z_value=0.0;float e_value=0.0;  float f_value=0.0; 
        float x_value_new=0.0;float y_value_new=0.0;float z_value_new=0.0; 
	
    	char buffer[80];  
        char data[80];char files[80];
        string str[10];//name of the files will be stored
        string strNew[10];
        string mod="modified_for_Delta_";
        int NUMofFiles=0.0;
	int i=0;
	ifstream infile("settings.txt"); //opening file for reading the data
	if (! infile)
	{ cout<<"\n The file \"settings.txt\"  must be there in the current directory for the required settings of the specific configuration of SCARA."<<endl;
	exit(1);
	}
	while(!infile.eof())
	{ infile.getline(data,80);
          istringstream in(data);      //make a stream for the line itself
           std::string type;
           in >> type;
         in >> x[i];
         i=i+1;
    	}
         cout<<"Arm length in milimeter :"<<x[0]<<endl;
        cout<<"Height :"<<x[1]<<endl;
        cout<<"radius_platform in milimeter :"<< x[2]<<endl;
        cout<<"radius_base_plate :"<< x[3]<<endl;
        cout<<"offset :"<< x[4]<<endl;
     	infile.close();
        ifstream infile2("targets.txt");
        
        if (! infile2)
	{ cout<<"\n The file \"targets.txt\"  must be there in the current directory for the required settings of the specific configuration of SCARA."<<endl;
	exit(1);
	} int j=0;
       while(!infile2.eof())
        {j++;infile2>>str[j];} int a=j;
         cout<<"The G-code files are : ";
         NUMofFiles=j-1;
         for(int i=1;i<=NUMofFiles;i++)
         cout<<str[i]<<"\t";
         cout<<"\nNumber of Gcode files to be processed :"<<NUMofFiles<<endl;
        




for(int k=1;k<=NUMofFiles;k++) /////////starting of for loop
       {  
       got_values=false;
       got_F=false;
       got_E=false;
       got_only_z=false;
       check_quality=false;
	got_G_M=false;
	x_value=0.0;y_value=0.0;z_value=0.0;e_value=0.0;f_value=0.0; 
        x_value_new=0.0;y_value_new=0.0;z_value_new=0.0; 
	gcde=0;
       	ifstream infile3(str[k].c_str()); //reads the file str1 for G-Code
        if (! infile3)
	{ cout<<"\n The target files do not exist"<<endl;
	exit(1);
	}
        str[k].insert(0, mod);
        str[k].erase(str[k].size()-3,3);
        str[k]=str[k]+"g";
     	ofstream outfile(str[k].c_str()); //writing G-code to a new file
        while(!infile3.eof())
        {infile3>>ch;
         exit2: if(infile3.eof()!=false) break;
         if(ch=='G')
          {infile3>>gcde;got_only_z=false;got_values=false;got_F=false;
            if(gcde==0)
              {
                infile3>>ch;
      switch(ch)
{ 
case 'X':  infile3>>x_value;//reads x value
           infile3>>ch;
           switch(ch)      		
           {   
           case 'Y': infile3>>y_value; got_values=true;
 		        infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            break;
	   	   case 'F':infile3>>f_value; got_F=true;
                            break;
                   case 'G':got_G_M=true;
                            break;
                    }
                        break;
           case 'Z':cout<<"lala:";
		    break;
	   case 'E':cout<<"lala:";
		    break;
           case 'G':cout<<"lala:";
		    break;
           }
	   break;                                                
case 'Y':  infile3>>y_value;
           infile3>>ch;
           if(ch=='Z')  {infile3>>y_value;} 
         break;
case 'Z':  infile3>>z_value;got_only_z=true;
         break;    
case 'E':  infile3>>e_value;
         break;       
}     } //done reading x,y and z values...........G0 code
                  
                
              if(gcde==28) 
               {infile3>>ch;
               if(ch=='X') outfile<<"G28 X\n";
                           else {if (ch=='Y') outfile<<"G28 Y\n";
                              else {if (ch=='Z') outfile<<"G28 Z\n";else {outfile<<"G28\n";if(ch !='X'||ch !='Y'||ch !='Z')     goto exit2;}}}
               } //////////////////////G28
               exit1:
               if(got_values==true)
               { 
	       cout<<x_value<<"\t"<<y_value<<"\t"<<z_value<<"\t"<<f_value<<endl;
	       x_value_new=cal_x(x_value,y_value,z_value,x[4],x[2],x[3],x[0],x[1]);
               y_value_new=cal_y(x_value,y_value,z_value,x[4],x[2],x[3],x[0],x[1]);
	       z_value_new=cal_z(x_value,y_value,z_value,x[4],x[2],x[3],x[0],x[1]);
                outfile<<"G0"<<" "<<"X"<<x_value_new<<" "<<"Y"<<y_value_new<<" Z"<<z_value_new;
		if(got_E==true) outfile<<" "<<"E"<<e_value;                
		if(got_F==true) outfile<<" "<<"F"<<f_value;
                 outfile<<" ;original point\n";
               }
            }//end of reading G-value
         }//end of while
} //end of for loop


       
     	
	return 0;
}
