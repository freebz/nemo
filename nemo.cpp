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



void BlockGraphicObject::setBlock(int x, int y, int type)		// ��ӹ޾� �������̵� �Ǿ����ϴ�.
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

					//	i�� ����, j �÷���
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

					//	i�� ����, j �÷���
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
	// ���� �̸��� ũ��, �ʵ���Ÿ�� ��Ƶ� ������ �ʿ��ϴ�.
	// ���� Ŀ���� ��ġ ������ �ʿ���.
	//	char		Title[255];	Ÿ��Ʋ ������ �ѱ� ������ ��������� �����ϱ�� ����(2005.7.25)

protected:
	int	Map_Size;	// 30x30 ũ����� �ۼ� �����̳�, �׷��� ���� ���� ���ο� ���� �ϴ� 20x20 ������ ������ �� ����
				// MAX_Size ��� �������� Map_Size�� ���� (2005.7.26)
	char	Label_Table[2][30][15];
					// ����, ���� ���̺�, �ִ� ���� 30����, �� 15���� ���ڰ� �� �� ����.
	char	Block[30][30];

		/*
			1 Byte ������ ��Ʈ���� ����� ������ ����.
				0 bit	CheckSum ��Ʈ : ���� Ŭ��� ���� �����
					Mark�� �ִ� ��츸 �̸� 1�� �µǾ� �־,
					���� Mark�� ������ �������� Ŭ���� �Ǵܿ� �����
				1 bit	Mark Bit : ũ���� �߻��Ҷ�, �Ǵ� ���η� ���
				2 bit	Crack Bit : ���� �������� �Ǵ� ���η� ���
				3 bit	Flag Bit : X�� ǥ�ø� �ߴ��� �Ǵ� ����
				4~7 bit	������ ���� �� ��Ʈ


				�� ��Ʈ ���¿� ���� ��Ÿ���� ���� ����
					 0 : ���� (������ 4�� �ذ��� ��, ��ŷ�ϸ� 8�� ������ ��)
					 3 : ���� (������ 6�� ������ ��, ��ŷ�ϸ� 11�� ������ ��)
					 4 : �ذ�
					 6 : ����
					 8 : ��ŷ�� ���� (��ŷ�� Ǯ�� 0�� ������ ��)
					11 : ��ŷ�� ���� (��ŷ�� Ǯ�� 3�� ������ ��)

					1,2,5,7,9,10 : ������ (�̰��� ��Ÿ���� ��� �Լ����߿� ������ ����)

		*/


	int		Cursor_x;
	int		Cursor_y;
//	private int		Time;
//	private boolean		Flag_status;

	/*
		���� ���� ���� ����� �����ؾ� �Ұ� ���� �޼ҵ��...

		�� �ε� �޼ҵ�,		--> loadBlock
		Ŀ���̵� �޼ҵ�,	--> leftCursor, rightCursor, upCursor, downCursor
		�÷���ǥ�� �޼ҵ�,	--> checkFlagBlock
		�ʵ����� Ȯ�� �޼ҵ�,	--> isGameClear
		���� ������ �޼ҵ�,	--> crackBlock
	*/


public:
	void loadBlock()
	{
		// ���� ���ӵ����Ͱ� ������� �ʾ����Ƿ� ������ �߻����Ѽ� ������ ������ �ʱ�ȭ
		randomSetBlock();
	}


	// �ε� �� �ӽ� �Լ�
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
	Ŀ���� �����̴� �޼���� (�����¿� �̵�)
--------------------------------------------------------------------------------


	boolean	leftCursor();		Ŀ���� ���� ��ǥ�� �·� �̵�
	boolean	rightCursor();		Ŀ���� ���� ��ǥ�� ��� �̵�
	boolean	upCursor();		Ŀ���� ���� ��ǥ�� ���� �̵�
	boolean	downCursor();		Ŀ���� ���� ��ǥ�� �Ʒ��� �̵�

	int	get_X();		���� Ŀ���� ���� X��ǥ���� ����
	int	get_Y();		���� Ŀ���� ���� Y��ǥ���� ����


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
//							������� Ŀ������ �޼���










/*------------------------------------------------------------------------------
	�� ó���� ���� �޼���
--------------------------------------------------------------------------------


	int	getSize();		���� ũ�⸦ ����
	int	getRemainBlock();	���� ����� ���� ����
	boolean	isGameClear();		���� Ŭ���� �ߴ����� �Ǵ��մϴ�.
	int	getBlockState();	���� Ŀ����ġ�� ���� ����� ����
	int	getBlockState(int, int);������ ��ġ�� ��ġ�� ��������� ����
						0 - �׳� ����	(����Ʈ 0, 3)
						1 - �ذ�	(����Ʈ 4)
						2 - ��ĭ	(����Ʈ 5)
						3 - ǥ���� ����	(����Ʈ 8, 11)
						9 - ������	(������ ��Ʈ����)

	void	crackBlock();		���� Ŀ����ġ�� ������ �μ��ϴ�.
	void	checkFlagBlock();	���� Ŀ����ġ�� ������ ����� ���Դϴ�.

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
		// CrackBit�� ��Ȱ��ȭ �� �������� �۵��� �Ѵ�.
		if(!isCrackBit(x, y) && !isFlagBit(x, y))
		{
			setCrackBit(x, y);	// CrackBit�� Ȱ��ȭ ��Ų��.
			if(isMarkBit(x, y))
			{
				// MarkBit�� Ȱ��ȭ �Ǿ� ������, CheckSum ��Ʈ�� �ʱ�ȭ �Ѵ�.
				Block[y][x] --;
			}
		}
	}



	void checkFlagBlock(int x, int y)
	{
		// CrackBit�� ��Ȱ��ȭ�� �������� �۵��� �Ѵ�.
		if(!isCrackBit(x, y))
		{
			setFlagBit(x, y);	// FlagBit�� ���� ��Ų��.
		}
	}








		// 2, 4, 8�� ���� 2, 3, 4��° ��Ʈ���꿡 �ʿ��� ����ũ ���̴�.
		// 253, 251, 247�� ���� ����ũ���� ������ ���̴�.


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
//						������� ��ó���� ���õ� �޼���








/*------------------------------------------------------------------------------
	��ó���� ���� �޼���
--------------------------------------------------------------------------------


	void set_Label();	Label_Table �� ����մϴ�.
	void set_Lable_init();	Label_Table �� ��� 0���� �ʱ�ȭ �մϴ�.


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
//						������� ��ó���� ���� �޼���







};













class NemoDataObject : public NemoBasicObject
{
/*
	�ʵ����͸� �����ϰ� �ִ� ������ ���Ϸκ��� ������ �о�ͼ� �����Ͽ�,
	NemoBasicObject	��ü�� �ʿ�ÿ� ������ �������� �ֵ��� �����ش�.
	���� ù�κп��� �о�� �������� ����, ������ ���������� �����Ͽ�,
	�ʿ��� ��ŭ �޸𸮸� Ȱ���Ͽ�, �ҷ����� ����̳�,
	�� ����������ŭ�� ������ �ʿ��Ҷ� �ҷ����� ����� �ϳ��� ������ �����̴�.

	�ʿ��� �������� �ҷ��ý�, �޸𸮿� ���� �̵��� ������, �� ���� ������� �̸� �о�ξ,
	���̵��� ���� ���� ������ �� �ֵ��� �ϰų�,

	�� ���̵��� ���� ������ ������ ó����� �и��Ͽ� �����ϴ� ����� ����� ���� �ִ�.


	���� ���̵����� ����ϰ� �ʿ信 ���� ���̵��� ������Ų��.

--------------------------------------------------------------------------------

	NemoDataObject�� �����߿� ���� ���� �����Ǿ����ϴ�.
	ó������ ����ó���� �ʵ����͸� �Ѱ��ִ� ������ü�� �����Ͽ���,

	�� �Ŀ�, NemoBasicObject�� ����ϴ� ������� ����Ǿ����ϴ�.



	NemoDataObject��
	��ü ���� �Ѱ���, ��ũ�⺰ ������, ����, ���� �ε��� �ʹ�ȣ�� �Ӽ����� �߰�������,



	������ ���� �޼��带 �߰� �����մϴ�.



	void loadBlock();	NemoBasicObject���� �����ε��Ǿ����ϴ�. ������ �������� �ƴ�
				nemo.dat ���Ͽ��� ���� ������ ������ �´� �� �����͸� �������� �ε��մϴ�.

	void levelUp();		������ �ø��ϴ�.
	void levelDown();	�����ٿ� �Լ��� �ӽ÷� �����մϴ�. �׽�Ʈ �߿� ������ ������� �����Ҽ� �ֵ��� �����˴ϴ�.
	void levelInit();	������ �ʱ�ȭ �մϴ�.



*/

protected:
	int	Data_Size;		// MAP_MAX_Num �� Data_Size ���� ����
	char*	Map_Size_Table;		// MAP_Size_Table �� Map_Size_Table ���� ����
	FILE	*f;
	int	Lv;
	int	Now_Map;		// nowMAP �� Now_Map ���� ����



public:

	NemoDataObject()		// ������ �Լ� ������ ���� �ʱⰪ�� �о�´�.
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
	�� �ε��� ���õ� �޼����
--------------------------------------------------------------------------------


	void loadBlock();	NemoBasicObject���� �����ε��Ǿ����ϴ�. ������ �������� �ƴ�
				nemo.dat ���Ͽ��� ���� ������ ������ �´� �� �����͸� �������� �ε��մϴ�.

	void levelUp();		������ �ø��ϴ�.
	void levelDown();	�����ٿ� �Լ��� �ӽ÷� �����մϴ�. �׽�Ʈ �߿� ������ ������� �����Ҽ� �ֵ��� �����˴ϴ�.
	void levelInit();	������ �ʱ�ȭ �մϴ�.



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
					/* getMapSize �Լ��� ���Ϸκ��� ��ũ�⸦ �о���� ������ �Լ��̴�.
					   ������� �� ũ�Ⱚ�� �ʿ��Ҷ�, getSize �Լ��� �̿��� ��. */

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
//						������� �ʷε��� ���� �޼ҵ�


};








class NemoScoreObject : public NemoDataObject
{
/*
	�׸������ �������� Ŭ���� ���ο�, ����, �����۵��� �߰� �����ϱ� ���Ͽ�
	�����ؾ��� �Լ���� �̷���� Ŭ�����Դϴ�.


	���� ��Ģ(���� ��Ȯ��)

		- �������� ���̵��� ���� ���� �� �Ѱ��� Ư�� ����Ʈ �ο�
			Lv.1	10 ����Ʈ
			Lv.2	20 ����Ʈ
			Lv.3	30 ����Ʈ
			Lv.4	50 ����Ʈ
			Lv.5	100 ����Ʈ
		- �������� Ŭ����� Ư�� ����Ʈ �ο�, ���� �ð�����Ʈ�� ȯ����
		- ���� ������ ���ʽ� ����Ʈ �ο�
		- ���� ��Ʈ�� ���ʽ� ����Ʈ �ο�
			5 ����Ʈ��	100 ��
			10 ����Ʈ��	400 ��	�հ�  500��
			20 ����Ʈ��	500 ��	�հ� 1000��



*/

	long	Score;		// ���� ���ھ�
	long	Point;		// ���ھ �ݿ��� ����		������ �ѹ��� ������ �ʵ��� �ϱ� ���� ����
	int	PointSignal;

	int	Stage;		// ���� ��������
	int	HitCount;	// ���� ��Ʈ��			���ʽ� ���� ��꿡 �̿���

	int	TimeCount;	// ���� �ð���			���� ���� �Ǵܿ� �����
	int	MaxTime;
	long	TimeSignal;

	int	ItemSignal;

	int	Help;		// ���� ī��Ʈ ��


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
	���� ���ھ ���õ� �޼����
--------------------------------------------------------------------------------



	int getStage();			���� ���������� ����
	int getScore();			���� ������ ����


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



	//  crackBlock() 	������ �߰� �Ҽ� �ֵ���, ������ �Ǿ����ϴ�.

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


	boolean Point2Score()		// ������ ����Ʈ�� ���ھ�� ���ݾ� �ٲپ� �ݴϴ�.
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
//						������� ������ ���� �޼���




/*------------------------------------------------------------------------------
	Time ����
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
//						������� �ð�ó���� ���� �޼���




/*------------------------------------------------------------------------------
	Help �� Item ������ ���� �޼���
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
//					������� ������ �������� ���� �޼���



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
			// ���ӿ���
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
				//�ð� ���� �׷��� ó��
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
