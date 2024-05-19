
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;

import javax.swing.JOptionPane;
public class Media_Punteggio {
	public static int main(String[] args, String cognome){
		int media=0;
		String Cognome= cognome;
		try {
			Class.forName("com.mysql.jdbc.Driver");
			Connection conn =DriverManager.getConnection("jdbc:mysql://localhost/punteggio?" + "user=root&password="
					);
			Statement stmt;
			ResultSet rs;
			stmt = conn.createStatement();
			int somma=0;
			int numero_cogn=0;
			int cogn=0;
			String query = "SELECT punteggio FROM punteggio WHERE Cognome= ";
			query+="'"+Cognome+"'";
			System.out.println(query);
			rs = stmt.executeQuery(query); 

			while(rs.next())
			{
				cogn = Integer.parseInt(rs.getString("Punteggio"));
				somma+=cogn;
				numero_cogn++;
			}

			media= somma/numero_cogn;
			stmt.close();
			conn.close();


		}catch(Exception e) {e.printStackTrace();}

		return media;

	}
}