#include <bits/stdc++.h>
#include <unistd.h> 
using namespace std; 

pthread_mutex_t del_lock, get_lock, put_lock, db_lock;

const int ALPHABET_SIZE = 58; 
struct Slice
{
	uint8_t size;
	char *data;

	Slice()
	{
		data = (char *) malloc(256);
		strcpy(data,"\0");
		size = 0;
	}
};

struct TrieNode 
{ 
	struct TrieNode* children[ALPHABET_SIZE]; 
	long long int descendants;
	bool isEndOfWord; 
	char* value;
	Slice word;

	TrieNode() {
		value =(char *) malloc(64);
		word.data=(char *)malloc(256);
	}
}; 

struct TrieNode* getNode(char* value) 
{ 
	struct TrieNode* pNode = new TrieNode; 
	strcpy(pNode->value, value);

	pNode->isEndOfWord = false;
	pNode->descendants = 0;

	for (int i = 0; i < ALPHABET_SIZE; i++) 
		pNode->children[i] = NULL; 

	return pNode; 
}

bool search(struct TrieNode* root, char* key, Slice &value, register int add=0) 
{ 
	struct TrieNode* pCrawl = root; 
	while(pCrawl != NULL && *key != '\0')
	{
		int index = (*key) - 'A'; 
		
		if (!pCrawl->children[index]) 
			return false;
		else 
		{
			pCrawl->descendants+=add;
			
			if(strcmp(key,pCrawl->children[index]->value) == 0)
			{
				strcpy(value.data,pCrawl->children[index]->word.data);
				value.size=pCrawl->children[index]->word.size;
				
				return (pCrawl->children[index]->isEndOfWord); 
			}
			else
			{	
				int size = 0;
				
				char *temp1 = key, *temp2 = pCrawl->children[index]->value;
				while(*temp1 == *temp2 && *temp1 != '\0' && *temp2 != '\0')
				{
					size++;
					temp1++;	temp2++;
				}
				key += size;
				pCrawl = pCrawl->children[index];				
			}
		}
	} 
} 

void insert(struct TrieNode* root, char *key, Slice value, bool &f) 
{ 
	struct TrieNode* pCrawl = root;
	char rand[] = ""; 

	if(*(key) == '\0')
		return;
	else
	{ 
		int index = *(key) - 'A'; 
		pCrawl->descendants += 1;
		
		if (!pCrawl->children[index]) 
		{
			char str[strlen(key)+1];
			strcpy(str,key);
			str[strlen(key)] = '\0';

			pCrawl->children[index] = getNode(str); 
			strcpy(pCrawl->children[index]->value, str);
			
			pCrawl->children[index]->isEndOfWord = true; 
			
			strcpy(pCrawl->children[index]->word.data,value.data); 
			pCrawl->children[index]->word.size = value.size;
		}
		else 
		{
			int size = 0;
			char *temp1 = key, *temp2 = pCrawl->children[index]->value;
			
			while(*temp1 == *temp2 && *temp1 != '\0' && *temp2 != '\0')
			{
				size++;
				temp1++;	temp2++;
			}		
			
			char str1[64], str2[64];
			strcpy(str1,temp1);
			strcpy(str2,temp2);

			if(*temp1 != '\0' && *temp2 != '\0')
			{
				strcpy(pCrawl->children[index]->value, str2);
				pCrawl->descendants -= 1;
				void * temp = pCrawl->children[index];
				pCrawl->children[index] = NULL;

				insert(pCrawl, key, value,f);

				insert(pCrawl->children[index], str1, value,f);
				pCrawl->children[index]->isEndOfWord = false;

				pCrawl->children[index]->children[str2[0]-'A'] = (TrieNode*)temp;
				pCrawl->children[index]->descendants += pCrawl->children[index]->children[str2[0]-'A']->descendants;
				
				if(pCrawl->children[index]->children[str2[0]-'A']->isEndOfWord)
					pCrawl->children[index]->descendants += 1;			

			}
			else if(*temp1 == '\0' && *temp2 != '\0')
			{
				strcpy(pCrawl->children[index]->value, str2);
				pCrawl->descendants -= 1;
				void * temp = pCrawl->children[index];
				pCrawl->children[index] = NULL;

				insert(pCrawl, key,value,f);
				
				pCrawl->children[index]->children[str2[0]-'A'] = (TrieNode*)temp;
				pCrawl->children[index]->descendants += pCrawl->children[index]->children[str2[0]-'A']->descendants ;
				
				if(pCrawl->children[index]->children[str2[0]-'A']->isEndOfWord)
					pCrawl->children[index]->descendants += 1;					
			}
			else if(*temp2 == '\0' && *temp1 != '\0')
				insert(pCrawl->children[index], str1,value,f);
			else
			{
				pCrawl->children[index]->word = value;
		
				if(pCrawl->children[index]->isEndOfWord == true)
					f = true;
				
				pCrawl->children[index]->isEndOfWord = true;
			}

			char str[size+1];
			strncpy(str,pCrawl->children[index]->value,size);
			str[size] = '\0';
			strcpy(pCrawl->children[index]->value,str);
		} 
	} 
} 


bool isEmpty(TrieNode* root) 
{ 
	for (int i = 0; i < ALPHABET_SIZE; i++) 
		if (root->children[i]) 
			return false; 
	return true; 
} 

TrieNode* TrieRemove(TrieNode* root, Slice key, uint8_t depth = 0) 
{ 
	if (!root) 
		return NULL; 

	if (depth == key.size) 
	{ 
		if (root->isEndOfWord) 
			root->isEndOfWord = false; 

		if (isEmpty(root)) { 
			delete (root); 
			root = NULL; 
		} 

		return root; 
	} 

	int index = key.data[depth] - 'A'; 

	int size = 0;				
	char *temp1 = key.data + depth, *temp2 = root->children[index]->value;
	while(*temp1 == *temp2 && *temp1 != '\0' && *temp2 != '\0')
	{
		size++;
		temp1++;	temp2++;
	}
	
	if(*temp2 == '\0')
		root->children[index] = TrieRemove(root->children[index], key, depth + size); 

	if (isEmpty(root) && root->isEndOfWord == false)
	{
		delete (root);
		root = NULL;
	}
	
	else
		root->descendants -= 1;

	return root;
} 


void TrieSearchN(struct TrieNode *root, register int n, char * str, Slice &value) 
{ 
	int x = 0, count = 0;
	int *current = &x;
	
	struct TrieNode *pCrawl = root; 
	struct TrieNode *temp = NULL;
	
	while(*current < n && pCrawl != NULL)
	{
		temp = NULL;

		for (int i = 0; i < ALPHABET_SIZE && temp == NULL ; i++) 
		{	
			if (pCrawl->children[i]) 
			{
				if(pCrawl->children[i]->isEndOfWord == true)
		        	*current += 1;
		        
		        register int sum = *current + pCrawl->children[i]->descendants;

		        if(sum < n) 
		        	*current = sum;
		        
		        else if(sum >= n)
		        	temp = pCrawl->children[i];
			}
		}
		pCrawl = temp;
		
		if(pCrawl != NULL)
			strcat(str,pCrawl->value);
		
		count++;
	}
	
	strcpy(value.data, pCrawl->word.data);
	value.size = pCrawl->word.size;
	value = pCrawl->word;
} 

void TrieRemoveN(TrieNode* root, int N)
{
	char str[64] = "\0";
	Slice value;
	
	TrieSearchN(root,N,str,value);
	
	Slice key;
	strcpy(key.data,str);
	key.size=(uint8_t)strlen(str);
	
	TrieRemove(root,key);
}


class kvStore
{
	public:
		
		struct TrieNode* root;
		
		kvStore(uint64_t max_entries)
		{
			pthread_mutex_init(&del_lock, NULL);
			pthread_mutex_init(&put_lock, NULL);
			pthread_mutex_init(&get_lock, NULL);
			pthread_mutex_init(&db_lock, NULL);

			char r[10]="root";			
			root = getNode(r); 
		};
		
		bool get(Slice& key, Slice& value)
		{
			pthread_mutex_lock(&put_lock);
			pthread_mutex_lock(&del_lock);
			
			char str[65];	int i;
			
			for(i=0; i<key.size; i++)
				str[i]=key.data[i];
			
			str[i]='\0';
			
			int x = search(root, str, value);
			key.size=(uint8_t)strlen(str);
			
			pthread_mutex_unlock(&put_lock);
			pthread_mutex_unlock(&del_lock);
			return x;
		}

		bool put(Slice& key, Slice& value)
		{
			pthread_mutex_lock(&get_lock);
			pthread_mutex_lock(&del_lock);
			bool f = false;		char str[65];	register int i;
			
			for(i=0; i<key.size; i++)
				str[i]=key.data[i];
			
			str[i]='\0';
			
			insert(root, str, value, f);
			
			if(f)
				search(root, str, value, -1);

			pthread_mutex_unlock(&get_lock);
			pthread_mutex_unlock(&del_lock);
			
			return f;
		}

		bool del(Slice& key)
		{
			pthread_mutex_lock(&put_lock);
			pthread_mutex_lock(&get_lock);
			TrieRemove(root, key);
			pthread_mutex_unlock(&put_lock);
			pthread_mutex_unlock(&get_lock); 
			return true; 
		}

		bool get(int N,Slice& key, Slice& value)
		{
			pthread_mutex_lock(&put_lock);
			pthread_mutex_lock(&del_lock);
			N+=1;
			
			if(N<=root->descendants)
			{
				TrieSearchN(root,N,key.data,value);
				key.size=(uint8_t)strlen(key.data);
				pthread_mutex_unlock(&put_lock);
				pthread_mutex_unlock(&del_lock);
				return true;
			}
			else {
				pthread_mutex_unlock(&put_lock);
				pthread_mutex_unlock(&del_lock);
				return false;
			}
		}

		bool del(int N)
		{
			pthread_mutex_lock(&put_lock);
			pthread_mutex_lock(&get_lock);
			N+=1;

			
			if(N<root->descendants)
			{
				TrieRemoveN(root,N);
				pthread_mutex_unlock(&put_lock);
			pthread_mutex_unlock(&get_lock);	
				return true;
			}
			else {
				pthread_mutex_unlock(&put_lock);
			pthread_mutex_unlock(&get_lock);
				return false;
			}
		
		}
};