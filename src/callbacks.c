		#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <mysql/mysql.h>

MYSQL mysql;
MYSQL_ROW row;
MYSQL_RES *res;
MYSQL_RES *resPedidos;
MYSQL_FIELD *field;
gboolean terminar=FALSE;

gint columnas=10;

char *homedir;  // Directorio HOME del usuario
char file_db_config[128];  // Variable para almacenar la ruta del archivo de configuración para la base de datos
enum
{
	ATRIBUTO,
	ATRIBUTO2,
	ATRIBUTO3,
	ATRAVISO1,
	COLPEDIDO,
	COLFECHA,
	COLCLIENTE,
	COLTOMO,
	COLHORA,
	COLAVISO,
	NUM_COLS
};

GTimer *timer;

/*void checar_pedidos(GtkWidget       *widget);

void checar_pedidos(GtkWidget       *widget)
{
	GtkTreeViewColumn *columna;
	//GtkTreeView *tvPedidos;
	//GtkListStore *modelo;
	GtkListStore *listPedidos;
	GtkTreeIter iter;
	GtkWidget *lblPedidos;
	GtkWidget *lblPedidosEntregados;
	GtkWidget *lblCumplidos;
	GtkWidget *lblAtrasados;
	int cont;
	
	GError *gError = NULL;
	
	char sql[600];
	char strMarkUp[200];
	int er;
	
	//tvPedidos       = GTK_TREE_VIEW(lookup_widget(widget,"treeViewPedidos"));
	
	lblPedidos           = lookup_widget(widget,"lblPedidos");
	lblPedidosEntregados = lookup_widget(widget,"lblPedidosEntregados");
	lblCumplidos         = lookup_widget(widget,"lblCumplidos");
	lblAtrasados         = lookup_widget(widget,"lblAtrasados");

	//modelo = GTK_LIST_STORE( gtk_tree_view_get_model (tvPedidos));
	//g_object_unref(G_OBJECT(modelo));
	
	//modelo = gtk_list_store_new(columnas, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	
	//se le da el modelo de la estructura del reporte
	//gtk_tree_view_set_model(tvPedidos, GTK_TREE_MODEL(modelo));
	
	//
	
	//Se agregan los pedidos pendientes
	if(conecta_bd() == 1)
	{
		sprintf (sql,"SELECT Pedido.id_pedido, DATE_FORMAT(Pedido.fecha,'%%d-%%m-%%y'), LEFT(Cliente.nombre,30), Usuario.nombre, Pedido.HoraEntrega, IF (Pedido.HoraEntrega < CURTIME(),1,0) FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario LEFT JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Venta.id_venta IS NULL AND FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' ORDER BY Pedido.HoraEntrega LIMIT 15");
		er = mysql_query(&mysql, sql);
		if(er == 0)
		{
			res = mysql_store_result(&mysql);
			if(res)
			{
				gtk_list_store_clear (modelo);
				
				cont = 0;
				while ((row = mysql_fetch_row(res)))
				{
					gtk_list_store_append (modelo, &iter);
					
					if ( (cont%2) == 0 )
					{
						gtk_list_store_set (modelo, &iter,
											ATRIBUTO,19*1024,
											ATRIBUTO2,"#EEEEEE",
											ATRIBUTO3,"#222222",
											ATRAVISO1,"#FF4444",
											COLPEDIDO,row[0],
											COLFECHA,row[1],
											COLCLIENTE,row[2],
											COLTOMO,row[3],
											COLHORA,row[4],
											COLAVISO,row[5],-1);
					}
					else
					{
						gtk_list_store_set (modelo, &iter,
											ATRIBUTO,19*1024,
											ATRIBUTO2,"#FFFFFF",
											ATRIBUTO3,"#222222",
											COLPEDIDO,row[0],
											COLFECHA,row[1],
											COLCLIENTE,row[2],
											COLTOMO,row[3],
											COLHORA,row[4],
											COLAVISO,row[5],-1);
					}
					cont++;
				}
			}
			printf ("SE LIBERA RES\n");
			mysql_free_result(res);
			//printf ("SE LIBERA\n");
			//g_object_unref(G_OBJECT(modelo));
		}
		
		//Se obtiene el total de pedidos registrados
		sprintf (sql,"SELECT id_pedido FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario LEFT JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' ");
		er = mysql_query(&mysql, sql);
		if(er == 0)
		{
			res = mysql_store_result(&mysql);
			if(res)
			{
				sprintf(strMarkUp,"<span size='36000' color='#444444'><b>%d</b></span>", mysql_num_rows(res));
				gtk_label_set_markup (GTK_LABEL(lblPedidos), strMarkUp);
			}
			mysql_free_result(res);
		}
		
		//Se obtiene el numero total de pedidos surtidos del dia
		sprintf (sql,"SELECT id_pedido FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario INNER JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' ");
		er = mysql_query(&mysql, sql);
		if(er == 0)
		{
			res = mysql_store_result(&mysql);
			if(res)
			{
				sprintf(strMarkUp,"<span size='36000' color='#222888'><b>%d</b></span>", mysql_num_rows(res));
				gtk_label_set_markup (GTK_LABEL(lblPedidosEntregados), strMarkUp);
			}
			mysql_free_result(res);
		}
		
		//Se obtiene el numero de pedidos surtidos en el tiempo establecido (15 min)
		sprintf (sql,"SELECT DATE_FORMAT( DATE_SUB( CONCAT( Pedido.FechaEntrega, ' ', Pedido.HoraEntrega ) , INTERVAL IF (Pedido.HoraSalida >0,Pedido.HoraSalida,Pedido.HoraEntrega) HOUR_SECOND ) , '%%i' ) AS MinutosTarde FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario INNER JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' HAVING MinutosTarde >= 15 ");
		er = mysql_query(&mysql, sql);
		if(er == 0)
		{
			res = mysql_store_result(&mysql);
			if(res)
			{
				sprintf(strMarkUp,"<span size='36000' color='#338823'><b>%d</b></span>", mysql_num_rows(res));
				gtk_label_set_markup (GTK_LABEL(lblCumplidos), strMarkUp);
			}
			mysql_free_result(res);
		}
		
		//Se obtiene el numero de pedidos que no se surtieron en el tiempo establecido (15 min)
		sprintf (sql,"SELECT DATE_FORMAT( DATE_SUB( CONCAT( Pedido.FechaEntrega, ' ', Pedido.HoraEntrega ) , INTERVAL IF (Pedido.HoraSalida >0,Pedido.HoraSalida,Pedido.HoraEntrega) HOUR_SECOND ) , '%%i' ) AS MinutosTarde FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario INNER JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' HAVING MinutosTarde < 15  ");
		er = mysql_query(&mysql, sql);
		if(er == 0)
		{
			res = mysql_store_result(&mysql);
			if(res)
			{
				sprintf(strMarkUp,"<span size='36000' color='#AA2822'><b>%d</b></span>", mysql_num_rows(res));
				gtk_label_set_markup (GTK_LABEL(lblAtrasados), strMarkUp);
			}
			mysql_free_result(res);
			
		}
		
		mysql_close(&mysql);
	}
	else
	{
		printf (" NO SE PUDO CONECTAR \n");
	}
	
	//gtk_window_maximize(GTK_WINDOW(widget));
}*/

int conecta_bd();

int conecta_bd()
{
	FILE *fconfiguracionbd;
	int i=0;
	int j=0;
	char tipo[50]="";
	char dato[50]="";
	char tmp;
	char ip[20]="";
	char usuario[20]="";
	char contrasena[20]="";
	char bd[30]="";

	if((fconfiguracionbd = fopen(file_db_config,"r")))
	{
		while(!feof(fconfiguracionbd))
		{
			tmp = fgetc(fconfiguracionbd);
			if(tmp != ' ')
			{
				tipo[i] = tmp;
				i++;
			}
			else
			{
				while(tmp != '\n')
				{
					tmp = fgetc(fconfiguracionbd);
					dato[j]=tmp;
					j++;
				}
				dato[j-1] = '\0';
				j=0;
				if(strcmp(tipo,"ip") == 0)
					strcpy(ip, dato);
				if(strcmp(tipo,"usuario") == 0)
					strcpy(usuario, dato);
				if(strcmp(tipo,"contrasena") == 0)
					strcpy(contrasena, dato);
				if(strcmp(tipo,"bd") == 0)
					strcpy(bd, dato);
				strcpy(tipo,"");
				for(i=0;i<30;i++)
					tipo[i]='\0';
				i=0;
			}
		}
		fclose(fconfiguracionbd);

		mysql_init(&mysql);
		if(!(mysql_real_connect(&mysql, ip, usuario, contrasena, bd, 0, NULL, 0)))
			return -1;
		else
			return 1;
	}
	else
	{
		printf("No se pudo abrir el archivo....\n");
		return -1;
	}
}

void
on_Pedidos_show                        (GtkWidget       *widget,
                                        gpointer         user_data)
{
	gulong microsegundos=1000000;
	GThread  *hilo;
	GThread *th;
	GError *error = NULL;
	gdouble tiempo=0;
	
	GtkWidget *lblPedidos;
	GtkWidget *lblPedidosEntregados;
	GtkWidget *lblCumplidos;
	GtkWidget *lblAtrasados;
	GtkTreeView *tvPedidos;
	GtkListStore *modelo;
	
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *columna;
	GtkTreeIter iter;
	gboolean     valid;
	
	char sql[1000];
	char strMarkUp[200];
	char color_aviso[10], color_aviso2[10];
	int cont=0;
	int cont2=0;
	int er;
	
	gchar *aviso;
        
        homedir = getenv("HOME");
        strcpy(file_db_config, homedir);
        strcat(file_db_config, "/.carnesbecerra/configuracionbd.dat");
	
	//Se Crea la estructura del tree view
	tvPedidos       = GTK_TREE_VIEW(lookup_widget(widget,"treeViewPedidos"));
	
	lblPedidos           = lookup_widget(widget,"lblPedidos");
	lblPedidosEntregados = lookup_widget(widget,"lblPedidosEntregados");
	lblCumplidos         = lookup_widget(widget,"lblCumplidos");
	lblAtrasados         = lookup_widget(widget,"lblAtrasados");
	
	renderer = gtk_cell_renderer_text_new();
	
	columna = gtk_tree_view_column_new_with_attributes("Pedido",
											renderer,
											"text",COLPEDIDO,
											"size", ATRIBUTO,
											"background", ATRIBUTO2,
											"foreground", ATRIBUTO3,
											NULL);
	gtk_tree_view_append_column(tvPedidos, columna);
	
	columna = gtk_tree_view_column_new_with_attributes("Fecha",
											renderer,
											"text",COLFECHA,
											//"background-gdk",COLOR,
											//"size",ATRIBUTO,
											NULL);
	gtk_tree_view_append_column(tvPedidos, columna);
	
	columna = gtk_tree_view_column_new_with_attributes("Cliente",
											renderer,
											"text",COLCLIENTE,
											//"background-gdk",COLOR,
											//"size",ATRIBUTO,
											NULL);
	gtk_tree_view_append_column(tvPedidos, columna);
	
	columna = gtk_tree_view_column_new_with_attributes("Carnicero",
											renderer,
											"text",COLTOMO,
											//"background-gdk",COLOR,
											//"size",ATRIBUTO,
											NULL);
	gtk_tree_view_append_column(tvPedidos, columna);
	
	columna = gtk_tree_view_column_new_with_attributes("Hora de entrega",
											renderer,
											"text",COLHORA,
											NULL);
	gtk_tree_view_append_column(tvPedidos, columna);
	
	
	columna = gtk_tree_view_column_new_with_attributes("Avisar",
											renderer,
											"text",COLAVISO,
											NULL);
	gtk_tree_view_append_column(tvPedidos, columna);
	gtk_tree_view_column_set_visible(columna,FALSE);

	//modelo = GTK_LIST_STORE( gtk_tree_view_get_model (tvPedidos));
	
	gtk_window_maximize( GTK_WINDOW(gtk_widget_get_toplevel(widget)) );
	
	/*************************************/
	
	//Se agregan los pedidos pendientes
	if(conecta_bd() == 1)
	{
		sprintf (sql,"SELECT Pedido.id_pedido, DATE_FORMAT(Pedido.fecha,'%%d-%%m-%%y'), LEFT(Cliente.nombre,30), LEFT(Empleado.nombre,10), Pedido.HoraEntrega, IF (Pedido.HoraEntrega < CURTIME(),1,  IF(DATE_SUB( CONCAT( Pedido.FechaEntrega, ' ', Pedido.HoraEntrega ) , INTERVAL 15 MINUTE) < CONCAT( CURDATE(), ' ', CURTIME() ),2,0) ) FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente LEFT JOIN Empleado ON Pedido.id_carnicero = Empleado.id_empleado LEFT JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Venta.id_venta IS NULL AND FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' ORDER BY Pedido.HoraEntrega LIMIT 15");
		er = mysql_query(&mysql, sql);
		if(er == 0)
		{
			resPedidos  = mysql_store_result(&mysql);
			if(resPedidos)
			{
				modelo = gtk_list_store_new(columnas, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
								
				gtk_list_store_clear (modelo);
				
				cont = 0;
				while ((row = mysql_fetch_row(resPedidos)))
				{
					gtk_list_store_append (modelo, &iter);
					
					if ( (cont%2) == 0 )
					{
						gtk_list_store_set (modelo, &iter,
											ATRIBUTO,16*1024,
											ATRIBUTO2,"#EEEEEE",
											ATRIBUTO3,"#222222",
											ATRAVISO1,"#FF4444",
											COLPEDIDO,row[0],
											COLFECHA,row[1],
											COLCLIENTE,/*g_utf8_normalize(*/row[2]/*,-1,G_NORMALIZE_ALL)*/,
											COLTOMO,row[3],
											COLHORA,row[4],
											COLAVISO,row[5],-1);
					}
					else
					{
						gtk_list_store_set (modelo, &iter,
											ATRIBUTO,16*1024,
											ATRIBUTO2,"#FFFFFF",
											ATRIBUTO3,"#222222",
											COLPEDIDO,row[0],
											COLFECHA,row[1],
											COLCLIENTE,/*g_utf8_normalize(*/row[2]/*,-1,G_NORMALIZE_ALL)*/,
											COLTOMO,row[3],
											COLHORA,row[4],
											COLAVISO,row[5],-1);
					}
					cont++;
				}
				gtk_tree_view_set_model(tvPedidos, GTK_TREE_MODEL(modelo));
				g_object_unref(modelo);
			}
			printf ("SE LIBERA RES\n");
			//mysql_free_result(resPedidos);
			//printf ("SE LIBERA\n");
			//g_object_unref(G_OBJECT(modelo));
		}
		
		//Se obtiene el total de pedidos registrados
		sprintf (sql,"SELECT id_pedido FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario LEFT JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' ");
		er = mysql_query(&mysql, sql);
		if(er == 0)
		{
			res = mysql_store_result(&mysql);
			if(res)
			{
				sprintf(strMarkUp,"<span size='36000' color='#444444'><b>%d</b></span>", mysql_num_rows(res));
				gtk_label_set_markup (GTK_LABEL(lblPedidos), strMarkUp);
			}
			mysql_free_result(res);
		}
		
		//Se obtiene el numero total de pedidos surtidos del dia
		sprintf (sql,"SELECT id_pedido FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario INNER JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' ");
		er = mysql_query(&mysql, sql);
		if(er == 0)
		{
			res = mysql_store_result(&mysql);
			if(res)
			{
				sprintf(strMarkUp,"<span size='36000' color='#222888'><b>%d</b></span>", mysql_num_rows(res));
				gtk_label_set_markup (GTK_LABEL(lblPedidosEntregados), strMarkUp);
			}
			mysql_free_result(res);
		}
		
		//Se obtiene el numero de pedidos surtidos en el tiempo establecido
		sprintf (sql,"SELECT Pedido.id_pedido, IF(Venta.fecha = Pedido.FechaEntrega, IF(Venta.id_venta IS NULL,'En Produccion',IF (Pedido.HoraEntrega > Venta.hora,'cumplido','incumplido') ), IF (Venta.fecha < Pedido.FechaEntrega, 'cumplido', 'incumplido') ) AS Status FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario INNER JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' HAVING Status = 'cumplido' ");
		er = mysql_query(&mysql, sql);
		if(er == 0)
		{
			res = mysql_store_result(&mysql);
			if(res)
			{
				sprintf(strMarkUp,"<span size='36000' color='#338823'><b>%d</b></span>", mysql_num_rows(res));
				gtk_label_set_markup (GTK_LABEL(lblCumplidos), strMarkUp);
			}
			mysql_free_result(res);
		}
		
		//Se obtiene el numero de pedidos que no se surtieron en el tiempo establecido
		sprintf (sql,"SELECT Pedido.id_pedido, IF(Venta.fecha = Pedido.FechaEntrega, IF(Venta.id_venta IS NULL,'En Produccion',IF (Pedido.HoraEntrega > Venta.hora,'cumplido','incumplido') ), IF (Venta.fecha < Pedido.FechaEntrega, 'cumplido', 'incumplido') ) AS Status FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario INNER JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' HAVING Status = 'incumplido' ");
		er = mysql_query(&mysql, sql);
		if(er == 0)
		{
			res = mysql_store_result(&mysql);
			if(res)
			{
				sprintf(strMarkUp,"<span size='36000' color='#AA2822'><b>%d</b></span>", mysql_num_rows(res));
				gtk_label_set_markup (GTK_LABEL(lblAtrasados), strMarkUp);
			}
			mysql_free_result(res);
			
		}
		
		mysql_close(&mysql);
	}
	else
	{
		printf (" NO SE PUDO CONECTAR \n");
	}

	/************************************/

	
	timer = g_timer_new();
	
	while(terminar == FALSE)
	{
		g_timer_reset(timer);
		tiempo = g_timer_elapsed(timer, &microsegundos);
		cont2=0;
		while(tiempo < 15)
		{   
			tiempo = g_timer_elapsed(timer, &microsegundos);
			//Se obtiene la primera fila
			cont=0;
                        cont2 = 0; 
                        if (strcmp (color_aviso,"#FF4444") == 0)
                                sprintf (color_aviso,"#000000");
                        else
                                sprintf (color_aviso,"#FF4444");

                        if (strcmp (color_aviso2,"#d0b520") == 0)
                                sprintf (color_aviso2,"#000000");
                        else
                                sprintf (color_aviso2,"#d0b520");

                        modelo = gtk_list_store_new(columnas, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

                        gtk_list_store_clear (modelo);

                        mysql_data_seek(resPedidos,0);

                        cont = 0;
                        while ((row = mysql_fetch_row(resPedidos)))
                        {
                                gtk_list_store_append (modelo, &iter);

                                if ( (cont%2) == 0 )
                                {
                                        if (strcmp(row[5],"1") == 0)
                                        {
                                                gtk_list_store_set (modelo, &iter,
                                                                                ATRIBUTO,16*1024,
                                                                                ATRIBUTO2,"#EEEEEE",
                                                                                ATRIBUTO3,color_aviso,
                                                                                ATRAVISO1,"#FF4444",
                                                                                COLPEDIDO,row[0],
                                                                                COLFECHA,row[1],
                                                                                COLCLIENTE,/*g_utf8_normalize(*/row[2]/*,-1,G_NORMALIZE_ALL)*/,
                                                                                COLTOMO,row[3],
                                                                                COLHORA,row[4],
                                                                                COLAVISO,row[5],-1);
                                        }
                                        else
                                        {

                                                if (strcmp(row[5],"2") == 0)
                                                {
                                                        //printf ("ROW = %s\n", row[5]);
                                                        gtk_list_store_set (modelo, &iter,
                                                                                        ATRIBUTO,16*1024,
                                                                                        ATRIBUTO2,"#EEEEEE",
                                                                                        ATRIBUTO3,color_aviso2,
                                                                                        ATRAVISO1,"#FF4444",
                                                                                        COLPEDIDO,row[0],
                                                                                        COLFECHA,row[1],
                                                                                        COLCLIENTE,/*g_utf8_normalize(*/row[2]/*,-1,G_NORMALIZE_ALL)*/,
                                                                                        COLTOMO,row[3],
                                                                                        COLHORA,row[4],
                                                                                        COLAVISO,row[5],-1);
                                                }
                                                else
                                                {
                                                        gtk_list_store_set (modelo, &iter,
                                                                                        ATRIBUTO,16*1024,
                                                                                        ATRIBUTO2,"#EEEEEE",
                                                                                        ATRIBUTO3,"#222222",
                                                                                        ATRAVISO1,"#FF4444",
                                                                                        COLPEDIDO,row[0],
                                                                                        COLFECHA,row[1],
                                                                                        COLCLIENTE,/*g_utf8_normalize(*/row[2]/*,-1,G_NORMALIZE_ALL)*/,
                                                                                        COLTOMO,row[3],
                                                                                        COLHORA,row[4],
                                                                                        COLAVISO,row[5],-1);
                                                }
                                        }
                                }
                                else
                                {
                                        if (strcmp(row[5],"1") == 0)
                                        {
                                                gtk_list_store_set (modelo, &iter,
                                                                                ATRIBUTO,16*1024,
                                                                                ATRIBUTO2,"#FFFFFF",
                                                                                ATRIBUTO3,color_aviso,
                                                                                COLPEDIDO,row[0],
                                                                                COLFECHA,row[1],
                                                                                COLCLIENTE,/*g_utf8_normalize(*/row[2]/*,-1,G_NORMALIZE_ALL)*/,
                                                                                COLTOMO,row[3],
                                                                                COLHORA,row[4],
                                                                                COLAVISO,row[5],-1);
                                        }
                                        else
                                        {
                                                if (strcmp(row[5],"2") == 0)
                                                {
                                                        gtk_list_store_set (modelo, &iter,
                                                                                        ATRIBUTO,16*1024,
                                                                                        ATRIBUTO2,"#FFFFFF",
                                                                                        ATRIBUTO3,color_aviso2,
                                                                                        COLPEDIDO,row[0],
                                                                                        COLFECHA,row[1],
                                                                                        COLCLIENTE,/*g_utf8_normalize(*/row[2]/*,-1,G_NORMALIZE_ALL)*/,
                                                                                        COLTOMO,row[3],
                                                                                        COLHORA,row[4],
                                                                                        COLAVISO,row[5],-1);
                                                }
                                                else
                                                {
                                                        gtk_list_store_set (modelo, &iter,
                                                                                        ATRIBUTO,16*1024,
                                                                                        ATRIBUTO2,"#FFFFFF",
                                                                                        ATRIBUTO3,"#222222",
                                                                                        COLPEDIDO,row[0],
                                                                                        COLFECHA,row[1],
                                                                                        COLCLIENTE,/*g_utf8_normalize(*/row[2]/*,-1,G_NORMALIZE_ALL)*/,
                                                                                        COLTOMO,row[3],
                                                                                        COLHORA,row[4],
                                                                                        COLAVISO,row[5],-1);
                                                }
                                        }
                                }
                                cont++;
                        }
                        gtk_tree_view_set_model(tvPedidos, GTK_TREE_MODEL(modelo));
                        g_object_unref(modelo);
			
			while(g_main_iteration(FALSE));
                        sleep(1);
		}
	
		//Se agregan los pedidos pendientes
		if(conecta_bd() == 1)
		{
			sprintf (sql,"SELECT Pedido.id_pedido, DATE_FORMAT(Pedido.fecha,'%%d-%%m-%%y'), LEFT(Cliente.nombre,30), LEFT(Empleado.nombre,10), Pedido.HoraEntrega, IF (Pedido.HoraEntrega < CURTIME(),1,  IF(DATE_SUB( CONCAT( Pedido.FechaEntrega, ' ', Pedido.HoraEntrega ) , INTERVAL 15 MINUTE) < CONCAT( CURDATE(), ' ', CURTIME() ),2,0) ) FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente LEFT JOIN Empleado ON Pedido.id_carnicero = Empleado.id_empleado LEFT JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Venta.id_venta IS NULL AND FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' ORDER BY Pedido.HoraEntrega LIMIT 15");
			er = mysql_query(&mysql, sql);
			if(er == 0)
			{
				mysql_free_result(resPedidos);
				resPedidos = mysql_store_result(&mysql);
				if(resPedidos)
				{
					modelo = gtk_list_store_new(columnas, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
					
					gtk_list_store_clear (modelo);
					
					cont = 0;
					while ((row = mysql_fetch_row(resPedidos)))
					{
						gtk_list_store_append (modelo, &iter);
						
						if ( (cont%2) == 0 )
						{
							gtk_list_store_set (modelo, &iter,
												ATRIBUTO,16*1024,
												ATRIBUTO2,"#EEEEEE",
												ATRIBUTO3,"#222222",
												ATRAVISO1,"#FF4444",
												COLPEDIDO,row[0],
												COLFECHA,row[1],
												COLCLIENTE,row[2],
												COLTOMO,row[3],
												COLHORA,row[4],
												COLAVISO,row[5],-1);
						}
						else
						{
							gtk_list_store_set (modelo, &iter,
												ATRIBUTO,16*1024,
												ATRIBUTO2,"#FFFFFF",
												ATRIBUTO3,"#222222",
												COLPEDIDO,row[0],
												COLFECHA,row[1],
												COLCLIENTE,row[2],
												COLTOMO,row[3],
												COLHORA,row[4],
												COLAVISO,row[5],-1);
						}
						cont++;
					}
					gtk_tree_view_set_model(tvPedidos, GTK_TREE_MODEL(modelo));
					g_object_unref(modelo);
				}
				//printf ("SE LIBERA RES\n");
			}
			
			//Se obtiene el total de pedidos registrados
			sprintf (sql,"SELECT id_pedido FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario LEFT JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' ");
			er = mysql_query(&mysql, sql);
			if(er == 0)
			{
				res = mysql_store_result(&mysql);
				if(res)
				{
					sprintf(strMarkUp,"<span size='36000' color='#444444'><b>%d</b></span>", mysql_num_rows(res));
					gtk_label_set_markup (GTK_LABEL(lblPedidos), strMarkUp);
				}
				mysql_free_result(res);
			}
			
			//Se obtiene el numero total de pedidos surtidos del dia
			sprintf (sql,"SELECT id_pedido FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario INNER JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' ");
			er = mysql_query(&mysql, sql);
			if(er == 0)
			{
				res = mysql_store_result(&mysql);
				if(res)
				{
					sprintf(strMarkUp,"<span size='36000' color='#222888'><b>%d</b></span>", mysql_num_rows(res));
					gtk_label_set_markup (GTK_LABEL(lblPedidosEntregados), strMarkUp);
				}
				mysql_free_result(res);
			}
			
			//Se obtiene el numero de pedidos surtidos en el tiempo establecido 
			sprintf (sql,"SELECT Pedido.id_pedido, IF(Venta.fecha = Pedido.FechaEntrega, IF(Venta.id_venta IS NULL,'En Produccion',IF (Pedido.HoraEntrega > Venta.hora,'cumplido','incumplido') ), IF (Venta.fecha < Pedido.FechaEntrega, 'cumplido', 'incumplido') ) AS Status FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario INNER JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' HAVING Status = 'cumplido' ");
			er = mysql_query(&mysql, sql);
			if(er == 0)
			{
				res = mysql_store_result(&mysql);
				if(res)
				{
					sprintf(strMarkUp,"<span size='36000' color='#338823'><b>%d</b></span>", mysql_num_rows(res));
					gtk_label_set_markup (GTK_LABEL(lblCumplidos), strMarkUp);
				}
				mysql_free_result(res);
				}
			
			//Se obtiene el numero de pedidos que no se surtieron en el tiempo establecido 
			sprintf (sql,"SELECT Pedido.id_pedido, IF(Venta.fecha = Pedido.FechaEntrega, IF(Venta.id_venta IS NULL,'En Produccion',IF (Pedido.HoraEntrega > Venta.hora,'cumplido','incumplido') ), IF (Venta.fecha < Pedido.FechaEntrega, 'cumplido', 'incumplido') ) AS Status FROM Pedido INNER JOIN Cliente ON Pedido.id_cliente = Cliente.id_cliente INNER JOIN Usuario ON Pedido.id_usuario = Usuario.id_usuario INNER JOIN Venta ON Venta.id_venta = Pedido.id_venta WHERE Pedido.FechaEntrega = CURDATE() AND Pedido.cancelado = 'n' HAVING Status = 'incumplido'");
			er = mysql_query(&mysql, sql);
			if(er == 0)
			{
				res = mysql_store_result(&mysql);
				if(res)
				{
					sprintf(strMarkUp,"<span size='36000' color='#AA2822'><b>%d</b></span>", mysql_num_rows(res));
					gtk_label_set_markup (GTK_LABEL(lblAtrasados), strMarkUp);
				}
				mysql_free_result(res);
				
			}
			
			mysql_close(&mysql);
		}
		else
		{
			printf (" NO SE PUDO CONECTAR \n");
		}

		//----------------------------------------/
		
	}
	
}

void
on_Pedidos_destroy                     (GtkObject       *object,
                                        gpointer         user_data)
{
    printf("Saliendo...\n");
	terminar=TRUE;
	g_timer_stop(timer);
	gtk_main_quit();
}
