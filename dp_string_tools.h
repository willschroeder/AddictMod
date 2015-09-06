//This is a class to create, delete, and populate double pointers, handy for managing flags

#ifndef DP_STRING_TOOLS
#define DP_STRING_TOOLS

class dp_string 
{
public:
	dp_string(int);
	~dp_string();

	bool addString(const char *);
	bool operator =(char *);

	int getMaxItems(void);

	//The actual pointer
	char ** dpList;

private:
	int numItems;
	int numMaxItems;

};

#endif