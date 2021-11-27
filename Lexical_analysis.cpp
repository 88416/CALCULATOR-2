#include<stdio.h>
#include<string.h>
#include<stdlib.h>

enum Type {
	Num, Id, Sign, LeftBarcket, RightBarcket, Equal, Point, Unknow
};

enum Symbol{
	E,T,F,H,IDENT,Number,Final
};

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

struct SymbolNode{
	int childCount; //孩子节点数量
	Symbol s;      //节点标志
	Token* token;  //数据信息
	SymbolNode* child;  //孩子节点（包含多个）
	SymbolNode* left;	//左兄弟
	SymbolNode* right;	//右兄弟
	SymbolNode* parent;
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

/*----------------------------------------------------------------------------------*/
SymbolNode* CreateSymbolNodeHead() { 
	SymbolNode* head = (SymbolNode*)malloc(sizeof(SymbolNode));
    head->s = Symbol('H');
    head->child = NULL;
	head->childCount = 0;
	head->token = NULL;
	head->left = NULL;
	head->right = NULL;
	head->parent = NULL;
    return head;
}

void InsertSymbolChild(SymbolNode* parent, Symbol* s, int childCount){ 
	SymbolNode* p = parent;
    p->childCount = childCount;
    p->child = (SymbolNode*)malloc(childCount * sizeof(SymbolNode));
    p = p->child;
    for(int i=0; i<=childCount-1; i++){
		(p + i)->token = NULL;
		(p + i)->parent = parent;
		(p + i)->childCount = 0;
        (p + i)->s = s[i];
        (p + i)->child = NULL;
		if(i == 0){
			(p + i)->left = NULL;
		}
		else{
			(p + i)->left = p + i - 1;
		}
		if(i == childCount - 1){
			(p + i)->right = NULL;
		}
		else{
			(p + i)->right = p + i + 1;
		}
    }
}

void PrintSymbolNode(SymbolNode* p){
	if(p){
		switch(p->s){
			case H:
				printf("[H]");
				break;
			case E:
				printf("[E]");
				break;
			case F:
				printf("[F]");
				break;
			case T:
				printf("[T]");
				break;
			case Number:
				printf("[Number]");
				break;
			case IDENT:
				printf("[ID]");
				break;
			case Final:
				printf("[Type:%d, info:", p->token->type);
				PrintWord(p->token->wordnode);
				printf("]");
				break;
			default:
				printf("[%c]", (char)p->s);
		}
	}
}

void PrintSymbolTree(SymbolNode* head){
	PrintSymbolNode(head);
	if(head->child != NULL){
		SymbolNode* p = head->child;
		for(int i=0; i<=head->childCount-1; i++){
			printf("(");
			PrintSymbolTree(p + i);
		}
	}
	printf(")");
}

void ReleaseSymbolTree(SymbolNode* head){ //递归删除整棵树
	if(head->child){
		int count = head->childCount;
		int i = count - 1;
		for(i=count-1; i>=0; i--){
			ReleaseSymbolTree(head->child + i);
		}
	}
	if(head->left == NULL){
		head->parent->childCount = 0;
		head->child = NULL;
		free(head);
	}
}

void DeleteChildSymbol(SymbolNode* parent){  //删除所有的孩子节点
	if(parent->child){
		int count = parent->childCount;
		int i;
		for(i=count-1; i>=0; i--){
			ReleaseSymbolTree(parent->child + i);
		}	
	}
	parent->childCount = 0;
	parent->child = NULL;
}

void InsertTokenFinally(Token* token, SymbolNode* parent){ // 在num或者id后面插入数据
	SymbolNode* p = parent;
	p->childCount = 1;
	p->child = (SymbolNode*)malloc(sizeof(SymbolNode));
	p = p->child;

	p->child = NULL;
	p->childCount = 0;
	p->left = NULL;
	p->right = NULL;
	p->parent = parent;
	p->s = Final;
	p->token = token;

}

/*----------------------------------------------------------------------------------*/

/*------------------------TOOL-FUNC---------------------------------------------------*/

int WordNodeToNum(WordNode* head){
	char* s = (char*)malloc(sizeof(char) * 15);
	int num;
	int i = 0;
	WordNode* p = head->next;
	while(p){
		s[i] = p->ch;
		p = p->next;
		i++;
	}
	s[i] = '\0';
	num = atoi(s);
	free(s);
	return num;
}

char* WordNodeToStr(WordNode* head){
	WordNode* p = head->next;
	int len = 0;
	int i = 0;
	while(p){
		len++;
		p = p->next;
	}
	if(len == 0){
		return NULL;
	}
	char* s = (char*)malloc(sizeof(char) * (len + 1));
	p = head->next;
	for(i=0; i<=len-1; i++){
		s[i] = p->ch;
		p=p->next;
	}
	s[i] = '\0';
	return s;
}

TokenNode* FindSign(TokenNode* startPtr, char sign){
	TokenNode* p = startPtr->next;
	while(p){
		if( (p->token.type == Sign || p->token.type == Equal) &&
            p->token.wordnode->next->ch==sign)
			return p;
		p = p->next;
	}
	return NULL;
}

void DivideToken(TokenNode* head,TokenNode* divideNode ,TokenNode*leftHead, TokenNode* rightHead){
	TokenNode* p = head->next;
	while(p!=divideNode){
		InsertToken(leftHead, p->token);
		p = p->next;
	}
	p = p->next;
	while(p){
		InsertToken(rightHead, p->token);
		p = p->next;
	}
}

void RemoveBarckets(TokenNode* head, TokenNode* dest){
	TokenNode* last = FindTokenLast(head);
	TokenNode* p = head->next->next;
	while(p!=last){
		InsertToken(dest, p->token);
		p = p->next;
	}
}
/*
E->E+T|E-T|T
T->T*F|T/F|F
F->(E)|ID|Num
*/
bool match(TokenNode* head, Symbol S, SymbolNode* symHead){ //待做：释放废弃内存
	TokenNode *p;
	if(head->next==NULL) return false;
	if(S == E){
		p = FindSign(head, '+');
		Symbol s[3] = {E, (Symbol)'+', T};
		if(p){
			InsertSymbolChild(symHead, s, 3);
			while(p){
				TokenNode* lhead = CreateTokenHead();
				TokenNode* rhead = CreateTokenHead();
				DivideToken(head, p, lhead, rhead);

				if((match(lhead, E, symHead->child) && match(rhead, T, symHead->child + 2))||   
					(match(lhead, T, symHead->child + 2) && match(rhead, E, symHead->child))){
					//ReleaseTokenList(lhead);
					//ReleaseTokenList(rhead);
					return true;
				}
				//ReleaseTokenList(lhead);
			//	ReleaseTokenList(rhead);
				DeleteChildSymbol(symHead->child);
				DeleteChildSymbol(symHead->child + 2);
				p = FindSign(p, '+');
			}// E->E+T
			DeleteChildSymbol(symHead);
		}

		p = FindSign(head, '-');
		s[1] = (Symbol)'-';
		if(p){
			InsertSymbolChild(symHead, s, 3);
			while(p){
			TokenNode* lhead = CreateTokenHead();
			TokenNode* rhead = CreateTokenHead();
			DivideToken(head, p, lhead, rhead);
			if(match(lhead, E, symHead->child) && match(rhead, T, symHead->child + 2)){
					//ReleaseTokenList(lhead);
					//ReleaseTokenList(rhead);
					return true;}
			//ReleaseTokenList(lhead);
			//ReleaseTokenList(rhead);
			DeleteChildSymbol(symHead->child);
			DeleteChildSymbol(symHead->child + 2);
			p = FindSign(p, '-');
			}// E->E-T
			DeleteChildSymbol(symHead);
		}
		s[0] = T;
		InsertSymbolChild(symHead, s, 1);
		if(match(head, T, symHead->child)){return true;}
		DeleteChildSymbol(symHead);
		return false;
	}
	else if(S == T){
		p = FindSign(head, '*');
		Symbol s[3] = {T, (Symbol)'*', F};
		if(p){
			InsertSymbolChild(symHead, s, 3);
			while(p){
				TokenNode* lhead = CreateTokenHead();
				TokenNode* rhead = CreateTokenHead();
				DivideToken(head, p, lhead, rhead);
				if((match(lhead, T, symHead->child) && match(rhead, F, symHead->child + 2))||
					(match(lhead, F, symHead->child + 2) && match(rhead, T, symHead->child))){return true;}
				DeleteChildSymbol(symHead->child);
				DeleteChildSymbol(symHead->child + 2);
				p = FindSign(p, '*');
			}// T->T*F
			DeleteChildSymbol(symHead);	
		}
		

		p = FindSign(head, '/');
		s[1] = (Symbol)'/';
		if(p){
			InsertSymbolChild(symHead, s, 3);
			while(p){
				TokenNode* lhead = CreateTokenHead();
				TokenNode* rhead = CreateTokenHead();
				DivideToken(head, p, lhead, rhead);
				if(match(lhead, T, symHead->child) && match(rhead, F, symHead->child + 2)){return true;}
				DeleteChildSymbol(symHead->child);
				DeleteChildSymbol(symHead->child + 2);
				p = FindSign(p, '/');
			}// T->T/F
			DeleteChildSymbol(symHead);	
		}
		

		s[0] = F;
		InsertSymbolChild(symHead, s, 1);
		if(match(head, F, symHead->child)){return true;}
		DeleteChildSymbol(symHead);	
		return false; // T->F
	}
	else if(S == F){
		Symbol s[3] = {(Symbol)'(', E, (Symbol)')'};
		if (head->next->token.type == LeftBarcket &&
			FindTokenLast(head)->token.type == RightBarcket){
			InsertSymbolChild(symHead, s, 3);
			TokenNode* midhead = CreateTokenHead();
			RemoveBarckets(head, midhead);
			if(match(midhead, E, symHead->child + 1)){return true;}
			DeleteChildSymbol(symHead);
		}//F->(E)

		Type t = head->next->token.type;
		if( (head->next->token.type == Id && head->next->next==NULL) || //F->ID
            (head->next->token.type == Num && head->next->next==NULL)) //F->Num  
        {
			if(t == Id){
				s[0] = IDENT;
			}
			else{
				s[0] = Number;
			}
			InsertSymbolChild(symHead, s, 1);
			InsertTokenFinally(&head->next->token, symHead->child);
			return true;
		}
		return false;
	}
	else if(S == H){
		TokenNode* p = head->next;
		if(p && p->token.type == Id){
			p = p->next;
			if(p && p->token.type == Equal){
				p = p->next;
				if(p){
					Symbol s[3] = {IDENT, (Symbol)'=', E};
					TokenNode* newHead = CreateTokenHead();
					while(p){
						InsertToken(newHead, p->token);
						p = p->next;
					}
					InsertSymbolChild(symHead, s, 3);
					InsertTokenFinally(&head->next->token, symHead->child);
					if(match(newHead, E, symHead->child + 2)){
						return true;
					}
					DeleteChildSymbol(symHead);
				}
			}
		}
		return false;
	}
	return false;
}

