//работает
//23-oct-2017

// сигнал cos (x) + i*sin (x) ; Re + i * Im ; I - real, Q - imaginary
//
// DQPSK map
// 00 -          0 deg;		cos   0 =  1; sin   0 =  0
// 01 - pi/2  = 90 deg;		cos  90 =  0; sin  90 =  1
// 10 - 3pi/2 = 270 deg;	cos 270 =  0; sin 270 = -1
// 11 - pi    = 180 deg;	cos 180 = -1; sin 180 = 0

//D8PSK map
// 000 -          0 deg;	cos   0 =  1    ; sin   0 = 0
// 001 - pi/4  = 45 deg;	cos  45 =  0.707; sin  45 = 0.707
// 010 - 3pi/4 = 135 deg;	cos 135 = -0.707; sin 135 = 0.707
// 011 - 2pi/4 = 90 deg;	cos  90 =  0    ; sin  90 =  1
// 100 - 7pi/4 = 315 deg;	cos 315 =  0.707; sin 315 = -0.707
// 101 - 6pi/4 = 270 deg;	cos 270 =  0    ; sin 270 = -1
// 110 - 4pi/4 = 180 deg;	cos 180 = -1    ; sin 180 = 0
// 111 - 5pi/4 = 225 deg;	cos 225 = -0.707; sin 225 = -0.707
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

void print_array_D8PSK (void);	//вспомогательные процедуры
void print_array_DQPSK (void);
void generate_array (void);
void print_array (int, float array[]);
void print_conj_array (int, float array[]);

#define carrier_first 23	//номер самой первой поднесущей в таблице FFT
#define	carriers 108		//количество поднесущих, умноженное на три. 1-2-3 бита в каждой, итого 36 * 3 = 108 бит максимум
#define fftsize 256

int			array_bit [carriers*3];		//здесь будут случайно сгенерированные значения поднесущих - до трёх бит на одну
float		array_mapped[carriers*2];	//мапим поднесущие в этот массив, получаем комплексные числа, Re+Im части
//int		array_mapped_conjugate[18]	//сюда пишем комплексно-сопряженные числа

int     i, j, k;
float	x,y;
char    z;
int		current_phase = 0;

int     map_dqpsk[4]={0, 90, 270, 180};							//значения разности фаз при DQPSK
int     map_d8psk[8]={0, 45 ,135, 90, 315, 270, 180, 225};		//значения разности фаз при D8PSK

enum	{dbpsk=0, dqpsk=1, d8psk=2, nomodtype=255};
int		modulation_type=nomodtype;

FILE	*myfile;


int main(int argc, char *argv[])
{
//определяем значения аргументов при вызове
	//printf("Arguments:%d\n", argc);
	if (argc==3) {
	if (!strcmp(argv[1],"dbpsk")) modulation_type=dbpsk;
	if (!strcmp(argv[1],"dqpsk")) modulation_type=dqpsk;
	if (!strcmp(argv[1],"d8psk")) modulation_type=d8psk;
	
	//printf("file for output:%s\n",argv[2]);
	
	myfile = fopen (argv[2], "w");
	//fprintf(myfile, "test");
	//fclose(myfile);
	
	}




if (modulation_type == 255) {
	printf("This program generates random bits, maps it with selected modulation type\nand print complex numbers\n\n");
	printf("usage: cplxgen <modulationtype> <output file>\n");
	printf("where <modulation type> is one from:\n");
	printf("d8psk - mapping with d8psk\n");
	printf("dqpsk - mapping with dqpsk\n");
	printf("dbpsk - mapping with dbpsk\n");
	exit (1);
}

//генерируем массив случайных бит
generate_array ();


printf("output format zXX := Re + i*Im\n");


//берём два бита и "мапим" поднесущие DQPSK - это не mapping как таковой в OFDM, это случайная генерация данных
//печатаем массив и проверяем суммы по 2 бита
//print_array_DQPSK();

k=carrier_first; //начинаем с поднесущей номер k. 
printf("carriers=%d\n",carriers/3);

//printf("modulation type=%d\n",modulation_type);


if (modulation_type == dbpsk) {
		
for (i=0; i< carriers/3*2; k++) {
	if (array_bit[i++] == 1) current_phase = current_phase + 180;
	
	if (current_phase >359) current_phase = current_phase - 360;

	switch (current_phase) {
		case   0: x=1; y=0; break;
		case 180: x=-1; y=0; break;
		default: x=-5; y=-5;
	}
	i++;
	array_mapped[i]=x; array_mapped[i+1]=y;
}
printf("\ndbpsk mapping");
}



if (modulation_type == dqpsk) {
	
for (i=0; i< carriers/3*2; k++) {
	j = array_bit[i++]*2 + array_bit[i++];
	current_phase = current_phase + map_dqpsk[j];
	if (current_phase >359) current_phase = current_phase - 360;

	switch (current_phase) {
		case   0: x=1; y=0; break;
		case  90: x=0; y=1; break;
		case 180: x=0; y=-1; break;
		case 270: x=-1; y=0; break;
		default: x=-5; y=-5;
	}

	array_mapped[i]=x; array_mapped[i+1]=y;
}
printf("\ndqpsk mapping");
}

if (modulation_type == d8psk) {

for (i=0; i< carriers/3*2; k++) {
	j = array_bit[i++]*4 + array_bit[i++]*2 + array_bit[i++];
	current_phase = current_phase + map_d8psk[j];
	if (current_phase >359) current_phase = current_phase - 360;

	switch (current_phase) {
		case   0: x=1;		y=0;	  break;
		case  45: x=0.707;	y=0.707;  break;
		case  90: x=0;		y=1;	  break;
		case 135: x=-0.707;	y=0.707;  break;
		case 180: x=0;		y=-1;	  break;
		case 225: x=-0.707;	y=-0.707; break;
		case 270: x=-1;		y=0;	  break;
		case 315: x=0.707;	y=-0.707; break;
		default: x=-5; y=-5;
	}

	array_mapped[i]=x; array_mapped[i+1]=y;
}
printf("\nd8psk mapping");

}


//"битовый поток" замаплен, печатаем результат в консоль
print_array (carrier_first, array_mapped);

//выводим на печать комплексно-сопряженные числа
//printf("\nconjugated complex numbers:\n");
print_conj_array (carrier_first, array_mapped);



printf(" done\n");
fclose(myfile);
return 0;
}

//
//
//подпрограммы

//генерируем случайный набор бит - из них будут формироваться значения поднесущих
void generate_array (void)
{
srand(time(0));
for (i=0; i<carriers*3; i++)
	array_bit[i]=(rand()>16354)?0:1;

//печатаем их
//printf("\n108 random generated bits\n");
//for (i=0; i<carriers; i++) printf("%d ", array_bit[i]);
}


void print_array_DQPSK (void)
{
	int i;
//печатаем массив и проверяем суммы по 2 бита
for (i=0; i<35; i++) {
//    printf("%d ",i);
	k = array_bit[i++]*2 + array_bit[i];
    printf("%d   ", k);
}
    printf("\n\n");
return;
}


void print_array_D8PSK (void)
{
	int i;
//печатаем массив и проверяем суммы по 3 бита
for (i=0; i<carriers/3; i++) {
//    printf("%d ",i);
	k = array_bit[i++]*4 + array_bit[i++]*2 + array_bit[i];
    printf("%d     ", k);
}
    printf("\n\n");
    return;
}

//печать массива комплексных чисел
void print_array (int j, float array[])
{
	int counter = 0;
	for (i=0; i< carriers/3*2; ) {

	x=array[i++];
	fprintf(myfile, "z %d := %6.3f", counter+j, x);
	counter++;

	y=array[i++];
	if (y<0) fprintf(myfile, " -");
	else fprintf(myfile, " +");

	fprintf(myfile," i * %.3f\n", fabs(y)); //i * %d

}
}


//создаём комплексно-сопряжённые числа к заданному массиву комплексных чисел и печатаем их
void print_conj_array (int j, float array[])
{
	int k, counter = 0;
	counter = fftsize-carriers/3-j+1;

for (i = 0; i < carriers/3*2; ) {

	x=array[carriers*2/3 - i-2];
	fprintf(myfile, "z %d := %6.3f", counter, x);
	counter++;
	y=array[carriers*2/3 - i-1];
	if (y<0) fprintf(myfile, " +");
	else fprintf(myfile, " -");

	fprintf(myfile, " i * %.3f\n", fabs(y)); //i * %d
	i=i+2;
}
}

