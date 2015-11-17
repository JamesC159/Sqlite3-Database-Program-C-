//
//  Cd.cpp
//  
//
//  Created by James Combs on 11/16/15.
//
//

#include "Cd.hpp"

Artist * Cd::getArtist(const int artistId, map<int, Artist*> artistMap) {
    map<int, Artist*>::iterator it = artistMap.begin();
    it = artistMap.find(artistId);
    if(it != artistMap.end()){
        return it->second;
    } else {
        return NULL;    // indicate exception
    }
}
