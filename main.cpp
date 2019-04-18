//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                             Gcode_translator_for_Delta
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*Fore translating the G-Codes the file format needs to be changed from ".g" or ".gcode" or ".gco" to ".txt". In the same directory where the main.cpp file is stored, there must be at least two more files - "settings.txt" and "targets.txt". Save the G-Code files in the same directory
The file "settings.txt" contains the following parameters of the Delta robot
arm_length_in_mm 230
height _____
radius_of_platform  _____
radius_of_base_plate ____
offset  ____
Measure above quantities from your machine and update the file.
The file "targets.txt" lists the G-Code files needed to be translated
Hardware:
Upload the cartesian firmware Sprinter/Marlin to your CNC machine. Connect the motor controlling the movement of the carriages arbitrarily to the X Y and Z motor point of the controlling board. Connect the corresponding limit switches to the minimum points as well.
Calibration:
 Install interface software like pronterface/Repitier-Host in your PC. connect your CNC machine to your PC through any of the interface software. Increment the x coordinate (as denoted in the interfacing software) by 10 unit. Then the carriage being connected with the X-motor must move downward by 10 mm. If it moves in the opposite direction, invert ht econnection.If it has not moved by 10mm, let's say it has moved by x1. Now go to "congiguration.h" file in the firmware. There must be a line like-
#define _AXIS_STEP_PER_UNIT {x2, y2, z2,e2} 
Now replace x2 by (x1/x2)*10. 
Repeat the same steps for Y-motor and Z-motor as well.
If the machine is used for 3D printing perform the same steps for extruder as well. 
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
	float ha=sqrt((l*l)-(aa*aa));                                    
	float Az=H-(z+d+ha);
return Az;
}
float cal_y(float x,float y, float z, float d, float r, float R, float l,float H)
{	float b1x=x+r*(-0.5);
	float b1y=y+r*(0.866);
	float b1z=z+d;
	float b2x=R*(-0.5);
        float b2y=R*(0.866);
	float ab=sqrt((b2x-b1x)*(b2x-b1x)+(b2y-b1y)*(b2y-b1y));                  
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
{       float x[10]; char ch;int gcde=0;int mcde=0;
        bool got_values=false;
        bool got_F=false;
	bool got_E=false;
        bool got_only_z=false;
        bool got_G_M=false;
	char ch_x;char ch_y;char ch_z;char ch_s; char ch_f;char g28;
        float x_value=0.0;float y_value=0.0;float z_value=0.0;float e_value=0.0;  float f_value=0.0; 
        float s_value=0.0; ;
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
            if(gcde==0||gcde==00||gcde==01||gcde==1)
              {
                infile3>>ch;
      switch(ch)
{ 
//The following case reads the G0/G1 argument of type
//(X_ Y_ Z_),(X_ Y_ Z_ E_ F_),(X_ Y_ Z_ E_),(X_ Y_ Z_ F_),(X_ Y_ E_ F_),(X_ Y_ E_),
//(X_ Y_ F_),(X_ Y_),(X_ Z_),(X_ Z_ E_ F_),(X_ Z_ E_),(X_ Z_ F_),(X_ E_ F_),(X_ E_),(X_)
case 'X':  infile3>>x_value;//reads x value
           infile3>>ch;
           switch(ch)      		
           {   
           case 'Y':infile3>>y_value;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
                              //X_ Y_ Z_ 
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                              infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                               //X_ Y_ Z_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
                                //X_ Y_ Z_ E_
                                       }
                            if(ch=='F')
                               infile3>>f_value;
                               got_F=true;
                               //X_ Y_ Z_ F_
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                                 //X_ Y_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
                                 //X_ Y_ E_
                            break;
	   	   case 'F':infile3>>f_value; got_F=true;
                            //X_ Y_ F_
                            break;
	           case 'G':got_G_M=true;
			    //X_ Y_
                            break;
                    }
                        break;
           case 'Z':infile3>>z_value;
                    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
                              //X_ Z_   
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                              infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                               //X_ Z_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
                                //X_ Z_ E_
                                       }
                            if(ch=='F')
                               infile3>>f_value;
                               got_F=true;
                               //X_ Z_ F_
                     break;
	   case 'E':infile3>>e_value;
                     got_E=true;
                     infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                               //X_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
                                //X_ E_
		    break;
	   case 'F':infile3>>f_value;
                     //X_ F_
                     got_F=true;
		    break;
           case 'G':got_G_M=true;
                    //X_
		    break;
           }
	   break; 
//The following case reads the G0/G1 argument of type
//(Y_ Z_),(Y_ Z_ E_ F_),(Y_ Z_ E_),(Y_ Z_ F_),(Y_ E_ F_),(Y_ E_),(Y_ F_),(Y_)
case 'Y':  infile3>>y_value;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
			      //Y_ Z_
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                              infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                                 //Y_ Z_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
                                 //Y_ Z_ E_
                                       }
                            if(ch=='F')
                               {infile3>>f_value;
			       //Y_ Z_ F_
                               got_F=true;}
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                                 //Y_ E_ F_
                              if(ch=='G')
                                 got_G_M=true;
		                 //Y_ E_
                            break;
	   	   case 'F':infile3>>f_value; 
                            //Y_ F_
                            got_F=true;
                            break;
                   case 'G':got_G_M=true;
                            //Y_ 
                            break;
                    }
         break;
//The following case reads the G0/G1 argument of type
//(Z_),(Z_E_ F_),(Z_E_),(Z_,F_)
case 'Z':  infile3>>z_value;
           infile3>>ch;
	   if(ch=='G')
              got_G_M=true;
              //Z_
	   if(ch=='E'){
	      infile3>>e_value;
	      got_E=true;
              infile3>>ch;
	      if(ch=='F')
                {
                infile3>>f_value;
                got_F=true;
                //Z_ E_ F_	        
		got_F=true;}
              if(ch=='G')
                got_G_M=true;
                //Z_ E_
                      }
            if(ch=='F')
               {infile3>>f_value;
               got_F=true;}
               //Z_ F_
           break;
//The following case reads the G0/G1 argument of type
//(E_ F_),(E_)   
case 'E':  infile3>>e_value;
           infile3>>ch;
	   if(ch=='F')
             {
             infile3>>f_value;
             got_F=true;}
             //E_ F_
           if(ch=='G')
               got_G_M=true;
               //E_
         break; 
//The following case reads the G0/G1 argument of type
//(F_ X_ Y_ Z_),(F_ X_ Y_ Z_ E_),(F_ X_ Y_),(F_ X_ Z_),(F_ X_ Z_ E_),(F_ X_ E_),(F_ X_)
//(F_ Y_ Z_),(F_ Y_ Z_ E_),(F_ Y_ E_),(F_ Y_),(F_ Z_),(F_ Z_ E_)
case 'F':infile3>>f_value;
         got_F=true;
         infile3>>ch;
         switch(ch)
         {
         case 'X':  infile3>>x_value;//reads x value
           infile3>>ch;
           switch(ch)      		
              {   
              case 'Y':infile3>>y_value;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
                              //F_ X_ Y_ Z_
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                                       }
                               //F_ X_ Y_ Z_ E_
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            //F_ X_ Y_ E_
                            break;
                   case 'G':got_G_M=true;
                            //F_ X_ Y_
                            break;
                    }
                        break;
              case 'Z':infile3>>z_value;
                       infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
			      //F_ X_ Z_
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;}
                              //F_ X_ Z_ E_ 
                       break;
	      case 'E':infile3>>e_value;
                     got_E=true;
                     //F_ X_ E_
		    break;
              case 'G':got_G_M=true;
                    //F_ X_
		    break;
               }
	         break;                                                
         case 'Y':  infile3>>y_value;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
                              //F_ Y_ Z_
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                                        }
                              //F_ Y_ Z_ E_
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            //F_ Y_ E_
                            break;
                   case 'G':got_G_M=true;
                            //F_ Y_
                            break;
                    }
               break;
       case 'Z':  infile3>>z_value;
                  infile3>>ch;
	   if(ch=='G')
              got_G_M=true;
              //F_ Z_
              if(ch=='E'){
	      infile3>>e_value;
	       got_E=true;
                         }
              //F_ Z_ E_
           break;
         }      

     
}   
	       x_value_new=cal_x(x_value,y_value,z_value,x[4],x[2],x[3],x[0],x[1]);
               y_value_new=cal_y(x_value,y_value,z_value,x[4],x[2],x[3],x[0],x[1]);
	       z_value_new=cal_z(x_value,y_value,z_value,x[4],x[2],x[3],x[0],x[1]);
	       if(gcde==01||gcde==1)
                                 outfile<<"G1";
               if(gcde==00||gcde==0)
                                 outfile<<"G0";                
		outfile<<" X"<<x_value_new<<" "<<"Y"<<y_value_new<<" Z"<<z_value_new;
		if(got_E==true) outfile<<" "<<"E"<<e_value;                
		if(got_F==true) outfile<<" "<<"F"<<f_value;
                 outfile<<"\n";
}
////end of if(gcde==0||gcde==00||gcde==01||gcde==1)
if(gcde==28) 
               {infile3>>ch;
               if(ch=='X') outfile<<"G28 X\n";
                           else {if (ch=='Y') outfile<<"G28 Y\n";
                              else {if (ch=='Z') outfile<<"G28 Z\n";else {outfile<<"G28\n";if(ch !='X'||ch !='Y'||ch !='Z')     goto exit2;}}}
               } // end of reading and writing G-code G28.        
if(gcde==90)  outfile<<"G90 ;use absolute coordinate"<<endl;
if(gcde==21)  outfile<<"G21 ; set units to millimeters"<<endl;
if(gcde==92) 
           {infile3>>ch;
switch(ch)
{ 
case 'X':  infile3>>x_value;
           got_X=true;
           infile3>>ch;
           switch(ch)      		
           {   
           case 'Y':infile3>>y_value;
                    got_Y=true;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
                            got_Z=true;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                                        }
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            break;
	           case 'G':got_G_M=true;
                            break;
                    }
                        break;
           case 'Z':infile3>>z_value;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
			      if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                              infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
                               if(ch=='G')
                                 got_G_M=true;
                                       }
                            if(ch=='F')
                               {infile3>>f_value;
			       got_F=true;}
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            infile3>>ch;
			      if(ch=='F')
                                 {
                                 infile3>>f_value;
				 got_F=true;}
			      if(ch=='G')
                                 got_G_M=true;
		            break;
           case 'G':got_G_M=true;
		    break;
           }
	   break;                                                
case 'Y':  infile3>>y_value;
		    got_Y=true;
 		    infile3>>ch;
		   switch(ch)
                   {                	
                   case 'Z':infile3>>z_value;
                            got_Z=true;
			    infile3>>ch;
			    if(ch=='G')
                              got_G_M=true;
			    if(ch=='E'){
			      infile3>>e_value;
                              got_E=true;
                                        }
                            break;
                   case 'E':infile3>>e_value;
                            got_E=true;
                            break;
                   case 'G':got_G_M=true;
                            break;
                    }
         break;
case 'Z':  infile3>>z_value;
           got_Z=true;
           infile3>>ch;
	   if(ch=='G')
              got_G_M=true;
	   if(ch=='E')
	      {infile3>>e_value;got_E=true;}
           break;    
case 'E':  infile3>>e_value;
           got_E=true;
           break;
case 'G':  got_G_M=true;
           break;  
}
//end of switch statemen
outfile<<"G92";
if(got_X==true)
          outfile<<" X"<<x_value;
if(got_Y==true)
	  outfile<<" Y"<<y_value;
if(got_Z==true)
          outfile<<" Z"<<z_value;
if(got_E==true)
          outfile<<" E"<<e_value;
outfile<<"\n";
if(got_G_M==true) goto exit2;
}//end of if(gcde==92)

}
//end of reading G-Codes
if(ch=='M')
          {infile3>>mcde;
switch(mcde)
{
	case 82 :outfile<<"M82 ; use absolute distances for extrusion"<<endl;
		 break;
	case 104:infile3>>ch_s;
                 infile3>>s_value;
                 outfile<<"M104 S"<<s_value;
                 outfile<<" ;set temperature"<<endl;
                 break;
	case 106:infile3>>ch_s;
                 infile3>>s_value;
                 outfile<<"M106 S"<<s_value<<endl;
                 break;
	case 107:outfile<<"M107"<<endl;
		 break;
	case 109:infile3>>ch_s;
                 infile3>>s_value;
                 outfile<<"M109 S"<<s_value<<endl;
                 break;
	case 140:infile3>>ch_s;
                 infile3>>s_value;
                 outfile<<"M140 S"<<s_value<<endl;
                 break;
	case 190:infile3>>ch_s;
                 infile3>>s_value;
                 outfile<<"M190 S"<<s_value;
		 outfile<<" ;wait for bed temperature to be reached"<<endl;
                 break;
}//end of switch(mcde) 
           }
//End of reading and writing M-codes
if(ch==';')
         {infile3>>comment;
          outfile<<";"<<comment<<endl;
         }
//end of reading and writing comments (starting with ';')
         }//end of while
} //end of for loop
	return 0;
}
