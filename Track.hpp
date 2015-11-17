//
//  Track.hpp
//  
//
//  Created by James Combs on 11/16/15.
//
//

#ifndef Track_hpp
#define Track_hpp

#include <stdio.h>
#include <map>

#include "Cd.hpp"

class Track
{
    
public:
    Track(){}
    void setCdId(const int id) {cdId = id;}
    void setTrackId(const int track) {trackId = track;}
    void setTitle(const unsigned char* title) {
        for (int i = 0; i < 70; i++) {
            this->title[i] = title[i];
        }
    }
    int getCdId() {return cdId;}
    int getTrackId() {return trackId;}
    char * getTitle() {
        char * temp = (char*) malloc (70 * sizeof(char));
        temp = title;
        return temp;
    }
    Cd * getCd(const int, map<int, Cd*>);
private:
    int cdId, trackId;
    char title[70];
    
};

#endif /* Track_hpp */
