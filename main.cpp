#include<iostream>
#include<string>
#include<fstream> 
#include<sstream> 

using namespace std;

#define ARSIZE 5500  //for changing size on the go
#define COLMAX 37

struct Query{
    int colCount = 0;
    int whereCount = 0;
    string colList[COLMAX] = {"","","","","","","","","","",
                              "","","","","","","","","","",
                              "","","","","","",""};
    string whereLeft[5]  = {"","","","",""};
    string whereRight[5] = {"","","","",""};
    string logicList[5]  = {"","","","",""};
    string ANDORlist[4]      = {"", "", "", ""};
};

/*
Function Identifier: Takes in data from Satellite Database txt file and 
will input all the data into a 2D array called rawData
Parameters: Passes the rawData array
Return Type: No return type 
*/
void readData(string rawData[ARSIZE][COLMAX])
{
    //Declaring string variables to get data from input file
    string line, col;
    //Declaring input file variable and opening input file
    ifstream iFile;
    iFile.open("UCS-Satellite-Database-5-1-2022.txt");
    //For loop to input data into the 2D array
    for(int i = 0; i < ARSIZE; i++)
    {   
        //Using getline to retrieve i row in database file
        getline(iFile, line);
        //Creating sstream variable to get each column from row
        stringstream ss(line);
        getline(ss, col, '\t');
        for(int j = 0; j < COLMAX; j++)
        {
            //Setting each array value equal to the j column, then 
            //getting the next column until the end of the row
            rawData[i][j] = col;
            getline(ss, col, '\t');
        }
        //Clearing anything leftover from the sstream variable
        ss.clear();
    }
    
    iFile.close();
}

/*
Function Identifier: Will create an output file caled "output.txt" that
displays the satellite data in a continous line
Parameters: Passes the rawData array (which contains the data from the 
file)
Return Type: No return type
*/
void writeData(string rawData[ARSIZE][COLMAX])
{
    //Creating output file called "output.txt" and opening it
    ofstream oFile;
    oFile.open("output.txt");
    //2D array to output data into output.txt
    for(int i = 0; i < ARSIZE; i++)
    {   
        for(int j = 0; j < COLMAX; j++)
        {   
            //Inputting each column from data followed by *Z*
            oFile << rawData[i][j] << "*Z*";
        }
        //After all columns for a row are added, add *N* to show new row
        //begins
        oFile << "*N*";
    }
    oFile.close();
}

/*
Function Identifier: Making the string query into lowercase
Parameters: Passes string query into the function
Return Type: Returns a string type
*/
string lower(string str)
{
    //For loop to iterate until it reaches the end of the string
    for(int i = 0; i <str.length(); i++)
    {
        //For each i char, will be converted to a lowercase char
        str[i] = tolower(str[i]);
    }
    //Returning lowercase string to runQuery function
    return str;
}

/*
Function Identifier: Will check the syntax of each column and ensure 
user used commas to separate columns and used double quotes to enclose
each individual column
Parameters: Passing string into allCol that stores every column user
wants to make a query in
Return Type: Bool, will return 0 if no error, 1 if error
*/
bool columnSyntax(Query &q, string allCol, string db[][COLMAX])
{
    //Declaring subcolumn variable to store each individual column
    string subCol;
    int i = 0;
    //Will loop infinitely until it reaches a return statement
    while(!allCol.empty())
    {
        //With every iteration, takes a new substring that will obtain
        //a new column every time
        subCol = allCol.substr(0, allCol.find(',') + 1);
        //Checks to see if use inputted one column/if only column remains
        //to be checked for syntax
        if(allCol.find(',') == string::npos)
        {
            if(allCol == "* ")
            {
                q.colCount = 37;
                for(int i = 0; i < COLMAX; i++)
                {
                    q.colList[i] = db[0][i];
                }
                return 0;
            }
            //Checks to see if column starts with a double quote
            if(allCol.find_first_of('\"') != string::npos)
            {
                //Checks to see if column ends with a double quote
                if(allCol.find_last_of('\"') != string::npos)
                {
                    //If it reaches this point, no errors and will go 
                    //back to checkSyntax function
                    ++q.colCount;
                    allCol.erase(0,1);
                    allCol.erase(allCol.length() - 2);
                    q.colList[i] = allCol;
                    allCol.clear();
                    return 0;
                }
                else
                {
                    //Error
                    return 1;
                }
            }
            else
            {
                //Error
                return 1;
            }
        }
        //If user inputted multiple columns, checks to see if first 
        //column in subCol starts with a double quote
        else if(subCol.find_first_of('\"') != string::npos)
        {
            //Checks to see if subCol ends with a double quote
            if(subCol.find_last_of('\"') != string::npos)
            {
                ++q.colCount;
                subCol.erase(0,1);
                subCol.erase(subCol.length() - 2);
                q.colList[i] = subCol;
                i++;
                subCol.clear();
                //Will store a new allCol value that has the next 
                //column in it
                allCol = allCol.substr(allCol.find_first_of(',') + 2, 
                allCol.length() - 1);
            }
            else
            {
                //Error
                return 1;
            }
        }
        //Last check to see if user inputted a * for all columns
        else
        {
            //If none of t
            return 1;
        }
    }

    return 1;
}   

/*
Function Identifier: Checks to see if all from clauses were written 
correctly without any errors to proceed with program
Parameters: Passing the string partofQuery which contains everything
after the "where " word from the initial user query
Return Type: Bool, returns 0 if no errors, 1 if errors are found
*/
bool fromSyntax(Query &q, string partofQuery)
{
    int i = 0;
    string where, whereLeft, whereRight, logic, ANDOR;
    //Declaring array that contains all logic operators to be checked
    //later in the function
    string logicArray[6] = {"==", "!=", ">=", ">", "<=", "<"};
    while(!partofQuery.empty())
    {
        //For loop that continues until it reaches the length of the
        //partofQuery string
        for(int i = 0; i < partofQuery.length(); i++)
        {
            //ANDOR string stores each i character into new string
            ANDOR += partofQuery[i];
            //First if statement to check if ANDOR contains "AND"
            if(ANDOR.find("AND") != string::npos)
            {
                //Removing the ANDOR string from the partofQuery string
                //so next loop can start after the "AND" statement
                partofQuery = partofQuery.substr(ANDOR.find("AND") + 4, 
                partofQuery.length());
                break;
            }
            //Second if statement to check if ANDOR contains "OR"
            else if(ANDOR.find(" OR ") != string::npos)
            {
                //Removing the ANDOR string from the partofQuery string
                //so next loop can start after the "OR" statement
                partofQuery = partofQuery.substr(ANDOR.find(" OR ") + 4, 
                partofQuery.length());
                break;
            }
            /*If neither "AND"/"OR" is found, loop will auto exit once 
            //complete indicating that one where clause remains/or was 
            initially inputted */
        }
        //Checks to see if ANDOR string contains "AND", and it doesn't
        //return empty
        if(ANDOR.find("AND") != string::npos)
        {
            q.ANDORlist[i] = "AND";
            //Removing the "AND" portion of the string 
            where = ANDOR.substr(0, ANDOR.find("AND"));
            //For loop designed to check through the logicArray
            for(int i = 0; i < 6; i++)
            {
                //If statement to ensure correct logic symbol found in
                //where string, and it doesn't return empty
                if(where.find(logicArray[i]) != string::npos)
                {
                    //Will make logic variable = to whatever is at 
                    //logicArray at index i 
                    logic = logicArray[i];
                    break;
                }
            }
            //After logicArray loop, checks to see if logic is filled
            if(!logic.empty())
            {
                q.logicList[i] = logic;
                //whereLeft variable made to get everything before logic
                //operator
                whereLeft = where.substr(0, where.find(logic));
                //Checking to see if front/back double quotes are found
                if(whereLeft.find_first_of('\"') != string::npos) 
                {
                    if(whereLeft.find_last_of('\"') != string::npos)
                    {
                        whereLeft.erase(0, 1);
                        whereLeft.erase(whereLeft.length() - 2);
                        q.whereLeft[i] = whereLeft;
                        //Will create new variable called whereRight to
                        //get rest of string after logic oeprator
                        whereRight = where.substr(where.find(logic) + 2);
                        //Checking to see if front/back double quotes are
                        //found
                        if(whereRight.find_first_of('\"') 
                        != string::npos) 
                        {
                            if(whereRight.find_last_of('\"') 
                            != string::npos)
                            {
                                //Success! Clear all variables and loop 
                                //again
                                whereRight.erase(0, 2);
                                whereRight.erase(whereRight.length() 
                                - 2);
                                q.whereRight[i] = whereRight;
                                ++q.whereCount;
                                i++;
                                where.clear();
                                ANDOR.clear();
                                whereLeft.clear();
                                whereRight.clear();
                                continue;
                            }
                            //If no back double quotes found
                            else
                            {
                                //Error
                                return 1;
                            }
                        }
                        //If no front double quotes are found
                        else
                        {
                            //Error
                            return 1;
                        }
                    }
                    else
                    {
                        //Error
                        return 1;
                    }
                }
                //If no front double quotes
                else
                {
                    //Error
                    return 1;
                }
            }
            //If logic wasn't filled
            else
            {
                //Error
                return 1;
            }
        }
        //Checks to see if ANDOR string contains "OR", and it doesn't
        //return empty
        if(ANDOR.find(" OR ") != string::npos)
        {
            q.ANDORlist[i] = "OR";
            //Removing the "OR" portion of the string 
            where = ANDOR.substr(0, ANDOR.find(" OR "));
            //For loop designed to check through the logicArray
            for(int i = 0; i < 6; i++)
            {
                //If statement to ensure correct logic symbol found in
                //where string, and it doesn't return empty
                if(where.find(logicArray[i]) != string::npos)
                {
                    //Will make logic variable = to whatever is at 
                    //logicArray at index i 
                    logic = logicArray[i];
                    break;
                }
            }
            //After logicArray loop, checks to see if logic is filled
            if(!logic.empty())
            {
                q.logicList[i] = logic;
                //whereLeft variable made to get everything before logic
                //operator
                whereLeft = where.substr(0, where.find(logic) - 1);
                //Checking to see if front/back double quotes are found
                if(whereLeft.find_first_of('\"') != string::npos) 
                {
                    if(whereLeft.find_last_of('\"') != string::npos)
                    {
                        whereLeft.erase(0, 1);
                        whereLeft.erase(whereLeft.length() - 1);
                        q.whereLeft[i] = whereLeft;
                        //Will create new variable called whereRight to
                        //get rest of string after logic oeprator
                        whereRight = where.substr(where.find(logic) + 2);
                        //Checking to see if front/back double quotes are
                        //found
                        if(whereRight.find_first_of('\"') 
                        != string::npos) 
                        {
                            if(whereRight.find_last_of('\"') 
                            != string::npos)
                            {
                                //Success! Clear all variables and loop 
                                //again
                                whereRight.erase(0, 2);
                                whereRight.erase(whereRight.length() 
                                - 1);
                                q.whereRight[i] = whereRight;
                                ++q.whereCount;
                                i++;
                                where.clear();
                                ANDOR.clear();
                                whereLeft.clear();
                                whereRight.clear();
                                continue;
                            }
                            //If no back double quotes found
                            else
                            {
                                //Error
                                return 1;
                            }
                        }
                        //If no front double quotes are found
                        else
                        {
                            //Error
                            return 1;
                        }
                    }
                    else
                    {
                        //Error
                        return 1;
                    }
                }
                //If no front double quotes
                else
                {
                    //Error
                    return 1;
                }
            }
            //If logic wasn't filled
            else
            {
                //Error
                return 1;
            }
        }
        //Will reach this point only if it can't find any "AND"/"OR" or
        //if there's only one where clause inputted by
        else
        {
            where = ANDOR.substr(0, ANDOR.length());
            //For loop designed to check through the logicArray
            for(int i = 0; i < 6; i++)
            {
                //If statement to ensure correct logic symbol found in
                //where string, and it doesn't return empty
                if(where.find(logicArray[i]) != string::npos)
                {
                    //Will make logic variable = to whatever is at 
                    //logicArray at index i 
                    logic = logicArray[i];
                    break;
                }
            }
            //After logicArray loop, checks to see if logic is filled
            if(!logic.empty())
            {
                q.logicList[i] = logic;
                //whereLeft variable made to get everything before logic
                //operator
                whereLeft = where.substr(0, where.find(logic) - 1);
                //Checking to see if front/back double quotes are found
                if(whereLeft.find_first_of('\"') != string::npos) 
                {
                    if(whereLeft.find_last_of('\"') != string::npos)

                    {
                        whereLeft.erase(0, 1);
                        whereLeft.erase(whereLeft.length() - 1);
                        q.whereLeft[i] = whereLeft;
                        //Will create new variable called whereRight to
                        //get rest of string after logic oeprator
                        whereRight = where.substr(where.find(logic) + 1);
                        //Checking to see if front/back double quotes are
                        //found
                        if(whereRight.find_first_of('\"') 
                        != string::npos) 
                        {
                            if(whereRight.find_last_of('\"') 
                            != string::npos)
                            {
                                //Success!! No errors found
                                whereRight.erase(0, 3);
                                whereRight.erase(whereRight.length() 
                                - 2);
                                q.whereRight[i] = whereRight;
                                ++q.whereCount;
                                where.clear();
                                ANDOR.clear();
                                whereLeft.clear();
                                whereRight.clear();
                                return 0;
                            }
                            //If no back double quotes found
                            else
                            {
                                //Error
                                return 1;
                            }
                        }
                        //If no front double quotes are found
                        else
                        {
                            //Error
                            return 1;
                        }
                    }
                    else
                    {
                        //Error
                        return 1;
                    }
                }
                //If no front double quotes
                else
                {
                    //Error
                    return 1;
                }
            }
            //If logic wasn't filled
            else
            {
                //Error
                return 1;
            }
        }

    }
    return 1;
}

/*
Function Identifier: Checks to see if the query was written correctly
Parameters: Passing the lowercase string query from lower function
Return Type: Bool, returns 0 if no error, 1 if error found in query
*/
bool checkSyntax(Query &q, string query, string db[][COLMAX])
{
    string partofQuery, allCol;
    //For loop that iterates to get first 7 characters of query
    for(int i = 0; i < 7; i++) 
    {
        partofQuery += query[i]; 
    } 
    //If statement to check if first 7 characters are equal to "select "
    if(partofQuery == "SELECT ") 
    {
        partofQuery.clear();
        //For loop to input the rest of the query into partofQuery
        //Does not include "select " portion of string
        for(int i = 7; i < query.length(); i++) 
        {
            partofQuery += query[i]; 
        }
        //allCol variable will contain every column that user inputted
        allCol = partofQuery.substr(0,partofQuery.find("FROM"));
        //Calling columnSyntax function to check to see if columns
        //were inputted correctly
        columnSyntax(q, allCol, db);
        //If columnSyntax found no errors
        if(true)
        {
            //Creating new string variable called from that takes in 
            //everything starting
            //with the "from" portion of the partofQuery string
            string from;
            from = partofQuery.substr(partofQuery.find("FROM"), 
            partofQuery.length());
            partofQuery.clear();
            //For loop designed to get the first 14 characters of from 
            //string
            for(int i = 0; i < 14; i++) 
            {
                partofQuery += from[i];
            }
            //If first 14 characters of from = "from db where "
            if(partofQuery == "FROM DB WHERE ") 
            {
            partofQuery.clear();
            //Taking new partofQuery substring that has all of where's 
            //condition
            partofQuery = from.substr(from.find("WHERE") + 6, 
            from.length());
            fromSyntax(q, partofQuery);
            //If fromSyntax found no errors
            if(true)
            {
                //Checking to see if last character is a semicolon
                if(partofQuery.back() != ';')
                {
                    //Error
                    return 1;
                }
                return 0;
            }
            //If fromSyntax has errors
            else
            {
                //Error
                return 1;
            }
            }
            //IF first 14 characters do not = "from db where"
            else 
            {
                return 1;
            }
        }
        //If columnSytax did find errors
        else
        {
            //Error
            return 1;
        }

    }
    else
    {
        return 1;
    }

    return 1;
}

string output[ARSIZE][COLMAX];

/*
Function Identifier: Checking to see if the whereLeft part of the current
where statement is a valid column name or not
Parameters: Passing, by reference, the query, database array, and the
current where statement index
Return Type: Bool, return 1 if no error, return 0 if errors
*/
bool whereLeft(Query &q, string db[][COLMAX], int &whereIndex, 
int &colIndex)
{
    //For loop to iterate through all the columns
    for(int k = 0; k < COLMAX; k++)
    {
        //Checking to see if whereLeft at j corresponds with a 
        //column name in database
        if(db[0][k] == q.whereLeft[whereIndex])  
        {
            //Storing the index of the column if name is found
            colIndex = k;
            return 1;
        }
        else if(k == 36)
        {
            //Error
            return 0;
        }
    }
    return 0;
}

/*
Function Identifer: Will determine the truth value of the whereRight
part of the current where statement
Parameters: Passing, by reference, the query, database array, boolean
array, and current where statement index alongside which column to check
Return Type: No return type
*/
void whereLogic(Query &q, string db[][COLMAX], bool array[], int 
&whereIndex, int colIndex)
{
    //If logic list contains ==
    if(q.logicList[whereIndex] == "==")
    {
        //For loop to iterate through each row
        for(int i = 1; i < ARSIZE; i++)
        {
            //Will check each row at whereLeft column to see if 
            //it matches the corresponding whereRight phrase
            if(db[i][colIndex] == q.whereRight[whereIndex])
            {
                //Becomes true index
                array[i] = true;
            }
            //If condition is not met
            else
            {
                //Becomes false index
                array[i] = false;
            }
        }
    }     
    //If logic list contains !=
    if(q.logicList[whereIndex] == "!=")
    {
        //For loop to iterate through each row
        for(int i = 1; i < ARSIZE; i++)
        {
            //Will check each row at whereLeft column to see if 
            //it matches the corresponding whereRight phrase
            if(db[i][colIndex] != q.whereRight[whereIndex])
            {
                //Becomes true index
                array[i] = true;
            }
            //If condition is not met
            else
            {
                //Becomes false index
                array[i] = false;
            }
        }
    }
    //If logic list contains >=
    if(q.logicList[whereIndex] == ">=")
    {
        //For loop to iterate through each row
        for(int i = 1; i < ARSIZE; i++)
        {
            //Will check each row at whereLeft column to see if 
            //it matches the corresponding whereRight phrase
            if(db[i][colIndex] < q.whereRight[whereIndex])
            {
                //Becomes true index
                array[i] = true;
            }
            //If condition is not met
            else
            {
                //Becomes false index
                array[i] = false;
            }
        }
    }

    //If logic list contains >
    if(q.logicList[whereIndex] == ">")
    {
        //For loop to iterate through each row
        for(int i = 1; i < ARSIZE; i++)
        {
            //Will check each row at whereLeft column to see if 
            //it matches the corresponding whereRight phrase
            if(db[i][colIndex] > q.whereRight[whereIndex])
            {
                //Becomes true index
                array[i] = true;
            }
            //If condition is not met
            else
            {
                //Becomes false index
                array[i] = false;
            }
        }
    }  
    //If logic list contains <=
    if(q.logicList[whereIndex] == "<=")
    {
        //For loop to iterate through each row
        for(int i = 1; i < ARSIZE; i++)
        {
            //Will check each row at whereLeft column to see if 
            //it matches the corresponding whereRight phrase
            if(db[i][colIndex] <= q.whereRight[whereIndex])
            {
                //Becomes true index
                array[i] = true;
            }
            //If condition is not met
            else
            {
                //Becomes false index
                array[i] = false;
            }
        }
    }  
    //If logic list contains <
    if(q.logicList[whereIndex] == "<")
    {
        //For loop to iterate through each row
        for(int i = 1; i < ARSIZE; i++)
        {
            //Will check each row at whereLeft column to see if 
            //it matches the corresponding whereRight phrase
            if(db[i][colIndex] < q.whereRight[whereIndex])
            {
                //Becomes true index
                array[i] = true;
            }
            //If condition is not met
            else
            {
                //Becomes false index
                array[i] = false;
            }
        }
    }      
}

/*
Function Identifier: Will create the queryoutput.txt file 
Parameters: Passing, by reference, the query at q and the database array
Return Type: Bool, will return 0 if error occurs, 1 if no errors
*/
bool generateResults(Query &q, string db[][COLMAX])
{
    ofstream oFile; 
    oFile.open("queryoutput.txt");
    oFile << "Queries: \n";

    bool array[ARSIZE] = {false};
    bool arraySecond[ARSIZE] = {false};
    bool arrayMAIN[ARSIZE] = {false};
    int colIndex = 0;
    int rowCount = 0;
    //Main loop that will go through each row of the database array
    //for(int i = 1; i < ARSIZE; i++)
    //{
        //Secondary loop to account for each where statement
        for(int j = 0; j < q.whereCount; j++)
        {
            //If statement to occur only during the first iteration of
            //loop when on the first where statement
            if(j == 0)
            {
                //Checks to see if whereLeft is valid or not
                //whereLeft(q, db, j, colIndex);
                if( whereLeft(q, db, j, colIndex))
                {
                     //Calls function to get boolean values of first 
                     //where statement
                    whereLogic(q, db, array, j, colIndex);
                }
                else
                {
                    return 0;
                }
            }
            //If there is an OR logical operator
            if(q.ANDORlist[j] == "OR")
            {
                //Iterate to next where statement
                j++;
                //Calling function and seeing if error is returned
                //for next where statement
                if(!whereLeft(q, db, j, colIndex))
                {
                    //Error
                    return 0;
                }
                //Calling function to get truth values for current
                //where statement
                whereLogic(q, db, arraySecond, j, colIndex);
                for(int i = 0; i < ARSIZE; i++)
                {
                    //If either values in bool arrays are true
                    if(array[i] || arraySecond[i])
                    {
                        //Make corresponding row true
                        arrayMAIN[i] = true;
                        //Add to row count
                        rowCount++;

                    }
                    else
                    {
                        //Make corresponding row false
                        arrayMAIN[i] = false;
                    }
                }
                //If there are more ANDORs to check for
                if(q.ANDORlist[j] != "")
                {
                    for(int i = 0; i < ARSIZE; i++)
                    {
                        //Setting first array values to equal main array
                        //values
                        array[i] = arrayMAIN[i];
                        //Setting all second array values to false
                        arraySecond[i] = false;
                        //Restarting row count
                        rowCount = 0;
                    }
                }
            }
            //If there is an AND logical operator
            else if (q.ANDORlist[j] == "AND")
            {
                //Iterate to next where statement
                j++;
                //Calling function and seeing if error is returned
                //for next where statement
                if(!whereLeft(q, db, j, colIndex))
                {
                    //Error
                    return 0;
                }
                //Calling function to get truth values for current
                //where statement
                whereLogic(q, db, arraySecond, j, colIndex);
                for(int i = 0; i < ARSIZE; i++)
                {
                    //If BOTH values in bool arrays are true
                    if(array[i] && arraySecond[i])
                    {
                        //Make corresponding row true
                        arrayMAIN[i] = true;
                        //Add to the row count
                        rowCount++;

                    }
                    else
                    {
                        //Make corresponding row false
                        arrayMAIN[i] = false;
                    }
                }
                //If there are more ANDORs to check for
                if(q.ANDORlist[j] != "")
                {
                    for(int i = 0; i < ARSIZE; i++)
                    {
                        //Setting first array values to equal main array
                        //values
                        array[i] = arrayMAIN[i];
                         //Setting all second array values to false
                        arraySecond[i] = false;
                        //Restarting row count
                        rowCount = 0;
                    }
                }
            }
             
            //If there is NO logical operators
            else
            {
                //For loop to run through each row
                for(int i = 0; i < ARSIZE; i++)
                {
                    //Setting first array's values equal to main array
                    arrayMAIN[i] = array[i];
                    if(arrayMAIN[i])
                    {
                        rowCount++;
                    }
                }
            }
        }
    int colNum[COLMAX];
    //For loop designed to input "Select" column names into the output
    //array
    //Will iterate based on the column count
    for(int i = 0; i < q.colCount; i++)
    {
        //Secondary for loop to loop through each column name in database
        //array
        for(int j = 0; j < COLMAX; j++)
        {
            //If column name is found in the database array
            if(q.colList[i] == db[0][j])
            {
                //Will add column name to output array's first row
                output[0][i] = q.colList[i];
                colNum[i] = j;
                break;
            }
            //If column name can't be found in database array
            else if(j == 37) 
            {
                //Error
                return 0;
            }

        }     
    }
    //Declaring variables to set up initial conditions of output array
    int outputRow = 1, outputCol = 0;
    //Main loop that iterates through the total number of columns 
    for(int i = 0; i < q.colCount; i++)
    {
        //Secondary for loop that iterates through all rows
        for(int j = 1; j < ARSIZE; j++)
        {
            //Checks to see if value in array is true
            if(arrayMAIN[j])
            {
                //Output will equal whatever the column number index
                //is at the current j row
                output[outputRow][outputCol] = db[j][colNum[i]];
                //Iterate row each time
                outputRow++;
            }
        }
        //Once all rows have been iterated, iterate column
        outputCol++;
        //Reset rows to start next column to input data into
        outputRow = 1;
    }
  
    oFile << "Query: \n";

    //For loop designed to output all data from output array
    //Iterates based on how many rows are found to be true
    for(int i = 0; i <= rowCount; i++)
    {
        //Secondary for loop that iterates through the number of 
        //columns found in output array
        for(int j = 0; j < q.colCount; j++)
        {
            //Tab will separate each element in output array
            oFile << output[i][j] << '\t';
        }
        //After each row is completed, output new line
        if(q.colCount == 37)
        {
            q.colCount = q.colCount - 6;
        }
        oFile << "\n";
    }
    //Output total number of rows as last line of outputquery
    oFile << "Total Rows: " << rowCount;

    oFile.close();
    return 1;
}

/*
Function Identifier: Responsible for analyzing the query, checks if
there's any syntax/semantic errors, and delivers the queryoutput file
Parameters: Reference parameter for the query (in case user wants
to do another query) and the rawData array 
Return Type: No return type
*/
void runQuery(string &query, string db[][COLMAX])
{
    Query q;
    //Calls to lower function to make all characters lowercase in string
    //Also, checks to see if user typed in q to end program
    if(lower(query) == "q")
    {
        query = "q";
        return;
    }
    //Select "COL6", "COL5", "COL4" FROM DB WHERE "COL3" == "6"
    //Calls checkSyntax function to see if user inputted a valid query
    if(checkSyntax(q, query, db))
    {
        cout << "Error: Invalid Query Syntax. "
             << "Get motivated. Try again!" << endl;
        return;
    }
    
    if(!generateResults(q, db))
    {
         cout << "Error: Invalid Query Semantic. "
              << "Get motivated. Try again!" << endl;
    } 
} 

/*
Function Identifier: Main function for this program
Parameters: None
Return Type: Int return type (No value returned)
*/
int main()
{
    string rawData[ARSIZE][COLMAX]; //first row has col names
    readData(rawData);
    writeData(rawData); //output.txt

    cout << "Welcome to the Satellite Database at home" << endl;
    string query = "";

    for(int i=0; i < ARSIZE; i++) //Clean up output global array
        for(int j=0; j< COLMAX; j++) output[i][j] = "";
    ofstream fout; fout.open("queryoutput.txt"); //Cleanup
    fout << "Queries:" << endl; fout.close(); //Cleanup output file

    //query="SELECT \"Users\" FROM DB;";   //Debug
    //runQuery(query, rawData); return 0;//Debug

    while(query != "q"){
        cout << "Please enter your query:" << endl;
        getline(cin, query);
        runQuery(query, rawData); //pass by ref
    }

    return 0;
}
