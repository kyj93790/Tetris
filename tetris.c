#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;
	int m;

	initscr(); //ncurses - curses mode로 화면 전환
	noecho();  //ncurses - 문자 입력시 화면에 표시하는 echoing 비활성화
	keypad(stdscr, TRUE); //표준 스크린에서의 입력을 받는다. True는 사용자가 기능키(화살표 등)를 사용할 수 있도록 해줌.

	srand((unsigned int)time(NULL));
	
	createRankList();
	ParentRoot = (RecNode*)malloc(sizeof(RecNode));
	ParentRoot->lv = -1;
	ParentRoot->score = 0;
	ParentRoot->f = field;

	while(!exit){
		clear();
		m = menu();
		switch(m){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_REC_PLAY: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}
	if(m!=MENU_EXIT){
		clearTree(ParentRoot);
	}
	endwin(); //ncurses - curses mode 종료
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	for(i=0;i<BLOCK_NUM;i++){
		nextBlock[i] = rand()%7;
	}
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	recommend(ParentRoot);
	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);	//원래 score가 있던 자리에 nextBlock을 하나 더 그려준다.

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	
	command = wgetch(stdscr);
	
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

	for(i = 0; i < 4; i++){
		move(10+i,WIDTH+13);
		for(j = 0; j < 4; j++){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				//그림자 기능은 '/', 현재 블록은 ' '
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}
		command = GetCommand();	
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	int x,y;

	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if (block[currentBlock][blockRotate][i][j]==1){
				x = blockX+j;
				y = blockY+i;
				
				if(f[y][x]==1) return 0;
				if(y>=HEIGHT) return 0;
				if(x<0) return 0;
				if(x>=WIDTH) return 0;
			}
		}
	}

	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int exRotate;
	int i,j;
	int x,y;

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	switch(command) {
		case KEY_UP	:
			exRotate = (blockRotate+3)%4; //시계방향으로 90도
			y = blockY;
			x = blockX;
			break;
		case KEY_DOWN :
			y = blockY-1;
			x = blockX;
			break;
		case KEY_LEFT :
			y = blockY;
			x = blockX+1;
			break;
		case KEY_RIGHT :
			y = blockY;
			x = blockX-1;
			break;
		default : return; // command가 위의 항목에 없다면 변경사항 없음
	}
	
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(block[currentBlock][exRotate][i][j] == 1 && i+y>=0){
				if(j+x+1<=0 || j+x+1>=WIDTH) continue;
				move(i+y+1,j+x+1);
				printw(".");
			}
		}
	}
	//3. 새로운 블록 정보를 그린다.
	DrawField();//필드를 그린 후 블럭을 그려야 함.
	//DrawBlock 대신 DrawBlockWithFeatures
	DrawBlockWithFeatures(y,x,currentBlock,blockRotate);
	move(HEIGHT,WIDTH+10); //DrawBlock과 동일하게 처리
}

void BlockDown(int sig){
	// user code
	int addition;
	int i,j;

	if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)){//한 칸 아래로 내려갈 수 있음
		blockY++;
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
		timed_out=0;
		return;
	}
	else if(blockY==-1){
		gameOver = 1;//gameOver되었는데 그 때 한 줄 채워져서 지워지면 ?
	}

	//score에 추가점수 부여(닿은 면적)
	score += AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
	//DeleteLine함수에서 2번째 줄 이상의 줄을 자꾸 삭제하는 오류 발생

	addition = DeleteLine(field);
	score += addition;

	for(i=0;i<BLOCK_NUM-1;i++){
		nextBlock[i]=nextBlock[i+1];
	}
	nextBlock[BLOCK_NUM-1] = rand()%7;
	recommend(ParentRoot);
	//modified_recommend(ParentRoot);

	//현재 블록 위치 초기화
	blockRotate = 0;
	blockX=(WIDTH/2)-2;
	blockY=-1;
	
	DrawNextBlock(nextBlock);
	PrintScore(score);
	DrawField();
	
	timed_out = 0;
	return;

	//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	int touched=0;	//닿은 면적을 count

	//Block이 추가된 영역의 필드값을 바꾼다.
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if (block[currentBlock][blockRotate][i][j] == 1){
				f[blockY+i][blockX+j] = 1;
				//블록에 대해서 1일 때 다음 행의 필드가 1일 때, 필드일 때 touched++;
				if(f[blockY+i+1][blockX+j]==1||blockY+i+1==HEIGHT) touched++;
			}
		}
	}
	return touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int checkfull = 1;
	int numfull = 0;
	int i,j;
	int r,c;

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
	for(i=0;i<HEIGHT;i++){
		checkfull = 1;
		for(j=0;j<WIDTH;j++){
			if(f[i][j]==0){
				checkfull=0;
				break;
			}
		}
		if(checkfull==1){
			numfull++;
			for(r=i;r>0;r--){
				for(c=0;c<WIDTH;c++){
					f[r][c] = f[r-1][c];
				}
			}
		}
	}

	return numfull*numfull*100;
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	//CheckToMove함수로 확인하면서 맨 아래까지 check
	y++;
	while(1){
		if(CheckToMove(field,blockID,blockRotate,y,x)){	// 더 내려갈 수 있는 경우
			y++;
		}
		else{	// 더 이상 내려갈 수 없는 부분
			y--;
			break;
		}
	}
	//이 위치에 대해서 DrawBlock 호출하여 그림자 그림
	DrawBlock(y,x,blockID,blockRotate,'/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	//블록과 그림자를 그린다.
	DrawRecommend();
	DrawShadow(y,x,blockID,blockRotate);
	DrawBlock(y,x,blockID,blockRotate,' ');
}

void createRankList(){
	// user code
	FILE *fp;
	int i;
	char currname[NAMELEN];
	int currscore;
	
	head = NULL;
	fp = fopen("rank.txt","r");

	if(fp==NULL){
		return;
	}

	fscanf(fp,"%d",&num_rank);

	for(i=0;i<num_rank;i++){
		fscanf(fp,"%s",currname);
		fscanf(fp,"%d",&currscore);
		push(currname,currscore);
	}
	fclose(fp);
}

void push(char* currname, int currscore){
	RANK *pnew, *curr;

	pnew = (RANK*)malloc(sizeof(RANK));
	strcpy(pnew->name,currname);
	pnew->score = currscore;
	pnew->link = NULL;

	if(head==NULL){
		head = pnew;
	}
	else{
		curr = head;
		while(1){
			if(curr->link == NULL) break;
			curr = curr->link;
		}
		curr->link = pnew;
	}
};

void rank(){
	// user code
	char op;
	int x, y;
	int count;
	RANK *curr, *pCur;
	char currname[NAMELEN];

	clear();
	
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	
	op = wgetch(stdscr);

	if(op=='1'){
		count=1;

		echo();
		printw("X: ");
		scanw("%d",&x);
		printw("Y: ");
		scanw("%d",&y);
		noecho();
		printw("       name       |   score   \n");
		printw("------------------------------\n");
		
		//존재하는 랭크 범위 외의 값을 주지 않는다고 하니 범위로 입력 없음 구분
		if((0>=x||x>num_rank)&&(0>=y||y>num_rank)){//둘 다 입력 없음
			x=1;
			y=5;
		}
		else if(0>=x||x>num_rank){//x만 입력 없음
			x=1;
		}
		else if(0>=y||y>num_rank){//y만 입력 없음
			y=x+2;
		}
		else{//둘 다 입력 있음
			if(x>y){
				printw("\nsearch failure: no rank in the list\n");
				getch();
				return;
			}
		}
		

		//탐색 및 출력
		curr = head;
		while(1){
			if(count>y) break;
			else if(count>=x){
				printw(" %-17s| %-10d\n",curr->name,curr->score);
			}
			if(curr->link==NULL) break;
			curr = curr->link;
			count++;
		}
	}
	else if(op=='2'){	//사용자  랭킹 모두 찾기
		count = 0;

		printw("input the name: ");
		echo();
		scanw("%s",currname);
		noecho();
		printw("       name       |   score   \n");
		printw("------------------------------\n");

		curr = head;
		while(1){
			if(head==NULL) break;		//랭킹이 전혀 없는 경우.
			if(!strcmp(curr->name,currname)){	//입력받은 이름이랑 노드의 이름이 같은 경우
				count++;
				printw(" %-17s| %-10d\n",curr->name,curr->score);
			}
			if(curr->link==NULL) break;
			curr = curr->link;
		}

		if(count==0){
			printw("search failure: no name in the list\n");
		}
	}
	else{	//원하는 랭킹 정보 삭제
		count = 1;

		printw("input the rank: ");
		echo();
		scanw("%d",&x);
		noecho();

		curr = head;
		while(1){
			if(head==NULL) break;
			if(count==x-1) break;		//삭제할 노드를 찾은 경우, 삭제를 위해 전 노드를 찾는다.
			if(curr->link==NULL) break;
			curr = curr->link;
			count++;
		}

		if(head==NULL){				//삭제 불가.
			printw("\nsearch failure: the rank not in the list\n");
		}
		else if(count==x-1){
			if(head->link==NULL){	//노드가 하나 뿐일 때의 처리
				pCur = head;
				head = NULL;
			}
			else{
				pCur = curr->link;
				curr->link = pCur->link;
			}
			free(pCur);
			printw("\nresult: the rank deleted\n");
			num_rank--;
			writeRankFile();		//수정된 랭크 자료 txt에 반영
		}
		else{
			printw("\nsearch failure: the rank not in the list\n");
		}
	}
	getch();	//마지막 출력을 보기 위함.
}

void writeRankFile(){
	// user code
	FILE *fp;
	int i;
	RANK* curr;

	fp = fopen("rank.txt","w");
	
	fprintf(fp,"%d\n",num_rank);
	curr = head;
	for(i=0;i<num_rank;i++){
		fprintf(fp,"%s %d\n",curr->name,curr->score);
		curr = curr->link;
	}
	fclose(fp);
}

void newRank(int score){
	// user code
	char currname[NAMELEN];
	RANK *pnew, *curr;
	clear();

	printw("your name: ");
	echo();// 사용자의 입력을 출력
	getstr(currname);
	noecho();
	
	pnew = (RANK*)malloc(sizeof(RANK));
	strcpy(pnew->name,currname);
	pnew->score = score;
	pnew->link = NULL;
	
	curr = head;

	if(head==NULL){
		head = pnew;
		num_rank = 1;
	}
	else{
		while(1){
			if(curr->link==NULL) break;
			if(curr->score < score) break;
			curr = curr->link;
		}
		if((curr==head)&&(head->score<score)){
			pnew->link = head;
			head = pnew;
		}
		else{
			pnew->link = curr->link;
			curr->link = pnew;
		}
		num_rank++;
	}
	writeRankFile();
}

void DrawRecommend(){
	// user code
	DrawBlock(tempY,tempX,nextBlock[0],tempR,'R');
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int x, y, r; 
	int i=-1;
	int xl, xr;
	int h,w;
	int currlv;
	int accscore=0;
	RecNode **child;
	// user code

	currlv = root->lv + 1;
	child = root->c;

	for(r=0;r<4;r++){	//모든 회전 고려하기
		xl = -2;
		xr = WIDTH+3;
		for(x = xl; x < xr; i++, x++){	//모든 위치 고려하기
			if(!CheckToMove(root->f,nextBlock[currlv],r,0,x)) continue;

			//자식 노드들 동적 할당 및 자료 저장
			child[i] = makenode();
			child[i]->lv = root->lv+1;
			child[i]->f = (char(*)[WIDTH])malloc(sizeof(char)*HEIGHT*WIDTH);
			//필드 복사
			for(h=0;h<HEIGHT;h++){
				for(w=0;w<WIDTH;w++){
					child[i]->f[h][w] = root->f[h][w];
				}
			}
			
			//최대로 블록 내리기
			y=0;
			if(CheckToMove(child[i]->f,nextBlock[currlv],r,y,x)){
				y++;
				while(CheckToMove(child[i]->f,nextBlock[currlv],r,y,x)){
					y++;
				}
				y--;
			}
			else continue;

			//점수 저장
			child[i]->score = root->score;
			child[i]->score += AddBlockToField(child[i]->f, nextBlock[currlv], r, y, x);
			child[i]->score += DeleteLine(child[i]->f);


			if(currlv < BLOCK_NUM-1){	//leaf 노드가 아닌  경우
				accscore = recommend(child[i]);
			}
			else{	//leaf 노드이면 score 넘기기
				accscore = child[i]->score;
			}

			if(max <= accscore){
				if(max==accscore && tempY>y){ //점수가 동일한 경우 높이가 낮을 때 선택
					continue;
				}
				max = accscore;
				if(currlv==0){//첫번째 블록에 대한 x,y,r 정보 넘기기
					tempY=y;
					tempX=x;
					tempR=r;
				}
			}
		}
	}
	return max;
}

int modified_recommend(RecNode *root){
	int max=0;
	int x,y,r;
	int i=-1;
	int xl, xr;
	int h,w;
	int currlv;
	int accscore=0;
	RecNode **child;

	currlv = root->lv + 1;
	child = root->c;

	for(r=0;r<4;r++){
		xl = -2;
		xr = WIDTH+3;
		for(x = xl; x < xr; x++, i++){
			if(!CheckToMove(root->f,nextBlock[currlv],r,0,x)) continue;

			child[i] = makenode();
			child[i]->lv = root->lv+1;
			child[i]->f = (char(*)[WIDTH])malloc(sizeof(char)*HEIGHT*WIDTH);

			memcpy(child[i]->f,root->f,sizeof(field));

			y=0;
			if(CheckToMove(child[i]->f,nextBlock[currlv],r,y,x)){
				y++;
				while(CheckToMove(child[i]->f,nextBlock[currlv],r,y,x)){
					y++;
				}
				y--;
			}
			else continue;

			child[i]->score = root->score;
			child[i]->score += AddBlockToField(child[i]->f,nextBlock[currlv],r,y,x);
			child[i]->score += DeleteLine(child[i]->f);

			//현재 child의 필드에서 놓았을 때 추가되는 점수가 30보다 작으면 더 이상 탐색하지 않음
			if(root!=ParentRoot && child[i]->score<30){
				return 0;
			}

			if(currlv < BLOCK_NUM-1){
				accscore = recommend(child[i]);
			}
			else{
				accscore = child[i]->score;
			}

			if(max <= accscore){
				if(max==accscore && tempY>y){
					continue;
				}
				max = accscore;
				if(currlv==0){
					tempY=y;
					tempX=x;
					tempR=r;
				}
			}
		}
	}
	return max;
}

RecNode* makenode(){
	RecNode* pnew;
	pnew = (RecNode*)malloc(sizeof(RecNode));
	return pnew;
}

void recommendedPlay(){
	int command;

	// user code
	clear();
	act.sa_handler = SetField;	//일정하게 필드 업데이트
	sigaction(SIGALRM,&act,&oact);
	InitTetris();

	while(1){
		if(timed_out==0){
			ualarm(625000,0);
			timed_out=1;
		}

		command = GetCommand();
		//Q를 입력받은 경우에만 종료
		if(command=='q' || command=='Q'){
			ualarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			getch();
			refresh();
			getch();
			return;
		}
		if(gameOver) break;
	}

	ualarm(0);
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	getch();
	refresh();
	getch();
	return;
}

//추천 위치에 필드 업데이트
void SetField(int sig){
	int i;

	//gameOver 체크
	if(!CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX)){
		gameOver = 1;
	}
	else{
		score += AddBlockToField(field,nextBlock[0],tempR,tempY,tempX);
		score += DeleteLine(field);
		
		for(i=0;i<BLOCK_NUM;i++){
			nextBlock[i] = nextBlock[i+1];
		}
		nextBlock[BLOCK_NUM-1] = rand()%7;
		//변수 초기화
		blockRotate=0;
		blockY=-1;
		blockX=(WIDTH/2)-2;
		
		//recommend(ParentRoot);
		modified_recommend(ParentRoot);
		DrawNextBlock(nextBlock);
		PrintScore(score);
		DrawField();
		DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
		timed_out=0;
	}
}

//메모리 해제
void clearTree(RecNode *root){
	int i;
	RecNode **child;

	child = root->c;
	for(i=0;i<CHILDREN_MAX;i++){
		if(child[i]->lv<BLOCK_NUM){
			clearTree(child[i]);
		}
		free(child[i]->f);
		free(child[i]);
	}
}
