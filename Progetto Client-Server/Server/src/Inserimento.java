import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Scanner;
import javax.swing.JOptionPane;
import java.sql.PreparedStatement;

 

public class Inserimento {

    public static void main(String[] args) {
    	Connection myConn = null;
		PreparedStatement myStmt = null;

		Scanner scanner = null;

        try {

            Class.forName("com.mysql.jdbc.Driver");

            Connection conn =

                DriverManager.getConnection(

                    "jdbc:mysql://localhost:3306/punteggio?" + "user=root&password="

                );
             // inserisco record
            scanner = new Scanner(System.in);
            
            String giornata=JOptionPane.showInputDialog("Inserisci Giornata:");
            
            String cognome=JOptionPane.showInputDialog("Inserisci Cognome del giocatore:");
            
            String punteggio=JOptionPane.showInputDialog("Inserisci i punti segnati:");
                        
            String sql = "insert into punteggio "
        			+ " (Giornata, Cognome, Punteggio)" + " values (?, ?, ?)";

        	myStmt = conn.prepareStatement(sql);

        	// setto i parametri
        	myStmt.setString(1, giornata);
        	myStmt.setString(2, cognome);
        	myStmt.setString(3, punteggio);
        	
        	// 3. Eseguo la query
        myStmt.executeUpdate();
                        
    	System.out.println("Inserimento avvenuto con successo.");
		} catch (Exception exc) {
			exc.printStackTrace();
		} finally {
			if (myStmt != null) {
				try {
					myStmt.close();
				
				if (myConn != null) {
				myConn.close();
				}}catch (SQLException e) {
				
				e.printStackTrace();
			}

			if (scanner != null) {
				scanner.close();
			}
		}
	}

}
}
