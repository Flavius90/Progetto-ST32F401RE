
import java.io.*;
import java.net.*;
import javax.swing.*; 
public class Client
{
	public static void main(String argv[])
	{
		BufferedReader in = null;
		PrintStream out = null;
		Socket socket = null;
		String Risposta = new String();
		String max = new String();
		String media = new String();
		//String IP=JOptionPane.showInputDialog("Inserisci IP del Server");
		do {
			String Username=JOptionPane.showInputDialog("Inserisci la Username:");
			String Password=JOptionPane.showInputDialog("Inserisci la Password:");
			try
			{
				// apertura della socket
				//socket = new Socket(IP, 4000);
				socket = new Socket("localhost", 4000);                             //COSì FUNZIONA SOLO SU UN PC
				// apertura canali I\O
				in = new BufferedReader(
						new InputStreamReader(socket.getInputStream()));
				out = new PrintStream(socket.getOutputStream(), true);
				//Scrivi al Server
				out.println(Username);
				out.println(Password);
				out.flush();
				// Legge dal Server
				Risposta = in.readLine();
				System.out.println(Risposta);	

				if(Risposta.equals("Accesso negato.")){
					System.out.println("Reinserire Username e password");
				}else {
					do{
						String Operazione = new String();
						String Cognome = new String();
						Operazione=JOptionPane.showInputDialog("Inserisci 'inserimento' per aggiungere i dati di una squadra \n"+"Inserisci 'massimo' per avere il punteggio max di un giocatore  \n"+"Inserisci 'media' per avere la media del punteggio di una giocatore:\n"+"Inserisci 'exit' per uscire:");



						switch(Operazione) {
						
						case "inserimento":
							out.println(Operazione);
							Inserimento.main(null);
							break;
							
						case "massimo":
							out.println(Operazione);
							Cognome=JOptionPane.showInputDialog("Inserisci il Cognome:");
							out.println(Cognome);
							max = in.readLine();
							System.out.println("Il punteggio massimo registrato da "+Cognome+ " è: "+max);
							System.out.println("Il punteggio massimo registrato è di: "+Cognome);
							break;
						case "media":
							out.println(Operazione);
							Cognome=JOptionPane.showInputDialog("Inserisci il Cognome:");
							out.println(Cognome);
							media = in.readLine();
							System.out.println("La media del punteggio registrata da "+Cognome+ " è: "+media);
							break;
						case "exit":  System.out.println("Chiusura programma...");
						System.exit(0);
						default:           
							System.out.println("Operazione non consentita!");



						}

					}while(true);
				}

				out.close();
				in.close();
			}
			catch(Exception e) { System.out.println(e.getMessage());}

		}while(Risposta.equals("Accesso negato."));

	}
}