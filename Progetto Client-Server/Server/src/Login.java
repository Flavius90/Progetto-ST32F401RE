
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import javax.swing.*;

public class Login {

	public static boolean  main(String[] args, String usr, String psw) {
		Boolean Controllo = false;
		try {
			Class.forName("com.mysql.jdbc.Driver");
			Connection conn =
					DriverManager.getConnection(
							"jdbc:mysql://localhost/utenti?" + "user=root&password="
							);
			Statement stmt;
			ResultSet rs;    
			String Username = usr;
			String Password = psw;
			stmt = conn.createStatement();

			// recupero  dati
			rs = stmt.executeQuery("SELECT * from utente");

			while(rs.next()&&!Controllo)
			{
				if (rs.getString("Username").equals(Username) && rs.getString("Password").equals(Password))
					Controllo=true;

			}


			stmt.close(); // rilascio  risorse
			conn.close(); // termine  connessione  
		}
		catch(ClassNotFoundException e)
		{
			System.out.println(e);
		}
		catch(SQLException e)
		{
			System.out.println(e);
		}
		return Controllo;
	}

}	