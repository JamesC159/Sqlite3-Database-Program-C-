//
//  main.cpp
//  
//
//  Created by James Combs on 11/14/15.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>

#include "Artist.hpp"
#include "Cd.hpp"
#include "Track.hpp"

using namespace std;

/********************************************************************************
 *  Callback function. For use with sqlite3
 ********************************************************************************/
static int callback(void *data, int argc, char *argv[], char **azColName) {
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

/********************************************************************************
 *  Song database record structures
 ********************************************************************************/
struct artistRecord {
    int id;
    const char *name;
};

struct cdRecord {
    int id;
    char *title;
    int artistId;
    char *catalogue;
};

struct trackRecord {
    int cdId;
    int trackId;
    char *title;
};

/***** GLOBAL CONSTANTS *******/
const int SQL_STMT_SIZE = 250;

/********************************************************************************
 *  Function prototypes
 ********************************************************************************/
void openInFiles(ifstream &, ifstream &, ifstream &);
void openDatabaseOutput(ofstream &);
void openDB(sqlite3 **);
void tokenizeArtists (vector<artistRecord*> &, artistRecord *, ifstream &, const char *);
void createArtistTable(sqlite3 **, const char *, char *);
void tokenizeCds(vector<cdRecord*> &, cdRecord *, ifstream &, const char *);
void createCdTable(sqlite3 **, const char *, char *);
void tokenizeTracks(vector<trackRecord*> &, trackRecord *, ifstream &, const char *);
void createTrackTable(sqlite3 **, const char *, char *);
void insertArtists(vector<artistRecord*> &, sqlite3 **, const char *, char *);
void insertCds(vector<cdRecord*> &, sqlite3 **, const char *, char *);
void insertTracks(vector<trackRecord*> &, sqlite3 **, const char *, char *);
void displayUpdates(sqlite3 **, ofstream &);
void dbMenu(sqlite3 **);
void printArtistRecords(sqlite3 **, ofstream &);
void printCdRecords(sqlite3 **, ofstream &);
void printTrackRecords(sqlite3 **, ofstream &);
void getArtists(sqlite3 **, Artist *, map<int, Artist*> &);
void getCds(sqlite3 **, Cd *, map<int, Cd*> &);
void getTracks(sqlite3 **, Track *, map<pair<int, int>, Track*> &);
void generateArtistReport(map<int, Artist*>, ofstream &);
void generateCdReport(map<int, Cd*>, ofstream &);
void generateTrackReport(map<pair<int, int>, Track*>, ofstream &);
void checkCdIntegrity(map<int, Cd*>, map<int, Artist*>);
void checkTrackIntegrity(map<pair<int, int>, Track*>, map<int, Cd*>);
void causeCorruption(sqlite3 **, const char *, char *);
void checkIsCorrupted(map<int, Cd*>, map<int, Artist*>);


/********************************************************************************
 *  Main Function
 ********************************************************************************/
int main(int argc, char **argv) {
    const char *DELIMS = "|";
    const char *data = "Callback function called";
    
    /* sqlite3 database variables */
    sqlite3 *db;
    char *zErrMsg;
    char *sql;
    int rc;
    
    /* Files to create database from */
    ifstream artistIn, cdIn, trackIn;
    ofstream databaseOutput;
    
    /* Vectors to hold structures of records to insert */
    vector<artistRecord*> artists;
    vector<cdRecord*> cds;
    vector<trackRecord*> tracks;
    
    /* Structures to represent a record in the database */
    struct artistRecord *artist;
    struct cdRecord *cd;
    struct trackRecord *track;
    
    /* Objects to represent a record in the database */
    Artist *artistObject;
    Cd *cdObject;
    Track *trackObject;
    
    /* Vectors to hold objects of each table in the database */
    vector<Artist*> artistObjects;
    vector<Cd*> cdObjects;
    vector<Track*> trackObjects;
    
    /* Maps to hold objects as records */
    static map<int, Artist*> artistMap;
    static map<int, Cd*> cdMap;
    static map<pair<int, int>, Track*> trackMap;
    
    /* Open files */
    openInFiles(artistIn, cdIn, trackIn);
    openDatabaseOutput(databaseOutput);
    
    /* PART A1 - Tokenize artist.txt, cd.txt, and track.txt */
    tokenizeArtists(artists, artist, artistIn, DELIMS);
    tokenizeCds(cds, cd, cdIn, DELIMS);
    tokenizeTracks(tracks, track, trackIn, DELIMS);
    
    
    /* PART A2- Open song Database */
    openDB(&db);
    
    /* PART A2 - Create database tables */
    createArtistTable(&db, data, zErrMsg);
    createCdTable(&db, data, zErrMsg);
    createTrackTable(&db, data, zErrMsg);
    
    cout << endl;
    
    /* PART A2 - Insert records into artist, cd, and track tables */
    insertArtists(artists, &db, data, zErrMsg);
    insertCds(cds, &db, data, zErrMsg);
    insertTracks(tracks, &db, data, zErrMsg);
    
    /* PART A3 - loop each record in database tables */
    printArtistRecords(&db, databaseOutput);
    printCdRecords(&db, databaseOutput);
    printTrackRecords(&db, databaseOutput);
    
    /* PART A4 - Menu system to select updating a database record */
    dbMenu(&db);
    
    /* PART A4 - Display updated records */
    displayUpdates(&db, databaseOutput);
    
    
    /* NO NEED TO UPDATE ARTISTS OR CDS FOR THIS ASSIGNMENT */
    
//    databaseOutput << "\nARTIST DATABASE RECORDS UPDATED\n______________________________________________________________________________\n\n";
//    cout << "\nARTIST DATABASE RECORDS UPDATED\n______________________________________________________________________________\n\n";
//    sqlite3_prepare_v2(db, "SELECT * FROM artist", -1, &statement, NULL);
//    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
//        databaseOutput << "artist id = " << sqlite3_column_int(statement, 0) << " artist name = "
//        << sqlite3_column_int(statement, 1) << endl;
//        cout << "artist id = " << sqlite3_column_int(statement, 0) << " artist name = "
//        << sqlite3_column_int(statement, 1) << endl;
//    }
//    sqlite3_finalize(statement);  // and done
//    
//    databaseOutput << endl;
//    cout << endl;
//
//    databaseOutput << "\nCD DATABASE RECORDS UPDATED\n______________________________________________________________________________\n\n";
//    cout << "\nCD DATABASE RECORDS UPDATED\n______________________________________________________________________________\n\n";
//    sqlite3_prepare_v2(db, "SELECT * FROM cd", -1, &statement, NULL);
//    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
//        databaseOutput << "cd id = " << sqlite3_column_int(statement, 0) << " cd title = "
//        << sqlite3_column_int(statement, 1) <<  " cd artist id = " << sqlite3_column_text(statement, 2)
//        << " cd catalogue = " << sqlite3_column_text(statement, 3) << endl;
//        cout << "cd id = " << sqlite3_column_int(statement, 0) << " cd title = "
//        << sqlite3_column_int(statement, 1) <<  " cd artist id = " << sqlite3_column_text(statement, 2)
//        << " cd catalogue = " << sqlite3_column_text(statement, 3) << endl;
//    }
//    sqlite3_finalize(statement);  // and done
//    
//    databaseOutput << endl;
//    cout << endl;
    
    
    /**
     *  Here we should delete all the records from the database. But if doing so,
     *  The assignment would not be able to be completed.Therefore, it is not implemented here
     *  But instead is implemented at the end of the program
     */
    
    
    
    /* PART B1 - CHECK CLASSES FOR IMPLEMENTATION OF CLASSES */
    
    
    /* PART B2 - Get data from the database to create objects */
    /* PART B2 - Generate a report for the objects */
    getArtists(&db, artistObject, artistMap);
    generateArtistReport(artistMap, databaseOutput);
    getCds(&db, cdObject, cdMap);
    generateCdReport(cdMap, databaseOutput);
    getTracks(&db, trackObject, trackMap);
    generateTrackReport(trackMap, databaseOutput);
    
    cout << endl;
    databaseOutput << endl;
    
    /* PART B4 - Implementation of Integrity Checks */
    cout << "BEGINNING INTEGRITY CHECKS\n________________________________________________________\n\n";
    checkCdIntegrity(cdMap, artistMap);
    checkTrackIntegrity(trackMap, cdMap);
    
    /* PART B4 - Simulate breaking the integrity of the database by inserting a record into the database */
    cout << "SIMULATING INTEGRITY CORRUPTION\n________________________________________________________\n\n";
    cout << "Inserting a new record with new cooresponding Artist ID into the CD table.\nThis record will not be added to the CD map\n\n";
	causeCorruption(&db, data, zErrMsg);
    cout << "Checking to see that the integrity of the system has been broken...\n\n";
	checkIsCorrupted(cdMap, artistMap);
    
    /* PART A4 - Should choose to delete all records */
    dbMenu(&db);
    
    sql = (char*) malloc (SQL_STMT_SIZE * sizeof(char));
    free(sql);
    
    sqlite3_close(db);
    return 0;
}



/********************************************************************************
 *  Opens input files and checks status
 ********************************************************************************/
void openInFiles(ifstream &artistFile, ifstream &cdFile, ifstream &trackFile) {
    artistFile.open("artist.txt");
    cdFile.open("cd.txt");
    trackFile.open("track.txt");
    if (!artistFile.good() || !cdFile.good() || !trackFile.good()) {
        cerr << "ERROR: did not successfully open all files\n";
        exit(1);
    }
    else {
        cout << "Files opened successfully\n\n";
    }
}

/********************************************************************************
 *  Opens output files and checks status
 ********************************************************************************/
void openDatabaseOutput(ofstream &output) {
    output.open("database_output.txt");
    if (!output.good()) {
        cerr << "ERROR: did not successfully open all files\n";
        exit(1);
    }
    else {
        cout << "Files opened successfully\n\n";
    }
}

/********************************************************************************
 *  Opens song database
 ********************************************************************************/
void openDB(sqlite3 ** db) {
    int rc = sqlite3_open("song.db", db);
    if (rc) {
        cerr << "SQL ERROR: " << sqlite3_errmsg(*db) << " COULD NOT OPEN song.db" << endl;
        exit(1);
    } else {
        cout << "Opened song.db successfully" << endl << endl;
    }

}

/********************************************************************************
 *  Parses and tokenizes the artist.txt file. Tokens are used to insert records
 *  into the artist database table in the song database
 ********************************************************************************/
void tokenizeArtists (vector<artistRecord*> &artists, artistRecord *artist, ifstream &artistIn, const char *DELIMS) {
    /* Tokenizer variables */
    string line;
    char *token;
    char c;
    bool foundAlpha = false;

    /* Now tokenize artist.txt */
    while (getline(artistIn, line)) {
        artist = new artistRecord();  // Create a new artist to create a record from
        char *linedup = strdup(line.c_str());   //Copy the string, must free since strdup allocated memory
        token = strtok(linedup, DELIMS);
        do {
            /* Loop through token to see if it is purely digits */
            for (int i = 0; token[i]; i++) {
                if (token[i] == '\0') {
                    break;
                }
                if (isalpha(token[i])) {
                    foundAlpha = true;
                    break;
                }
            }
            if (foundAlpha) {
                artist->name = token;
            } else {
                artist->id = atoi(token);
            }
        
            foundAlpha = false;
        } while ((token = strtok(NULL, DELIMS)) != NULL);
    
        artists.push_back(artist);
    }
}


/********************************************************************************
 *  Creates table for artist in database
 ********************************************************************************/
void createArtistTable(sqlite3 **db, const char *data, char *zErrMsg) {
    char * sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
    strcpy(sql, "create table artist ( id INTEGER AUTO_INCREMENT NOT NULL PRIMARY KEY,");
    strcat(sql, "name VARCHAR(100) NOT NULL);");
    int rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL ERROR: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    } else {
        cout << "artist table created successfully!" << endl;
    }
    free(sql);
}

/********************************************************************************
 *  Parses and tokenizes the cd.txt file. Tokens are used to insert records
 *  into the cd database table in the song database
 ********************************************************************************/
void tokenizeCds(vector<cdRecord*> &cds, cdRecord *cd, ifstream &cdIn, const char *DELIMS) {
    /* Tokenizer variables */
    string line;
    char *token;
    char c;
    bool foundAlpha = false;
    bool foundFirstId = true;
    bool foundFirstString = true;
    
    /* Now tokenize cd.txt */
    while (getline(cdIn, line)) {
        cd = new cdRecord();
        string temp(line);
        char *linedup = strdup(line.c_str());   //Copy the string, must free since strdup allocated memory
        token = strtok(linedup, DELIMS);
        do {
            /* Loop through token to see if it is purely digits */
            for (int i = 0; token[i]; i++) {
                if (token[i] == '\0') {
                    break;
                }
                if (isalpha(token[i])) {
                    foundAlpha = true;
                    break;
                }
            }
            if (foundAlpha && foundFirstString) {
                cd->title = token;
                foundFirstString = false;
            } else if(foundAlpha && !foundFirstString){
                cd->catalogue = token;
            } else if (!foundAlpha && foundFirstId) {
                cd->id = atoi(token);
                foundFirstId = false;
            } else if (!foundAlpha && !foundFirstId) {
                cd->artistId = atoi(token);
            } else {
                cout << "BAD TOKEN!" << endl;
            }
            
            foundAlpha = false;
        } while ((token = strtok(NULL, DELIMS)) != NULL);
        foundFirstId = true;
        foundFirstString = true;
        cds.push_back(cd);
    }
}

/********************************************************************************
 *  Creates table for artist in database
 ********************************************************************************/
void createCdTable(sqlite3 **db, const char *data, char *zErrMsg) {
    char *sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
    strcpy(sql, "create table cd ( id INTEGER AUTO_INCREMENT NOT NULL PRIMARY KEY,");
    strcat(sql, "title VARCHAR(70) NOT NULL,");
    strcat(sql, "artist_id INTEGER NOT NULL,");
    strcat(sql, "catalogue VARCHAR(30) NOT NULL);");
    int rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL ERROR: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    } else {
        cout << "cd table created successfully!" << endl;
    }
    free(sql);
}

/********************************************************************************
 *  Parses and tokenizes the track.txt file. Tokens are used to insert records
 *  into the track database table in the song database
 ********************************************************************************/
void tokenizeTracks (vector<trackRecord*> &tracks, trackRecord *track, ifstream &trackIn, const char *DELIMS) {
    /* Tokenizer variables */
    string line;
    char *token;
    char c;
    bool foundAlpha = false;
    bool foundFirstId = true;
    
    while (getline(trackIn, line)) {
        track = new trackRecord();
        char *linedup = strdup(line.c_str());   //Copy the string, must free since strdup allocated memory
        token = strtok(linedup, DELIMS);
        do {
            /* Loop through token to see if it is purely digits */
            for (int i = 0; token[i]; i++) {
                if (token[i] == '\0') {
                    break;
                }
                if (isalpha(token[i])) {
                    foundAlpha = true;
                    break;
                }
            }
            if (!foundAlpha && foundFirstId) {
                track->cdId = atoi(token);
                foundFirstId = false;
            } else if(!foundAlpha && !foundFirstId){
                track->trackId = atoi(token);
            } else if (foundAlpha) {
                track->title = token;
            } else {
                cout << "BAD TOKEN!" << endl;
            }
            
            foundAlpha = false;
        } while ((token = strtok(NULL, DELIMS)) != NULL);
        foundFirstId = true;
        tracks.push_back(track);
    }
}

/********************************************************************************
 *  Creates table for track in database
 ********************************************************************************/
void createTrackTable(sqlite3 **db, const char *data, char *zErrMsg) {
    char *sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
    strcpy(sql, "create table track ( cd_id INTEGER NOT NULL,");
    strcat(sql, "track_id INTEGER NOT NULL,");
    strcat(sql, "title VARCHAR(70),");
    strcat(sql, "PRIMARY KEY(cd_id, track_id));");
    int rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL ERROR: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    } else {
        cout << "track table created successfully!" << endl;
    }
    free(sql);
}

/********************************************************************************
 *  Insert Artists into the artist table
 ********************************************************************************/
void insertArtists(vector<artistRecord*> &artists, sqlite3 **db, const char *data, char *zErrMsg) {
    for (int i = 0; i < artists.size(); i++) {
        char *idBuf = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
        sprintf(idBuf, "%d", artists[i]->id);  // convert artist id's into char*'s
        char *sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
        strcpy(sql, "insert into artist(id, name) values(");
        strcat(sql, idBuf);
        strcat(sql, ", '");
        strcat(sql, artists[i]->name);
        strcat(sql, "');");
        int rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL ERROR: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
        }
        free(sql);
        free(idBuf);
    }
}

/********************************************************************************
 *  Insert CDs into the cd table
 ********************************************************************************/
void insertCds(vector<cdRecord*> &cds, sqlite3 **db, const char *data, char *zErrMsg) {
    for (int i = 0; i < cds.size(); i++) {
        char *idBuf = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
        char *artistIdBuf = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
        sprintf(idBuf, "%d", cds[i]->id);
        sprintf(artistIdBuf, "%d", cds[i]->artistId);
        char *sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
        strcpy(sql, "insert into cd(id, title, artist_id, catalogue) values(");
        strcat(sql, idBuf);
        strcat(sql, ", '");
        strcat(sql, cds[i]->title);
        strcat(sql, "', ");
        strcat(sql, artistIdBuf);
        strcat(sql, ", '");
        strcat(sql, cds[i]->catalogue);
        strcat(sql, "');");
        int rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL ERROR: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
        }
        free(sql);
        free(idBuf);
        free(artistIdBuf);
    }
}

/********************************************************************************
 *  Insert tracks into the track table
 ********************************************************************************/
void insertTracks(vector<trackRecord*> &tracks, sqlite3 **db, const char *data, char *zErrMsg) {
    for (int i = 0; i < tracks.size(); i++) {
        char *cdIdBuf = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
        char *trackIdBuf = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
        sprintf(cdIdBuf, "%d", tracks[i]->cdId);
        sprintf(trackIdBuf, "%d", tracks[i]->trackId);
        char *sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
        strcpy(sql, "insert into track(cd_id, track_id, title) values(");
        strcat(sql, cdIdBuf);
        strcat(sql, ", ");
        strcat(sql, trackIdBuf);
        strcat(sql, ", '");
        strcat(sql, tracks[i]->title);
        strcat(sql, "');");
        int rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL ERROR: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
        }
        free(sql);
        free(cdIdBuf);
        free(trackIdBuf);
    }
}

/********************************************************************************
 *  Menu system used to Insert, Delete, Update, and Delete All records from
 *  the song database. Currently only used to update the track table titles
 ********************************************************************************/
void dbMenu(sqlite3 **db) {
    int choice = -1;
    int tableChoice = -1;
    string title;
    char *zErrMsg;
    char *sql;
    int rc;
    bool notValidChoice = true;
    const char *data = "Callback function called";
    
    cout << "DATABASE MENU" << endl << "_______________________________________" << endl << endl;
    while (notValidChoice) {
        cout << "(1) Update\n(2) Delete All\n";
        cin >> choice;
        switch (choice) {
            case 1:
            {
                notValidChoice = false;
                cout << "Please select a database table\n";
                cout << "(1) Track\n(2) CD (NOT NEEDED FOR THIS ASSIGNMENT - DO NOT PRESS)\n(3) Artist (NOT NEEDED FOR THIS ASSIGNMENT - DO NOT PRESS)\n";
                break;
            }
            case 2:
            {
                notValidChoice = false;
                cout << "Deleting all records from the database...\n";
                sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
                strcpy(sql, "delete from artist;");
                rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
                if (rc != SQLITE_OK) {
                    cerr << "SQL ERROR: " << zErrMsg << endl;
                }
                free(sql);
                sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
                strcpy(sql, "delete from cd;");
                rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
                if (rc != SQLITE_OK) {
                    cerr << "SQL ERROR: " << zErrMsg << endl;
                }
                free(sql);
                sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
                strcpy(sql, "delete from track;");
                rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
                if (rc != SQLITE_OK) {
                    cerr << "SQL ERROR: " << zErrMsg << endl;
                }
                cout << "Done!\n";
                free(sql);
                break;
            }
            default:
            {
                cerr << "Invalid menu choice, please try again\n";
                break;
            }
        }
    }
    
    if (choice == 2) {
        return;
    }
    
    bool notValidTable = true;
    while (notValidTable) {
        cin >> tableChoice;
        switch (tableChoice) {
                
                /* NO NEED TO UPDATE ARTISTS OR CDS FOR THIS ASSIGNMENT */
                
                
//            case 1:
//            {
//                notValidTable = false;
//                cout << "Please select a record to update (id)\n";
//                string id, name;
//                cout << "id: ";
//                cin >> id;
//                cout << "Please enter new artist name: ";
//                cin >> name;
//                sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
//                strcpy(sql, "update artist set name='");
//                strcat(sql, name.c_str());
//                strcat(sql, "' where id=");
//                strcat(sql, id.c_str());
//                strcat(sql, ";");
//                rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
//                if (rc != SQLITE_OK) {
//                    cerr << "SQL ERROR: " << zErrMsg << endl;
//                }
//                free(sql);
//                break;
//            }
//            case 2:
//            {
//                notValidTable = false;
//                cout << "Please select a record to update (id, artist_id)\n";
//                string id, artist_id, title, catalogue;
//                cout << "id: ";
//                cin >> id;
//                cout << "artist_id: ";
//                cin >> artist_id;
//                cout << "Please enter new track title: ";
//                cin >> title;
//                cout << "Please enter a new catalogue: ";
//                cin >> catalogue;
//                sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
//                strcpy(sql, "update cd set title='");
//                strcat(sql, title.c_str());
//                strcat(sql, "' where id=");
//                strcat(sql, id.c_str());
//                strcat(sql, " and artist_id=");
//                strcat(sql, artist_id.c_str());
//                rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
//                if (rc != SQLITE_OK) {
//                    cerr << "SQL ERROR: " << zErrMsg << endl;
//                }
//                free(sql);
//                sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
//                strcat(sql, "update cd set catalogue='");
//                strcat(sql, catalogue.c_str());
//                strcat(sql, "' where id=");
//                strcat(sql, id.c_str());
//                strcat(sql, " and artist_id=");
//                strcat(sql, artist_id.c_str());
//                rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
//                if (rc != SQLITE_OK) {
//                    cerr << "SQL ERROR: " << zErrMsg << endl;
//                }
//                free(sql);
//                break;
//            }
            case 1:
            {
                notValidTable = false;
                cout << "Please select a record to update (cd_id, track_id)\n";
                string cd_id, track_id;
                cout << "cd_id: ";
                cin >> cd_id;
                cout << "track_id: ";
                cin >> track_id;
                cout << "Please enter new track title : ";
                cin >> title;
                sql = (char *) malloc (SQL_STMT_SIZE * sizeof(char));
                strcpy(sql, "update track set title='");
                strcat(sql, title.c_str());
                strcat(sql, "' where cd_id=");
                strcat(sql, cd_id.c_str());
                strcat(sql, " and track_id=");
                strcat(sql, track_id.c_str());
                rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
                if (rc != SQLITE_OK) {
                    cerr << "SQL ERROR: " << zErrMsg << endl;
                }
                break;
            }
            default:
            {
                cerr << "Invalid menu choice, please try again\n";
                break;
            }
        }
    }
}

/********************************************************************************
 *  Print all artists from the artist table
 ********************************************************************************/
void printArtistRecords(sqlite3 **db, ofstream &databaseOutput) {
    databaseOutput << "ARTIST DATABASE RECORDS\n______________________________________________________________________________\n\n";
    cout << "ARTIST DATABASE RECORDS\n______________________________________________________________________________\n\n";
    sqlite3_stmt* statement;
    sqlite3_prepare_v2(*db, "SELECT * FROM artist", -1, &statement, NULL);
    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
        databaseOutput << "artist id = " << sqlite3_column_int(statement, 0) << " artist name = "
        << sqlite3_column_text(statement, 1) << endl;
        cout << "artist id = " << sqlite3_column_int(statement, 0) << " artist name = "
        << sqlite3_column_text(statement, 1) << endl;
    }
    sqlite3_finalize(statement);  // and done
    
    databaseOutput << endl;
    cout << endl;
}

/********************************************************************************
 *  Print all CDs from the cd table
 ********************************************************************************/
void printCdRecords(sqlite3 **db, ofstream &databaseOutput) {
    sqlite3_stmt* statement;
    databaseOutput << "CD DATABASE RECORDS\n______________________________________________________________________________\n\n";
    cout << "CD DATABASE RECORDS\n______________________________________________________________________________\n\n";
    sqlite3_prepare_v2(*db, "SELECT * FROM cd", -1, &statement, NULL);
    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
        databaseOutput << "cd id = " << sqlite3_column_int(statement, 0) << " cd title = "
        << sqlite3_column_text(statement, 1) <<  " cd artist_id = " << sqlite3_column_int(statement, 2)
        << " cd catalogue = " << sqlite3_column_text(statement, 3) << endl;
        cout << "cd id = " << sqlite3_column_int(statement, 0) << " cd title = "
        << sqlite3_column_text(statement, 1) <<  " cd artist_id = " << sqlite3_column_int(statement, 2)
        << " cd catalogue = " << sqlite3_column_text(statement, 3) << endl;
    }
    sqlite3_finalize(statement);  // and done
    
    databaseOutput << endl;
    cout << endl;
}

/********************************************************************************
 *  Print all tracks from the track table
 ********************************************************************************/
void printTrackRecords(sqlite3 **db, ofstream &databaseOutput) {
    sqlite3_stmt* statement;
    databaseOutput << "TRACK DATABASE RECORDS\n______________________________________________________________________________\n\n";
    cout << "TRACK DATABASE RECORDS\n______________________________________________________________________________\n\n";
    sqlite3_prepare_v2(*db, "SELECT * FROM track", -1, &statement, NULL);
    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
        databaseOutput << "track cd_id = " << sqlite3_column_int(statement, 0) << " track id = "
        << sqlite3_column_int(statement, 1) <<  " track title = " << sqlite3_column_text(statement, 2) << endl;
        cout << "track cd_id = " << sqlite3_column_int(statement, 0) << " track id = "
        << sqlite3_column_int(statement, 1) <<  " track title = " << sqlite3_column_text(statement, 2) << endl;
    }
    sqlite3_finalize(statement);  // and done
    
    databaseOutput << endl;
    cout << endl;
}

/********************************************************************************
 *  Display updates to the track table
 ********************************************************************************/
void displayUpdates(sqlite3 **db, ofstream &databaseOutput) {
    sqlite3_stmt *statement;
    databaseOutput << "\nTRACK DATABASE RECORDS UPDATED\n______________________________________________________________________________\n\n";
    cout << "\nTRACK DATABASE RECORDS UPDATED\n______________________________________________________________________________\n\n";
    sqlite3_prepare_v2(*db, "SELECT * FROM track", -1, &statement, NULL);
    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
        databaseOutput << "track cd_id = " << sqlite3_column_int(statement, 0) << " track artist_id = "
        << sqlite3_column_int(statement, 1) <<  " track title = " << sqlite3_column_text(statement, 2) << endl;
        cout << "track cd_id = " << sqlite3_column_int(statement, 0) << " track artist_id = "
        << sqlite3_column_int(statement, 1) <<  " track title = " << sqlite3_column_text(statement, 2) << endl;
    }
    sqlite3_finalize(statement);  // and done
    
    databaseOutput << endl;
    cout << endl;
}

/********************************************************************************
 *  Get all artist records from the artist table and place them into the
 *  map that contains artists
 ********************************************************************************/
void getArtists(sqlite3 **db, Artist *artistObject, map<int, Artist*> &artistMap) {
    sqlite3_stmt *statement;
    sqlite3_prepare_v2(*db, "SELECT * FROM artist", -1, &statement, NULL);
    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
        artistObject = new Artist();
        artistObject->setId(sqlite3_column_int(statement, 0));
        artistObject->setName(sqlite3_column_text(statement, 1));
        
        /* PART B3 - Map for each object */
        artistMap.insert(pair<int, Artist*>(artistObject->getId(), artistObject));
    }
    sqlite3_finalize(statement);
}

/********************************************************************************
 *  Get all ds records from the cds table and place them into the
 *  map that contains cds
 ********************************************************************************/
void getCds(sqlite3 **db, Cd *cdObject, map<int, Cd*> &cdMap) {
    sqlite3_stmt *statement;
    sqlite3_prepare_v2(*db, "SELECT * FROM cd", -1, &statement, NULL);
    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
        cdObject = new Cd();
        cdObject->setId(sqlite3_column_int(statement, 0));
        cdObject->setArtistId(sqlite3_column_int(statement, 2));
        cdObject->setTitle(sqlite3_column_text(statement, 1));
        cdObject->setCatalogue(sqlite3_column_text(statement, 3));
        
        /* PART B3 - Map for each object */
        cdMap.insert(pair<int, Cd*>(cdObject->getId(), cdObject));
    }
    sqlite3_finalize(statement);
}

/********************************************************************************
 *  Get all tracks records from the tracks table and place them into the
 *  map that contains tracks
 ********************************************************************************/
void getTracks(sqlite3 **db, Track *trackObject, map<pair<int, int>, Track*> &trackMap) {
    sqlite3_stmt *statement;
    sqlite3_prepare_v2(*db, "SELECT * FROM track", -1, &statement, NULL);
    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
        trackObject = new Track();
        trackObject->setCdId(sqlite3_column_int(statement, 0));
        trackObject->setTrackId(sqlite3_column_int(statement, 1));
        trackObject->setTitle(sqlite3_column_text(statement, 2));
        
        /* PART B3 - Map for each object */
        trackMap.insert( pair<pair<int, int>, Track*> (pair<int, int>(trackObject->getCdId(), trackObject->getTrackId()), trackObject));
    }
    sqlite3_finalize(statement);
}

/********************************************************************************
 *  Print a report for all artists in the artist table
 ********************************************************************************/
void generateArtistReport(map<int, Artist*> artistMap, ofstream &databaseOutput) {
    cout << "ARTIST GENERATED REPORT\n_____________________________________________________\n\n";
    databaseOutput << "ARTIST GENERATED REPORT\n_____________________________________________________\n\n";
    
    for (map<int, Artist*>::iterator it = artistMap.begin();
         it != artistMap.end(); ++it) {
        cout << "Artist ID = " << it->second->getId() << " Artist Name = " << it->second->getName() << endl;
        databaseOutput << "Artist ID = " << it->second->getId() << " Artist Name = " << it->second->getName() << endl;
    }
}

/********************************************************************************
 *  Print a report for all cds in the cd table
 ********************************************************************************/
void generateCdReport(map<int, Cd*> cdMap, ofstream &databaseOutput) {
    cout << endl;
    cout << "CD GENERATED REPORT\n_____________________________________________________\n\n";
    databaseOutput << endl;
    databaseOutput << "CD GENERATED REPORT\n_____________________________________________________\n\n";
    
    for (map<int, Cd*>::iterator it = cdMap.begin();
         it != cdMap.end(); ++it) {
        cout << "CD ID = " << it->second->getId() << " CD Title = " << it->second->getTitle()
        <<  " CD Artist ID = " << it->second->getArtistId() << " CD Catalogue = "
        << it->second->getCatalogue() << endl;
        databaseOutput << "CD ID = " << it->second->getId() << " CD Title = " << it->second->getTitle()
        <<  " CD Artist ID = " << it->second->getArtistId() << " CD Catalogue = "
        << it->second->getCatalogue() << endl;
    }
}

/********************************************************************************
 *  Print a report for all tracks in the track table
 ********************************************************************************/
void generateTrackReport(map<pair<int, int>, Track*> trackMap, ofstream &databaseOutput) {
    cout << endl;
    cout << "TRACK GENERATED REPORT\n_____________________________________________________\n\n";
    databaseOutput << endl;
    databaseOutput << "TRACK GENERATED REPORT\n_____________________________________________________\n\n";
    
    for (map<pair<int, int>, Track*>::iterator it = trackMap.begin();
         it != trackMap.end(); ++it) {
        cout << "Track CD ID = " << it->second->getCdId() << " Track ID = " << it->second->getTrackId()
        <<  " Track Title = " << it->second->getTitle() << endl;
        databaseOutput << "Track CD ID = " << it->second->getCdId() << " Track ID = " << it->second->getTrackId()
        <<  " Track Title = " << it->second->getTitle() << endl;
    }
}

/********************************************************************************
 *  Checks integrity of the cd map. Meaning that it checks to see if the user
 *  Input artist id actually exists in the cd map and actually exists as an object
 ********************************************************************************/
void checkCdIntegrity(map<int, Cd*> cdMap, map<int, Artist*> artistMap) {
    string input = "";
    cout << "Please enter an Artist ID to search for the cooresponding artist in the CD map: ";
    cin >> input;
    
    for (map<int, Cd*>::iterator it = cdMap.begin();
         it != cdMap.end(); ++it) {
        Artist *artist = it->second->getArtist(atoi(input.c_str()), artistMap);
        if (artist == NULL){
            cerr << "Integrity Corrupted! Artist object does not exist" << endl << endl;
            break;
        } else {
            cout << "Artist: " << artist->getName() << endl << endl;
            break;
        }
    }
}

/********************************************************************************
 *  Checks integrity of the track map. Meaning that it checks to see if the user
 *  Input cd id actually exists in the track map and actually exists as an object
 ********************************************************************************/
void checkTrackIntegrity(map<pair<int, int>, Track*> trackMap, map<int, Cd*> cdMap) {
    string input = "";
    cout << "Please enter a CD ID to search for the cooresponding CD in the Track map: ";
    cin >> input;
    
    for (map<pair<int, int>, Track*>::iterator it = trackMap.begin();
         it != trackMap.end(); ++it) {
        Cd *cd = it->second->getCd(atoi(input.c_str()
                                        ), cdMap);
        if (cd == NULL){
            cerr << "Integrity Corrupted! CD object does not exist" << endl << endl;
            break;
        } else {
            cout << "CD: " << cd->getTitle() << endl << endl;
            break;
        }
        
    }

}

/********************************************************************************
*  Causes corruption between database and cd map.
********************************************************************************/
void causeCorruption(sqlite3 **db, const char *data, char *zErrMsg) {
	char *sql = (char *)malloc(SQL_STMT_SIZE * sizeof(char));
	strcpy(sql, "insert into cd(id, title, artist_id, catalogue) values(7, 'Northern Start', 5, 'B00004YMST');");
	int rc = sqlite3_exec(*db, sql, callback, (void*)data, &zErrMsg);
	if (rc != SQLITE_OK) {
		cerr << "SQL ERROR: " << zErrMsg << endl;
		sqlite3_free(zErrMsg);
	}
	free(sql);
}

/********************************************************************************
*  Checks that the corruption from causeCorruption() did indeed corrupt the
*  cd map.
********************************************************************************/
void checkIsCorrupted(map<int, Cd*> cdMap, map<int, Artist*> artistMap) {
	for (map<int, Cd*>::iterator it = cdMap.begin();
	it != cdMap.end(); ++it) {
		Artist *artist = it->second->getArtist(5, artistMap);
		if (artist == NULL) {
			cerr << "Integrity Corrupted! Artist object does not exist!" << endl << endl;
			break;
		}
		else {
			cout << "Artist: " << artist->getName() << endl << endl;
			break;
		}
	}
}
