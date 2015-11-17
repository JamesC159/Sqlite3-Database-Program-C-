//
//  Cd.hpp
//  
//
//  Created by James Combs on 11/16/15.
//
//

#ifndef Cd_hpp
#define Cd_hpp

#include <stdio.h>
#include <string>
#include <map>

#include "Artist.hpp"

using namespace std;

class Cd
{
    
public:
    Cd(){}
    void setId(const int id) {this->id = id;}
    void setArtistId(const int artistId) {this->artistId = artistId;}
    void setTitle (const unsigned char *title) {
        for (int i = 0; i < 70; i++) {
            this->title[i] = title[i];
        }
    }
    void setCatalogue (const unsigned char *catalogue) {
        for (int i = 0; i < 30; i++) {
            this->catalogue[i] = catalogue[i];
        }
    }
    int getId() {return id;}
    int getArtistId() {return artistId;}
    char * getTitle() {
        char *temp = (char*) malloc (70 * sizeof(char));
        temp = title;
        return temp;
    }
    char * getCatalogue() {
        char *temp = (char*) malloc (70 * sizeof(char));
        temp = catalogue;
        return temp;
    }
    Artist * getArtist(const int, map<int, Artist*>);
    
private:
    int id, artistId;
    char title[70], catalogue[30];
};

#endif /* Cd_hpp */
