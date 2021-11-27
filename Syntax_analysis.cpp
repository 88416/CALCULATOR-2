#include<stdio.h>
#include<string.h>
#include<stdlib.h>

enum Type {
	Num, Id, Sign, LeftBarcket, RightBarcket, Equal, Point, Unknow
};

char NUM[] = "1234567890";
char ID[] = "abcdefghijklmnopqrstuvwxyz";
char SIGN[] = "+-*/";

struct WordNode {
	int line, col;
	char ch;
	WordNode* next;
};
struct Token {
	Type type;
	WordNode* wordnode;
};
struct TokenNode {
	Token token;
	TokenNode* next;
};

/*------------------------------------------------------------------------------------*/
WordNode* CreateWordHead() {
	WordNode* head = (WordNode*)malloc(sizeof(WordNode));
	head->next = NULL;
	return head;
}

WordNode* FindWordLast(WordNode* head) {  
	WordNode* p = head;
	while (p->next) {
		p = p->next;
	}
	return p;
}

void InsertWord(WordNode* head, char ch, int line = 0, int col = 0) {  //锟斤拷锟斤拷锟斤拷锟�
	WordNode* newWordNode = (WordNode*)malloc(sizeof(WordNode));
	newWordNode->next = NULL;
	newWordNode->ch = ch;
	newWordNode->line = line;
	newWordNode->col = col;
	WordNode* p = FindWordLast(head);
	p->next = newWordNode;
}

void FreeWord(WordNode* head) {
	WordNode* p = head->next;
	free(head);
	if(!p)	return;
	WordNode* f;
	while (p) {
		f = p;
		p = p->next;
		free(f);
	}
}

void PrintWord(WordNode* head) {
	WordNode* p = head->next;
	while (p) {
		printf("%c", p->ch);
		p = p->next;
	}
}
/*---------------------------------------------------------------------------*/
TokenNode* CreateTokenHead() { //锟斤拷锟斤拷锟斤拷锟斤拷头
	TokenNode* head = (TokenNode*)malloc(sizeof(TokenNode));
	head->next = NULL;
	return head;
}

TokenNode* FindTokenLast(TokenNode* head) {  //锟揭碉拷锟斤拷锟斤拷末尾一锟斤拷
	TokenNode* p = head;
	while (p->next) {
		p = p->next;
	}
	return p;
}

void InsertToken(TokenNode* head, Token token) {  //锟斤拷锟斤拷锟斤拷锟�
	TokenNode* newTokenNode = (TokenNode*)malloc(sizeof(TokenNode));
	newTokenNode->next = NULL;
	newTokenNode->token = token;
	TokenNode* p = FindTokenLast(head);
	p->next = newTokenNode;
}

Token CreateToken(Type type) {
	WordNode* wordNodeHead = CreateWordHead();
	Token token = { type, wordNodeHead };
	return token;
}

void PrintTokens(TokenNode* head) {
	TokenNode* p = head->next;
	WordNode* p1;
	while (p) {
		p1 = p->token.wordnode;
        printf("%d  ", p->token.type);
		PrintWord(p1);
		printf("\n");
		p = p->next;
	}
}

void ReleaseTokenList(TokenNode* head){
	if(head->next == NULL){
		return;
	}
	TokenNode* f;
	TokenNode* p = head->next;
	head->next = NULL;
	while(p){
		f = p;
		p = p->next;
		FreeWord(f->token.wordnode);
		free(f);
	}
}

bool ReadWordPP(char* s, TokenNode* head){
	int startIndex = 0;
	int endIndex = 0;
	int length = strlen(s);
	Token* newToken = (Token*)malloc(sizeof(Token));
	newToken->wordnode = CreateWordHead();
	int state = 0;
	Type t;//录入单词类型
	char c;//录入单词符号
	while(true){
		c = s[endIndex];
		switch(state){
		case 0:
			if(c >= '0' && c <= '9'){
				state = 1;
				t = Num;
				endIndex++;
			}
			else if(c == '+' || c == '-'){
				state = 5;
				t = Sign;
				endIndex++;
			}
			else if((c >= 'a' && c <= 'z') || (c>='A'&&c<='Z')){
				state = 4;
				t = Id;
				endIndex++;
			}
			else if(c == '*'||c == '/'){
				state = 6;
				t = Sign;
			}
			else if(c == '('){
				state = 6;
				t = LeftBarcket;
			}
			else if(c == ')'){
				state = 6;
				t = RightBarcket;
			}
			else if(c == '='){
				state = 6;
				t = Equal;
			}
			else {
				return false;
			}
			break;
		case 1:
			if(c>='0' && c<='9'){
				state = 1;
				t = Num;
				endIndex++;
			}
			else if(c == '.'){
				state = 2;
				endIndex++;
			}
			else{
				state = 6;
				t = Num;
				endIndex--;
			}
			break;
		case 2:
			if(c>='0' && c<='9'){
				state = 3;
				t = Num;
				endIndex++;
			}
			else{
				return false;
			}
			break;
		case 3:
			if(c>='0' && c<='9'){
				state = 3;
				t = Num;
				endIndex++;
			}
			else{
				state = 6;
				t = Num;
				endIndex--;
			}
			break;
		case 4:
			if((c>='0' && c<='9') || (c >= 'a' && c <= 'z')){
				state = 4;
				t = Id;
				endIndex++;
			}
			else{
				state = 6;
				t = Id;
				endIndex--;
			}
			break;
		case 5:
			if(c>='0' && c<='9' && (endIndex-1 == 0 ||
					(endIndex-2 >= 0 && 
					(s[endIndex-2] == '(' || s[endIndex-2] == '=')
					))){
				state = 1;
				t = Num;
				endIndex++;
			}
			else{
				state = 6;
				t = Sign;
				endIndex--;
			}
			break;
		case 6:
			newToken->type = t;
			for(int i=startIndex; i<=endIndex; i++){
				InsertWord(newToken->wordnode, s[i]);
			}
			InsertToken(head, *newToken);

			state = 0;
			endIndex++;
			if(endIndex == length){
				return true;
			}
			startIndex = endIndex;
			newToken = (Token*)malloc(sizeof(Token));
			newToken->wordnode = CreateWordHead();
			break;
		}
		if(endIndex == length){
			endIndex--;
			state = 6;
		}
	}
}
/*----------------------------------------------------------------------*/