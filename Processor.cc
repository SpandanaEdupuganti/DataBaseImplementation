#include "Processor.h"

#include <iostream>

#include "Errors.h"
#include "ParseTree.h"
#include "Queryops.h"
#include "Statistics.h"
#include "QueryPlan.h"

using std::cout;

extern "C" {
  int yyparse(void);     // defined in y.tab.c
}

extern struct FuncOperator *finalFunction; // the aggregate function (NULL if no agg)
extern struct TableList *tables; // the list of tables and aliases in the query
extern struct AndList *boolean; // the predicate in the WHERE clause
extern struct NameList *groupingAtts; // grouping atts (NULL if no grouping)
extern struct NameList *attsToSelect; // the set of attributes in the SELECT (NULL if no such atts)
extern int distinctAtts; // 1 if there is a DISTINCT in a non-aggregate query
extern int distinctFunc;  // 1 if there is a DISTINCT in an aggregate query
extern char* newtable;
extern char* oldtable;
extern char* newfile;
extern char* deoutput;
extern struct AttrList *newattrs; //Use this to build Attribute* array and record schema

void Processor::run() {
  char *fileName = "Statistics.txt";
  /*  Statistics s;

  Ddl d;
  QueryPlan plan(&s); */
      Statistics s;
  Queryops d;
  QueryPlan plan(&s);
bool started= false;
int option;
  while(true) {
    cout<<"1: Start a Database"<<endl;
    cout<<"2: RUn a Query" <<endl;
    cout << "3: Shudown a Database"<<endl;;
    cin>>option;
switch(option){
  case 1: cout<<"-----------DATABASE STARTED---------------" << endl;
	        started=true;
          break;
  case 2:
    if(!started){cout<<"DataBase is not yet started"<<endl;}
    else{ 
    cout << "Enter your CNF(included with ';'): " << endl;
    // if (yyparse() != 0) {
    //   cout << "Can't parse your CNF.\n";
    //   continue;
    // }
    yyparse();

    s.Read(fileName);

    if (newtable) {
      if (d.createTable()) cout << "Created  " << newtable <<" Table;" << std::endl;
      else cout << "Table " << newtable << " already exists." << std::endl;
    } else if (oldtable && newfile) {
      if (d.insertInto()) cout << "Inserted into " << oldtable << std::endl;
      else cout << "Insert failed." << std::endl;
    } else if (oldtable && !newfile) {
      if (d.dropTable()) cout << "Dropped table " << oldtable <<" table" << std::endl;
      else cout << "Table " << oldtable << " does not exist." << std::endl;
    } else if (deoutput) {
      plan.setOutput(deoutput);
    } else if (attsToSelect || finalFunction) {
      plan.plan();
      plan.print();
      plan.execute();
    }
    }
    break;

case 3: if (!started){cout<<"-----------DATABASE Need to be started before shutting Down-----------"<<endl;}
	      else{cout<< "-----------DATABASE SHUTDOWN-----------"<<endl;
	       exit(0);}
         break;
}
    clear();
    //exit(0);
  }
}

// TODO: free lists
void Processor::clear() {
  newattrs = NULL;
  finalFunction = NULL;
  tables = NULL;
  boolean = NULL;
  groupingAtts = NULL;
  attsToSelect = NULL;
  newtable = oldtable = newfile = deoutput = NULL;
  distinctAtts = distinctFunc = 0;
  FATALIF (!remove ("*.tmp"), "remove tmp files failed");
}
