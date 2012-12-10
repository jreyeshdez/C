/* José Julian Reyes Hernandez */
/* University of Las Palmas de Gran Canaria
/* Electronic and Telecommunication Engineering School (EITE) at ULPGC. */

#include <stdio.h> 
#include <stdlib.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/un.h> 
#include <netdb.h> 
#include <arpa/inet.h> 
#include <sys/types.h>
#include <string.h> 
#define LGREP 256 
#define PORT 8020 
typedef struct PDU{
	int tipo;
	int actividad;
	int tamano;
	char datos[9];
	short chk;//usamos short para que acepte numeros negativos.
}PDU;
int i;
int q=0;
int k=0;
int posic;
int aux;
int error=1;//Inicializamos a 0 suponiendo que no hay error
int s=0;
PDU pdudatos[33];//VECTOR ALMACENAMOS PDU
PDU pdutemporal;
PDU pdurespuesta;
PDU pdures;
char mensaje[255];//mensaje LEIDA DESDE TECLADO
char sms[255];
char temp[8];
char operacion[5];
char resul[5];
int chktemp;
char oper;
int act_res;
float a;
float b;
float resultado;

void calculodeCHK(){//FUNCION CALCULA EL CHK DE LO QUE RECIBIMOS PARA LUEGO COMPARAR

	int i,total,chk;
	total=0;
	total=pdutemporal.tipo;
	total=total+pdutemporal.actividad;
	total=total+pdutemporal.tamano;

	i=0;
	while (i<9){
	
		total=total+pdutemporal.datos[i];
		i++;
	}

	chk=~total;  /*realizamos el complemento a 1*/

	
	chktemp=chk;
}
void prepararPDU(char res[5]){
//preparamos la PDU de respuesta de operación
	
	int o=0;
	int w=0;
	int aux=0;
	int i,crc,chk;
	int lg,sock; 
	char long_parte_entera[1];
	char long_parte_decimal[1];
	char signo[1];
	char parte_entera[2];
	char parte_decimal[1];
	char total[6];
	
	struct sockaddr_in dir_local,dir_remota; 
	
	if(res[0]=='-')
	{
		signo[0]='-';
	}else{
		signo[0]='+';
	}
	o=0;
	aux=0;
	while(o<5)
	{
		if(res[o]=='.')
		{
			aux=o;
			if(res[o+1]=='0')
			{
				long_parte_decimal[0]='0';
				parte_decimal[0]=res[o+1];
			}else{
				long_parte_decimal[0]='1';
				parte_decimal[0]=res[o+1];
			}
		}
		o++;	
	}
	if(res[0]=='-')
	{
		aux=aux-1;
	}
	switch(aux)
	{
		case 0:
			long_parte_entera[0]='0';
		break;
		case 1:
			long_parte_entera[0]='1';
		break;
		case 2:
			long_parte_entera[0]='2';
		break;
		case 3:
			long_parte_entera[0]='3';
		break;
	}
	w=0;
	o=0;
	while(res[w]!='.')
	{
		if(res[w]!='-' && res[w]!='.')
		{
			parte_entera[o]=res[w];
			o++;
		}
		w++;
	}

	pdurespuesta.datos[0]=signo[0];
	pdurespuesta.datos[1]=long_parte_entera[0];
	if(parte_entera[1]=='0' || parte_entera[1]=='1' || parte_entera[1]=='2' || parte_entera[1]=='3' || parte_entera[1]=='4' || parte_entera[1]=='5' || parte_entera[1]=='6' || parte_entera[1]=='7' || parte_entera[1]=='8' || parte_entera[1]=='9')
	{
		for(i=0;i<2;i++)
		{
			pdurespuesta.datos[i+2]=parte_entera[i];
		}
		pdurespuesta.datos[4]=long_parte_decimal[0];
		pdurespuesta.datos[5]=parte_decimal[0];
	}else{
		pdurespuesta.datos[2]=parte_entera[0];
		pdurespuesta.datos[3]=long_parte_decimal[0];
		pdurespuesta.datos[4]=parte_decimal[0];
	}
	pdurespuesta.tipo=93;
	pdurespuesta.actividad=act_res;
	pdurespuesta.tamano=strlen(pdurespuesta.datos);
	crc=0;
	crc=pdurespuesta.tipo;
	crc=crc+pdurespuesta.actividad;
	crc=crc+pdurespuesta.tamano;
	i=0;
	while (i<8){
	
		crc=crc+pdurespuesta.datos[i];
		i++;
	}
	chk=~crc;  /*realizamos el complemento a 1*/
	pdurespuesta.chk=chk;
}
void calculadora(float x1,float x2, char opr)
//diferenciamos que tipo de operación quiere hacer el usuario
{
	int o=0;
	int w=0;
	char long_parte_entera;
	char long_parte_decimal;
	char signo;
	char parte_entera[1];
	char parte_decimal;
	char total[6];
	switch(opr)
	{
		case '+':
			resultado=x1+x2;
		break;
		case '-':
			resultado=x1-x2;
		break;
		case '*':
			resultado=x1*x2;
		break;
		case '/':
			resultado=x1/x2;
		break;
	}
	sprintf(resul,"%.1f",resultado);

	prepararPDU(resul);	
}
void obtenerDatos(){
//obtenemos los datos de tipo int a partir de la cadena, tuvimos que hacerlo así ya que
//no encontramos una instrucción que te lo hiciera directamente.
	if (operacion[0]=='-')
	{
		switch(operacion[1])
		{
			case '0':
				a=0;
			break;
			case '1':
				a=-1;
			break;
			case '2':
				a=-2;
			break;
			case '3':
				a=-3;
			break;
			case '4':
				a=-4;
			break;
			case '5':
				a=-5;
			break;
			case '6':
				a=-6;
			break;
			case '7':
				a=-7;
			break;
			case '8':
				a=-8;
			break;
			case '9':
				a=-9;
			break;	
		}	
		oper=operacion[2];
	}else{
		switch(operacion[0])
		{
			case '0':
				a=0;
			break;
			case '1':
				a=1;
			break;
			case '2':
				a=2;
			break;
			case '3':
				a=3;
			break;
			case '4':
				a=4;
			break;
			case '5':
				a=5;
			break;
			case '6':
				a=6;
			break;
			case '7':
				a=7;
			break;
			case '8':
				a=8;
			break;
			case '9':
				a=9;
			break;	
		}	
		oper=operacion[1];
	}
	switch(operacion[2])
	{
		case '0':
			b=0;
		break;
		case '1':
			b=1;
		break;
		case '2':
			b=2;	
		break;
		case '3':
			b=3;
		break;
		case '4':
			b=4;
		break;
		case '5':
			b=5;
		break;
		case '6':
			b=6;
		break;
		case '7':
			b=7;
		break;
		case '8':
			b=8;
		break;
		case '9':
			b=9;
		break;	
	}

	if(operacion[2]=='0' || operacion[2]=='1' || operacion[2]=='2' || operacion[2]=='3' || operacion[2]=='4' || operacion[2]=='5' || operacion[2]	=='6' || operacion[2]=='7' || operacion[2]=='8' || operacion[2]=='9')
	{
		oper=operacion[1];
	}
	switch(operacion[3])
	{
		case '0':
			if(operacion[0]=='-')
			{
				b=0;
				oper=operacion[2];
			}else{
				b=-0;
				oper=operacion[1];
			}
		break;
		case '1':
			if(operacion[0]=='-')
			{
				b=1;
				oper=operacion[2];
			}else{
				b=-1;
				oper=operacion[1];
			}
		break;
		case '2':
			if(operacion[0]=='-')
			{
				b=2;
				oper=operacion[2];
			}else{
				b=-2;
				oper=operacion[1];
			}	
		break;
		case '3':
			if(operacion[0]=='-')
			{
				b=3;
				oper=operacion[2];
			}else{
				b=-3;
				oper=operacion[1];
			}
		break;
		case '4':
			if(operacion[0]=='-')
			{
				b=4;
				oper=operacion[2];
			}else{
				b=-4;
				oper=operacion[1];
			}
		break;
		case '5':
			if(operacion[0]=='-')
			{
				b=5;
				oper=operacion[2];
			}else{
				b=-5;
				oper=operacion[1];
			}
		break;
		case '6':
			if(operacion[0]=='-')
			{
				b=6;
				oper=operacion[2];
			}else{
				b=-6;
				oper=operacion[1];
			}
		break;
		case '7':
			if(operacion[0]=='-')
			{
				b=7;
				oper=operacion[2];
			}else{
				b=-7;
				oper=operacion[1];
			}
		break;
		case '8':
			if(operacion[0]=='-')
			{
				b=8;
				oper=operacion[2];
			}else{
				b=-8;
				oper=operacion[1];
			}
		break;
		case '9':
			if(operacion[0]=='-')
			{
				b=9;
				oper=operacion[2];
			}else{
				b=-9;
				oper=operacion[1];
			}
		break;	

	}
	switch(operacion[4])
	{
		case '0':
			b=0;
		break;
		case '1':
			b=-1;
		break;
		case '2':
			b=-2;
		break;
		case '3':
			b=-3;
		break;
		case '4':
			b=-4;
		break;
		case '5':
			b=-5;
		break;
		case '6':
			b=-6;
		break;
		case '7':
			b=-7;
		break;
		case '8':
			b=-8;
		break;
		case '9':
			b=-9;
		break;	
	}
	if(operacion[4]=='0' || operacion[4]=='1' || operacion[4]=='2' || operacion[4]=='3' || operacion[4]=='4' || operacion[4]=='5' || operacion[4]=='6' || operacion[4]=='7' || operacion[4]=='8' || operacion[4]=='9')
	{
		oper=operacion[2];
	}
	calculadora(a,b,oper);
}

//bucle principal del programa
main (argc,argv) 
	int argc; 
	char *argv[]; 
{ 
	int lg,sock; 
	int d; 
	int n; 
	char cadena[256], respuesta[256],temp[8];
	struct sockaddr_in dir_local,dir_remota; 
	int len_dir_local=sizeof(dir_local); 
	int len_dir_remota=sizeof(dir_remota); 
	struct hostent *host; 
	char hostname[256]; 
	gethostname(hostname,sizeof(hostname)); /* strlen no va */ 
	printf("\n");
	printf("\E[0;1mNombre Local : %s\E[00m\n",hostname); 
	host=gethostbyname(hostname); 

	if(host==NULL) 
	{ 
		fprintf(stderr,"Problemas en gethostbyname\n"); 
		perror("Error"); 
		exit(-1); 
	} 
	else 
	{ 
		printf("\E[0;1mNombre completo : %s\E[00m\n",host->h_name); 
		printf("\E[0;1mLongitud de direccion : %d\E[00m\n",host->h_length); 
		printf("\E[0;1mTipo de direccion : %d\E[00m\n",host->h_addrtype); 
	} 

	if ((sock = socket(AF_INET,SOCK_DGRAM, 0)) == -1) 
	/* creacion de socket */ 
	{ 
		fprintf(stderr,"Problemas en creacion de socket : socket"); 
		perror("error"); 
		exit(1); 
	} 
	/* bzero((char *)&adr, sizeof(adr)); */ 
	dir_local.sin_port=PORT; /* OK con htons(PORT) */ 
	dir_local.sin_addr.s_addr = INADDR_ANY; /* *(long *)host->h_addr */ 
	dir_local.sin_family = AF_INET; 
	if (bind(sock, (struct sockaddr *)&dir_local, len_dir_local)== -1) 
	/* enlace al puerto de servicio */ 
	{ 
		fprintf(stderr,"Problemas con bind\n"); 
		perror("error"); 
		exit(2); 
	} 

	while(1) 
	{	
		do{ /* bucle infinito espera de peticiones */ 
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
				if(pdutemporal.tipo==1)
				{
					strncpy(temp,pdutemporal.datos,8);
					printf("\E[1;30m############################\E[00m\n");
					printf("\E[1;32mDatos : %s\E[00m\n",temp);
					printf("\E[1;32mTipo : %d\E[00m\n",pdutemporal.tipo);
					printf("\E[1;32mTamaño : %d\E[00m\n",pdutemporal.tamano);
					printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
					printf("\E[1;32mCHK : %d\E[00m\n",pdutemporal.chk);
					printf("\E[1;30m############################\E[00m\n");
					calculodeCHK();
					if(pdutemporal.chk!=chktemp)
					{
						printf("\n");
						printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
						error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  					}
  					do{
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
							if(pdutemporal.tipo==3)
							{
								strncpy(temp,pdutemporal.datos,8);
								printf("\E[1;30m############################\E[00m\n");
								printf("\E[1;32mDatos : %s\E[00m\n",temp);
								printf("\E[1;32mTipo : %d\E[00m\n",pdutemporal.tipo);
								printf("\E[1;32mTamaño : %d\E[00m\n",pdutemporal.tamano);
								printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
								printf("\E[1;32mCHK : %d\E[00m\n",pdutemporal.chk);
								printf("\E[1;30m############################\E[00m\n");
								calculodeCHK();
								if(pdutemporal.chk!=chktemp)
								{
									printf("\n");
									printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
									error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  								}
								printf("\n");
								printf("\E[1;31mInterrumpida la actividad numero: %d\E[00m\n",pdutemporal.actividad);
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
									if(pdutemporal.tipo==1)
									{
										act_res=pdutemporal.actividad;
										strncpy(temp,pdutemporal.datos,8);
										printf("\E[1;30m############################\E[00m\n");
										printf("\E[1;32mDatos : %s\E[00m\n",temp);
										printf("\E[1;32mTipo : %d\E[00m\n",pdutemporal.tipo);
										printf("\E[1;32mTamaño : %d\E[00m\n",pdutemporal.tamano);
										printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
										printf("\E[1;32mCHK : %d\E[00m\n",pdutemporal.chk);
										printf("\E[1;30m############################\E[00m\n");
										calculodeCHK();
										if(pdutemporal.chk!=chktemp)
										{
											printf("\n");
											printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
											error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  										}
										s=1;					
										do{
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
												calculodeCHK();
												if(pdutemporal.chk!=chktemp)
												{
													printf("\n");
													printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
													error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  												}
  												for(i=0;i<5;i++)
												{
													operacion[k]=temp[i];
													k++;
												}
											}			
										}while(pdutemporal.tipo!=2);
										if(pdutemporal.tipo==2)
										{
											printf("\n");
											printf("\E[1;31mOperacion a realizar: \E[00m%s\n",operacion);
											printf("\n");
											obtenerDatos();
											s=0;
											if(sendto(sock,&pdurespuesta,sizeof(pdurespuesta),0,(struct sockaddr *)&dir_remota,sizeof(dir_remota))<0){
	   		 									printf("Error en el envío.\n");
											}
											strncpy(temp,pdurespuesta.datos,8);
											printf("\E[1;30m############################\E[00m\n");
											printf("\E[1;32mPDU de Respuesta Enviada\E[00m\n");
											printf("\E[1;30m############################\E[00m\n");
											printf("\E[1;32mDatos: %s\E[00m\n",pdurespuesta.datos);
											printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdurespuesta.tipo);
											printf("\E[1;32mTamaño: %d\E[00m\n",pdurespuesta.tamano);
											printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdurespuesta.actividad);
											printf("\E[1;32mCRC: %d\E[00m\n",pdurespuesta.chk);
											printf("\E[1;30m############################\E[00m\n");
											k=0;
										}
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
											calculodeCHK();
											if(pdutemporal.chk!=chktemp)
											{
												printf("\n");
												printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
												error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  											}
  											if(pdutemporal.tipo==4)
  											{
  												printf("\n");
  												printf("\E[1;31mReinicio de la actividad numero: %d\E[00m\n",pdutemporal.actividad); 
  												printf("\n");
  											}
										}
								
									}
								}
								
							}else{
								strncpy(temp,pdutemporal.datos,8);
								printf("\E[1;30m############################\E[00m\n");
								printf("\E[1;32mDatos : %s\E[00m\n",temp);
								printf("\E[1;32mTipo : %d\E[00m\n",pdutemporal.tipo);
								printf("\E[1;32mTamaño : %d\E[00m\n",pdutemporal.tamano);
								printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
								printf("\E[1;32mCHK : %d\E[00m\n",pdutemporal.chk);
								printf("\E[1;30m############################\E[00m\n");
								calculodeCHK();
								if(pdutemporal.chk!=chktemp)
								{
									printf("\n");
									printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
									error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  								}
								if(pdutemporal.tipo==91)
								{
									s=1;
									for(i=0;i<8;i++)
									{
										mensaje[q]=temp[i];
										sms[q]=temp[i];
										q++;
									}
									aux=pdutemporal.actividad;
								}
								if(pdutemporal.tipo==92)
								{
									act_res=pdutemporal.actividad;
									strncpy(temp,pdutemporal.datos,8);
									printf("\E[1;30m############################\E[00m\n");
									printf("\E[1;32mDatos : %s\E[00m\n",temp);
									printf("\E[1;32mTipo : %d\E[00m\n",pdutemporal.tipo);
									printf("\E[1;32mTamaño : %d\E[00m\n",pdutemporal.tamano);
									printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
									printf("\E[1;32mCHK : %d\E[00m\n",pdutemporal.chk);
									printf("\E[1;30m############################\E[00m\n");
									calculodeCHK();
									if(pdutemporal.chk!=chktemp)
									{
										printf("\n");
										printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
										error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  									}
									s=1;
									for(i=0;i<5;i++)
									{
										operacion[k]=temp[i];
										k++;
									}
									do{
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
											calculodeCHK();
											if(pdutemporal.chk!=chktemp)
											{
												printf("\n");
												printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
												error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  											}
										}			
									}while(pdutemporal.tipo!=2);
									if(pdutemporal.tipo==2)
									{
										printf("\n");
										printf("\E[1;31mOperacion a realizar: \E[00m%s\n",operacion);
										printf("\n");
										obtenerDatos();
										s=0;
										if(sendto(sock,&pdurespuesta,sizeof(pdurespuesta),0,(struct sockaddr *)&dir_remota,sizeof(dir_remota))<0){
	   		 								printf("Error en el envío.\n");
										}
										strncpy(temp,pdurespuesta.datos,8);
										printf("\E[1;30m############################\E[00m\n");
										printf("\E[1;32mPDU de Respuesta Enviada\E[00m\n");
										printf("\E[1;30m############################\E[00m\n");
										printf("\E[1;32mDatos: %s\E[00m\n",pdurespuesta.datos);
										printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdurespuesta.tipo);
										printf("\E[1;32mTamaño: %d\E[00m\n",pdurespuesta.tamano);
										printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdurespuesta.actividad);
										printf("\E[1;32mCRC: %d\E[00m\n",pdurespuesta.chk);
										printf("\E[1;30m############################\E[00m\n");
										k=0;
									}
								}	
							}	
						}			
					}while(pdutemporal.tipo!=2);
					
					if(pdutemporal.tipo==2 && pdutemporal.actividad==aux  )
					{
						printf("\n");
						printf("\E[1;31mMensaje Recibido: \E[00m%s\n",mensaje);
						printf("\n");
						for(i=0;i<255;i++)
						{
							mensaje[i]=' ';
						}
						q=0;
					}	
  				}
				if(pdutemporal.tipo==91)
				{
					strncpy(temp,pdutemporal.datos,8);
					printf("\E[1;30m############################\E[00m\n");
					printf("\E[1;32mDatos : %s\E[00m\n",temp);
					printf("\E[1;32mTipo : %d\E[00m\n",pdutemporal.tipo);
					printf("\E[1;32mTamaño : %d\E[00m\n",pdutemporal.tamano);
					printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
					printf("\E[1;32mCHK : %d\E[00m\n",pdutemporal.chk);
					printf("\E[1;30m############################\E[00m\n");
					calculodeCHK();
					if(pdutemporal.chk!=chktemp)
					{
						printf("\n");
						printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
						error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  					}
					s=1;
					for(i=0;i<8;i++)
					{
						mensaje[q]=temp[i];
						sms[q]=temp[i];
						q++;
					}
					do{
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
							if(pdutemporal.tipo==3)
							{
								strncpy(temp,pdutemporal.datos,8);
								printf("\E[1;30m############################\E[00m\n");
								printf("\E[1;32mDatos : %s\E[00m\n",temp);
								printf("\E[1;32mTipo : %d\E[00m\n",pdutemporal.tipo);
								printf("\E[1;32mTamaño : %d\E[00m\n",pdutemporal.tamano);
								printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
								printf("\E[1;32mCHK : %d\E[00m\n",pdutemporal.chk);
								printf("\E[1;30m############################\E[00m\n");
								calculodeCHK();
								if(pdutemporal.chk!=chktemp)
								{
									printf("\n");
									printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
									error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  								}
								printf("\n");
								printf("\E[1;31mInterrumpida la actividad numero: %d\E[00m\n",pdutemporal.actividad);
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
									if(pdutemporal.tipo==1)
									{
										act_res=pdutemporal.actividad;
										strncpy(temp,pdutemporal.datos,8);
										printf("\E[1;30m############################\E[00m\n");
										printf("\E[1;32mDatos : %s\E[00m\n",temp);
										printf("\E[1;32mTipo : %d\E[00m\n",pdutemporal.tipo);
										printf("\E[1;32mTamaño : %d\E[00m\n",pdutemporal.tamano);
										printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
										printf("\E[1;32mCHK : %d\E[00m\n",pdutemporal.chk);
										printf("\E[1;30m############################\E[00m\n");
										calculodeCHK();
										if(pdutemporal.chk!=chktemp)
										{
											printf("\n");
											printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
											error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  										}
										s=1;					
										do{
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
												calculodeCHK();
												if(pdutemporal.chk!=chktemp)
												{
													printf("\n");
													printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
													error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  												}
  												for(i=0;i<5;i++)
												{
													operacion[k]=temp[i];
													k++;
												}
											}			
										}while(pdutemporal.tipo!=2);
										if(pdutemporal.tipo==2)
										{
											printf("\n");
											printf("\E[1;31mOperacion a realizar: \E[00m%s\n",operacion);
											printf("\n");
											obtenerDatos();
											s=0;
											if(sendto(sock,&pdurespuesta,sizeof(pdurespuesta),0,(struct sockaddr *)&dir_remota,sizeof(dir_remota))<0){
	   		 									printf("Error en el envío.\n");
											}
											strncpy(temp,pdurespuesta.datos,8);
											printf("\E[1;30m############################\E[00m\n");
											printf("\E[1;32mPDU de Respuesta Enviada\E[00m\n");
											printf("\E[1;30m############################\E[00m\n");
											printf("\E[1;32mDatos: %s\E[00m\n",pdurespuesta.datos);
											printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdurespuesta.tipo);
											printf("\E[1;32mTamaño: %d\E[00m\n",pdurespuesta.tamano);
											printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdurespuesta.actividad);
											printf("\E[1;32mCRC: %d\E[00m\n",pdurespuesta.chk);
											printf("\E[1;30m############################\E[00m\n");
											k=0;
										}
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
											calculodeCHK();
											if(pdutemporal.chk!=chktemp)
											{
												printf("\n");
												printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
												error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  											}
  											if(pdutemporal.tipo==4)
  											{
  												printf("\n");
  												printf("\E[1;31mReinicio de la actividad numero: %d\E[00m\n",pdutemporal.actividad); 
  												printf("\n");
  											}
										}
								
									}
								}
								
							}else{
								strncpy(temp,pdutemporal.datos,8);
								printf("\E[1;30m############################\E[00m\n");
								printf("\E[1;32mDatos : %s\E[00m\n",temp);
								printf("\E[1;32mTipo : %d\E[00m\n",pdutemporal.tipo);
								printf("\E[1;32mTamaño : %d\E[00m\n",pdutemporal.tamano);
								printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
								printf("\E[1;32mCHK : %d\E[00m\n",pdutemporal.chk);
								printf("\E[1;30m############################\E[00m\n");
								calculodeCHK();
								if(pdutemporal.chk!=chktemp)
								{
									printf("\n");
									printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
									error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  								}
								if(pdutemporal.tipo==91)
								{
									s=1;
									for(i=0;i<8;i++)
									{
										mensaje[q]=temp[i];
										sms[q]=temp[i];
										q++;
									}
								}
								if(pdutemporal.tipo==2)
								{
									printf("\n");
									printf("\E[1;31mMensaje Recibido: \E[00m%s\n",mensaje);
									printf("\n");
									for(i=0;i<255;i++)
									{
										mensaje[i]=' ';
									}
									q=0;
								}
							}
							
							
							
						}			
					}while(pdutemporal.tipo!=2);
					if(pdutemporal.tipo==2)
					{
						printf("\n");
						printf("\E[1;31mMensaje Recibido: \E[00m%s\n",mensaje);
						printf("\n");
						for(i=0;i<255;i++)
						{
							mensaje[i]=' ';
						}
						q=0;
					}
				}
				if(pdutemporal.tipo==92)
				{
					act_res=pdutemporal.actividad;
					strncpy(temp,pdutemporal.datos,8);
					printf("\E[1;30m############################\E[00m\n");
					printf("\E[1;32mDatos : %s\E[00m\n",temp);
					printf("\E[1;32mTipo : %d\E[00m\n",pdutemporal.tipo);
					printf("\E[1;32mTamaño : %d\E[00m\n",pdutemporal.tamano);
					printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdutemporal.actividad);
					printf("\E[1;32mCHK : %d\E[00m\n",pdutemporal.chk);
					printf("\E[1;30m############################\E[00m\n");
					calculodeCHK();
					if(pdutemporal.chk!=chktemp)
					{
						printf("\n");
						printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
						error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  					}
					s=1;
					for(i=0;i<5;i++)
					{
						operacion[k]=temp[i];
						k++;
					}
					do{
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
							calculodeCHK();
							if(pdutemporal.chk!=chktemp)
							{
								printf("\n");
								printf("\E[40m\E[1;31mERROR EN CHK!!: %d.\E[00m\n",chktemp);
								error=2;//VARIABLE error A UNO SE HA PRODUCIDO UN error
  							}
						}			
					}while(pdutemporal.tipo!=2);
					if(pdutemporal.tipo==2)
					{
						printf("\n");
						printf("\E[1;31mOperacion a realizar: \E[00m%s\n",operacion);
						printf("\n");
						obtenerDatos();
						s=0;
						if(sendto(sock,&pdurespuesta,sizeof(pdurespuesta),0,(struct sockaddr *)&dir_remota,sizeof(dir_remota))<0){
	   		 				printf("Error en el envío.\n");
						}
						strncpy(temp,pdurespuesta.datos,8);
						printf("\E[1;30m############################\E[00m\n");
						printf("\E[1;32mPDU de Respuesta Enviada\E[00m\n");
						printf("\E[1;30m############################\E[00m\n");
						printf("\E[1;32mDatos: %s\E[00m\n",pdurespuesta.datos);
						printf("\E[1;32mTipo de PDU: %d\E[00m\n",pdurespuesta.tipo);
						printf("\E[1;32mTamaño: %d\E[00m\n",pdurespuesta.tamano);
						printf("\E[1;32mNumero Actividad: %d\E[00m\n",pdurespuesta.actividad);
						printf("\E[1;32mCRC: %d\E[00m\n",pdurespuesta.chk);
						printf("\E[1;30m############################\E[00m\n");
						k=0;
					}
				}
			}
		}while(1);
	break;
	} 
}
