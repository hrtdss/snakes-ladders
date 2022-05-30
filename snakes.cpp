#include <bits/stdc++.h>
#include "graphics.h"

const int WIDTH = 1024, HEIGHT = 768; // Высота и ширина окна
const int CELLSIZE = 70; // Сторона квадратов, составляющих игровое поле

IMAGE *menuscreen, *gamewindow, *rulesscreen, *aboutscreen;
IMAGE *dice[6], *grass, *winner[2], *ld[3], *sn[3];
IMAGE *cells[101]; // Массив изображений(ячеек поля), необходимый для перекрытия поврежденных во время игры полей

struct snl
{
   int playerI, playerJ; // Переменные, хранящие текущее положение игрока на поле
   int prevI, prevJ; // Переменные, хранящие предыдущее положение игрока на поле
   int arr[10][10]; // Матирица имитирующая игровое поле
} players[2];

struct buttons
{
   int left, top, w, h = 20; // Координаты левого верхнего угла кнопки, ее высота и ширина
} butt[7];

int menustate; // В каком пункте меню находится пользователь
int playerNum; // Номер игрока, который в данный момент делает ход
char buff[110]; // Массив-буфер, используемый для вывода чисел(номеров клеток) на экран
char buffer[100]; // Массив-буфер для составления названия файла
int topleftcornerX = 152, topleftcornerY = 34; // Координаты левого верхнего угла игрового поля
bool thereisahundred; // Указатель на наличие победителя
int snakes [3][4] = {{5, 4, 7, 9}, {2, 2, 5, 1}, {0, 3, 4, 4}}; // Позиция на поле головы и хвоста для каждой змеи
int ladders [3][4] = {{9, 4, 6, 5}, {5, 0, 3, 0}, {5, 9, 2, 8}}; // Позиция на поле начала и конца каждой лестницы
int picnums[34]; // Массив номеров изображений

void loadfiles(); // Загрузка необходимых изображений и значений
void initbuttons(); 
int menuButtons(int currentStatus); // Выбор пункта меню
void rules(); // Переключение на окно с правилами игры
void about(); // Переключение на окно с информацией о игре
void magnetacolor(); // Сделать пурпурный цвет основным
void lgreencolor(); // Сделать зеленый цвет основным
void lorangecolor(); // Сделать светло-оранжевый цвет основным
void lyellowcolor(); // Сделать светло-желтый основным
void createlevel(); // Построение игрового поля
void initgame(); // Инициализация игры
int rollthedice(); // Симуляция  броска игральной кости, для определения количества шагов фишкой
void drawplayer(int num, int currx, int curry); // Отрисовка на поле фишки соответствующего игрока
void removeplayer(int num, int lastx, int lasty, int leftcond, int rightcond); // Удаление с поля (перекрытие цветом клетки) фишки соответствующего игрока
void laststeps(int num); // Обработка последней строки в случаях, когда кол-во очков больше 100
void someonewon(int num); // Действие в случае победы одного из игроков
void showactiveplayer(int f, int s); // Сигнал, указывающий какой игрок делает текущий ход
void startgame(); // Запуск процесса игры

int main()
{
   srand(time(NULL));
   initwindow(WIDTH, HEIGHT);
   loadfiles();
   initbuttons();
   
   do 
   {
      menustate = menuButtons(menustate);

      switch (menustate) 
      {
         case 1: 
         {
            initgame();
            createlevel();
            startgame();
            break;
         }
         case 2: 
         {
            rules();
            break;
         }
         case 3: 
         {
            about();
            break;
         }
      }
   } while (menustate != 4);
   
   closegraph();
}

void loadfiles() 
{
   FILE *fin = fopen("data.txt", "r");
   
   for (int i = 0; i < 34; i++)
      fscanf(fin, "%d", &picnums[i]);
   
   fclose(fin);
   
   menuscreen = loadBMP("bmpImages/MainMenu.bmp");
   gamewindow = loadBMP("bmpImages/GameWindow.bmp");
   rulesscreen = loadBMP("bmpImages/Rules.bmp");
   aboutscreen = loadBMP("bmpImages/AboutTheProgram.bmp");
   ld[0] = loadBMP("bmpImages/ladder1.bmp");
   ld[1] = loadBMP("bmpImages/ladder2.bmp");
   ld[2] = loadBMP("bmpImages/ladder3.bmp");
   sn[0] = loadBMP("bmpImages/snake1.bmp");
   sn[1] = loadBMP("bmpImages/snake2.bmp");
   sn[2] = loadBMP("bmpImages/snake3.bmp");
   winner[0] = loadBMP("bmpImages/1stWon.bmp");
   winner[1] = loadBMP("bmpImages/2ndWon.bmp");
   grass = loadBMP("bmpImages/grass.bmp");
   
   for (int i = 0; i < 34; i++)
   {
      // Составление названия файла
      snprintf(buffer, 100, "bmpImages/");
      snprintf(buffer + 10, 100, "%c", ((picnums[i] + 1) / 10) + '0');
      snprintf(buffer + 11, 100, "%c", ((picnums[i] + 1) % 10) + '0');
      snprintf(buffer + 12, 100, ".bmp");
      
      // Загрузка файла с полученным названием
      if (i >= 0 && i <= 5)
         dice[picnums[i]] = loadBMP(buffer);
      else
         cells[picnums[i]] = loadBMP(buffer);
   }
}

void initbuttons()
{
   // new game
   butt[0].left = 393;
   butt[0].top = 334;
   butt[0].w = 208;
   // about
   butt[1].left = 368;
   butt[1].top = 380;
   butt[1].w = 260;
   // rules
   butt[2].left = 378;
   butt[2].top = 425;
   butt[2].w = 236;
   // exit
   butt[3].left = 357;
   butt[3].top = 472;
   butt[3].w = 277;
   // oth exit
   butt[4].left = 880;
   butt[4].top = 718;
   butt[4].w = 103;
   butt[4].h += 2;
   // oth exit
   butt[5].left = 907;
   butt[5].top = 25;
   butt[5].w = 85;
   butt[5].h += 64;
   // dice
   butt[6].left = 894;
   butt[6].top = 616;
   butt[6].w = 117;
   butt[6].h += 97;
}

int menuButtons(int currentStatus)
{
   putimage(0, 0, menuscreen, COPY_PUT);

   while (1)
   {
      int x, y;
      
      do
      {
         x = mousex();
         y = mousey();
      } while (mousebuttons() != 1); 
      delay(150);
      
      for (int i = 0; i < 4; i++)
         if (x >= butt[i].left && y >= butt[i].top && x <= butt[i].left + butt[i].w && y <= butt[i].top + butt[i].h) // Определении кнопки меню по положению курсора
            return i + 1;
   }
}

void rules()
{
   putimage(0, 0, rulesscreen, COPY_PUT);

   while (1)
   {
      int x, y;
      
      do
      {
         x = mousex();
         y = mousey();
      } while (mousebuttons() != 1); 
      delay(150);

      if (x >= butt[4].left && y >= butt[4].top && x <= butt[4].left + butt[4].w && y <= butt[4].top + butt[4].h) // Кнопка возврата в меню
         return;
   }
}

void about()
{
   putimage(0, 0, aboutscreen, COPY_PUT);

   while (1) 
   {
      int x, y;
      
      do
      {
         x = mousex();
         y = mousey();
      } while (mousebuttons() != 1); 
      delay(150);

      if (x >= butt[4].left && y >= butt[4].top && x <= butt[4].left + butt[4].w && y <= butt[4].top + butt[4].h) // Кнопка возврата в меню
         return;
   }
}

void magnetacolor()
{
   setcolor(COLOR(216, 35, 255));
   setfillstyle(1, COLOR(216, 35, 255));
}
void lgreencolor()
{
   setcolor(COLOR(89, 225, 49));
   setfillstyle(1, COLOR(89, 225, 49));
}
void lorangecolor()
{
   setcolor(COLOR(255, 240, 130));
   setfillstyle(1, COLOR(255, 240, 130));
}
void lyellowcolor()
{
   setcolor(COLOR(254, 162, 1));
   setfillstyle(1, COLOR(254, 162, 1));
}

void createlevel() 
{
   int x, y, counter = 100, hh;
   bool fl = 0;
   
   for (int i = 0; i < 10; i++)
      for (int j = 0; j < 10; j++)
      {
         y = topleftcornerY + i * CELLSIZE; x = topleftcornerX + j * CELLSIZE;
         
         if ((i + j) % 2 != 0) // Раскраска ячеек в шахматном порядке
            lorangecolor();
         else 
            lyellowcolor();
         
         bar(x, y, x + CELLSIZE, y + CELLSIZE);
         
         setcolor(BLACK);
         settextstyle(8, 0, 2);
         
         if (i % 2 == 0) // Нумерование ячеек
         {
            fl = 0;
            
            if (j % 2 == 0)
               setbkcolor(COLOR(254, 162, 1));
            else
               setbkcolor(COLOR(255, 240, 130));
         
            int h = snprintf(buff, 110, "%d", counter);
            if (counter == 75 || counter == 51 || counter == 15)
               outtextxy(x + 40, y + 5, buff);
            else
               outtextxy(x + 5, y + 5, buff);
            
            counter--;
         }
         else
         {
            if (!fl)
               hh = counter - 9, fl = 1;
            
            if (j % 2 == 0)
               setbkcolor(COLOR(255, 240, 130));
            else
               setbkcolor(COLOR(254, 162, 1));

            int h = snprintf(buff, 110, "%d", hh);
            if (hh == 70 || hh == 26)
               outtextxy(x + 40, y + 5, buff);
            else
               outtextxy(x + 5, y + 5, buff);
            
            counter--; 
            hh++;
         }
      }
   
   putimage(461, 416, sn[0], COPY_PUT);
   putimage(252, 200, sn[1], COPY_PUT);
   putimage(367, 68, sn[2], COPY_PUT);
   putimage(461, 587, ld[0], COPY_PUT);
   putimage(186, 292, ld[1], COPY_PUT);
   putimage(742, 209, ld[2], COPY_PUT);
   
   lgreencolor();
   fillellipse(65, 340, 20, 20);
   magnetacolor();
   fillellipse(65, 426, 20, 20);
} 

void initgame()
{
   playerNum = -1;
   thereisahundred = 0;
   
   for (int i = 0; i < 2; i++)
   {
      players[i].playerI = 9;
      players[i].playerJ = -1;
      players[i].prevI = 0;
      players[i].prevJ = 0;
      
      for (int j = 0; j < 10; j++)
         for (int k = 0; k < 10; k++)
            players[i].arr[j][k] = 0;
   }
   
   putimage(0, 0, gamewindow, COPY_PUT);
}


int rollthedice()
{
   int dicerand = 1 + rand() % 6;
   
   putimage(924, 208, dice[dicerand - 1], COPY_PUT);
   delay(1000);
   putimage(924, 208, grass, COPY_PUT);
   
   return dicerand;
}

void removeplayer(int num, int lastx, int lasty, int leftcond, int rightcond) 
{
   int x, y, pos;
   y = topleftcornerY + lasty * CELLSIZE + 50;
   
   if ((lasty + lastx) % 2 != 0) // Определение цвета прошлой клетки
      lorangecolor();
   else 
      lyellowcolor();
   
   if (leftcond >= rightcond) 
   {
      if (num == 0) // По номеру игрока определяются координаты фишки, которая будет перекрываться цветом клетки
         x = topleftcornerX + lastx * CELLSIZE + 19;
      else
         x = topleftcornerX + lastx * CELLSIZE + 51;
      
      fillellipse(x, y, 15, 15);
   }
   
   if (lasty % 2 != 0) // Вычисление номера клетки на поле на основании предыдущего положения игрока
      pos = 10 * (9 - lasty)  + lastx + 1;
   else 
      pos = 10 * (9 - lasty)  + (10 - lastx);
   
   if (cells[pos - 1] != NULL) // Проверка на наличие поврежденных картинок(часть картинки была перекрыта фишкой игрока, в следствии чего потеряла свою целостность)
   {
      if (lasty == 6 && lastx == 10 || lastx == -1 && lasty == 3|| lastx == 10 && lasty == 2)
         return;
      else
         putimage(topleftcornerX + lastx * CELLSIZE, topleftcornerY + lasty * CELLSIZE, cells[pos - 1], COPY_PUT);
   
      for (int i = 0; i < 2; i++)
         if (players[i].arr[lasty][lastx])
            drawplayer(i, lastx, lasty);
   }
}

void drawplayer(int num, int currx, int curry)
{
   int x, y;
   
   if (num == 0) // Определение цвета фишки текущего игрока
   {
      lgreencolor();
      x = topleftcornerX + currx * CELLSIZE + 19;
   }
   else
   {
      magnetacolor();
      x = topleftcornerX + currx * CELLSIZE + 51;
   }
   
   y = topleftcornerY + curry * CELLSIZE + 50;
   
   fillellipse(x, y, 15, 15);
   delay(300);
}

void playermoves(int num)
{
   bool flag = 0; // Флаг, указывающий на наличие смены строки
   int h;
   
   if (players[num].prevJ == -1)
      players[num].prevJ = 0;

   if (players[num].prevI % 2 != 0) // Определение направления нумерации ячеек строки
   {
      if (players[num].prevI == players[num].playerI)
         h = players[num].playerJ;
      else 
         h = 9, flag = 1;
   
      for (int j = players[num].prevJ; j <= h; j++) // Обходим всю строку до конца (слева направо)
      {
         removeplayer(num, j - 1, players[num].prevI, j - 1, 0);
         drawplayer(num, j, players[num].prevI);
      }
      
      if (flag) // Если есть переход на другую строку - идем с правого края игрового поля до нужной клетки
      {
         removeplayer(num, 9, players[num].prevI, 0, 0);
         
         for (int j = h; j >= players[num].playerJ; j--)
         {            
            removeplayer(num, j + 1, players[num].playerI, 9, j + 1);
            drawplayer(num, j, players[num].playerI);
         }
      }
   }
   else
   {
      if (players[num].prevI == players[num].playerI)
         h = players[num].playerJ;
      else 
         h = 0, flag = 1;
      
      for (int j = players[num].prevJ; j > h; j--) // Обходим всю строку до конца (справа налево)
      {
         removeplayer(num, j, players[num].prevI, 0, 0);
         drawplayer(num, j - 1, players[num].prevI);
      }
      
      if (flag)
      {
         removeplayer(num, 0, players[num].prevI, 0, 0);
         
         for (int j = h; j <= players[num].playerJ; j++) // Если есть переход на другую строку - идем с левого края игрового поля до нужной клетки
         {
            removeplayer(num, j - 1, players[num].playerI, j - 1, 0);
            drawplayer(num, j, players[num].playerI);
         }
      }
   }
}

void someonewon(int num)
{
   delay(1000);
   putimage(0, 0, winner[num], COPY_PUT);
   delay(500);
   getch();
   thereisahundred = 1;
}

void showactiveplayer(int f, int s)
{
   setlinestyle(0, 0, 3);
   setcolor(COLOR(244, 152, 58));
   circle(65, f, 22);
   setcolor(COLOR(228, 227, 235));
   circle(65, s, 22);
}

void laststeps(int num)
{
   for (int j = players[num].prevJ; j > 0; j--)
   {
      removeplayer(num, j, players[num].prevI, 0, 0);
      drawplayer(num, j - 1, players[num].prevI);
   }

   for (int j = 0; j <= players[num].playerJ; j++)
   {
      removeplayer(num, j - 1, players[num].playerI, j - 1, 0);
      drawplayer(num, j, players[num].playerI);
   }
}

void startgame()
{
   int x, y, a, b, throwres = 0;
   bool islast = 0;
   
   while (!thereisahundred)
   {
      do 
      {
         x = mousex();
         y = mousey();
      } while (mousebuttons() != 1); 
      delay(150);
      
      if (x >= butt[5].left && y >= butt[5].top && x <= butt[5].left + butt[5].w && y <= butt[5].top + butt[5].h) // Кнопка выхода из игры(возврат в главное меню)
         return;
      
      if (x >= butt[6].left && y >= butt[6].top && x <= butt[6].left + butt[6].w && y <= butt[6].top + butt[6].h) // dice
      {
         if (playerNum == 0)
         {
            playerNum = 1;
            showactiveplayer(340, 426);
         }
         else
         {
            playerNum = 0;
            showactiveplayer(426, 340);
         }
         
         throwres = rollthedice();
         
         players[playerNum].prevI = players[playerNum].playerI, players[playerNum].prevJ = players[playerNum].playerJ;
         players[playerNum].arr[players[playerNum].prevI][players[playerNum].prevJ] = 0;
         
         if (players[playerNum].playerI % 2 != 0)
         {
            if (players[playerNum].playerJ + throwres <= 9)
               players[playerNum].playerJ += throwres;
            else
            {
               players[playerNum].playerI--;
               players[playerNum].playerJ = 19 - throwres - players[playerNum].playerJ;
            }
         }
         else if (players[playerNum].playerI % 2 == 0)
         {
            if (players[playerNum].playerI == 0 && players[playerNum].playerJ - throwres <= 0)
            {
               islast = 1;
               players[playerNum].playerJ = throwres - players[playerNum].playerJ;
               
               laststeps(playerNum);
            }
            else if (players[playerNum].playerJ - throwres >= 0)
               players[playerNum].playerJ -= throwres;
            else
            {
               players[playerNum].playerI--;
               players[playerNum].playerJ = throwres - players[playerNum].playerJ - 1;
            }
         }
         
         a = players[playerNum].playerI, b = players[playerNum].playerJ;
         players[playerNum].arr[a][b] = 1;
         
         if (!islast)
            playermoves(playerNum);
         
         for (int i = 0; i < 3; i++)
         {
            if (snakes[i][0] == a && snakes[i][1] == b)
            {
               if (a == 0 && b == 3)
                  islast = 0;
               
               players[playerNum].playerI = snakes[i][2], players[playerNum].playerJ = snakes[i][3];
               players[playerNum].arr[players[playerNum].playerI][players[playerNum].playerJ] = 1;
               players[playerNum].prevI = a, players[playerNum].prevJ = b;
               players[playerNum].arr[a][b] = 0;
               removeplayer(playerNum, players[playerNum].prevJ, players[playerNum].prevI, 0, 0);
               drawplayer(playerNum, players[playerNum].playerJ, players[playerNum].playerI);
            }
            
            if (ladders[i][0] == a && ladders[i][1] == b)
            {
               players[playerNum].playerI = ladders[i][2], players[playerNum].playerJ = ladders[i][3]; 
               players[playerNum].arr[players[playerNum].playerI][players[playerNum].playerJ] = 1;
               players[playerNum].prevI = a, players[playerNum].prevJ = b;
               players[playerNum].arr[a][b] = 0;
               removeplayer(playerNum, players[playerNum].prevJ, players[playerNum].prevI, 0, 0);
               drawplayer(playerNum, players[playerNum].playerJ, players[playerNum].playerI);
            }
         }
         
         if (a == 0 && b == 0)
            someonewon(playerNum);
      }
   }
}
