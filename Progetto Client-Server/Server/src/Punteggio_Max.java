import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;

import javax.swing.JOptionPane;
public class Punteggio_Max {
	public static int main(String[] args, String cognome){
		int max=0;
		String Cognome = cognome;
		try {
			Class.forName("com.mysql.jdbc.Driver");
			Connection conn =DriverManager.getConnection("jdbc:mysql://localhost/punteggio?" + "user=root&password="
					);
			Statement stmt;
			ResultSet rs;
			stmt = conn.createStatement();
			String query = "SELECT Cognome  FROM Punteggio  WHERE Punteggio in (SELECT MAX(Punteggio) FROM Punteggio";
			rs = stmt.executeQuery(query); 
			rs.next();
			cognome = rs.getString("Cognome");
			max = Integer.parseInt(rs.getString("MAX(Punteggio)"));	
			stmt.close();
			conn.close();
		}catch(Exception e) {e.printStackTrace();}
		System.out.println("punt max fatto da "+Cognome+"segnando "+max);
		return max;
	}
}