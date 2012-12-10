/* Jose Julian Reyes Hernandez */
/* University of Las Palmas de Gran Canaria
/* Electronic and Telecommunication Engineering School (EITE) at ULPGC. */


#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h>
#include <sys/un.h>
#include <arpa/inet.h>
#define LGREP 256 
#define PORT 8020
//VARIABLES GLOBALES
//definimos la estructura de la PDU
typedef struct PDU{
int tipo;
int actividad;
int tamano;
char datos[9];//lo ponemos de tamaño 9 para ponerle al final /0 y así no meta basura
short chk;//usamos short para que acepte numeros negativos.
}PDU;

PDU pdudatos[33];//vector donde vamos a guardar las pdu
char mensaje[255];//mensaje a leer por el teclado
char operacion[4];//operacion a realizar 
PDU pdutemporal,pdurespuesta;
int sock,uid,gid; 
char p[LGREP]; 
char cadena[256]; 
int n; 
int chktemp;
int num_actividad=99;
char sms[6];
int error=1;//Inicializamos a 0 suponiendo que no hay error
int lg; 
struct sockaddr_in dir_remota; 
struct hostent *hp; 
int len_dir_remota; 
int i;
int q=0;
int entro=0;
int m=0;
int o=0;
char respuesta[256],temp[8];
int lastcon,res,cont;
int opcion;
char op[1];

//-------------------------------------------------------funciones a realizar-----------------------------------------------------------
void calculoCHK(){//funcion para calcular el CRC de cada PDU
//para el calculo del CHK sumamos el TIPO+ACT+tamaño+datos y luego
//le aplicamos el complemento a 1
	int j=-1;
	int x=0;
	if(opcion==3)
	{
		do{
			j++;
			int i,total,chk;
			total=0;
			total=pdudatos[j].tipo;
			total=total+pdudatos[j].actividad;
			total=total+pdudatos[j].tamano;
			i=0;
			while(i<9){
				total=total+pdudatos[j].datos[i];
				i++;
			}
			chk=~total;
			pdudatos[j].chk=chk;
		}while(pdudatos[j].tipo!=2);
		if(pdudatos[j+1].tipo==1)
		{
			do{
				j++;
				int i,total,chk;
				total=0;
				total=pdudatos[j].tipo;
				total=total+pdudatos[j].actividad;
				total=total+pdudatos[j].tamano;
				i=0;
				while(i<9){
					total=total+pdudatos[j].datos[i];
					i++;
				}
				chk=~total;
				pdudatos[j].chk=chk;
			}while(pdudatos[j].tipo!=2);
			printf("\E[0;1mEl CRC se ha calculado satisfactoriamente\E[00m\n");
		}
	}else if(opcion==4)
	{
		do{
			j++;
			int i,total,chk;
			total=0;
			total=pdudatos[j].tipo;
			total=total+pdudatos[j].actividad;
			total=total+pdudatos[j].tamano;
			i=0;
			while(i<9){
				total=total+pdudatos[j].datos[i];
				i++;
			}
			chk=~total;
			pdudatos[j].chk=chk;
		}while(pdudatos[j].tipo!=4);
		printf("\E[0;1mEl CRC se ha calculado satisfactoriamente\E[00m\n");
	}
}
void ComprobarCHK(){//FUNCION CALCULA EL CHK DE LO QUE RECIBIMOS PARA LUEGO COMPARAR

	int i,total,chk;
	total=0;
	total=pdutemporal.tipo;
	total=total+pdutemporal.actividad;
	total=total+pdutemporal.tamano;
	i=0;
	while (i<8){
	
		total=total+pdutemporal.datos[i];
		i++;
	}
	chk=~total;  /*realizamos el complemento a 1*/
	chktemp=chk;
}
void insertarFinMensaje(){
//función que insertar PDU tipo 2 para cerrar la cadena de texto.
	int lon=strlen(mensaje);
	int posi=lon/8;
	int resto=lon%8;
	int opc=2;
	
	if(lon<8){
		opc=1;
	}
	switch(opc){
	
		case 1:
			strcpy(pdudatos[posi+1].datos,"        ");
			pdudatos[posi+1].tipo=2;
			pdudatos[posi+1].tamano=0;
		break;
		case 2:
			pdudatos[posi].tamano=resto;
			if(resto==0){//miramos si tenemos que poner otra PDU en funcion de cuanto sea el resto
				strcpy(pdudatos[posi].datos,"        ");
				pdudatos[posi].tipo=2;
				pdudatos[posi].tamano=0;
			}else{
				strcpy(pdudatos[posi+1].datos,"        ");
				pdudatos[posi+1].tipo=2;
				pdudatos[posi+1].tamano=0;
			}
		break;
		default:
			printf("Fallo al montar el vector\n");
		break;
	}
}
void insertarCabecera(){
//Insertamos la PDU tipo 1 al principio de la trama.
	int i;
        for(i=0; i<8; i++)
        {
		pdudatos[0].datos[i]=' ';
	}
        pdudatos[0].tipo=1;
	pdudatos[0].tamano=0;
	pdudatos[0].actividad=num_actividad;

}
void limpiarVector(){
//recorremos todo el vector de PDU y lo ponemos todo a 0
        int x=0;
	int i=0;
	int j=0;
	int l=0;
	for (x=0; x<32; x++){
		for(i=0; i<8; i++){
			pdudatos[x].datos[i]=' ';
		}
		//este punto es muy importante porque si no le ponemos el /0 nos mete basura
		pdudatos[x].datos[8]='\0';
		pdudatos[x].tipo=0;
		pdudatos[x].tamano=0;
		pdudatos[x].actividad=0;
		pdudatos[x].chk=0;
	}

}
void montarVector(){//Separamos el mensaje en trozos de 8 para luego introducir en el campo datos de cada PDU en un vector.
	int x=0;
	int i=0;
	int j=0;
	int l=0;
	for (x=0; x<32; x++){
		for(i=0; i<8; i++){
			pdudatos[x].datos[i]=' ';
		}
		pdudatos[x].tipo=0;
		pdudatos[x].tamano=0;
		pdudatos[x].actividad=0;
		pdudatos[x].chk=0;
	}
	for(x=0; x<32; x++)
	{
		for(i=0; i<8; i++){
			pdudatos[x].datos[i]=mensaje[j];
			j++;	
		}
		pdudatos[x].tipo=91;
		if(strlen(mensaje)<8)
		{
			pdudatos[x].tamano=strlen(mensaje);	
		}else{
			pdudatos[x].tamano=8;
		}
		pdudatos[x].actividad=num_actividad;
	}
	insertarFinMensaje();
	for(x=31;x>=0;x--)
	{
		pdudatos[x+1]=pdudatos[x];
	}
	insertarCabecera();
	x=-1;
	printf("\n");
	do{
		x++;
		printf("\E[1;30mPDU Número %d\n############################\E[00m\n",x);
		printf("\E[1;32mDatos: %s\E[00m\n",pdudatos[x].datos);
		printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdudatos[x].tipo);
		printf("\E[1;32mTamaño: %d\E[00m\n",pdudatos[x].tamano);
		printf("\E[1;32mNumero de Actividad: %d\E[00m\n",pdudatos[x].actividad);
		printf("\E[1;32mCRC: %d\E[00m\n",pdudatos[x].chk);
		printf("\E[1;30m############################\E[00m\n");
	}while(pdudatos[x].tipo!=2);
	printf("\E[0;1mVector montado\E[00m\n");
}
//===================================================================================================================================================
void montarVector2(){//Separamos el mensaje en trozos de 8 para luego introducir en el campo datos de cada PDU en un vector.
//lo llamamos desde insertar operación y te permite insertarla aunque no haya texto.
	int x=0;
	int i=0;
	int j=0;
        int q=0;
        int p=0;
       	while(pdudatos[q].tipo!=2 && q<33)
        {
       		q++;
       	}
       	if(pdudatos[q].tipo==2)
       	{
  		entro=0;
  	}else{
  		q=-1;
  		entro=2;
  	}
  	i=0;
  	strcpy(pdudatos[q+1].datos,"        ");
  	pdudatos[q+1].tipo=1;
	pdudatos[q+1].tamano=0;
	pdudatos[q+1].actividad=num_actividad;
	for(i=0; i<5; i++){
		pdudatos[q+2].datos[i]=operacion[j];
		j++;	
	}
	pdudatos[q+2].tipo=92;
	pdudatos[q+2].tamano=3;
	pdudatos[q+2].actividad=num_actividad;
	strcpy(pdudatos[q+3].datos,"        ");
  	pdudatos[q+3].tipo=2;
	pdudatos[q+3].tamano=0;
	pdudatos[q+3].actividad=num_actividad;
	x=-1;
	printf("\n");
	do{
		x++;
		printf("\E[1;30mPDU Número %d\n############################\E[00m\n",x);
		printf("\E[1;32mDatos: %s\E[00m\n",pdudatos[x].datos);
		printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdudatos[x].tipo);
		printf("\E[1;32mTamaño: %d\E[00m\n",pdudatos[x].tamano);
		printf("\E[1;32mNumero de Actividad: %d\E[00m\n",pdudatos[x].actividad);
		printf("\E[1;32mCRC: %d\E[00m\n",pdudatos[x].chk);
		printf("\E[1;30m############################\E[00m\n");
	}while(pdudatos[x].tipo!=2);
	if(q>0)
	{
		do{
			x++;
			printf("\E[1;30mPDU Número %d\n############################\E[00m\n",x);
			printf("\E[1;32mDatos: %s\E[00m\n",pdudatos[x].datos);
			printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdudatos[x].tipo);
			printf("\E[1;32mTamaño: %d\E[00m\n",pdudatos[x].tamano);
			printf("\E[1;32mNumero de Actividad: %d\E[00m\n",pdudatos[x].actividad);
			printf("\E[1;32mCRC: %d\E[00m\n",pdudatos[x].chk);
			printf("\E[1;30m############################\E[00m\n");
		}while(pdudatos[x].tipo!=2);
	}
	printf("\E[0;1mVector montado\E[00m\n");
}
void Recibir()
{
	int pos=0;
	char signo[1];
	char resultado[2];
	char punto[1];
	char decimal[1];
	char total[5];
	punto[0]='.';
	
	printf("\n");
	printf("\E[1;35mEsperando datos entrantes.....\E[00m\n"); 
	printf("\n");
	lg=sizeof(dir_remota); 
	bzero((char *)&cadena,strlen(cadena)); 
	n=recvfrom(sock,&pdutemporal,sizeof(pdutemporal),0,(struct sockaddr *)&dir_remota,&lg); /* tratar peticion */ 
	if(n == -1) 
	{ 
		fprintf(stderr,"Problemas con recvfrom\n"); 
		perror("error"); 
		exit(-1); 
	} 
	if(n>0) 
	{ 
		strncpy(temp,pdutemporal.datos,8);
		printf("\E[1;30m############################\E[00m\n");
		printf("\E[1;32mDatos : %s\E[00m\n",temp);
		printf("\E[1;32mTipo : %d\E[00m\n",pdutemporal.tipo);
		printf("\E[1;32mTamaño : %d\E[00m\n",pdutemporal.tamano);
		printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
		printf("\E[1;32mCHK : %d\E[00m\n",pdutemporal.chk);
		printf("\E[1;30m############################\E[00m\n");
		ComprobarCHK();
		if(pdutemporal.chk!=chktemp)
		{
			printf("\n");
			printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
			error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
		}
		if(pdutemporal.tipo==93)
		{
			for(i=0;i<8;i++)
			{
				sms[q]=temp[i];
				q++;
			}
		}
		if(sms[0]=='-')
		{
			signo[0]='-';
		}else if(sms[0]=='+')
		{
			signo[0]='+';
		}
		if(sms[1]=='1')
		{
			if(sms[3]=='0')//no hay decimales
			{
				resultado[0]=sms[2];
			}else if(sms[3]=='1')//hay decimales
			{
				resultado[0]=sms[2];
				decimal[0]=sms[4];
			}
		}else if(sms[1]=='2')
		{
			if(sms[4]=='0')//no hay decimales
			{
				resultado[0]=sms[2];
				resultado[1]=sms[3];
			}else if(sms[4]=='1')//hay decimales
			{
				resultado[0]=sms[2];
				resultado[1]=sms[3];
				decimal[0]=sms[5];
			}
		}
		total[0]=signo[0];
		if(sms[1]=='1')
		{
			total[1]=resultado[0];
			total[2]=punto[0];
			total[3]=decimal[0];
		}else if(sms[1]=='2')
		{
			for(i=0;i<2;i++)
			{
				total[i+1]=resultado[i];
			}
			total[3]=punto[0];
			total[4]=decimal[0];
		}
		printf("\n");
		printf("\E[1;31mEl Resultado de la Operacion solicitada es: \E[00m%s\n",total);
		printf("\n");		
	}
}
void TxSecuencial(){
  //tx secuencialmente lo que hay dentro del vector
	int pos=0;
	char signo[1];
	char resultado[2];
	char punto[1];
	char decimal[1];
	char total[5];
	punto[0]='.';
	calculoCHK();
	do{
		pdutemporal=pdudatos[pos];
		if(sendto(sock,&pdutemporal,sizeof(pdutemporal),0,(struct sockaddr *)&dir_remota,sizeof(dir_remota))<0){
	   		 printf("Error en el envío.\n");
	    	}
		strncpy(temp,pdutemporal.datos,8);
		printf("\E[1;30mPDU Enviada %d\n############################\E[00m\n",pos);
		printf("\E[1;35mPosicion en el vector: %d\E[00m\n",pos);
		printf("\E[1;32mDatos: %s\E[00m\n",pdutemporal.datos);
		printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdutemporal.tipo);
		printf("\E[1;32mTamaño: %d\E[00m\n",pdutemporal.tamano);
		printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
		printf("\E[1;32mCRC: %d\E[00m\n",pdutemporal.chk);
		printf("\E[1;30m############################\E[00m\n");
		pos++;
	}while(pdudatos[pos-1].tipo!=2);
	if(pdudatos[pos].tipo==1)
	{
		do{
			pdutemporal=pdudatos[pos];
			if(sendto(sock,&pdutemporal,sizeof(pdutemporal),0,(struct sockaddr *)&dir_remota,sizeof(dir_remota))<0)
			{
	   			 printf("Error en el envío.\n");
	    		}
			strncpy(temp,pdutemporal.datos,8);
			printf("\E[1;30mPDU Enviada %d\n############################\E[00m\n",pos);
			printf("\E[1;35mPosicion en el vector: %d\E[00m\n",pos);
			printf("\E[1;32mDatos: %s\E[00m\n",pdutemporal.datos);
			printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdutemporal.tipo);
			printf("\E[1;32mTamaño: %d\E[00m\n",pdutemporal.tamano);
			printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
			printf("\E[1;32mCRC: %d\E[00m\n",pdutemporal.chk);
			printf("\E[1;30m############################\E[00m\n");
   			pos++;
		}while(pdudatos[pos-1].tipo!=2);
		Recibir();
	}
	if(entro==2)
	{
		Recibir();
		entro=0;
	}
}
void TxPriorizando(){
	//tx priosizando operación, es decir tx la mitad del tecto luego salta a
//tx toda la operación y vuelve a tx lo que le resta del texto.	
	int fin1=0;
	int fin2=0;
	int intr=0;
	int i=0;
	int x=0;
	int pos=-1;
	
	while(pdudatos[i].tipo!=2)
	{
		i++;
	}
	fin1=i;
	i++;
	while(pdudatos[i].tipo!=2)
	{
		i++;
	}
	fin2=i;
	intr=fin1-1;
	intr=intr/2;
	intr=intr+1;
	for(i=fin2;i>=intr;i--)
	{
		pdudatos[i+1]=pdudatos[i];
	}
	pdudatos[intr].tamano=0;
	pdudatos[intr].actividad=num_actividad-1;
	pdudatos[intr].chk=0;
	pdudatos[intr].tipo=3;
	strcpy(pdudatos[intr].datos,"        ");
	pdudatos[fin2+2].tamano=0;
	pdudatos[fin2+2].actividad=num_actividad-1;
	pdudatos[fin2+2].chk=0;
	pdudatos[fin2+2].tipo=4;
	strcpy(pdudatos[fin2+2].datos,"        ");
	x=-1;
	do{
		x++;
		printf("\E[1;30mPDU Número %d\n############################\E[00m\n",x);
		printf("\E[1;32mDatos: %s\E[00m\n",pdudatos[x].datos);
		printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdudatos[x].tipo);
		printf("\E[1;32mTamaño: %d\E[00m\n",pdudatos[x].tamano);
		printf("\E[1;32mNumero de Actividad: %d\E[00m\n",pdudatos[x].actividad);
		printf("\E[1;32mCRC: %d\E[00m\n",pdudatos[x].chk);
		printf("\E[1;30m############################\E[00m\n");
	}while(pdudatos[x].tipo!=4);
	printf("\E[0;1mVector montado\E[00m\n");
	printf("\n");
	calculoCHK();
	do{
		pos++;
		pdutemporal=pdudatos[pos];
		if(sendto(sock,&pdutemporal,sizeof(pdutemporal),0,(struct sockaddr *)&dir_remota,sizeof(dir_remota))<0)
		{
	   		 printf("Error en el envío.\n");
	    	}
		printf("\E[1;30mPDU Enviada %d\n############################\E[00m\n",pos);
		printf("\E[1;35mPosicion en el vector: %d\E[00m\n",pos);
		printf("\E[1;32mDatos: %s\E[00m\n",pdutemporal.datos);
		printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdutemporal.tipo);
		printf("\E[1;32mTamaño: %d\E[00m\n",pdutemporal.tamano);
		printf("\E[1;32mNumero de Actividad: %d\E[00m\n",pdutemporal.actividad);
		printf("\E[1;32mCRC: %d\E[00m\n",pdutemporal.chk);
		printf("\E[1;30m############################\E[00m\n");
	}while(pdudatos[pos].tipo!=3);
	if(pdudatos[pos].tipo==3)
	{
		printf("\n");
		printf("\E[1;31mInterrumpida la actividad numero: %d\E[00m\n",pdudatos[pos].actividad); 
		pos++;
	}
	pos=fin1+1;
	printf("\n");
	pos++;
	do{
		pdutemporal=pdudatos[pos];
		if(sendto(sock,&pdutemporal,sizeof(pdutemporal),0,(struct sockaddr *)&dir_remota,sizeof(dir_remota))<0)
		{
	   		 printf("Error en el envío.\n");
	    	}
		printf("\E[1;30mPDU Enviada %d\n############################\E[00m\n",pos);
		printf("\E[1;35mPosicion en el vector: %d\E[00m\n",pos);
		printf("\E[1;32mDatos: %s\E[00m\n",pdutemporal.datos);
		printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdutemporal.tipo);
		printf("\E[1;32mTamaño: %d\E[00m\n",pdutemporal.tamano);
		printf("\E[1;32mNumero de Actividad: %d\E[00m\n",pdutemporal.actividad);
		printf("\E[1;32mCRC: %d\E[00m\n",pdutemporal.chk);
		printf("\E[1;30m############################\E[00m\n");
		pos++;
	}while(pdudatos[pos].tipo!=4);
	if(pdudatos[pos].tipo==4)
	{
		Recibir();
		pdutemporal=pdudatos[pos];
		if(sendto(sock,&pdutemporal,sizeof(pdutemporal),0,(struct sockaddr *)&dir_remota,sizeof(dir_remota))<0)
		{
	   		 printf("Error en el envío.\n");
	    	}
		printf("\E[1;30mPDU Enviada %d\n############################\E[00m\n",pos);
		printf("\E[1;35mPosicion en el vector: %d\E[00m\n",pos);
		printf("\E[1;32mDatos: %s\E[00m\n",pdutemporal.datos);
		printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdutemporal.tipo);
		printf("\E[1;32mTamaño: %d\E[00m\n",pdutemporal.tamano);
		printf("\E[1;32mNumero de Actividad: %d\E[00m\n",pdutemporal.actividad);
		printf("\E[1;32mCRC: %d\E[00m\n",pdutemporal.chk);
		printf("\E[1;30m############################\E[00m\n");
		printf("\n");
		printf("\E[1;31mReinicio de la actividad numero: %d\E[00m\n",pdudatos[pos].actividad); 
		pos++;
	}
	printf("\n");
	pos=intr;
	do{
		pos++;
		pdutemporal=pdudatos[pos];
		if(sendto(sock,&pdutemporal,sizeof(pdutemporal),0,(struct sockaddr *)&dir_remota,sizeof(dir_remota))<0)
		{
	   		 printf("Error en el envío.\n");
	    	}
		printf("\E[1;30mPDU Enviada %d\n############################\E[00m\n",pos);
		printf("\E[1;35mPosicion en el vector: %d\E[00m\n",pos);
		printf("\E[1;32mDatos: %s\E[00m\n",pdutemporal.datos);
		printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdutemporal.tipo);
		printf("\E[1;32mTamaño: %d\E[00m\n",pdutemporal.tamano);
		printf("\E[1;32mNumero de Actividad: %d\E[00m\n",pdutemporal.actividad);
		printf("\E[1;32mCRC: %d\E[00m\n",pdutemporal.chk);
		printf("\E[1;30m############################\E[00m\n");
	}while(pdudatos[pos].tipo!=2);
	

}
//---------------------------------------------------------------------MAIN----------------------------------------------------------------
//a partir de aqui es simplemente el menú con el cual leemos la opción y saltamos al main
main (argc,argv)
	int argc;
	char *argv[];
{
	if(argc<2) 
	{ 
		printf("\E[0;1mNumero de parametro incorrecto (cli_user maquina)\E[00m\n"); 
		exit(1); 
	} 
	/* Preparacion de la direccion del socket destino */ 
	hp= gethostbyname(argv[1]); 
	if (hp == NULL) 
	{ 
		fprintf(stderr,"\E[0;1mProblemas con hosts remoto : gethostbyname\E[00m\n"); 
		perror("Error"); 
		exit(2); 
	} 
	else 
	{ 
		printf("\n");
		printf("\E[0;1mMaquina destino : %s\E[00m\n",hp->h_name); 
		printf("\E[0;1mTipo de direccion : %d\E[00m\n",hp->h_addrtype); 
		printf("\E[0;1mLongitud de direccion : %d\E[00m\n",hp->h_length); 
	} 

	if ((sock = socket(AF_INET,SOCK_DGRAM, 0)) == -1) 
	/* creacion de socket */ 
	{ 
		fprintf(stderr,"\E[0;1mProblemas en creacion de socket: socket\E[00m\n"); 
		perror("Error"); 
		exit(3); 
	} 
	bzero((char *)&dir_remota, sizeof(dir_remota)); 
	dir_remota.sin_port= PORT; /* o htons(PORT) */ 
	dir_remota.sin_family = AF_INET; 
	bcopy(hp->h_addr, &dir_remota.sin_addr, hp->h_length); 
	/* envio de mensaje constituido por el nombre del usuario */

	do{

		printf("\n");
		printf("\E[44m\E[1;33m########## - MENU - ##########\E[00m\n");
		printf("\E[1;31m 1. Introduce cadena a transmistir\E[00m\n");
		printf("\E[1;31m 2. Introduce operacion a realizar\E[00m\n");
		printf("\E[1;31m 3. Transmitir secuencialmente\E[00m\n");
		printf("\E[1;31m 4. Transmitir priorizando operación\E[00m\n");
		printf("\E[1;31m 5. Salir\E[00m\n");
		printf("\E[44m\E[1;33m########## - MENU - ##########\E[00m\n");
		printf("\n");
		fflush(stdin);
		printf("\E[0;4m\E[0;1mLa opcion elegida es\E[00m: ");
		gets(op);
		opcion=atoi(op);
		switch (opcion)
		{	
			case 1://leemos el texto y montamos el vector
				num_actividad++;
				printf ("\E[0;34mEscriba el mensaje a enviar:\E[00m\n");
                		gets(mensaje);
				montarVector();
				printf("\n");
				m=1;
			break;
	
			case 2://leemos la operación y la insertamos en el vector haya o no haya texto
				num_actividad++;
				printf ("\E[0;34mEscriba la operacion que quiere que se realice:\E[00m\n");
                		gets(operacion);
                		q=0;
                		montarVector2();
				printf("\n");
				o=1;
				
			break;
	
		  case 3://tx secuencialmente: primero el texto y luego la operación
		  	if(m==1 || o==1)
		  	{
					printf ("\E[0;34mTransmitiendo Secuencialmente...:\E[00m\n");
					TxSecuencial();
					limpiarVector();
					m=0;
					o=0;
				}else{
					printf("\n");
					printf("\E[40m\E[1;31mERROR: INTRODUZCA UN MENSAJE Y/O OPERACION!!!!!\E[00m\n");
					printf("\n");
				}
			break;

			case 4://tx priorizando primero la primera parte del texto luego saltamos a la operación y volvemos a lo que resta del texto.
				if(m==1 && o==1)
				{
					printf ("\E[0;34mTransmitiendo priorizando operación...:\E[00m\n");
					TxPriorizando();
				}else{
					printf("\n");
					printf("\E[40m\E[1;31mERROR: INTRODUZCA UN MENSAJE Y UNA OPERACION!!!!!\E[00m\n");
					printf("\n");
				}
				m=0;
				o=0;
			break;
		
	
			case 5: //salimos del programa.
				printf("\E[0;1mFIN DEL PROGRAMA\E[00m\n");
				exit(-1);
			break;
			default:
				printf("\E[0;1mRegreso al menu\E[00m\n");
			break;
		}
	}while(opcion!=5 );

}
