
import java.io.*;
import java.net.*;
import java.util.*;

public class Server extends Thread
{
	private ServerSocket Server;
	public static void main(String argv[]) throws Exception
	{
		new Server();
	}

	public Server() throws Exception
	{
		Server = new ServerSocket(4000);
		System.out.println("Il Server è in attesa sulla porta 4000.");
		this.start();
	}

	public void run()
	{
		while(true)
		{
			try {

				System.out.println("In attesa di Connessione.");
				Socket client = Server.accept();
				System.out.println("Connessione accettata da: "+
						client.getInetAddress());
				ConnectPlus c = new ConnectPlus(client);

			}
			catch(Exception e) {}
		}
	}
}

class ConnectPlus extends Thread
{
	private Socket client = null;
	BufferedReader in = null;
	PrintStream out = null;
	public ConnectPlus(Socket clientSocket)
	{
		client = clientSocket;
		try
		{
			in = new BufferedReader(
					new InputStreamReader(client.getInputStream()));
			out = new PrintStream(client.getOutputStream(), true);
		}
		catch(Exception e1)
		{
			try { client.close();  }
			catch(Exception e) { System.out.println(e.getMessage());}
			return;
		}
		this.start();                         
	}

	public void run()
	{

		try
		{
			int max;
			int media;
			String Operazione = new String();
			String Username = new String();
			String Password = new String();
			String Risposta = new String();
			String Cognome = new String();
			Username = in.readLine();
			Password = in.readLine();
			System.out.println(Username);
			System.out.println(Password);
			Login autenticazione = new Login ();
			Punteggio_Max puntmax = new Punteggio_Max();
			Media_Punteggio mediacogn = new Media_Punteggio();
			autenticazione.main(null , Username, Password);
			if(autenticazione.main(null,Username,Password)) {
				Risposta = "Autenticato.";
				out.println(Risposta);
				System.out.println(Risposta);
				do {	
					Operazione =in.readLine();

					switch(Operazione) {
					case "media" :   Cognome = in.readLine();
					media = mediacogn.main(null,Cognome);
					System.out.println("La media del punteggio registrata da "+Cognome+ " è: " + media);
					out.println(media);
					break; 


					case "massimo" : Cognome = in.readLine();
					max = puntmax.main(null,Cognome);
					System.out.println("Il punteggio massimo registrato da " +Cognome+" è: " + max);
					
					out.println(max);
					break; 




					}

				}while(Operazione!="exit");


			}else {
				Risposta = "Accesso negato.";
				out.println(Risposta);
				System.out.println(Risposta);
			}
			out.println(Risposta);
			out.flush();
			// chiusura stream e connessioni.
			out.close();
			in.close();
			client.close();
		}
		catch(Exception e) {}
	}

}

