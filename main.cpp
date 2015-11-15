#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>
#include <vector>
#include <sqlite3.h>

static int callback(void *data, int argc, char **argv, char **azColName) {
	int i;
	fprintf(stderr, "%s: ", (const char*)data);
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}


void openFiles(FILE **, FILE **, FILE **); 
void tokenize(FILE **, const int);

int main(int argc, char *argv[])
{
	const char *DELIMS = "|";  //Delimiters
    const char *data = "Callback function called";
    const int BUFF_SIZE = 1024;

	sqlite3 *db;
	char *zErrMsg = 0;
    char *sql;
    char *line;	// Represents one line in the file
    char buffer[BUFF_SIZE];
    char *cdTokens[BUFF_SIZE];
    char *trackTokens[BUFF_SIZE];
    char *artistTokens[BUFF_SIZE];
    char *token;
	int rc;			//return code
	FILE *artist_in, *cd_in, *track_in;	// Files to read and popualate database with
	size_t nbytes = 100;	// Use to allocation the number of bytes to read for the line
    int i = 0;
    int artistTokenCount = 0;
    int cdTokenCount = 0;
    int trackTokenCount = 0;
    
//	/* Open files for reading */
//	openFiles(&artist_in, &cd_in, &track_in);
    
    artist_in = fopen("artist.txt", "r");
    cd_in = fopen("cd.txt", "r");
    track_in = fopen("track.txt", "r");
    
    /* Open files */
    if (artist_in == NULL || cd_in == NULL || track_in == NULL) {
        fprintf(stderr, "ERROR: did not successully open files\n");
        exit(1);
    } else {
        fprintf(stderr, "Successfully opened files\n");
    }
    
    /* Part A1  - tokenizing files*/
    while(fgets(buffer, BUFF_SIZE, artist_in) != NULL) {
        token = strtok(buffer, DELIMS);
        while(token != NULL) {
            artistTokens[i++] = strdup(token);      // will need to free tokens when done since this performs malloc
            token = strtok(NULL, DELIMS);
            artistTokenCount++;
        }
    }
    
    i = 0;
    
    while(fgets(buffer, BUFF_SIZE, cd_in) != NULL) {
        token = strtok(buffer, DELIMS);
        while(token != NULL) {
            cdTokens[i++] = strdup(token);      // will need to free tokens when done since this performs malloc
            token = strtok(NULL, DELIMS);
            cdTokenCount++;
        }
    }
    
    i = 0;
    
    
    while(fgets(buffer, BUFF_SIZE, track_in) != NULL) {
        token = strtok(buffer, DELIMS);
        while(token != NULL) {
            trackTokens[i++] = strdup(token);      // will need to free tokens when done since this performs malloc
            token = strtok(NULL, DELIMS);
            trackTokenCount++;
        }
    }
    
    //
    //
    //  Print tokens for assignment here
    //
    //
    
    /* Close files since done processing */
    fclose(artist_in);
    fclose(cd_in);
    fclose(track_in);
	
	/* Open the database */
	rc = sqlite3_open("song.db", &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
	} else {
		fprintf(stderr, "Opened database successfully\n");
	}
    
    /* Part A2 - create database tables and insert records */
    sql = (char*) malloc(500 * sizeof(char));
    sqlite3 *statement;
    
    /* Create tables */
    strcpy(sql, "create table cd (");
    strcat(sql, "id INTEGER AUTO_INCREMENT NOT NULL PRIMARY KEY,");
    strcat(sql, "title VARCHAR(70) NOT NULL,");
    strcat(sql, "artist_id INTEGER NOT NULL,");
    strcat(sql, "catalogue VARCHAR(30) NOT NULL);");
    
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stderr, "SQL statement successful: %s\n", zErrMsg);
    }
    
    free(sql);
    sql = (char*) malloc(500 * sizeof(char));
    
    strcpy(sql, "create table artist (");
    strcat(sql, "id INTEGER AUTO_INCREMENT NOT NULL PRIMARY KEY,");
    strcat(sql, "name VARCHAR(100) NOT NULL);");
    
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stderr, "SQL statement successful: %s\n", zErrMsg);
    }
    
    free(sql);
    sql = (char*) malloc(500 * sizeof(char));
    
    strcpy(sql, "create table track (");
    strcat(sql, "cd_id INTEGER NOT NULL,");
    strcat(sql, "track_id INTEGER NOT NULL,");
    strcat(sql, "title VARCHAR(70),");
    strcat(sql, "PRIMARY KEY(cd_id, track_id));");
    
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stderr, "SQL statement successful: %s\n", zErrMsg);
    }
    
    free(sql);
    
    /* Insert Records */
    for (i = 0; i < artistTokenCount; i++) {
        sql = (char*) malloc(500 * sizeof(char));
        token = artistTokens[i];
        int foundAlpha = 0;
        char c;
        /* Parse through token to see if it is a digit or alphanumeric */
        while ((c = getc(token)) != '\0') {
            if (isalpha(c)) {
                foundAlpha = 1;
            }
        }
        strcpy(sql, "insert into artist(id, name) values(");
        if (foundAlpha) {
            
        }
        
        free(sql);
    }
    

//	/* Create SQL statement */
//	sql = "SELECT * from COMPANY";
//
//	/* Execute the SQL statement */
//	rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
//	if (rc != SQLITE_OK) {
//		fprintf(stderr, "SQL error: %s\n", zErrMsg);
//		sqlite3_free(zErrMsg);
//	} else {
//		fprintf(stderr, "SQL statement successful: %s\n", zErrMsg);
//	}
//
//	/* Loop each record in company table */
//	sqlite3_stmt *statement;
//	sqlite3_prepare_v2(db, "SELECT * FROM artist", -1, &statement, NULL);
//	while (sqlite3_step(statement) == SQLITE_ROW) {
//		printf(" artist id=%u |", sqlite3_column_int(statement, 0));
//		printf(" artist name=%s \n", sqlite3_column_text(statement, 1));
//		//printf(" \n");
//	}
//	sqlite3_finalize(statement);

	sqlite3_close(db);
	return 0;
}

void openFiles(FILE **artist, FILE **cd, FILE **track) {
	*artist = fopen("artist.txt", "r");
	*cd = fopen("cd.txt", "r");
	*track = fopen("track.txt", "r");
	
	if (*artist == NULL || *cd == NULL || *track == NULL) {
		fprintf(stderr, "ERROR: did not successully open files\n");
		exit(1);
	} else {
		fprintf(stderr, "Successfully opened files\n");
	}
}

void tokenize(FILE **file, const int numTokens) {
	/*if (numTokens == 2) {

	} else if (numTokens == 3) {
 */
}
