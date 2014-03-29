#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include <bios.h>		//bioskey()
#include <time.h>



/*
	ATI_Video_Dispaly    	0123H
	nVidia_Video_Display	0118H
*/
#define VGA_CARD		0118H


#define	DOWN	0x5000
#define	LEFT	0x4B00
#define	RIGHT	0x4D00
#define	UP	0x4800
#define	HOME	0x4700
#define	END	0x4F00
#define	ALT_X	0x2D00


#define	boolean	int
#define	true	1
#define	false	0
#define	Data_File	"c:\\nemo\\nemo.dat"




class BasicGraphicObject
{
	unsigned int	SelectedPage;

public:
	BasicGraphicObject();
	~BasicGraphicObject();
	void setPixel(int x, int y, unsigned char R, unsigned char G, unsigned char B);
	void Rect(int x1, int y1, int x2, int y2, unsigned char R, unsigned char G, unsigned char B);
	void setBlock(int x, int y, int type);

protected:
	void setPixel_Red(int x, int y, unsigned char R);
	void setPixel_Green(int x, int y, unsigned char G);
	void setPixel_Blue(int x, int y, unsigned char B);
	void setPage(unsigned int page);

	unsigned char getPixel_Red(int x, int y);
	unsigned char getPixel_Green(int x, int y);
	unsigned char getPixel_Blue(int x, int y);
};

class BlockImageObject
{
	int	i;
	int	Max_Block_Type;
protected:
	int	Lv;
public:
	unsigned char BlockImage[2][2][40][40][3];

	BlockImageObject();
	void loadBlockImage();
	void reloadBlockImage();
};


class LabelImageObject
{
	int	Max_Label_Type;
	int	Selected_Label_Type;
	
protected:
	int	Lv;
	
public:
	unsigned char LabelImage[40][40][3];
	LabelImageObject();
	void changeLabelType();
	void loadLabelImage(int number);
};

class CursorImageObject
{
	int	Max_Cursor_Type;
	int	Selected_Cursor_Type;
	
public:
	int	Cursor_X, Cursor_Y;
	unsigned char CursorImage[60][60][3];
	unsigned char CacheImage[60][60][3];
	CursorImageObject();
	void changeCursorType();
};


class BlockGraphicObject : public BasicGraphicObject, public BlockImageObject, public LabelImageObject, public CursorImageObject
{
	int	BlockSize;
	
public:
	BlockGraphicObject();
	void loadBackgroundImage();
	void setBlock(int x, int y, int type);
	void crackEffect(int x, int y);
	void setLabel(int x, int y, int number);
	void setXLabel(int line, int offset, int number);
	void setYLabel(int line, int offset, int number);
	void showCursor(int x, int y);
	void hiddenCursor();
	void setCursor(int x, int y);
	void moveCursor(int x, int y);
	void setLevel(int Lv);
	void setTimeOver();
	void setStageClear();
};

class ScoreGraphicObject : public BasicGraphicObject
{
public:
	void setStageTitle();
	void setScoreTitle();
	void setStage(int stage);
	void setScore(long score);
	void setCache();

private:
	int	log(long score);
	long	pow(int c);
	void setNumber(int score, int position);
	void getCache(int cnt);
};


class TimeGraphicObject : public BasicGraphicObject
{
	int Time;
public:
	void setTimeClear()
	{
		Time = 400;
		Rect(500, 680, 400, 30, 190, 0, 0);
	}
	void setTime(int time)
	{
		if(Time<time)
			Rect(500, 680, time, 30, 190,0,0);
		else
			Rect(500+time, 680, 400-time, 30, 190,190,190);
		Time = time;
	}
};



class BackgroundImageObject
{
	int	fp, i;
	unsigned char buffer[1024*3];
public:
	BackgroundImageObject();
	~BackgroundImageObject();
	void next();
	unsigned char getData();
};








BasicGraphicObject::BasicGraphicObject()
{
	asm{
		MOV	AX,	4F02H
		MOV	BX,	VGA_CARD
		INT	10H
	}
}


BasicGraphicObject::~BasicGraphicObject()
{
	asm{
		MOV	AX,	4F02H
		MOV	BX,	108H
		INT	10H
	}
}



void BasicGraphicObject::setPixel(int x, int y, unsigned char R, unsigned char G, unsigned char B)
{
	setPixel_Blue(x, y, B);
	setPixel_Green(x, y, G);
	setPixel_Red(x, y, R);
}


void BasicGraphicObject::Rect(int x1, int y1, int x2, int y2, unsigned char R, unsigned char G, unsigned char B)
{
	int	i, j;
	for(i = y1; i <= y1 + y2; i++)
	{
		for(j = x1; j <= x1 + x2; j++)
		{
			setPixel(j, i, R, G, B);
		}
	}
}









void BasicGraphicObject::setPixel_Red(int x, int y, unsigned char R)
{
	unsigned long VideoPointer;
	unsigned int page, WindowPointer;
	VideoPointer = (long)y * 1024 * 4 + x * 4 + 2;
	page = VideoPointer / 65536;
	WindowPointer = VideoPointer % 65536;
	setPage(page);
	asm{
		MOV	AX,	0A000H
		MOV	ES,	AX
		MOV	BX,	WindowPointer
		MOV	AL,	R
		MOV	ES:[BX],	AL
	}
}


void BasicGraphicObject::setPixel_Green(int x, int y, unsigned char G)
{
	unsigned long VideoPointer;
	unsigned int page, WindowPointer;
	VideoPointer = (long)y * 1024 * 4 + x * 4 + 1;
	page = VideoPointer / 65536;
	WindowPointer = VideoPointer % 65536;
	setPage(page);
	asm{
		MOV	AX,	0A000H
		MOV	ES,	AX
		MOV	BX,	WindowPointer
		MOV	AL,	G
		MOV	ES:[BX],	AL
	}
}


void BasicGraphicObject::setPixel_Blue(int x, int y, unsigned char B)
{
	unsigned long VideoPointer;
	unsigned int page, WindowPointer;
	VideoPointer = (long)y * 1024 * 4 + x * 4;
	page = VideoPointer / 65536;
	WindowPointer = VideoPointer % 65536;
	setPage(page);
	asm{
		MOV	AX,	0A000H
		MOV	ES,	AX
		MOV	BX,	WindowPointer
		MOV	AL,	B
		MOV	ES:[BX],	AL
	}
}



void BasicGraphicObject::setPage(unsigned int page)
{
	if(SelectedPage != page)
	{
		SelectedPage = page;
		asm{
			MOV	AX,	4F05H
			MOV	BX,	0
			MOV	DX,	page
			INT	10H
		}
	}
}

unsigned char BasicGraphicObject::getPixel_Red(int x, int y)
{
	unsigned char R;
	unsigned long VideoPointer;
	unsigned int page, WindowPointer;
	VideoPointer = (long)y * 1024 * 4 + x * 4 + 2;
	page = VideoPointer / 65536;
	WindowPointer = VideoPointer % 65536;
	setPage(page);
	asm{
		MOV	AX,	0A000H
		MOV	ES,	AX
		MOV	BX,	WindowPointer
		MOV	AL,	ES:[BX]
		MOV	R,	AL
	}
	return	R;
}


unsigned char BasicGraphicObject::getPixel_Green(int x, int y)
{
	unsigned char G;
	unsigned long VideoPointer;
	unsigned int page, WindowPointer;
	VideoPointer = (long)y * 1024 * 4 + x * 4 + 1;
	page = VideoPointer / 65536;
	WindowPointer = VideoPointer % 65536;
	setPage(page);
	asm{
		MOV	AX,	0A000H
		MOV	ES,	AX
		MOV	BX,	WindowPointer
		MOV	AL,	ES:[BX]
		MOV	G,	AL
	}
	return	G;
}


unsigned char BasicGraphicObject::getPixel_Blue(int x, int y)
{
	unsigned char B;
	unsigned long VideoPointer;
	unsigned int page, WindowPointer;
	VideoPointer = (long)y * 1024 * 4 + x * 4;
	page = VideoPointer / 65536;
	WindowPointer = VideoPointer % 65536;
	setPage(page);
	asm{
		MOV	AX,	0A000H
		MOV	ES,	AX
		MOV	BX,	WindowPointer
		MOV	AL,	ES:[BX]
		MOV	B,	AL
	}
	return B;
}





//-----------------------------------------

void BasicGraphicObject::setBlock(int x, int y, int type)
{
	unsigned char R, G, B;
	switch(type)
	{
		case 0:
			R = 255;
			G = 255;
			B = 255;
			break;
		case 1:
			R = 255;
			G = 0;
			B = 0;
			break;
		case 2:
			R = 0;
			G = 0;
			B = 255;
			break;
		case 3:
			R = 255;
			G = 255;
			B = 0;
			break;
	}
	Rect(x*40+500, y*40+250, 40, 40, R, G, B);
}



//-----------------------------------------

BlockGraphicObject::BlockGraphicObject()
{
	setLevel(1);
}


void BlockGraphicObject::loadBackgroundImage()
{
	reloadBlockImage();
	BackgroundImageObject bgi;
	int x, y;

	for(y=767; y>=0; y--){
		bgi.next();
		for(x=0;x<1024;x++){
			setPixel_Blue(x,y,bgi.getData());
			setPixel_Green(x,y,bgi.getData());
			setPixel_Red(x,y,bgi.getData());
		}
	}
}



void BlockGraphicObject::setBlock(int x, int y, int type)		// 상속받아 오버라이딩 되었습니다.
{
	int i, j, style=0, ck=0;
	switch(type)
	{
		case 0:
			style = 1;
			ck = 0;
			break;
		case 1:
			style = 0;
			ck = 1;
			break;
		case 2:
			style = 0;
			ck = 0;
			break;
		case 3:
			style = 1;
			ck = 1;
			break;
	}
	for(i=0;i<BlockSize;i++)
	{
		for(j=0;j<BlockSize;j++)
		{
			setPixel_Blue(j+x*BlockSize+500,i+y*BlockSize+250,BlockImage[style][ck][j][i][0]);
			setPixel_Green(j+x*BlockSize+500,i+y*BlockSize+250,BlockImage[style][ck][j][i][1]);
			setPixel_Red(j+x*BlockSize+500,i+y*BlockSize+250,BlockImage[style][ck][j][i][2]);
		}
	}
}

void BlockGraphicObject::crackEffect(int x, int y)
{
	int i, j;
	for(i=0;i<BlockSize;i++)
	{
		for(j=0;j<BlockSize;j++)
		{
			setPixel_Blue(j+x*BlockSize+500,i+y*BlockSize+250, 255);
			setPixel_Green(j+x*BlockSize+500,i+y*BlockSize+250, 255);
			setPixel_Red(j+x*BlockSize+500,i+y*BlockSize+250, 255);
		}
	}
	delay(20);
}


void BlockGraphicObject::setLabel(int x, int y, int number)
{
	int	i, j;
	loadLabelImage(number);
	for(j=0;j<BlockSize;j++)
	{
		for(i=0;i<BlockSize;i++)
		{
			if(LabelImage[i][j][0] != 83 || LabelImage[i][j][1] != 138 || LabelImage[i][j][2] != 138)
			{
				setPixel_Blue(i+x,j+y,LabelImage[i][j][0]);
				setPixel_Green(i+x,j+y,LabelImage[i][j][1]);
				setPixel_Red(i+x,j+y,LabelImage[i][j][2]);
			}
		}
	}
}

void BlockGraphicObject::setXLabel(int line, int offset, int number)
{
	int	x, y;
	x = 500 - (offset + 1) * BlockSize;
	y = line * BlockSize + 250;
	setLabel(x, y, number);
}

void BlockGraphicObject::setYLabel(int line, int offset, int number)
{
	int	x, y;
	x = line * BlockSize + 500;
	y = 250 - (offset + 1) * BlockSize;
	setLabel(x, y, number);
}

void BlockGraphicObject::showCursor(int x, int y)
{
	int	i, j;
	for(j=0;j<60;j++)
	{
		for(i=0;i<60;i++)
		{
			CacheImage[i][j][0] = getPixel_Blue(i+x, j+y);
			CacheImage[i][j][1] = getPixel_Green(i+x, j+y);
			CacheImage[i][j][2] = getPixel_Red(i+x, j+y);
			if(CursorImage[i][j][0] != 83 || CursorImage[i][j][1] != 138 || CursorImage[i][j][2] != 138)
			{
				setPixel_Blue(i+x, j+y, CursorImage[i][j][0]);
				setPixel_Green(i+x, j+y, CursorImage[i][j][1]);
				setPixel_Red(i+x, j+y, CursorImage[i][j][2]);
			}
		}
	}
}


void BlockGraphicObject::hiddenCursor()
{
	int	i, j;
	int	x, y;
	x = Cursor_X;
	y = Cursor_Y;
	for(j=0;j<60;j++)
	{
		for(i=0;i<60;i++)
		{
			setPixel_Blue(i+x, j+y, CacheImage[i][j][0]);
			setPixel_Green(i+x, j+y, CacheImage[i][j][1]);
			setPixel_Red(i+x, j+y, CacheImage[i][j][2]);
		}
	}
}

void BlockGraphicObject::setCursor(int x, int y)
{
	Cursor_X = x * BlockSize + 500 + BlockSize / 2;
	Cursor_Y = y * BlockSize + 250 + BlockSize / 2;
	showCursor(Cursor_X, Cursor_Y);
}

void BlockGraphicObject::moveCursor(int x, int y)
{
	hiddenCursor();
	setCursor(x, y);
}

void BlockGraphicObject::setLevel(int Lv)
{
	switch(Lv){
		case 1:
			BlockSize = 40;
			break;
		case 2:
			BlockSize = 28;
			break;
		case 3:
			BlockSize = 20;
			break;
		case 4:
			BlockSize = 16;
			break;
		case 5:
			BlockSize = 14;
			break;
	}
	BlockImageObject::Lv = Lv;
	LabelImageObject::Lv = Lv;
}


void BlockGraphicObject::setTimeOver()
{
	int	x, y, fp;
	char str[] = "c:\\nemo\\image\\timeover.bmp";
	unsigned char buffer[480*3];
	fp = _open(str, O_BINARY);
	if(fp == -1)
	{
		printf("Time Over Image Open Error");
		getch();
		exit(1);
	}
	_read(fp, buffer, 54);
	for(y=199; y>=0; y--)
	{
		_read(fp, buffer, 480*3);
		for(x=0; x<480; x++)
		{
			if(buffer[x*3]!=83 || buffer[x*3+1]!=138 || buffer[x*3+2]!=138)
			{
				setPixel(x+400, y+300, buffer[x*3+2], buffer[x*3+1], buffer[x*3]);
			}
		}
	}
	_close(fp);
}


void BlockGraphicObject::setStageClear()
{
	int	x, y, fp;
	char str[] = "c:\\nemo\\image\\clear.bmp";
	unsigned char buffer[600*3];
	fp = _open(str, O_BINARY);
	if(fp == -1)
	{
		printf("Stage Clear Image Open Error");
		getch();
		exit(1);
	}
	_read(fp, buffer, 54);
	for(y=199; y>=0; y--)
	{
		_read(fp, buffer, 600*3);
		for(x=0; x<600; x++)
		{
			if(buffer[x*3]!=83 || buffer[x*3+1]!=138 || buffer[x*3+2]!=138)
			{
				setPixel(x+350, y+300, buffer[x*3+2], buffer[x*3+1], buffer[x*3]);
			}
		}
	}
	_close(fp);
}



//--------------------------------------



void ScoreGraphicObject::setStageTitle()
{
	int	fp, x, y;
	char	str[] = "c:\\nemo\\image\\stage.bmp";
	unsigned char buffer[200*3];
	fp = _open(str, O_BINARY);
	if(fp == -1)
	{
		printf("Stage Title Image Open Error");
		getch();
		exit(1);
	}
	_read(fp, buffer, 54);
	for(y=49;y>=0;y--){
		_read(fp, buffer, 200*3);
		for(x=0;x<200;x++){
			if(buffer[x*3]!=83 || buffer[x*3+1]!=138 || buffer[x*3+2]!=138){
				setPixel_Blue(x+10, y+20, buffer[x*3]);
				setPixel_Green(x+10, y+20, buffer[x*3+1]);
				setPixel_Red(x+10, y+20, buffer[x*3+2]);
			}
		}
	}
	_close(fp);
}





void ScoreGraphicObject::setScoreTitle()
{
	int	fp, x, y;
	char	str[] = "c:\\nemo\\image\\score.bmp";
	unsigned char buffer[200*3];
	fp = _open(str, O_BINARY);
	if(fp == -1){
		printf("Score Title Image Open Error\n");
		getch();
		exit(1);
	}
	_read(fp, buffer, 54);
	for(y=49;y>=0;y--){
		_read(fp, buffer, 200*3);
		for(x=0;x<200;x++){
			if(buffer[x*3]!=83 || buffer[x*3+1]!=138 || buffer[x*3+2]!=138){
				setPixel_Blue(x+330, y+20, buffer[x*3]);
				setPixel_Green(x+330, y+20, buffer[x*3+1]);
				setPixel_Red(x+330, y+20, buffer[x*3+2]);
			}
		}
	}
	_close(fp);
}

void ScoreGraphicObject::setStage(int stage)
{
	int i, cnt;
	cnt = log(stage);
	for(i=cnt; i>0; i--)
	{
		setNumber(stage/pow(i), (cnt-i)*40+220);
		stage = stage % pow(i);
	}
}



void ScoreGraphicObject::setScore(long score)
{
	int i, cnt;
	cnt = log(score);
	getCache(cnt - 1);
	for(i=cnt; i>0; i--)
	{
		setNumber(score/pow(i), (cnt-i)*40+540);
		score = score % pow(i);
	}
}

int ScoreGraphicObject::log(long score)
{
	if(score/10) return 1 + log(score/10);
	else return 1;
}

long ScoreGraphicObject::pow(int c)
{
	long temp = 1;
	for(int i=1; i<c; i++){
		temp *= 10;
	}
	return temp;
}

void ScoreGraphicObject::setNumber(int score, int position)
{
	int fp, x, y;
	char str[] = "c:\\nemo\\image\\0.bmp";
	unsigned char buffer[40*3];
	str[14] = score | 48;
	fp = _open(str, O_BINARY);
	if(fp == -1){
		printf("Number File Open Error");
		getch();
		exit(1);
	}
	_read(fp, buffer, 54);
	for(y=49;y>=0;y--){
		_read(fp, buffer, 40*3);
		for(x=0;x<40;x++){
			if(buffer[x*3]!=83 || buffer[x*3+1]!= 138 || buffer[x*3+2]!=138){
				setPixel_Blue(x+position, y+20, buffer[x*3]);
				setPixel_Green(x+position, y+20, buffer[x*3+1]);
				setPixel_Red(x+position, y+20, buffer[x*3+2]);
			}
		}
	}
	_close(fp);
}



void ScoreGraphicObject::setCache()
{
	int x, y, fp;
	unsigned char buffer[480*3];
	char str[] = "c:\\nemo\\cache.dat";
	fp = _open(str, O_BINARY | O_WRONLY | O_TRUNC);
	if(fp == -1){
		printf("%c", 0x07);
		printf("Cache Data Write Error");
		getch();
		exit(1);
	}
	for(y=69; y>=20; y--){
		for(x=0; x<480; x++){
			buffer[x*3]=getPixel_Blue(x+540, y);
			buffer[x*3+1]=getPixel_Green(x+540, y);
			buffer[x*3+2]=getPixel_Red(x+540, y);
		}
		_write(fp, buffer, 480*3);
	}
	_close(fp);
}

void ScoreGraphicObject::getCache(int cnt)
{
	int x, y, fp;
	unsigned char buffer[480*3];
	char str[] = "c:\\nemo\\cache.dat";
	fp = _open(str, O_BINARY);
	if(fp == -1){
		printf("%c", 0x07);
		printf("Cache Data Read Error");
		getch();
		exit(1);
	}
	for(y=69; y>=20; y--){
		_read(fp, buffer, 480*3);
		for(x=0; x<40*cnt; x++){
				setPixel_Blue(x+540, y, buffer[x*3]);
				setPixel_Green(x+540, y, buffer[x*3+1]);
				setPixel_Red(x+540, y, buffer[x*3+2]);
		}
	}
	_close(fp);
}





//------------------------------------------

BackgroundImageObject::BackgroundImageObject()
{
	char str[] = "c:\\nemo\\image\\b1.bmp";
	str[15] = (rand()%10) | 48;
	fp = _open(str, O_BINARY);
	if(fp == -1){
		printf("Background Image Open Error\n");
		getch();
		exit(1);
	}
	_read(fp, buffer, 54);
}


BackgroundImageObject::~BackgroundImageObject()
{
	_close(fp);
}



void BackgroundImageObject::next()
{
	i = 0;
	_read(fp, buffer, 1024 * 3);
}




unsigned char BackgroundImageObject::getData()
{
	return buffer[i++];
}








BlockImageObject::BlockImageObject()
{
	Lv = 1;
	Max_Block_Type = 5;
	loadBlockImage();
}



void BlockImageObject::loadBlockImage()
{
	int	x, y, c, fp;
	int	i, j;
	unsigned char temp[40*3];
	char str[] = "c:\\nemo\\image\\bk1_L1.bmp";
	str[19] = Lv | 48;
	for(i = 0; i <= 0; i++)
	{
		for(j = 0; j <= 1; j++)
		{
			if(j == 0)
			{
				str[15] = 'k';
			}
			else
			{
				str[15] = 'x';
			}
			str[16] = i | 48;
			fp = _open(str, O_BINARY);
			if(fp == -1){
				printf("Block Image Open Error\n%s", str);
				getch();
				exit(1);
			}
			_read(fp, temp, 54);
			for(y=39;y>=0;y--)
			{
				_read(fp, temp, 40*3);
				for(x=0;x<40;x++)
				{
					BlockImage[i][j][x][y][0]=temp[x*3];
					BlockImage[i][j][x][y][1]=temp[x*3+1];
					BlockImage[i][j][x][y][2]=temp[x*3+2];

					//	i블럭 종류, j 플래그
				}
			}
			_close(fp);
		}
	}
}

void BlockImageObject::reloadBlockImage()
{
	loadBlockImage();
	int	x, y, c, fp;
	int	j;
	unsigned char temp[40*3];
	char str[] = "c:\\nemo\\image\\bk1_L1.bmp";
	str[16] = (rand()%Max_Block_Type+1) | 48;
	str[19] = Lv | 48;
	for(j = 0; j <= 1; j++)
	{
		if(j == 0)
		{
			str[15] = 'k';
		}
		else
		{
			str[15] = 'x';
		}
		fp = _open(str, O_BINARY);
		if(fp == -1){
			printf("Block Image Open Error\n%s", str);
			getch();
			exit(1);
		}
		_read(fp, temp, 54);
		for(y=39;y>=0;y--)
		{
			_read(fp, temp, 40*3);
			for(x=0;x<40;x++)
			{
				BlockImage[1][j][x][y][0]=temp[x*3];
				BlockImage[1][j][x][y][1]=temp[x*3+1];
				BlockImage[1][j][x][y][2]=temp[x*3+2];

					//	i블럭 종류, j 플래그
			}
		}
		_close(fp);
	}

}


LabelImageObject::LabelImageObject()
{
	Max_Label_Type = 2;
	changeLabelType();
}


void LabelImageObject::changeLabelType()
{
	Selected_Label_Type = rand() % Max_Label_Type + 1;
}


void LabelImageObject::loadLabelImage(int number)
{
	int	x, y, fp;
	unsigned char temp[40*3];
	char str[] = "c:\\nemo\\image\\n1_01_L1.bmp";
	str[15] = Selected_Label_Type | 48;
	str[17] = (number / 10) | 48;
	str[18] = (number % 10) | 48;
	str[21] = Lv | 48;
	fp = _open(str, O_BINARY);
	if(fp == -1){
		printf("Label Image Open Error\n%s", str);
		getch();
		exit(1);
	}
	_read(fp, temp, 54);
	for(y=39;y>=0;y--)
	{
		_read(fp, temp, 40*3);
		for(x=0;x<40;x++)
		{
			LabelImage[x][y][0]=temp[x*3];
			LabelImage[x][y][1]=temp[x*3+1];
			LabelImage[x][y][2]=temp[x*3+2];
		}
	}
	_close(fp);
}

CursorImageObject::CursorImageObject()
{
	Max_Cursor_Type = 3;
	changeCursorType();
}


void CursorImageObject::changeCursorType()
{
	Selected_Cursor_Type = rand() % Max_Cursor_Type + 1;

	int	x, y, fp;
	unsigned char temp[60*3];
	char str[] = "c:\\nemo\\image\\cursor01.bmp";
	str[21] = Selected_Cursor_Type | 48;
	fp = _open(str, O_BINARY);
	if(fp == -1){
		printf("Cursor Image Open Error\n%s", str);
		getch();
		exit(1);
	}
	_read(fp, temp, 54);
	for(y=59;y>=0;y--)
	{
		_read(fp, temp, 60*3);
		for(x=0;x<60;x++)
		{
			CursorImage[x][y][0]=temp[x*3];
			CursorImage[x][y][1]=temp[x*3+1];
			CursorImage[x][y][2]=temp[x*3+2];
		}
	}
	_close(fp);
}



//------------------------------------------------------------












class NemoBasicObject
{
	// 맵의 이름과 크기, 맵데이타를 담아둘 변수가 필요하다.
	// 현제 커서의 위치 변수가 필요함.
	//	char		Title[255];	타이틀 정보는 한글 구현의 어려움으로 삭제하기로 결정(2005.7.25)

protected:
	int	Map_Size;	// 30x30 크기까지 작성 예정이나, 그래픽 엔진 개발 여부에 따라 일단 20x20 까지만 구현해 볼 예정
				// MAX_Size 라는 변수명을 Map_Size로 변경 (2005.7.26)
	char	Label_Table[2][30][15];
					// 수직, 수평 레이블, 최대 각각 30개씩, 총 15개의 문자가 들어갈 수 있음.
	char	Block[30][30];

		/*
			1 Byte 변수를 비트별로 나누어서 정보를 저장.
				0 bit	CheckSum 비트 : 개임 클리어를 위해 예약됨
					Mark가 있는 경우만 미리 1로 셋되어 있어서,
					남은 Mark의 갯수와 스테이지 클리어 판단에 사용함
				1 bit	Mark Bit : 크랙이 발생할때, 판단 여부로 사용
				2 bit	Crack Bit : 깨진 벽돌인지 판단 여부로 사용
				3 bit	Flag Bit : X자 표시를 했는지 판단 여부
				4~7 bit	사용되지 않을 블럭 비트


				각 비트 상태에 따라서 나타나는 블럭의 상태
					 0 : 벽돌 (누르면 4번 해골이 됨, 마킹하면 8번 벽돌이 됨)
					 3 : 벽돌 (누르면 6번 정답이 됨, 마킹하면 11번 벽돌이 됨)
					 4 : 해골
					 6 : 정답
					 8 : 마킹된 벽돌 (마킹을 풀면 0번 벽돌이 됨)
					11 : 마킹된 벽돌 (마킹을 풀면 3번 벽돌이 됨)

					1,2,5,7,9,10 : 오류값 (이값이 나타나면 계산 함수들중에 오류가 있음)

		*/


	int		Cursor_x;
	int		Cursor_y;
//	private int		Time;
//	private boolean		Flag_status;

	/*
		생각 날때 마다 적어둔 구현해야 할것 같은 메소드들...

		맵 로딩 메소드,		--> loadBlock
		커서이동 메소드,	--> leftCursor, rightCursor, upCursor, downCursor
		플레그표시 메소드,	--> checkFlagBlock
		맵데이터 확인 메소드,	--> isGameClear
		상자 누르기 메소드,	--> crackBlock
	*/


public:
	void loadBlock()
	{
		// 아직 게임데이터가 구축되지 않았으므로 난수를 발생시켜서 적절한 값으로 초기화
		randomSetBlock();
	}


	// 로드 블럭 임시 함수
private:
	void randomSetBlock()
	{
		Map_Size = 30;
		Cursor_x = 0;
		Cursor_y = 0;

		for(int i = 0; i < Map_Size; i++)
		{
			for(int j = 0; j < Map_Size; j++)
			{
				if(rand()%5 == 0)
				{
					Block[i][j] = 0;
				}
				else
				{
					Block[i][j] = 3;
				}

			}
		}
	}













/*------------------------------------------------------------------------------
	커서를 움직이는 메서드들 (상하좌우 이동)
--------------------------------------------------------------------------------


	boolean	leftCursor();		커서의 논리적 좌표를 좌로 이동
	boolean	rightCursor();		커서의 논리적 좌표를 우로 이동
	boolean	upCursor();		커서의 논리적 좌표를 위로 이동
	boolean	downCursor();		커서의 논리적 좌표를 아래로 이동

	int	get_X();		현재 커서의 논리적 X좌표값을 구함
	int	get_Y();		현재 커서의 논리적 Y좌표값을 구함


*/

public:
	boolean leftCursor()
	{
		if(Cursor_x > 0)
		{
			Cursor_x --;
			return true;
		}
		return false;
	}



	boolean rightCursor()
	{
		if(Cursor_x < Map_Size - 1)
		{
			Cursor_x ++;
			return true;
		}
		return false;
	}



	boolean upCursor()
	{
		if(Cursor_y > 0)
		{
			Cursor_y --;
			return true;
		}
		return false;
	}



	boolean downCursor()
	{
		if(Cursor_y < Map_Size - 1)
		{
			Cursor_y ++;
			return true;
		}
		return false;
	}


	int get_X()
	{
		return Cursor_x;
	}



	int get_Y()
	{
		return Cursor_y;
	}



//------------------------------------------------------------------------------
//   					The End of Methords about the Cursor
//							여기까지 커서관련 메서드










/*------------------------------------------------------------------------------
	블럭 처리를 위한 메서드
--------------------------------------------------------------------------------


	int	getSize();		맵의 크기를 구함
	int	getRemainBlock();	남은 정답블럭 수를 구함
	boolean	isGameClear();		게임 클리어 했는지를 판단합니다.
	int	getBlockState();	현재 커서위치의 벽돌 모양을 구함
	int	getBlockState(int, int);지정된 위치의 위치의 벽돌모양을 구함
						0 - 그냥 벽돌	(블럭비트 0, 3)
						1 - 해골	(블럭비트 4)
						2 - 빈칸	(블럭비트 5)
						3 - 표시한 벽돌	(블럭비트 8, 11)
						9 - 오류시	(나머지 비트값들)

	void	crackBlock();		현재 커서위치의 벽돌을 부숩니다.
	void	checkFlagBlock();	현재 커서위치의 벽돌에 깃발을 붙입니다.

*/

public:


	int getSize()
	{
		return Map_Size;
	}


	int getRemainBlock()
	{
		int	Check_Sum = 0;
		for(int i = 0; i < Map_Size; i++)
		{
			for(int j = 0; j < Map_Size; j++)
			{
				Check_Sum += Block[i][j] & 1;
			}
		}
		return	Check_Sum;
	}



	boolean isGameClear()
	{
		if(getRemainBlock() == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	int getBlockState()
	{
		getBlockState(Cursor_x, Cursor_y);
	}


	int getBlockState(int x, int y)
	{
		switch(Block[y][x]){
			case 0:
			case 3:
				return 0;
				break;
			case 4:
				return 1;
				break;
			case 6:
				return 2;
				break;
			case 8:
			case 11:
				return 3;
				break;
			default:
				return 9;
				break;
		}
	}






	void crackBlock()
	{
		crackBlock(Cursor_x, Cursor_y);
	}




	void checkFlagBlock()
	{
		checkFlagBlock(Cursor_x, Cursor_y);
	}




protected:

	void crackBlock(int x, int y)
	{
		// CrackBit가 비활성화 된 블럭에서만 작동을 한다.
		if(!isCrackBit(x, y) && !isFlagBit(x, y))
		{
			setCrackBit(x, y);	// CrackBit를 활성화 시킨다.
			if(isMarkBit(x, y))
			{
				// MarkBit가 활성화 되어 있으면, CheckSum 비트를 초기화 한다.
				Block[y][x] --;
			}
		}
	}



	void checkFlagBlock(int x, int y)
	{
		// CrackBit가 비활성화된 블럭에서만 작동을 한다.
		if(!isCrackBit(x, y))
		{
			setFlagBit(x, y);	// FlagBit를 반전 시킨다.
		}
	}








		// 2, 4, 8은 각각 2, 3, 4번째 비트연산에 필요한 마스크 값이다.
		// 253, 251, 247은 각각 마스크값의 반전된 값이다.


	int getMarkBit(int x, int y)
	{
		return Block[y][x] & 2;
	}


	int getCrackBit(int x, int y)
	{
		return Block[y][x] & 4;
	}


	int getFlagBit(int x, int y)
	{
		return Block[y][x] & 8;
	}



	void setMarkBit(int x, int y)
	{
		if(isMarkBit(x, y))
		{
			Block[y][x] &= 253;
		}
		else
		{
			Block[y][x] |= 2;
		}
	}



	void setCrackBit(int x, int y)
	{
		if(isCrackBit(x, y))
		{
			Block[y][x] &= 251;
		}
		else
		{
			Block[y][x] |= 4;
		}
	}


	void setFlagBit(int x, int y)
	{
		if(isFlagBit(x, y))
		{
			Block[y][x] &= 247;
		}
		else
		{
			Block[y][x] |= 8;
		}
	}



	void setMarkBit()
	{
		setMarkBit(Cursor_x, Cursor_y);
	}



	void setCrackBit()
	{
		setCrackBit(Cursor_x, Cursor_y);
	}



	void setFlagBit()
	{
		setFlagBit(Cursor_x, Cursor_y);
	}







	boolean isMarkBit(int x, int y)
	{
		if((getMarkBit(x, y) != 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}



	boolean isCrackBit(int x, int y)
	{
		if((getCrackBit(x, y) != 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}



	boolean isFlagBit(int x, int y)
	{
		if((getFlagBit(x, y) != 0))
		{
			return true;
		}
		else
		{
			return false;
		}
	}



	boolean isMarkBit()
	{
		return isMarkBit(Cursor_x, Cursor_y);
	}



	boolean isCrackBit()
	{
		return isCrackBit(Cursor_x, Cursor_y);
	}



	boolean isFlagBit()
	{
		return isFlagBit(Cursor_x, Cursor_y);
	}


//------------------------------------------------------------------------------
//						The End of Methods about Block
//						여기까지 블럭처리에 관련된 메서드








/*------------------------------------------------------------------------------
	라벨처리를 위한 메서드
--------------------------------------------------------------------------------


	void set_Label();	Label_Table 을 계산합니다.
	void set_Lable_init();	Label_Table 을 모두 0으로 초기화 합니다.


*/

public:
	char getLabel(int i, int j, int length)
	{
		return Label_Table[i][j][length];
	}

protected:
	void set_Label()
	{
		set_Label_init();
		int	count, i, j, check;

		for(i = 0; i < Map_Size; i++)
		{
			count = 0;
			check = 0;
			for(j = Map_Size - 1; j >= 0; j--)
			{
				if(isMarkBit(j, i) == true)
				{
					Label_Table[0][i][count]++;
					check = 1;
				}
				else if(check)
				{
					count++;
					check = 0;
				}
			}
		}

		for(i = 0; i < Map_Size; i++)
		{
			count = 0;
			check = 0;
			for(j = Map_Size - 1; j >= 0; j--)
			{
				if(isMarkBit(i, j) == true)
				{
					Label_Table[1][i][count]++;
					check = 1;
				}
				else if(check)
				{
					count++;
					check = 0;
				}
			}
		}

	}

private:
	void set_Label_init()
	{
		for(int i = 0; i < 2; i++)
		{
			for(int j = 0; j < 30; j++)
			{
				for(int k = 0; k < 15; k++)
				{
					Label_Table[i][j][k] = 0;
				}
			}
		}
	}



//------------------------------------------------------------------------------
//						The End of Methods about Label
//						여기까지 라벨처리를 위한 메서드







};













class NemoDataObject : public NemoBasicObject
{
/*
	맵데이터를 저장하고 있는 데이터 파일로부터 정보를 읽어와서 보관하여,
	NemoBasicObject	객체가 필요시에 정보를 가져갈수 있도록 도와준다.
	파일 첫부분에는 읽어올 데이터의 갯수, 간락한 파일정보를 포함하여,
	필요한 만큼 메모리를 활당하여, 불러오는 방법이나,
	한 스테이지만큼의 정보만 필요할때 불러오는 방법중 하나를 구현할 예정이다.

	필요한 맵정보만 불러올시, 메모리에 많은 이득이 있으나, 각 맵의 사이즈만은 미리 읽어두어서,
	난이도에 따른 맵을 선택할 수 있도록 하거나,

	각 난이도에 따른 데이터 파일을 처움부터 분리하여 저장하는 방법을 사용할 수도 있다.


	현재 난이도값을 기억하고 필요에 따라 난이도를 증가시킨다.

--------------------------------------------------------------------------------

	NemoDataObject는 개발중에 여러 차례 수정되었습니다.
	처음에는 파일처리와 맵데이터를 넘겨주는 독립객체로 구성하였고,

	그 후에, NemoBasicObject를 상속하는 방법으로 변경되었습니다.



	NemoDataObject는
	전체 맵의 총갯수, 맵크기별 데이터, 레벨, 현재 로딩한 맵번호를 속성값이 추가됬으며,



	다음과 같은 메서드를 추가 지원합니다.



	void loadBlock();	NemoBasicObject에서 오버로딩되었습니다. 이제는 랜덤값이 아닌
				nemo.dat 파일에서 현제 설정된 레벨에 맞는 맵 데이터를 무작위로 로딩합니다.

	void levelUp();		레벨을 올립니다.
	void levelDown();	레벨다운 함수는 임시로 지원합니다. 테스트 중에 레벨을 마음대로 변경할수 있도록 지원됩니다.
	void levelInit();	레벨을 초기화 합니다.



*/

protected:
	int	Data_Size;		// MAP_MAX_Num 을 Data_Size 으로 변경
	char*	Map_Size_Table;		// MAP_Size_Table 을 Map_Size_Table 으로 변경
	FILE	*f;
	int	Lv;
	int	Now_Map;		// nowMAP 을 Now_Map 으로 변경



public:

	NemoDataObject()		// 생성자 함수 파일을 열고 초기값을 읽어온다.
	{
		getMapSize();
		Lv = 1;
	}

	~NemoDataObject()
	{
		if(Map_Size_Table != NULL)
		{
			free(Map_Size_Table);
		}
	}



/*------------------------------------------------------------------------------
	맵 로딩에 관련된 메서드들
--------------------------------------------------------------------------------


	void loadBlock();	NemoBasicObject에서 오버로딩되었습니다. 이제는 랜덤값이 아닌
				nemo.dat 파일에서 현제 설정된 레벨에 맞는 맵 데이터를 무작위로 로딩합니다.

	void levelUp();		레벨을 올립니다.
	void levelDown();	레벨다운 함수는 임시로 지원합니다. 테스트 중에 레벨을 마음대로 변경할수 있도록 지원됩니다.
	void levelInit();	레벨을 초기화 합니다.



*/


	void loadBlock()
	{
		Now_Map = selectMap(Lv);
		Map_Size = Map_Size_Table[Now_Map];
		Cursor_x = 0;
		Cursor_y = 0;

		f = fopen(Data_File, "rb");
		if(f == NULL)
		{
			printf("Data file error : Read Error");
			exit(0);
		}
		fseek(f, (long)Now_Map * 901 + 3, SEEK_SET);
		for(int i = 0; i < 30; i++)
		{
			for(int j = 0; j < 30; j++)
			{
				Block[i][j] = fgetc(f);
			}
		}
		if(f)
		{
			fclose(f);
		}

		set_Label();
	}



	void levelUp()
	{
		if(Lv<5)
		{
			Lv++;
		}
	}

	void levelDown()
	{
		if(Lv>1)
		{
			Lv--;
		}
	}

	void levelInit()
	{
		Lv = 1;
	}

	int getLevel()
	{
		return Lv;
	}


private:
					/* getMapSize 함수는 파일로부터 맵크기를 읽어오는 내부적 함수이다.
					   사용중인 맵 크기값이 필요할땐, getSize 함수를 이용할 것. */

	void getMapSize()
	{
		f = fopen(Data_File, "rb");
		if(f == NULL)
		{
			printf("Data file error : Not File");
			exit(0);
		}
		fread(&Data_Size, sizeof(Data_Size), 1, f);

		Map_Size_Table = (char*) malloc(Data_Size);

		if(Map_Size_Table == NULL)
		{
			printf("Memory Error.");
			exit(0);
		}
		for(int i = 0; i < Data_Size; i++){
			Map_Size_Table[i] = fgetc(f);
			fseek(f, 900, SEEK_CUR);
		}
		if(f){
			fclose(f);
		}
	}


	int select_MAP(int Map_Size)
	{
		char	selected_MAP;
		do{
			selected_MAP = rand() % Data_Size;
		} while(Map_Size_Table[selected_MAP]!=Map_Size);
		return	selected_MAP;
	}




	int selectMap(int Lv)
	{
		char	Map_Size;
		switch(Lv){
			case 1:
				Map_Size = 10;
				break;
			case 2:
				Map_Size = 15;
				break;
			case 3:
				Map_Size = 20;
				break;
			case 4:
				Map_Size = 25;
				break;
			case 5:
				Map_Size =30;
				break;
		}
		return select_MAP(Map_Size);
	}



//------------------------------------------------------------------------------
//						The End of Methods about Loading
//						여기까지 맵로딩을 위한 메소드


};








class NemoScoreObject : public NemoDataObject
{
/*
	네모로직의 스테이지 클리어 여부와, 점수, 아이템등을 추가 지원하기 위하여
	구현해야할 함수들로 이루어진 클래스입니다.


	점수 규칙(아직 미확정)

		- 스태이지 난이도에 따라 정답 블럭 한개당 특정 포인트 부여
			Lv.1	10 포인트
			Lv.2	20 포인트
			Lv.3	30 포인트
			Lv.4	50 포인트
			Lv.5	100 포인트
		- 스태이지 클리어시 특정 포인트 부여, 남은 시간포인트를 환산함
		- 일정 난수로 보너스 포인트 부여
		- 연속 히트당 보너스 포인트 부여
			5 포인트당	100 점
			10 포인트당	400 점	합게  500점
			20 포인트당	500 점	합계 1000점



*/

	long	Score;		// 게임 스코어
	long	Point;		// 스코어에 반영될 점수		점수가 한번에 오르지 않도록 하기 위한 버퍼
	int	PointSignal;

	int	Stage;		// 게임 스테이지
	int	HitCount;	// 연속 히트수			보너스 점수 계산에 이용함

	int	TimeCount;	// 남은 시간수			게임 오버 판단에 사용함
	int	MaxTime;
	long	TimeSignal;

	int	ItemSignal;

	int	Help;		// 헬프 카운트 수


public:

	NemoScoreObject()
	{
		Stage = 1;
		Score = 0;
		Point = 0;
		PointSignal = 5000;
		HitCount = 0;
		Help = 3;
		MaxTime = 400;
		TimeCount = MaxTime;
		TimeSignal = 50000;

	}



/*------------------------------------------------------------------------------
	게임 스코어에 관련된 메서드들
--------------------------------------------------------------------------------



	int getStage();			현재 스테이지를 구함
	int getScore();			현재 점수를 구함


*/

	void clearStage()
	{
		Stage = 1;
	}


	int getStage()
	{
		return Stage;
	}


	void clearScore()
	{
		Score = 0;
	}

	long getScore()
	{
		return Score;
	}



	//  crackBlock() 	점수를 추가 할수 있도록, 재정의 되었습니다.

	boolean crackBlock()
	{
		boolean check;
		check = !isCrackBit();
		if(isMarkBit() && !isFlagBit() && check)
		{
			switch(Lv){
				case 1:
					Point += 10;
					break;
				case 2:
					Point += 20;
					break;
				case 3:
					Point += 30;
					break;
				case 4:
					Point += 50;
					break;
				case 5:
					Point += 100;
					break;
			}
			HitCount++;
			plusHitPoint();
			plusTime(10);
		}
		else if(!isMarkBit() && !isFlagBit() && check)
		{
			HitCount = 0;
			plusTime(-40);
		}
		NemoBasicObject::crackBlock();
		return check;
	}

	boolean isGameClear()
	{
		if(NemoBasicObject::isGameClear())
		{
			clearPoint();
		}
		return NemoBasicObject::isGameClear();

	}


	boolean Point2Score()		// 누적된 포인트를 스코어로 조금씩 바꾸어 줍니다.
	{
		if(PointSignal>0)
		{
			PointSignal--;
			return false;
		}
		else
		{
			PointSignal = 5000;
			if(Point)
			{
				Point -= 10;
				Score += 10;
				return true;
			}
		}
		return false;
	}



	void clearPoint()
	{
		Point += (Stage/10+1)*1000;
		Stage++;
	}



	void plusHitPoint()
	{
		if(HitCount % 5 == 0)
		{
			Point += 100;
		}
		if(HitCount % 10 == 0)
		{
			Point += 400;
		}
		if(HitCount % 20 == 0)
		{
			Point += 500;
		}
	}


	int getHitCount()
	{
		return HitCount;
	}




//------------------------------------------------------------------------------
//					The End of Methods about the Socre
//						여기까지 점수를 위한 메서드




/*------------------------------------------------------------------------------
	Time 구현
--------------------------------------------------------------------------------


*/

	void setTime()
	{
		TimeCount = MaxTime;
	}

	void setTime(int count)
	{
		TimeCount = count;
	}


	int getTime()
	{
		return TimeCount;
	}


	boolean spendTime()
	{
		if(TimeSignal>0)
		{
			TimeSignal --;
			return false;
		}
		else
		{
			TimeSignal = 50000;
			TimeCount -= 1;
			if(TimeCount<0)
			{
				TimeCount = 0;
			}
			return true;
		}
		return false;
	}



//------------------------------------------------------------------------------
//						The End of Methods about the Time
//						여기까지 시간처리를 위한 메서드




/*------------------------------------------------------------------------------
	Help 및 Item 구현을 위한 메서드
--------------------------------------------------------------------------------



*/


	boolean useHelp()
	{
		if(Help > 0)
		{
			Help--;
			for(int i = 0; i < Map_Size; i++)
			{
				if(isCrackBit(Cursor_x, i) == false)
				{
					if(isMarkBit(Cursor_x, i))
					{
						if(isFlagBit(Cursor_x, i) == true)
						{
							checkFlagBlock(Cursor_x, i);
						}
						NemoBasicObject::crackBlock(Cursor_x, i);
					}
					else if(isFlagBit(Cursor_x, i) == false)
					{
						checkFlagBlock(Cursor_x, i);
					}
				}
				if(isCrackBit(i, Cursor_y) == false)
				{
					if(isMarkBit(i, Cursor_y))
					{
						if(isFlagBit(i, Cursor_y) == true)
						{
							checkFlagBlock(i, Cursor_y);
						}
						NemoBasicObject::crackBlock(i, Cursor_y);
					}
					else if(isFlagBit(i, Cursor_y) == false)
					{
						checkFlagBlock(i, Cursor_y);
					}
				}
			}
			return true;
		}
		return false;
	}

	void setHelp()
	{
		Help = 3;
	}

protected:

	void plusHelp()
	{
		if(Help < 3)
		{
			Help++;
		}
	}


	void plusTime(int Time)
	{
		TimeCount += Time;
		if(TimeCount > MaxTime)
		{
			TimeCount = MaxTime;
		}
		if(TimeCount < 0)
		{
			TimeCount = 0;
		}
	}


//------------------------------------------------------------------------------
//					The End of Methods about Help and Items.
//					여기까지 헬프와 아이템을 위한 메서드



};











void main()
{
	srand(time(NULL));
	int i, j, ch;
//	NemoBasicObject nb;
//	NemoDataObject	nb;

	NemoScoreObject    nb;

//	BasicGraphicObject bg;

	BlockGraphicObject bg;
	ScoreGraphicObject sg;
	TimeGraphicObject tg;


Restart:


	bg.loadBackgroundImage();
	sg.setStageTitle();
	sg.setScoreTitle();
	sg.setCache();
	sg.setStage(nb.getStage());
	sg.setScore(nb.getScore());
	bg.changeLabelType();
	bg.changeCursorType();
	nb.loadBlock();
	tg.setTimeClear();
	nb.setTime();
	nb.setHelp();


	for(i=0; i<nb.getSize(); i++){
		for(j=0; j<15; j++){
			if(nb.getLabel(0,i,j)!=0)
				bg.setXLabel(i, j, nb.getLabel(0, i, j));
		}
	}

	for(i=0; i<nb.getSize(); i++){
		for(j=0; j<15; j++){
			if(nb.getLabel(1,i,j)!=0)
				bg.setYLabel(i, j, nb.getLabel(1, i, j));
		}
	}


	for(i=0; i<nb.getSize(); i++){
		for(j=0;j<nb.getSize();j++)
		{
		    bg.setBlock(j,i,nb.getBlockState(j,i));
		}
	}

	bg.setCursor(0,0);
	while(bioskey(1)) bioskey(0);
	while(!bioskey(1));
	do{
		if(nb.Point2Score()){
			sg.setScore(nb.getScore());
		}
		if(nb.getTime()==0)
		{
			// 게임오버
			tg.setTime(nb.getTime());
			bg.setTimeOver();

			delay(5000);
			nb.setTime();
			nb.clearScore();
			nb.clearStage();
			goto Restart;
		}
		else
		{
			if(nb.spendTime())
			{
				//시간 감소 그래픽 처리
				tg.setTime(nb.getTime());
			}
		}

		if(kbhit()){
			ch=bioskey(0);
			if((ch & 0x00FF) != 0){
				switch((char)(ch&0x00FF)){
					case ' ':
						if(nb.crackBlock())
							bg.crackEffect(nb.get_X(), nb.get_Y());
						bg.hiddenCursor();
						bg.setBlock(nb.get_X(), nb.get_Y(), nb.getBlockState());
						bg.setCursor(nb.get_X(), nb.get_Y());
						if(nb.isGameClear()){
							bg.hiddenCursor();
							bg.setStageClear();
							nb.setTime();
							delay(5000);
							goto Restart;
						}
						break;
					case 'Z':
					case 'z':
						nb.checkFlagBlock();
						bg.hiddenCursor();
						bg.setBlock(nb.get_X(), nb.get_Y(), nb.getBlockState());
						bg.setCursor(nb.get_X(), nb.get_Y());
						break;
					case 'h':
					case 'H':
						if(nb.useHelp()){
							bg.hiddenCursor();
							for(i=0;i<nb.getSize();i++)
							{
								for(j=0;j<nb.getSize();j++){
									bg.setBlock(i, j, nb.getBlockState(i,j));
								}
							}
							bg.setCursor(nb.get_X(), nb.get_Y());
						}
						break;
					case '`':
						goto Restart;
						break;
				}
			}
			else{
				switch(ch){
					case UP:
						nb.upCursor();
						break;
					case RIGHT:
						nb.rightCursor();
						break;
					case LEFT:
						nb.leftCursor();
						break;
					case DOWN:
						nb.downCursor();
						break;
					case HOME:
						nb.levelUp();
						bg.setLevel(nb.getLevel());
						goto Restart;
						break;
					case END:
						nb.levelDown();
						bg.setLevel(nb.getLevel());
						goto Restart;
						break;
				}
				bg.moveCursor(nb.get_X(), nb.get_Y());
			}
		}
	}while(ch != ALT_X);
}
