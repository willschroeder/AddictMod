#include "eiface.h"
#include "dp_string_tools.h"

dp_string::dp_string(int n)
{

	//Make sure its not 0
	if(n==0)
		n = 1;
	
	dpList = new char *[n];
	numMaxItems = n;
	numItems = 0;

	for(int i = 0; i < n;i++)
		dpList[i] = NULL;
}

dp_string::~dp_string()
{

	for(int i = 0; i < numMaxItems; i++)
		if(dpList[i] != NULL)
				delete dpList[i];


	//Delete the array of pointers
	delete [] dpList;

}

bool dp_string::addString(const char * s)
{
	if(numItems == numMaxItems || s == NULL)
		return false;

	dpList[numItems] = new char[Q_strlen(s)+1];
	Q_strcpy(dpList[numItems],s);
	numItems++;
	
	return true;
}

bool dp_string::operator =(char * s)
{
	return addString(s);	
}


int dp_string::getMaxItems()
{
	return numMaxItems;
}