//
//  Artist.hpp
//  
//
//  Created by James Combs on 11/16/15.
//
//

#ifndef Artist_hpp
#define Artist_hpp

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>

using namespace std;

class Artist
{
    
public:
    Artist(){}
    void setId(const int id) {this->id = id;}
    void setName(const unsigned char *name) {
        for (int i = 0; i < 100; i++) {
            this->name[i] = name[i];
        }
    }
    char * getName() {
        char *temp = (char*) malloc(100 * sizeof(char));
        temp = name;
        return temp;
    }
    int getId() {return id;}
private:
    int id;
    char name[100];
};

#endif /* Artist_hpp */
