//
//  main2.cpp
//  
//
//  Created by James Combs on 11/14/15.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <string>
#include <sstream>
#include <vector>
#include <sqlite3.h>

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

/********************************************************************************
 *  Function prototypes
 ********************************************************************************/
void openInFiles(ifstream &, ifstream &, ifstream &);
void openDatabaseOutput(ofstream &);
void tokenizeArtists (vector<artistRecord*> &, artistRecord *, ifstream &, const char *);
void tokenizeCds(vector<cdRecord*> &, cdRecord *, ifstream &, const char *);
void tokenizeTracks(vector<trackRecord*> &, trackRecord *, ifstream &, const char *);
void execDatabaseMenu(sqlite3 **);

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
    struct artistRecord *artist;
    struct cdRecord *cd;
    struct trackRecord *track;
    
    /* Open files */
    openInFiles(artistIn, cdIn, trackIn);
    openDatabaseOutput(databaseOutput);
    
    /* Tokenize artist.txt, cd.txt, and track.txt */
    tokenizeArtists(artists, artist, artistIn, DELIMS);
    tokenizeCds(cds, cd, cdIn, DELIMS);
    tokenizeTracks(tracks, track, trackIn, DELIMS);
    
    
    
    /**
     * Here We have completed part A1, show we have completed part A1
     */
    
    
    
    /* Open song Database */
    rc = sqlite3_open("song.db", &db);
    if (rc) {
        cerr << "SQL ERROR: " << sqlite3_errmsg(db) << " COULD NOT OPEN song.db" << endl;
        exit(1);
    } else {
        cout << "Opened song.db successfully" << endl << endl;
    }
    
    /* Create database tables */
    sql = (char *) malloc (100 * sizeof(char));
    strcpy(sql, "create table artist ( id INTEGER AUTO_INCREMENT NOT NULL PRIMARY KEY,");
    strcat(sql, "name VARCHAR(100) NOT NULL);");
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL ERROR: " << zErrMsg << " COULD NOT CREATE TABLE artist" << endl;
        sqlite3_free(zErrMsg);
    } else {
        cout << "artist table created successfully!" << endl;
    }
    free(sql);
    
    sql = (char *) malloc (200 * sizeof(char));
    strcpy(sql, "create table cd ( id INTEGER AUTO_INCREMENT NOT NULL PRIMARY KEY,");
    strcat(sql, "title VARCHAR(70) NOT NULL,");
    strcat(sql, "artist_id INTEGER NOT NULL,");
    strcat(sql, "catalogue VARCHAR(30) NOT NULL);");
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL ERROR: " << zErrMsg << " COULD NOT CREATE TABLE cd" << endl;
        sqlite3_free(zErrMsg);
    } else {
        cout << "cd table created successfully!" << endl;
    }
    free(sql);
    
    sql = (char *) malloc (200 * sizeof(char));
    strcpy(sql, "create table track ( cd_id INTEGER NOT NULL,");
    strcat(sql, "track_id INTEGER NOT NULL,");
    strcat(sql, "title VARCHAR(70),");
    strcat(sql, "PRIMARY KEY(cd_id, track_id));");
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL ERROR: " << zErrMsg << " COULD NOT CREATE TABLE track" << endl;
        sqlite3_free(zErrMsg);
    } else {
        cout << "track table created successfully!" << endl;
    }
    free(sql);
    
    /* Insert records into artist, cd, and track tables */
    for (int i = 0; i < artists.size(); i++) {
        char *idBuf = (char *) malloc (100 * sizeof(char));
        sprintf(idBuf, "%d", artists[i]->id);  // convert artist id's into char*'s
        sql = (char *) malloc (100 * sizeof(char));
        strcpy(sql, "insert into artist(id, name) values(");
        strcat(sql, idBuf);
        strcat(sql, ", '");
        strcat(sql, artists[i]->name);
        strcat(sql, "');");
        rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL ERROR: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
        }
        free(sql);
        free(idBuf);
    }
    
    for (int i = 0; i < cds.size(); i++) {
        char *idBuf = (char *) malloc (100 * sizeof(char));
        char *artistIdBuf = (char *) malloc (100 * sizeof(char));
        sprintf(idBuf, "%d", cds[i]->id);
        sprintf(artistIdBuf, "%d", cds[i]->artistId);
        sql = (char *) malloc (200 * sizeof(char));
        strcpy(sql, "insert into cd(id, title, artist_id, catalogue) values(");
        strcat(sql, idBuf);
        strcat(sql, ", '");
        strcat(sql, cds[i]->title);
        strcat(sql, "', ");
        strcat(sql, artistIdBuf);
        strcat(sql, ", '");
        strcat(sql, cds[i]->catalogue);
        strcat(sql, "');");        rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL ERROR: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
        }
        free(sql);
        free(idBuf);
        free(artistIdBuf);
    }
    
    for (int i = 0; i < tracks.size(); i++) {
        char *cdIdBuf = (char *) malloc (100 * sizeof(char));
        char *trackIdBuf = (char *) malloc (100 * sizeof(char));
        sprintf(cdIdBuf, "%d", tracks[i]->cdId);
        sprintf(trackIdBuf, "%d", tracks[i]->trackId);
        sql = (char *) malloc (200 * sizeof(char));
        strcpy(sql, "insert into track(cd_id, track_id, title) values(");
        strcat(sql, cdIdBuf);
        strcat(sql, ", ");
        strcat(sql, trackIdBuf);
        strcat(sql, ", '");
        strcat(sql, tracks[i]->title);
        strcat(sql, "');");
        rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL ERROR: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
        }
        free(sql);
        free(cdIdBuf);
        free(trackIdBuf);
    }
    
    
    
    /**
     *  Part A2 is done, show here
     */
    
    
    
    // loop each record in artist table
    databaseOutput << "ARTIST DATABASE RECORDS\n______________________________________________________________________________\n\n";
    sqlite3_stmt* statement;
    sqlite3_prepare_v2(db, "SELECT * FROM artist", -1, &statement, NULL);
    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
        databaseOutput << "artist id = " << sqlite3_column_int(statement, 0) << " artist name = "
                    << sqlite3_column_text(statement, 1) << endl;
    }
    sqlite3_finalize(statement);  // and done
    
    databaseOutput << endl;
    
    databaseOutput << "CD DATABASE RECORDS\n______________________________________________________________________________\n\n";
    sqlite3_prepare_v2(db, "SELECT * FROM cd", -1, &statement, NULL);
    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
        databaseOutput << "cd id = " << sqlite3_column_int(statement, 0) << " cd title = "
                    << sqlite3_column_text(statement, 1) <<  " cd artist_id = " << sqlite3_column_int(statement, 2)
                    << " cd catalogue = " << sqlite3_column_text(statement, 3) << endl;
    }
    sqlite3_finalize(statement);  // and done
    
    databaseOutput << endl;
    
    databaseOutput << "TRACK DATABASE RECORDS\n______________________________________________________________________________\n\n";
    sqlite3_prepare_v2(db, "SELECT * FROM track", -1, &statement, NULL);
    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
        databaseOutput << "track cd_id = " << sqlite3_column_int(statement, 0) << " track artist_id = "
        << sqlite3_column_int(statement, 1) <<  " track title = " << sqlite3_column_text(statement, 2) << endl;
    }
    sqlite3_finalize(statement);  // and done
    
    
    
    /**
     *  Part A3 is done, show here
     */
    
    
    
    /* Menu system to select updating a database record */
    execDatabaseMenu(&db);
    
    /* Display updated record */
    databaseOutput << "\nTRACK DATABASE RECORDS UPDATED\n______________________________________________________________________________\n\n";
    cout << "\nTRACK DATABASE RECORDS UPDATED\n______________________________________________________________________________\n\n";
    sqlite3_prepare_v2(db, "SELECT * FROM track", -1, &statement, NULL);
    while (sqlite3_step(statement) == SQLITE_ROW) {  // get the record
        databaseOutput << "track cd_id = " << sqlite3_column_int(statement, 0) << " track artist_id = "
                    << sqlite3_column_int(statement, 1) <<  " track title = " << sqlite3_column_text(statement, 2) << endl;
        cout << "track cd_id = " << sqlite3_column_int(statement, 0) << " track artist_id = "
                << sqlite3_column_int(statement, 1) <<  " track title = " << sqlite3_column_text(statement, 2) << endl;
    }
    sqlite3_finalize(statement);  // and done
    
    
    
    /**
     *  Part A4 is done, show here
     */
    
    
    
    
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
 *  Menu system used to Insert, Delete, Update, and Delete All records from
 *  the song database. Currently only used to update the track table titles
 ********************************************************************************/
void execDatabaseMenu(sqlite3 **db) {
    int choice;
    char *zErrMsg;
    char *sql;
    int rc;
    const char *data = "Callback function called";
    
    cout << "DATABASE MENU" << endl << "_______________________________________" << endl << endl;
    cout << "(1) Insert\n(2) Delete\n(3) Update\n(4) Delete All\n";
    cin >> choice;
    switch (choice) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            int tableChoice;
            cout << "Please select a database table\n";
            cout << "(1) artist\n(2) cd\n(3) track\n";
            cin >> tableChoice;
            switch (tableChoice) {
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    cout << "Please select a record to update (cd_id, track_id)\n";
                    string cd_id, track_id;
                    cout << "cd_id: ";
                    cin >> cd_id;
                    cout << "track_id: ";
                    cin >> track_id;
                    cout << "Please enter new title: ";
                    string title;
                    cin >> title;
                    sql = (char *) malloc (100 * sizeof(char));
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
            if (tableChoice != 1 && tableChoice != 2 && tableChoice != 3) {
                cerr << "Invalid database table choice, please try again\n";
            }
            break;
        case 4:
            break;
        default:
            cerr << "Invalid menu choice, please try again\n";
            break;
    }
}
