/**
 *
 * \file sql_managing.c
 * Source of the library to manage the SQLite database
 *******************************************************************************/

#ifndef sql_managing_h
#include "sql_managing.h"
#endif
// **** Declarations of public functions **** //

/**
 * \name Public functions
 * Better organized in \ref sql_managing.h
 *******************************************************************************/
///@{


// *** Sampling variables managing *** //

// *** Database Initialization functions ***//

long int InitDB(sqlite3 **database, char * db_filename)
{
	int db_iserror;
	char *zErrMsg = 0;
	char query[1000];

	// Database opening
	db_iserror = sqlite3_open(db_filename, database);
	if(db_iserror!=SQLITE_OK)
	{
	  fprintf(stderr, "\n\nCan't open database: %s\n", sqlite3_errmsg(*database));
	  sqlite3_close(*database);
	  return(DB_ERROR);
	}

	//Table creation
	sprintf(query,"DROP TABLE IF EXISTS Species_Trees; DROP TABLE IF EXISTS Locus_Trees; DROP TABLE IF EXISTS Gene_Trees; PRAGMA foreign_keys=yes");
	db_iserror=sqlite3_exec(*database,query,NULL,NULL,&zErrMsg);


	sprintf(query,"CREATE TABLE IF NOT EXISTS Species_Trees(SID INTEGER PRIMARY KEY,Leaves INTEGER,Height_cu REAL,Length_cu REAL, Outgroup_intlength_gen REAL, Ind_per_sp INTEGER,N_loci INTEGER,Alpha_s REAL,Alpha_l REAL,Alpha_g REAL,Ne INTEGER,Mu REAL,Gen_time REAL,Alpha_site REAL, Shape_seqlength REAL, Logscale_seqlength REAL)");
	db_iserror=sqlite3_exec(*database,query,NULL,NULL,&zErrMsg);
	if(db_iserror!=SQLITE_OK)
	{
	  fprintf(stderr, "\n\nCan't create the tables: %s\n", zErrMsg);
	  sqlite3_free(zErrMsg);
	  sqlite3_close(*database);
	  return(DB_ERROR);
	}


	sprintf(query,"CREATE TABLE IF NOT EXISTS Locus_Trees(LID INTEGER PRIMARY KEY,n_ltree INTEGER NOT NULL,SID INTEGER NOT NULL,b_rate REAL,d_rate REAL,t_rate REAL,n_leaves INTEGER,n_dup INTEGER,n_loss INTEGER, n_transf INTEGER, gamma REAL, FOREIGN KEY(SID) REFERENCES Species_Trees(SID))");
	db_iserror=sqlite3_exec(*database,query,NULL,NULL,&zErrMsg);
	if(db_iserror!=SQLITE_OK)
	{
	  fprintf(stderr, "\n\nCan't create the tables: %s\n", zErrMsg);
	  sqlite3_free(zErrMsg);
	  sqlite3_close(*database);
	  return(DB_ERROR);
	}

	sprintf(query,"CREATE TABLE IF NOT EXISTS Gene_Trees(GID INTEGER PRIMARY KEY,n_gtree INTEGER NOT NULL,LID INTEGER NOT NULL,n_ltree INTEGER NOT NULL, SID INTEGER NOT NULL,n_leaves INTEGER,Extra_l INTEGER,Tree_h_cu REAL,Tree_l_bl REAL,FOREIGN KEY(LID) REFERENCES Locus_Trees(LID),FOREIGN KEY(SID) REFERENCES Species_Trees(SID))");
	db_iserror=sqlite3_exec(*database,query,NULL,NULL,&zErrMsg);
	if(db_iserror!=SQLITE_OK)
	{
	  fprintf(stderr, "\n\nCan't create the tables: %s\n", zErrMsg);
	  sqlite3_free(zErrMsg);
	  sqlite3_close(*database);
	  return(DB_ERROR);
	}
    sprintf(query,"BEGIN IMMEDIATE TRANSACTION");
	db_iserror=sqlite3_exec(*database,query,NULL,NULL,&zErrMsg);
	if(db_iserror!=SQLITE_OK)
	{
        fprintf(stderr, "\n\nCan't begin the transaction: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(*database);
        return(DB_ERROR);
	}
    
    return NO_ERROR;
}

long int CloseDB(sqlite3 **database)
{
	char *zErrMsg=NULL;
    char query[19]="COMMIT TRANSACTION";
	
	int db_iserror=0;
    
	db_iserror=sqlite3_exec(*database,query,NULL,NULL,&zErrMsg);
	
	if(db_iserror!=SQLITE_OK)
	{
  		fprintf(stderr, "\n\nCan't commit the results to the db: %s\n", zErrMsg);
  		sqlite3_free(zErrMsg);
  		sqlite3_close(*database);
  		return(DB_ERROR);
	}
    
    sqlite3_close(*database);
    
    return NO_ERROR;

}


// *** Database write values *** //

long int WriteSTreeDB(sqlite3 **database, int n_leaves, double height, double length, double outgroup, int ind_per_sp, int n_loci, double alpha_s, double alpha_l, double alpha_g, int Ne, double mu, double gen_time)
{
	char *query;
	char *zErrMsg=NULL;
	
	int db_iserror=0;
    
    query=calloc(1000,sizeof(char));
	
	sprintf(query,"INSERT INTO Species_Trees VALUES(NULL,'%d','%e','%e','%e','%d','%d','%e','%e','%e','%d','%e','%e',0,0,0)",n_leaves,height,length,outgroup,ind_per_sp,n_loci,alpha_s,alpha_l,alpha_g,Ne,mu,gen_time); //67+numbers
    
	db_iserror=sqlite3_exec(*database,query,NULL,NULL,&zErrMsg);
	
	if(db_iserror!=SQLITE_OK)
	{
  		fprintf(stderr, "\n\nCan't write into the db: %s\n", zErrMsg);
  		sqlite3_free(zErrMsg);
  		sqlite3_close(*database);
  		return(DB_ERROR);
	}
    free(query);
    return NO_ERROR;
}

long int WriteLTreeDB(sqlite3 **database, int n_ltree, int SID, double b_rate, double d_rate, double t_rate, int n_leaves, int n_dup, int n_loss, int n_transf, double gamma)
{
	char *query;
	char *zErrMsg=NULL;
	int db_iserror=0;
    query=calloc(1000, sizeof(char));
	
	sprintf(query,"INSERT INTO Locus_Trees VALUES(NULL,'%d','%d','%e','%e','%e','%d','%d','%d','%d','%e')",n_ltree,SID,b_rate,d_rate,t_rate,n_leaves,n_dup,n_loss,n_transf,gamma); //63+numbers
	db_iserror=sqlite3_exec(*database,query,NULL,NULL,&zErrMsg);
	
	if(db_iserror!=SQLITE_OK)
	{
  		fprintf(stderr, "\n\nCan't write into the db: %s\n", zErrMsg);
  		sqlite3_free(zErrMsg);
  		sqlite3_close(*database);
  		return(DB_ERROR);
	}
    free(query);
    return NO_ERROR;
}

long int WriteGTreeDB(sqlite3 **database, int n_gtree, unsigned long long LID, int n_ltree, int SID, int n_leaves, int extra_lineages, double tree_h_cu, double tree_l_ss)
{
	char *query;
	char *zErrMsg=NULL;
	
	int db_iserror=0;
    
    query=calloc(1000, sizeof(char));
	
	sprintf(query,"INSERT INTO Gene_Trees VALUES(NULL,'%d','%lld','%d','%d','%d','%d','%e','%e')",n_gtree,LID,n_ltree,SID,n_leaves,extra_lineages,tree_h_cu,tree_l_ss);//52+numbers
	db_iserror=sqlite3_exec(*database,query,NULL,NULL,&zErrMsg);
	
	if(db_iserror!=SQLITE_OK)
	{
  		fprintf(stderr, "\n\nCan't write into the db: %s\n", zErrMsg);
  		sqlite3_free(zErrMsg);
  		sqlite3_close(*database);
  		return(DB_ERROR);
	}
    
    free(query);
    return NO_ERROR;
}

///@}

