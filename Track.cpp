//
//  Track.cpp
//  
//
//  Created by James Combs on 11/16/15.
//
//

#include "Track.hpp"

Cd * Track::getCd(const int cdId, map<int, Cd*> cdMap) {
    map<int, Cd*>::iterator it = cdMap.begin();
    it = cdMap.find(cdId);
    if(it != cdMap.end()){
        return it->second;
    } else {
        return NULL;    // indicate exception
    }
}

