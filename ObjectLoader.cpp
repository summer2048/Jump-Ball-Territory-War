#include "ObjectLoader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

ObjectLoader::ObjectLoader(){
	px = 0;
	py = 0;
	pz = 0;
}

ObjectLoader::ObjectLoader(float inX, float inY, float inZ){
	px = inX;
	py = inY;
	pz = inZ;
}


//Used to load object file
void ObjectLoader::loadObject(const char* filename){
    ifstream Objfile;
    Objfile.open(filename);
    //Read object data
    while(!Objfile.eof())
    {
        string line;
        getline(Objfile, line);
        string type = line.substr(0,2);
        //Get first 2 characters in each line
        if(type.compare("v ") == 0)
        {
    		// 1
    		// Copy line for parsing
    		char* l = new char[line.size()+1];
    		memcpy(l, line.c_str(), line.size()+1);
    		        
    		// 2
    		// Extract tokens
            strtok(l, " "); 
    		triple v; //store the vertex value
    		v.mX = stof(strtok(NULL, " "));
    		v.mY = stof(strtok(NULL, " ")); 
    		v.mZ = stof(strtok(NULL, " ")); 
    		vVertex.push_back(v);
        }
	    // Texels
		else if(type.compare("vt") == 0)
		{
		    char* l = new char[line.size()+1];
		    memcpy(l, line.c_str(), line.size()+1);

		    strtok(l, " ");
            pairs t;  
		    t.mX = stof(strtok(NULL, " ")); 
		    t.mY = stof(strtok(NULL, " "));

		    vText.push_back(t);
		}   
		// Normals
		else if(type.compare("vn") == 0)
		{
		    char* l = new char[line.size()+1];
		    memcpy(l, line.c_str(), line.size()+1);
   
		    strtok(l, " ");
            triple n;
		    n.mX = stof(strtok(NULL, " "));
		    n.mY = stof(strtok(NULL, " "));
		    n.mZ = stof(strtok(NULL, " "));

		    vNorm.push_back(n);	
		} 

		// Faces
		else if(type.compare("f ") == 0)
		{
		    char* l = new char[line.size()+1];
		    memcpy(l, line.c_str(), line.size()+1); 
		        
		    strtok(l, " ");
		    for(int i=0; i<3; i++){
		    	Fvertex.push_back(stoi(strtok(NULL, " /")));
		    	Ftext.push_back(stoi(strtok(NULL, " /")));
		    	Fnorm.push_back(stoi(strtok(NULL, " /")));
		    }   
		}

	}
	Objfile.close();
}