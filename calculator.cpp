#include<stdio.h>
#include<string.h>
#include<stdlib.h>

enum Type {
	Num, Id, Sign, LeftBarcket, RightBarcket, Equal, Point, Unknow
};

enum Symbol{
	E,T,F,H,IDENT,Number,Final
};

enum ErrorType{
    normal,divideZero,UnknowId,symTreeError,errorSign
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

typedef struct Variable {
	char *name;
	int value;
	Variable* next;
}Variable;

typedef struct Result{
	void* resultValue;
    ErrorType resultError;
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

void InsertWord(WordNode* head, char ch, int line = 0, int col = 0) {  
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

Variable* CreateVarHead() {
	Variable* head = (Variable*)malloc(sizeof(Variable));
	head->next = NULL;
	head->name = NULL;
	head->value = 0;
	return head;
}

Variable* FindVarLast(Variable* head) {
	Variable* p = head;
	while (p->next) {
		p = p->next;
	}
	return p;
}

void InsertVar(char* name, int value, Variable* head) {
	Variable* p = head->next;
	Variable* f=head;
	while (p) {
		if (strcmp(name, p->name) == 0) {
			p->value = value;
			return;
		}
		f = p;
		p = p->next;
	}
	Variable* newVar = (Variable*)malloc(sizeof(Variable));
	newVar->name = name;
	newVar->value = value;
	newVar->next = NULL;
	f->next = newVar;
}


Result* FindVar(char* name, Variable* head) {
	int *rv = (int*)malloc(sizeof(int));
	Result* result = (Result*)malloc(sizeof(Result));
	Variable* p = head->next;
	while (p) {
		if (strcmp(name, p->name) == 0){
			*rv = p->value;
			result->resultValue = (void*)rv;
			result->resultError = normal;
			return result;
		}
		p = p->next;
	}
	result->resultError = UnknowId;
	result->resultValue = NULL;
	return result;
}

void PrintVarTable(Variable* varTable) {
	Variable* p = varTable->next;
	while (p) {
		printf("%s:%d\n", p->name, p->value);
		p = p->next;
	}
}

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

Result* calculate(SymbolNode* head, Variable* valTableHead){
	if(head->childCount == 3){
		if(char(head->child->s) == '('){
			return calculate((head->child)+1, valTableHead);
		}
		else if(char((head->child+1)->s) == '='){
			Result* res;
			res = (Result*)malloc(sizeof(Result));
			res = calculate(head->child + 2, valTableHead);
			if(res->resultError == normal){
				char* s = WordNodeToStr(head->child->child->token->wordnode);
				InsertVar(s, *(int*)res->resultValue, valTableHead);
			}
			return res;
		}
		else{
			Result *r1,*r2,*res;
			res = (Result*)malloc(sizeof(Result));
			r1 = calculate(head->child, valTableHead);
			r2 = calculate(head->child + 2, valTableHead);
			if(r1->resultError != normal || r2->resultError != normal){
				if(r1->resultError != normal){
					res->resultError = r1->resultError;
					res->resultValue = NULL;
				}
				else{
					res->resultError = r2->resultError;
					res->resultValue = NULL;
				}
			}
			else{
				char tempc = (char)((head->child + 1)->s);
				int tempi;
				if(tempc == '+'){
					tempi = *(int*)r1->resultValue + *(int*)r2->resultValue;
					res->resultValue = &tempi;
					res->resultError = normal;
				}
				else if(tempc == '-'){
					tempi = *(int*)r1->resultValue - *(int*)r2->resultValue;
					res->resultValue = &tempi;
					res->resultError = normal;
				}
				else if(tempc == '*'){
					tempi = *(int*)r1->resultValue * *(int*)r2->resultValue;
					res->resultValue = &tempi;
					res->resultError = normal;
				}
				else if(tempc == '/'){
					if(*(int*)r2->resultValue == 0){
						res->resultError = divideZero;
						res->resultValue = NULL;
					}
					else{
						tempi = *(int*)r1->resultValue / *(int*)r2->resultValue;
						res->resultValue = &tempi;
						res->resultError = normal;
					}
				}
				else{
					res->resultError = errorSign;
					res->resultValue = NULL;
				}
			}
			return res;
		}
	}
	else if(head->childCount == 0 && head->token){
		if(head->token->type == Num){
			int *val = (int*)malloc(sizeof(int));
			*val = WordNodeToNum(head->token->wordnode);
			Result* res = (Result*)malloc(sizeof(Result));
			res->resultError = normal;
			res->resultValue = (void*)val;
			return res;
		}
		else if(head->token->type == Id){
			Result* res = FindVar(WordNodeToStr(head->token->wordnode), valTableHead);
			return res;
		}
		else{
			Result* res = (Result*)malloc(sizeof(Result));
			res->resultError = symTreeError;
			res->resultValue = NULL;
			return res;
		}
	}
	else if(head->childCount == 1){
		return calculate(head->child, valTableHead);
	}
	else{
		Result* res = (Result*)malloc(sizeof(Result));
		res->resultError = symTreeError;
		res->resultValue = NULL;
		return res;
	}
}


int main() {
	char temp[30];
	TokenNode* tokenHead = CreateTokenHead();
	SymbolNode* symHead = CreateSymbolNodeHead();
	Variable* valTableHead = CreateVarHead();
	Result* rs;
	while(true){
		printf("\n*******************************Enter*Your*Expression*************************************\n");
		scanf("%s", temp);
		if(strcmp(temp, "exit") == 0){
			break;
		}
		if(ReadWordPP(temp, tokenHead)){
			PrintTokens(tokenHead);
			printf("\n");
			if(FindSign(tokenHead, '=') == NULL){
				symHead->s = E;
				if(match(tokenHead, E, symHead)){
					printf("表达式正确\n");
					printf("--------------------Symbol-Tree-------------------------\n");
					PrintSymbolTree(symHead);
					printf("\n--------------------------------------------------------\n");
					rs = calculate(symHead, valTableHead);
					switch(rs->resultError){
						case normal:
							printf("\n计算结果正确，结果为：%d\n", *(int*)rs->resultValue);
							printf("----------------------val-Table-------------------------\n");
							PrintVarTable(valTableHead);
							printf("--------------------------------------------------------\n");
							break;
						case divideZero:
							printf("除0错误。\n");
							break;
						case UnknowId:
							printf("未知变量。\n");
							break;
						case symTreeError:
							printf("树建立错误！\n");
							break;
						case errorSign:
							printf("符号错误！\n");
							break;
						default:
							printf("未知错误。\n");
					}
				}
				else{
					printf("表达式语法错误\n");
				}
			}
			else{
				symHead->s = H;
				if(match(tokenHead, H, symHead)){
					printf("等式表达式正确\n");
					printf("--------------------Symbol-Tree-------------------------\n");
					PrintSymbolTree(symHead);
					printf("\n--------------------------------------------------------\n");
					rs = calculate(symHead, valTableHead);
					switch(rs->resultError){
						case normal:
							printf("\n计算结果正确,结果为:%d\n", *(int*)rs->resultValue);
							printf("----------------------val-Table-------------------------\n");
							PrintVarTable(valTableHead);
							printf("--------------------------------------------------------\n");
							break;
						case divideZero:
							printf("除0错误。\n");
							break;
						case UnknowId:
							printf("未知变量。\n");
							break;
						case symTreeError:
							printf("树建立错误！\n");
							break;
						case errorSign:
							printf("符号错误！\n");
							break;
						default:
							printf("未知错误。\n");
					}
				}
				else{
					printf("等式语法错误\n");
				}
			}
			ReleaseTokenList(tokenHead);	
		}
		else{
			printf("词法错误。\n");
		}
	}
	system("pause");
	return 0;
}